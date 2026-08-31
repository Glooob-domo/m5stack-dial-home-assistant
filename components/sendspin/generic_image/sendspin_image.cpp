#include "sendspin_image.h"

#if defined(USE_ESP32) && defined(USE_SENDSPIN_ARTWORK)

#include "esphome/core/application.h"
#include "esphome/core/log.h"

#include <esp_heap_caps.h>
#include <esp_timer.h>
#include <utility>

namespace esphome {
namespace sendspin {

static const char *const TAG = "sendspin.generic_image";

// Stack size for the decode task - TODO: Determine how much stack is actually necessary
static const size_t DECODE_TASK_STACK_SIZE = 8192;
static const UBaseType_t DECODE_TASK_PRIORITY = 2;
static const size_t MIN_INTERNAL_BLOCK_FOR_JPEG_DECODE = 40 * 1024;

SendspinImage::SendspinImage(int fixed_width, int fixed_height, runtime_image::ImageFormat format,
                             image::ImageType type, image::Transparency transparency, bool is_big_endian,
                             image::Image *placeholder)
    : runtime_image::RuntimeImage(format, type, transparency, placeholder, is_big_endian, fixed_width, fixed_height) {}

void SendspinImage::setup() {
  // Map runtime_image format to SendspinImageFormat
  switch (this->get_format()) {
    case runtime_image::BMP:
      this->sendspin_format_ = SendspinImageFormat::BMP;
      break;
    case runtime_image::JPEG:
      this->sendspin_format_ = SendspinImageFormat::JPEG;
      break;
    case runtime_image::PNG:
      this->sendspin_format_ = SendspinImageFormat::PNG;
      break;
    case runtime_image::AUTO:
      // AUTO is not supported for sendspin images; default to JPEG
      this->sendspin_format_ = SendspinImageFormat::JPEG;
      break;
  }

  // Register image preference with the hub
  ImageSlotPreference preference = {
      .slot = this->slot_,
      .source = this->source_,
      .format = this->sendspin_format_,
      .width = static_cast<uint16_t>(this->fixed_width_),
      .height = static_cast<uint16_t>(this->fixed_height_),
  };
  this->parent_->add_image_preferred_format(preference);

  // Register slot-specific callback
  this->parent_->add_image_slot_callback(
      this->slot_, [this](const uint8_t *data, size_t length, SendspinImageFormat format, int64_t server_timestamp) {
        ESP_LOGI(TAG, "Artwork callback: slot=%u length=%u format=%s", this->slot_, static_cast<unsigned>(length),
                 to_cstr(format));
        if (this->sendspin_format_ != format) {
          ESP_LOGW(TAG, "Ignoring artwork format %s, expected %s", to_cstr(format), to_cstr(this->sendspin_format_));
          return;
        }

        // Check if a decode/display cycle is already in progress (atomic — safe across threads)
        if (this->decode_active_.load(std::memory_order_acquire)) {
          // Cancel any pending display timeout. The old decoded data will be overwritten by
          // the new begin_decode() call once the current cycle finishes.
          ESP_LOGW(TAG, "Artwork decode already active, dropping pending display and waiting for current decode");
          this->cancel_timeout("display_image");
          return;
        }

        this->server_timestamp_ = server_timestamp;

        if (length == 0) {
          ESP_LOGW(TAG, "Empty artwork payload, releasing image");
          this->release();
          return;
        }

        // Temporarily store encoded image data by copy, image decoders don't work with const pointers
        this->encoded_data_.assign(data, data + length);

        // Mark decode cycle as active before spawning the task
        this->decode_active_.store(true, std::memory_order_release);

        // Create a FreeRTOS task to decode the image without blocking the main loop
        xTaskCreate(SendspinImage::decode_task, "image_decode", DECODE_TASK_STACK_SIZE, (void *) this,
                    DECODE_TASK_PRIORITY, &this->decode_task_handle_);
      });
}

void SendspinImage::add_on_image_received_callback(std::function<void()> &&callback) {
  this->image_received_callback_.add(std::move(callback));
}

void SendspinImage::add_on_image_decoded_callback(std::function<void()> &&callback) {
  this->image_decoded_callback_.add(std::move(callback));
}

void SendspinImage::add_on_image_error_callback(std::function<void()> &&callback) {
  this->image_error_callback_.add(std::move(callback));
}

void SendspinImage::decode_task(void *params) {
  SendspinImage *this_image = (SendspinImage *) params;

  const size_t largest_internal_block = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
  if (largest_internal_block < MIN_INTERNAL_BLOCK_FOR_JPEG_DECODE) {
    ESP_LOGW(TAG, "Skipping album artwork decode: largest internal block is %zu bytes", largest_internal_block);
    this_image->encoded_data_.clear();
    this_image->decode_active_.store(false, std::memory_order_release);
    this_image->defer("image_error", [this_image]() { this_image->image_error_callback_.call(); });
    App.wake_loop_threadsafe();
    vTaskDelete(nullptr);
    return;
  }

  // Trigger the image received callback (thread-safe via defer)
  this_image->defer("image_received", [this_image]() { this_image->image_received_callback_.call(); });
  App.wake_loop_threadsafe();

  this_image->begin_decode(this_image->encoded_data_.size());
  ESP_LOGI(TAG, "Decoding album artwork: encoded=%u bytes", static_cast<unsigned>(this_image->encoded_data_.size()));

  // Feed the encoded data to the decoder (decodes into internal buffer, but not yet visible)
  this_image->feed_data(this_image->encoded_data_.data(), this_image->encoded_data_.size());

  // Clear the encoded data after feeding to decoder
  this_image->encoded_data_.clear();

  // Defer to main loop for timestamp-aware display timing
  this_image->defer("image_display", [this_image]() {
    this_image->decode_task_handle_ = nullptr;

    // Lambda to finalize display: call end_decode() and fire the appropriate callback
    auto finalize_display = [this_image]() {
      bool decode_success = this_image->end_decode();
      // Release the decode cycle lock so new artwork can be accepted
      this_image->decode_active_.store(false, std::memory_order_release);
      if (!decode_success) {
        ESP_LOGW(TAG, "Album artwork decode failed");
        this_image->image_error_callback_.call();
      } else {
        ESP_LOGI(TAG, "Album artwork decode complete");
        this_image->image_decoded_callback_.call();
      }
    };

    int64_t client_target = this_image->parent_->get_client_time(this_image->server_timestamp_);
    if (client_target != 0) {
      int64_t delay_us = client_target - esp_timer_get_time();
      if (delay_us > 0) {
        uint32_t delay_ms = static_cast<uint32_t>(std::min(delay_us / 1000, (int64_t) 30000));
        this_image->set_timeout("display_image", delay_ms, finalize_display);
        return;
      }
    }
    // Past due or no time filter — display immediately
    finalize_display();
  });
  App.wake_loop_threadsafe();

  // Delete this task
  vTaskDelete(nullptr);
}

}  // namespace sendspin
}  // namespace esphome

#endif
