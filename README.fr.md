<div align="center">

<img src="src/assets/images/logo.png" width="96" alt="Logo Home Assistant Controller for M5Stack Dial" />

# Home Assistant Controller for M5Stack Dial

### Un contrôleur Home Assistant circulaire pour M5Stack Dial, basé sur ESPHome et LVGL.

**Maintenu par [Glooob Domo](https://github.com/Glooob-domo)** — firmware, interface et intégrations Home Assistant sur mesure pour le M5Stack Dial.

**Langues :** [English](README.md) · Français (cette page)

[![ESPHome](https://img.shields.io/badge/ESPHome-2026.7+-blue?style=flat-square&logo=esphome)](https://esphome.io/)
[![Platform](https://img.shields.io/badge/Platform-ESP32--S3-red?style=flat-square&logo=espressif)](https://www.espressif.com/)
[![Display](https://img.shields.io/badge/Display-GC9A01A%20240x240-purple?style=flat-square)](#matériel)
[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)](#crédits-et-licence)

</div>

Ce firmware transforme un M5Stack Dial en contrôleur physique pour Home Assistant. La molette, l'écran tactile et le bouton avant rendent accessibles les actions du quotidien — consulter une pièce, changer une lumière, ajuster le chauffage ou contrôler la musique — sans rouvrir sans cesse un tableau de bord sur le téléphone.

**Glooob Domo** développe et publie cette version : listes d'entités configurables, statuts en direct dans le menu, températures par pièce, contrôle du portail/garage, réglage fin de la molette, interface localisée, pochettes d'album via SendSpin, et les pages documentées ci-dessous. Ce n'est pas un projet officiel de M5Stack, ESPHome ou Home Assistant.

## Origines et crédits

Ce firmware suit une chaîne de forks communautaires. Chaque étape a conservé la licence MIT et s'est appuyée sur le travail précédent.

| Étape | Auteur | Contribution |
| --- | --- | --- |
| 1 | [Jason Wen](https://github.com/Jasionf) | Projet original [**Smart Home Button**](https://github.com/Jasionf/smart-home-button) : base matérielle M5Stack Dial, fondations ESPHome/LVGL, et les premières pages Horloge, Lumière, Chauffage, Musique et Minuteur. |
| 2 | [hectorzin](https://github.com/hectorzin) | Dérivé Home Assistant Controller : listes d'entités configurables `dial_*`, pages de menu optionnelles, sous-titres de menu en direct, AQI sur l'Horloge, navigation améliorée, gestion de la mise en veille de l'écran et pochettes d'album SendSpin. [Article](https://hectorzin.com/en/posts/m5stack-dial-home-assistant-esphome-controller) · [Vidéo](https://www.youtube.com/watch?v=EskhrfUTLOM) |
| 3 | **[Glooob Domo](https://github.com/Glooob-domo)** | Ce dépôt : températures par pièce, page garage/portail, réglage de la molette au pas unique, format de date localisé, anneau des secondes sur l'Horloge, améliorations UI des scènes et du garage, alarme locale, template de config francophone en priorité, et maintenance continue. |

Merci à **Jason Wen** et **hectorzin** d'avoir créé et partagé les fondations sur lesquelles ce projet s'appuie.

## Fonctionnalités

- Horloge avec anneau de progression des secondes, format de date localisé et météo depuis Home Assistant.
- Indice de qualité de l'air (AQI) depuis un capteur Home Assistant.
- Navigation circulaire au menu via la molette, les gestes tactiles et le bouton avant.
- Lumières Home Assistant configurables via `dial_lights`.
- Contrôle du chauffage via `dial_climates`.
- Contrôle des volets via `dial_covers`.
- Contrôle du garage / portail via `dial_garages` (ouvrir, fermer, stop — sans pourcentage de position).
- Contrôle des prises / interrupteurs via `dial_switches`.
- Activation de scènes et scripts via `dial_scenes`.
- Températures par pièce via `dial_temperatures`.
- Lecture média, volume et métadonnées via `dial_media_players`.
- Une alarme quotidienne locale et persistante — réglée et armée entièrement sur le Dial, sans entité Home Assistant requise.
- Sous-titres de menu basés sur les états Home Assistant en direct.
- Retour automatique à l'horloge après inactivité.
- Assombrissement du rétroéclairage et extinction d'écran configurables.
- Réveil depuis l'état assombri ou éteint sans quitter la page en cours.
- Notification visuelle et sonore quand l'alarme sonne.

## Galerie

| | | | |
| --- | --- | --- | --- |
| <img src="docs/images/gallery/clock-weather-page.jpg" alt="Écran horloge et météo" width="240"> | <img src="docs/images/gallery/menu-page.jpg" alt="Menu circulaire" width="240"> | <img src="docs/images/gallery/light-page.jpg" alt="Contrôle lumière" width="240"> | <img src="docs/images/gallery/ac-page.jpg" alt="Contrôle chauffage" width="240"> |
| <img src="docs/images/gallery/music-page.jpg" alt="Écran lecteur média" width="240"> | | | |

## Pages

| Page | Description |
| --- | --- |
| Horloge | Affiche l'heure, un anneau des secondes, la date localisée, la météo et l'AQI depuis Home Assistant. |
| Menu | Navigation circulaire avec sous-titres en direct pour les fonctionnalités Home Assistant configurées. |
| Lumières | Contrôle les entités lumière Home Assistant déclarées dans `dial_lights`. Une seule lumière ouvre directement la page ; plusieurs ouvrent d'abord un sélecteur. |
| Volets | Contrôle les entités volet Home Assistant déclarées dans `dial_covers`. Même règle que les lumières. |
| Garage | Contrôle les volets de type garage / portail déclarés dans `dial_garages`. Ouvrir, fermer et stop uniquement — pas d'arc de position. |
| Prises | Bascule les entités `switch` ou `input_boolean` déclarées dans `dial_switches`. |
| Scènes | Exécute les entités `scene` ou `script` déclarées dans `dial_scenes`. |
| Pièces | Affiche les températures des entités `sensor` ou `climate` déclarées dans `dial_temperatures`. La molette fait défiler les pièces. |
| Clim | Contrôle les entités climate Home Assistant déclarées dans `dial_climates`. Même règle que les lumières. |
| Musique | Contrôle les lecteurs média Home Assistant déclarés dans `dial_media_players`. Même règle que les lumières. |
| Alarme | Une alarme quotidienne locale — réglez l'heure et armez-la directement sur le Dial. Toujours disponible ; aucune configuration nécessaire. |

## Matériel

Le firmware cible la plateforme M5Stack Dial et son contrôleur ESP32-S3. La configuration utilise l'écran rond 240 × 240 GC9A01A du Dial, le contrôleur tactile capacitif FT5x06, la molette rotative, le bouton avant, l'horloge temps réel PCF8563, le buzzer et le rétroéclairage.

Le M5Stack Dial V1.1 est la cible testée. Le maintien d'alimentation GPIO46 est configuré pour la V1.1 afin que l'appareil reste alimenté sur batterie. D'autres révisions peuvent fonctionner, mais ne sont pas actuellement vérifiées par Glooob Domo.

## Prérequis

- Un M5Stack Dial compatible.
- Home Assistant.
- ESPHome.
- Un accès Wi-Fi pour le Dial.
- Des entités Home Assistant uniquement pour les fonctionnalités que vous souhaitez activer.

Météo et AQI sont optionnels : laissez-les en `weather.disabled` / `sensor.disabled` (ou pointez vers des entités indisponibles) et l'Horloge affiche `--`. Clim, Musique, Lumières et Volets sont également optionnels ; ces entrées de menu disparaissent quand la ligne correspondante dans `m5-dial.FR.yaml` / `m5-dial.EN.yaml` reste `*.disabled`, ou, pour les pages à liste, quand la liste est vide. L'Alarme ne nécessite aucune configuration — elle est toujours dans le menu et se règle entièrement sur le Dial.

## Installation rapide

Vous ajoutez seulement **un fichier** à ESPHome. ESPHome télécharge le firmware depuis GitHub ; vous ne copiez pas le reste de ce dépôt.

1. Dans le dashboard ESPHome, créez un appareil et collez [`m5-dial.FR.yaml`](m5-dial.FR.yaml) (commentaires en français) — ou [`m5-dial.EN.yaml`](m5-dial.EN.yaml) pour la version en anglais.
2. Mettez le Wi-Fi, `api_encryption_key` et `ota_password` dans `secrets.yaml`.
3. Dans ce même YAML, réglez `ui_language`, remplacez `weather_entity` / `aqi_entity` si vous voulez ces champs, et décommentez les blocs `dial_*` dont vous avez besoin. Omettez une liste entièrement pour masquer cette page du menu (inutile d'ajouter des entrées `dial_*: []` vides).
4. Installez en USB la première fois, puis utilisez l'OTA.

Exemple des champs à modifier :

```yaml
substitutions:
  timezone: Europe/Paris
  ui_language: fr
  encoder_resolution: "1"
  weather_entity: weather.maison
  aqi_entity: sensor.aqi_salon

# --- Lumière / Light ---
dial_lights:
  - entity_id: light.salon
    name: Salon

# --- Chauffage / Climate ---
dial_climates:
  - entity_id: climate.salon
    name: Salon

# --- Media Player ---
dial_media_players:
  - entity_id: media_player.salon
    name: Salon

# Pages optionnelles : ajoutez seulement les listes dont vous avez besoin.
# Sans dial_covers, dial_garages, etc., la page reste masquée.

packages:
  m5_dial:
    url: https://github.com/Glooob-domo/m5stack-dial-home-assistant
    ref: main
    files:
      - dial.yaml
    refresh: 0s
```

Changer un identifiant d'entité nécessite une recompilation (ou un OTA). `ref: main` suit la branche publiée ; épinglez un tag ou un commit pour une version figée.

Pour tous les champs, leurs valeurs par défaut et les cas avancés, voir [la référence de configuration](docs/configuration.fr.md).

## Navigation

Le Dial gère la molette rotative, le bouton avant et les gestes tactiles horizontaux. En général, un appui court ouvre ou valide, un double appui rapide fait Retour, et un appui long n'a pas d'action. Les éléments tactiles conservent leurs actions propres à chaque page.

| Contexte | Rotation | Appui court | Double appui / Tactile |
| --- | --- | --- | --- |
| Horloge (Accueil) | Aucune action | Ouvre le Menu | Appui long : aucune action. Balayer à gauche ou à droite ouvre le Menu. |
| Menu | Déplace la sélection circulaire | Ouvre la page sélectionnée ; Accueil retourne à l'Horloge | Toucher un élément visible du menu l'ouvre. Balayer à gauche valide ; balayer à droite retourne à l'Horloge. |
| Lumières | Change la luminosité ou la valeur du sélecteur actif | Ouvre/valide la lumière sélectionnée, selon le contexte | Double appui ou balayage à droite = retour. Le tactile contrôle l'allumage, le sélecteur de couleur et sa validation. |
| Volets | Change la position | Bascule ouvert/fermé, ou stoppe si en mouvement | Double appui ou balayage à droite = retour. Le tactile envoie ouvrir, stop et fermer. |
| Garage | Aucune action | Stop si en mouvement, sinon aucune action | Double appui ou balayage à droite = retour. Le tactile envoie ouvrir, fermer et stop. |
| Prises | Aucune action | Bascule la prise | Double appui ou balayage à droite = retour. Toucher le contrôle central pour basculer. |
| Scènes | Aucune action | Active la scène ou le script | Double appui ou balayage à droite = retour. Toucher ACTIVER pour l'exécuter. |
| Pièces | Fait défiler les pièces | Retour | Double appui ou balayage à droite = retour. |
| Clim | Change la valeur sélectionnée | Valide ou confirme la modification en cours | Double appui ou balayage à droite = retour. Le tactile sélectionne les contrôles et bascule l'allumage, le mode ventilateur ou le mode HVAC. |
| Musique | Change le volume | Valide l'action en cours le cas échéant | Double appui ou balayage à droite = retour. Le tactile contrôle la lecture et le transport. |
| Alarme | Ajuste l'heure ou la minute en cours d'édition | Passe de heure → minute → confirmé pendant l'édition ; arrête la sonnerie si elle sonne | Double appui ou balayage à droite = retour. Toucher ON/OFF arme ou désarme ; toucher n'importe où arrête la sonnerie. |

Le premier mouvement de molette, appui bouton ou geste tactile après l'assombrissement ou l'extinction de l'écran ne fait que le réveiller ; répétez l'action pour contrôler l'interface.

```yaml
substitutions:
  encoder_resolution: "1"
```

`encoder_resolution` vaut `1`, `2` ou `4`. La valeur par défaut `1` correspond à un pas d'interface par cran mécanique. Utilisez `4` pour une molette plus sensible.

Sur Lumières, Volets, Clim et Musique — les pages qui modifient la valeur d'une entité — chaque pas change la valeur de `light_brightness_step`, `cover_position_step`, `climate_temperature_step` ou `music_volume_step` (par défaut `1%`, `5%`, `1°C`, `10%`). Voir [la référence de configuration](docs/configuration.fr.md) pour les détails.

## Gestion de l'écran

Le package propose quatre substitutions pour le comportement en cas d'inactivité :

```yaml
substitutions:
  screen_dim_timeout: 45s
  screen_return_timeout: 5min
  screen_off_timeout: 30min
  screen_dim_brightness: "20%"
```

`DIM` baisse le rétroéclairage et conserve la page en cours. `RETURN` navigue volontairement vers l'Horloge. `OFF` éteint uniquement le rétroéclairage et conserve la page en cours. Ce sont des étapes indépendantes. Réglez un délai à `0s` pour désactiver cette étape. Quand `OFF` est activé, son délai effectif est relevé si besoin pour ne jamais être antérieur à celui de DIM ou RETURN s'ils sont actifs.

Une alarme qui sonne bloque uniquement le retour automatique à l'horloge ; DIM et OFF continuent de fonctionner malgré tout. Quand l'alarme se déclenche, le Dial se réveille, ouvre la page Alarme et joue son effet clignotant + sonnerie jusqu'à ce qu'elle soit arrêtée.

## Statut du menu en direct

Le Menu est plus qu'un simple lanceur : sa sélection en cours affiche un sous-titre en direct. Alarme affiche l'heure réglée ou `OFF` ; Lumières affiche la luminosité d'une seule lumière ou le nombre de lumières allumées ; Volets et Garage affichent ouvert/fermé ou le nombre ouvert ; Prises affiche allumé/éteint ; Scènes affiche le nom ou le nombre configuré ; Pièces affiche la température ou le nombre de pièces configurées ; Clim affiche le mode HVAC et la température cible ; Musique affiche le titre ou l'état de lecture ; et Accueil affiche `Horloge`.

## Notes par fonctionnalité

### Horloge

L'arc extérieur suit les **secondes** et se rafraîchit toutes les 5 s. La ligne de date suit `ui_language` : l'anglais utilise mois/jour (`Mon  09/02`) ; le français, l'espagnol, l'allemand et l'italien utilisent jour/mois (`Mar  02/09`).

Un petit point rouge apparaît au-dessus de l'heure uniquement quand le Dial a perdu sa connexion à Home Assistant (vérifié toutes les 5 s), pour qu'une coupure Wi-Fi ou un redémarrage de Home Assistant ne ressemble pas à un appareil figé ou cassé. Il disparaît dès que la connexion revient.

### Musique

La page Musique est contrôlée via `dial_media_players`. Home Assistant fournit l'état de lecture, les actions play/pause et transport, le volume, le titre et les métadonnées, ainsi que la durée et la position quand disponibles.

Tant qu'une source **SendSpin** (optionnelle, par exemple Music Assistant avec SendSpin activé) n'a pas poussé au moins une pochette d'album, la page utilise une mise en page plus grande sans illustration : un grand cadran de volume à la place de la pochette, un texte titre/artiste plus grand et des boutons de transport plus grands. Dès qu'une vraie pochette arrive, la page bascule — pour le reste de ce démarrage — vers la mise en page compacte avec une pochette 100 × 100 (remplaçant le logo Home Assistant par un fondu) aux côtés des petites barres de volume. La lecture et les métadonnées fonctionnent à l'identique dans les deux cas ; SendSpin ne change que l'espace utilisé par la pochette sur la page.

### Chauffage

Les contrôles de chauffage disponibles dépendent de l'entité sélectionnée. La page lit et modifie la température cible, et utilise les modes HVAC et ventilateur annoncés par l'entité quand disponibles. Ne vous attendez pas à un contrôle que l'intégration Home Assistant du chauffage sélectionné n'expose pas.

### Volets

La molette règle la position par pas de 5 % (`encoder_resolution: "1"` donne un pas par cran mécanique). Un appui court bascule ouvert/fermé, ou stoppe si le volet est déjà en mouvement. Les boutons tactiles envoient ouvrir, stop et fermer.

### Garage

Les entités garage et portail utilisent une page séparée des volets : boutons **ouvrir** et **fermer** au centre, **stop** en dessous. Pas d'arc ni de pourcentage de position. La molette n'a aucun effet sur cette page ; un appui court stoppe le volet pendant qu'il bouge.

### Scènes

Une seule scène ou script ouvre directement la page ; plusieurs ouvrent d'abord un sélecteur. Un grand bouton **ACTIVER** exécute l'entité ; un appui court ou un toucher fait la même chose.

### Alarme

Une alarme quotidienne unique, entièrement locale au Dial — aucune entité ni configuration Home Assistant impliquée. Appuyez une fois sur la page Alarme pour commencer à éditer l'heure (elle clignote) ; la molette l'ajuste. Appuyez à nouveau pour passer à la minute, puis une fois de plus pour confirmer et enregistrer. Touchez **ON**/**OFF** pour l'armer ou la désarmer. L'heure réglée et l'état armé survivent à un redémarrage. Une fois armée, elle sonne à nouveau à la même heure chaque jour jusqu'à être désarmée. Quand elle se déclenche, le Dial se réveille, ouvre l'Alarme et bipe toutes les quelques secondes jusqu'à ce que vous touchiez l'écran ou appuyiez sur le bouton.

### Batterie

Le package active le GPIO46 au démarrage pour le maintien d'alimentation batterie du M5Dial V1.1. Cela garde cette révision alimentée après réveil quand elle fonctionne sur batterie.

## Dépannage

- **L'appareil n'apparaît pas dans Home Assistant :** vérifiez le Wi-Fi, la connectivité API et une `api_encryption_key` valide.
- **Une entrée de menu est masquée :** ajoutez au moins une entrée à la liste correspondante (`dial_lights`, `dial_climates`, `dial_media_players`, `dial_covers`, `dial_garages`, `dial_switches`, `dial_scenes`, `dial_temperatures`). Les listes vides masquent cette page. L'Alarme n'est jamais masquée.
- **L'AQI affiche `--` :** utilisez un capteur numérique existant pour `aqi_entity`.
- **Une fonctionnalité est indisponible :** vérifiez que son entité configurée existe et est disponible dans Home Assistant.
- **La musique est indisponible :** utilisez l'entité qui joue réellement l'audio et expose son état média.
- **La pochette reste sur le logo Home Assistant :** SendSpin doit être activé sur la source musicale et connecté au Dial ; sans cela, seul le logo s'affiche.
- **L'alarme ne sonne pas :** vérifiez qu'elle est bien armée (**ON**) sur la page Alarme et que l'horloge du Dial est correcte.
- **Les changements du package n'apparaissent pas :** utilisez `refresh: 0s` pendant les tests, puis rechargez ou recompilez la configuration ESPHome.
- **Les polices, glyphes ou la compilation échouent :** assurez-vous que la première compilation puisse télécharger ses dépendances, et utilisez la version ESPHome indiquée dans `requirements.txt`.
- **La première installation échoue en réseau :** flashez d'abord en USB, puis utilisez les mises à jour OTA d'ESPHome.

## Structure du projet

```text
m5stack-dial-home-assistant/
├── m5-dial.FR.yaml            # Fichier appareil ESPHome unique, commentaires en français (package GitHub + entités)
├── m5-dial.EN.yaml            # Même fichier, commentaires en anglais
├── m5-dial.local.yaml         # Compile ce clone (non utilisé dans le dashboard ESPHome)
├── dial.yaml                 # Package du firmware (récupéré depuis GitHub)
├── secrets.example.yaml      # Exemple d'identifiants pour le développement local
├── requirements.txt          # Version d'ESPHome utilisée par ce projet
├── src/
│   ├── main/                 # Matériel, valeurs par défaut, logique de mise en veille
│   ├── pages/                # Pages LVGL pour l'horloge, le menu et les fonctionnalités
│   └── assets/               # Polices et images embarquées
├── components/               # Composants ESPHome locaux, dont SendSpin
├── docs/                     # Documentation de configuration et de maintenance
├── hardware/                 # Fichiers liés au matériel
├── LICENSE
└── THIRD_PARTY_NOTICES.md
```

## Développement et personnalisation

Cette section s'adresse à celles et ceux qui modifient le firmware. Créez un `secrets.yaml` local à partir de `secrets.example.yaml`, puis compilez **ce clone** (le dashboard ESPHome utilise `m5-dial.FR.yaml` / `m5-dial.EN.yaml` + GitHub à la place) :

```bash
python -m venv .venv
# Activez .venv (Scripts\Activate.ps1 sous Windows, bin/activate sous macOS/Linux)
python -m pip install -r requirements.txt
esphome config m5-dial.local.yaml
esphome compile m5-dial.local.yaml
```

La personnalisation des pages se trouve dans `src/pages/` ; le matériel et la logique de mise en veille dans `src/main/`. Gardez vos identifiants locaux hors de Git.

## Vidéo

Une vidéo de présentation et de démonstration pour **cette version Glooob Domo** arrive bientôt sur la chaîne YouTube **Glooob Domo**.

Pour une présentation antérieure du dérivé hectorzin (architecture et premières intégrations HA), voir :

- [hectorzin — article](https://hectorzin.com/en/posts/m5stack-dial-home-assistant-esphome-controller)
- [hectorzin — démo YouTube](https://www.youtube.com/watch?v=EskhrfUTLOM)

[![Regarder la démo M5Stack Dial de hectorzin](docs/images/m5stack-dial-home-assistant.webp)](https://www.youtube.com/watch?v=EskhrfUTLOM)

## Documentation

- [Référence de configuration](docs/configuration.fr.md) ([English](docs/configuration.md))
- [Structure du projet](docs/project-structure.md)
- [Licence](LICENSE)
- [Mentions tierces](THIRD_PARTY_NOTICES.md)

## Crédits et licence

Home Assistant Controller for M5Stack Dial se base sur le projet original [**Smart Home Button**](https://github.com/Jasionf/smart-home-button) de [Jason Wen](https://github.com/Jasionf), étendu ensuite par [hectorzin](https://github.com/hectorzin) en contrôleur Home Assistant configurable.

Cette version est développée et maintenue par **[Glooob Domo](https://github.com/Glooob-domo)**. Vous pouvez la forker, la modifier et la republier sous [licence MIT](LICENSE), à condition de conserver la notice de copyright originale et la licence. Voir [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) pour SendSpin, les polices et les autres composants tiers.
