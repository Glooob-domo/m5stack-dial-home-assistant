# Home Assistant Controller for M5Stack Dial — référence de configuration

Maintenu par **[Glooob Domo](https://github.com/Glooob-domo)**, basé sur le fork Home Assistant Controller de [hectorzin](https://github.com/hectorzin) et le projet original Smart Home Button de [Jason Wen](https://github.com/Jasionf). Pour une vue d'ensemble et un exemple d'installation prêt à l'emploi, retournez au [README](../README.fr.md).

**Langues :** [English](configuration.md) · Français (cette page)

Une installation normale utilise un seul fichier dans ESPHome : [`m5-dial.FR.yaml`](../m5-dial.FR.yaml) (commentaires en français) ou [`m5-dial.EN.yaml`](../m5-dial.EN.yaml) (commentaires en anglais) — mêmes champs dans les deux cas. Il récupère le firmware depuis GitHub et contient vos identifiants d'entités. Vous ne copiez pas `src/` ni `components/` dans ESPHome.

## Identifiants de l'appareil

```yaml
substitutions:
  timezone: Europe/Paris
  api_encryption_key: !secret api_encryption_key
  wifi_ssid: !secret wifi_ssid
  wifi_password: !secret wifi_password
```

Le package fixe par défaut `device_name`, `device_friendly_name`, les valeurs du point d'accès de secours et le mot de passe OTA, pour la validation. Remplacez ces valeurs, surtout le mot de passe OTA, pour une vraie installation. Gardez les identifiants dans `secrets.yaml` et ne les commitez jamais.

## Langue

```yaml
substitutions:
  ui_language: fr
```

`ui_language` détermine les libellés sur l'Horloge, le menu et les autres pages. Valeurs prises en charge : `en`, `fr`, `es`, `de`, `it`. La modifier nécessite une recompilation. La valeur par défaut est `en` si la clé est omise.

Les langues autres que l'anglais inversent aussi l'**ordre de la date** sur l'Horloge en jour/mois (`Mar  02/09`). L'anglais conserve mois/jour (`Mon  09/02`).

## Affichage de l'Horloge

La page horloge est toujours disponible. En plus de l'heure, de la météo et de l'AQI optionnel, elle affiche un anneau de progression des **secondes** qui se met à jour toutes les 5 secondes.

```yaml
substitutions:
  timezone: Europe/Paris
  ui_language: fr
  weather_entity: weather.maison
  aqi_entity: sensor.aqi_salon
```

Laissez `weather.disabled` ou `sensor.disabled` dans votre fichier de config pour garder `--` sur ces champs. Si une entité est manquante ou indisponible, l'Horloge affiche aussi `--`. `weather_entity` fournit les informations météo ; `aqi_entity` doit être numérique, l'attribut historique `aqi` de l'entité météo n'étant utilisé qu'en repli. Trouvez les identifiants d'entité sous **Outils de développement → États**.

## Fonctionnalités de menu optionnelles

Les fonctionnalités optionnelles non configurées sont masquées du menu. Remplissez la liste correspondante pour afficher une page. **Omettez** entièrement une clé de liste pour masquer cette page — inutile d'ajouter des entrées `dial_*: []` vides dans votre fichier de config. La météo/AQI de l'Horloge restent sur la page horloge et affichent `--` quand elles sont laissées en `*.disabled`. L'Alarme ne nécessite aucun champ — elle est toujours dans le menu, réglée entièrement sur le Dial.

| Champ | Active | Exemple |
| --- | --- | --- |
| `dial_lights` | Lumières | Une liste d'entités lumière Home Assistant. |
| `dial_climates` | Clim | Une liste d'entités climate. |
| `dial_media_players` | Musique | Une liste de lecteurs média. |
| `dial_covers` | Volets | Une liste d'entités volet / store. |
| `dial_garages` | Garage | Une liste d'entités volet de type garage / portail. |
| `dial_switches` | Prises | Une liste d'entités `switch` ou `input_boolean`. |
| `dial_scenes` | Scènes | Une liste d'entités `scene` ou `script`. |
| `dial_temperatures` | Pièces | Une liste de capteurs de température (ou d'entités climate). |

### Lumières

```yaml
dial_lights:
  - entity_id: light.salon
    name: Salon
  - entity_id: light.bureau
    name: Bureau
```

Chaque entrée nécessite un `entity_id` et un `name` affiché. Omettez `dial_lights` pour masquer Lumières du menu. Une seule lumière ouvre directement la page de contrôle ; deux ou plus ouvrent d'abord un sélecteur.

### Chauffage

```yaml
dial_climates:
  - entity_id: climate.salon
    name: Salon
  - entity_id: climate.chambre
    name: Chambre
```

Même règle que les lumières : une seule entité climate ouvre directement la page, plusieurs ouvrent un sélecteur.

La page gère la température cible et utilise les modes annoncés par l'entité. Le mode HVAC, le mode ventilateur et les capacités liées au swing varient selon les intégrations Home Assistant, donc n'attendez que les contrôles réellement pris en charge par l'entité.

### Musique

```yaml
dial_media_players:
  - entity_id: media_player.salon
    name: Salon
```

Sélectionnez l'entité qui joue réellement l'audio. Home Assistant fournit l'état du lecteur, les actions play/pause et transport, le volume et les métadonnées disponibles, dont `media_title`, `media_artist`, `media_duration` et `media_position`.

Sans **SendSpin** (optionnel — une source compatible comme Music Assistant poussant une pochette JPEG 100 × 100 vers le Dial), la page utilise une mise en page plus grande : un grand cadran de volume à la place de la pochette, et des contrôles titre/artiste/transport plus grands. La première pochette que SendSpin livre bascule la page, pour le reste de ce démarrage, vers la mise en page compacte (petite pochette + petites barres de volume). La lecture et les métadonnées fonctionnent à l'identique dans les deux cas.

### Volets

```yaml
dial_covers:
  - entity_id: cover.salon
    name: Salon
  - entity_id: cover.chambre
    name: Chambre
```

La molette règle la position par pas de 5 %. Un appui court bascule ouvert/fermé (ou stoppe si le volet est en mouvement). Les boutons tactiles envoient ouvrir, stop et fermer. Avec `encoder_resolution: "1"` par défaut, un cran mécanique équivaut à un pas de 5 %.

### Garage

```yaml
dial_garages:
  - entity_id: cover.portail
    name: Portail
```

Même règle qu'aux volets pour l'ouverture directe si une seule entité. La page affiche des boutons tactiles **ouvrir** et **fermer** au centre, et **stop** en dessous. La molette est ignorée. Un appui court stoppe le volet pendant qu'il s'ouvre ou se ferme. Pas d'arc ni de pourcentage de position.

### Prises

```yaml
dial_switches:
  - entity_id: switch.salon
    name: Salon
```

Un appui court ou un toucher bascule la prise. Les helpers `input_boolean.*` sont acceptés dans la même liste.

### Scènes

```yaml
dial_scenes:
  - entity_id: scene.cinema
    name: Cinéma
  - entity_id: script.tout_eteindre
    name: Tout éteindre
```

Un appui court ou un toucher exécute `scene.turn_on` ou `script.turn_on` selon le domaine de l'entité. La page de contrôle affiche un grand bouton **ACTIVER** au centre.

### Pièces

```yaml
dial_temperatures:
  - entity_id: sensor.temperature_salon
    name: Salon
  - entity_id: sensor.temperature_chambre
    name: Chambre
```

Chaque entrée nécessite un `entity_id` et un `name` affiché. Utilisez un `sensor.*` numérique (l'état est la température) ou une entité `climate.*` (la page affiche `current_temperature`). Omettez `dial_temperatures` pour masquer l'entrée de menu. Une seule pièce ouvre directement la page ; plusieurs pièces restent sur cette page et la molette les fait défiler.

### Alarme

Rien à configurer — l'Alarme est toujours dans le menu. C'est une alarme quotidienne unique, entièrement locale au Dial : l'heure cible et l'état armé vivent sur l'appareil (ils survivent à un redémarrage) et aucune entité Home Assistant n'est impliquée.

Appuyez une fois sur la page Alarme pour éditer l'heure (elle clignote ; la molette l'ajuste), appuyez à nouveau pour passer à la minute, puis une fois de plus pour confirmer. Touchez **ON**/**OFF** pour l'armer ou la désarmer. Une fois armée, elle se déclenche à cette heure chaque jour jusqu'à être désarmée. Quand elle se déclenche, le Dial se réveille, ouvre l'Alarme et bipe toutes les quelques secondes jusqu'à ce qu'elle soit arrêtée (toucher l'écran ou appuyer sur le bouton).

## Molette

```yaml
substitutions:
  encoder_resolution: "1"
```

La molette du Dial produit quatre impulsions en quadrature par cran mécanique. `encoder_resolution` détermine combien de ces impulsions comptent pour un pas d'interface. Valeurs autorisées : `1`, `2` ou `4`. La valeur par défaut **`1`** correspond à un pas d'interface par cran mécanique sur toutes les pages qui utilisent la molette (lumières, volets, pièces, volume, édition de l'alarme, sélection dans le menu, etc.). Utilisez `4` uniquement si vous voulez une molette plus sensible. La modifier nécessite une recompilation.

### Amplitude du pas de valeur

Sur les pages qui modifient la valeur d'une entité (Lumières, Volets, Clim, Musique), chaque pas de molette listé ci-dessus modifie la valeur d'une quantité configurable :

```yaml
substitutions:
  cover_position_step: "5"       # % par pas, de 1 à 100
  light_brightness_step: "1"     # % par pas, de 1 à 100
  climate_temperature_step: "2"  # en demi-degrés : 2 = 1°C, 1 = 0,5°C
  music_volume_step: "10"        # % par pas, de 1 à 100
```

Ces réglages ne s'appliquent qu'à Lumières, Volets, Clim et Musique — les pages qui ne font que naviguer (Menu, Pièces) ou basculer/activer (Prises, Scènes, Garage) ne sont pas concernées. Les modifier nécessite une recompilation.

## Référence : gestion de l'écran

```yaml
substitutions:
  screen_dim_timeout: 45s
  screen_return_timeout: 5min
  screen_off_timeout: 30min
  screen_dim_brightness: "20%"
```

| Réglage | Défaut | Comportement |
| --- | --- | --- |
| `screen_dim_timeout` | `45s` | Assombrit le rétroéclairage de la page en cours. |
| `screen_return_timeout` | `5min` | Retourne à l'Horloge. |
| `screen_off_timeout` | `30min` | Éteint uniquement le rétroéclairage. |
| `screen_dim_brightness` | `"20%"` | Niveau du rétroéclairage pendant DIM. |

Réglez un délai à `0s` pour le désactiver. DIM et RETURN sont indépendants. Avec OFF activé, le package relève un délai OFF plus court qu'un délai DIM ou RETURN actif, jusqu'au délai le plus tardif des deux. DIM et OFF conservent la page active ; RETURN navigue volontairement vers l'Horloge. Le premier mouvement de molette, appui bouton avant ou geste tactile depuis DIM ou OFF réveille l'écran et est consommé.

Seule une alarme **en train de sonner** bloque RETURN ; DIM et OFF continuent de fonctionner malgré tout. Une alarme qui se déclenche réveille l'écran, ouvre l'Alarme et déclenche son effet clignotant et sonore jusqu'à être arrêtée.

## Rafraîchissement et validation du package

`ref: main` suit la version actuellement publiée sur la branche `main` du dépôt. `refresh: 0s` fait vérifier à ESPHome le package distant à chaque configuration ou compilation, ce qui est utile pendant le suivi actif, mais dépend de l'accessibilité de GitHub et peut ajouter du temps de téléchargement. C'est optionnel ; épinglez un tag ou un commit dans `ref` pour des compilations reproductibles. Recompilez après une mise à jour du package.

Validez avant de flasher :

```bash
esphome config votre-dial.yaml
esphome compile votre-dial.yaml
```

Utilisez l'USB pour l'installation initiale si l'appareil n'est pas sur le Wi-Fi ; les mises à jour suivantes peuvent utiliser l'OTA d'ESPHome.

## Erreurs de configuration courantes

Chaque liste `dial_*` valide ses entités **à la compilation**, avant même de flasher l'appareil :

- **Mauvais domaine :** une entité dans la mauvaise liste (par exemple `switch.salon` sous `dial_lights`) fait échouer `esphome config`/`compile` avec un message explicite, ex. `Entity ID 'switch.salon' is not valid for dial_lights: expected an entity starting with 'light.'`. Déplacez l'entrée vers la bonne liste ou corrigez la faute de frappe dans le domaine.
- **Entité en double :** lister le même `entity_id` deux fois dans une liste (une erreur de copier-coller fréquente) échoue avec `Duplicate entity_id '...' in dial_switches: already used for '...'`. Supprimez l'entrée en trop.

Les placeholders `*.disabled` sont exemptés des deux vérifications : `weather.disabled` et `sensor.disabled` restent valides quelle que soit la liste.

## Dépannage technique

- **Clé de chiffrement API invalide :** générez une clé API ESPHome valide et mettez-la dans `secrets.yaml`.
- **Entité introuvable ou indisponible :** vérifiez son identifiant exact et sa disponibilité dans Home Assistant ; une fonctionnalité configurée mais indisponible reste dans le menu mais ne peut pas se synchroniser.
- **Lumières manquantes :** assurez-vous que `dial_lights` contient au moins une entrée et n'est pas `[]`.
- **AQI vide :** utilisez un capteur numérique, pas un état textuel.
- **La musique est indisponible :** utilisez l'entité qui joue réellement l'audio et expose son état média.
- **La pochette n'apparaît jamais :** activez SendSpin sur la source musicale et pointez-la vers le Dial ; sans SendSpin, le logo Home Assistant reste affiché.
- **Les polices ou glyphes échouent à la compilation :** laissez la première compilation télécharger les Google Fonts et dépendances ; utilisez la version épinglée dans `requirements.txt`.
- **Erreur de compilation après une mise à jour :** validez l'intégralité du YAML local et rafraîchissez le package avant de réessayer.

## Personnalisation (développement uniquement)

Clonez le dépôt quand vous devez modifier le firmware lui-même. Les identifiants d'entités vont dans votre fichier `m5-dial.FR.yaml` / `m5-dial.EN.yaml` (dashboard). Pour compiler ce clone sans passer par GitHub, utilisez `m5-dial.local.yaml`. `src/pages/` contient les pages LVGL ; `src/main/` contient le matériel, la logique de mise en veille et les valeurs par défaut. Installez `requirements.txt`, copiez `secrets.example.yaml` vers un `secrets.yaml` local, et lancez `esphome config m5-dial.local.yaml` avant de compiler.
