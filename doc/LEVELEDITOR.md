# R-Type Level Editor : Documentation

## 1. Présentation

Le **Level Editor** est un outil graphique (SDL3 + Dear ImGui) qui permet de créer et modifier les niveaux du jeu **sans toucher au code C++**.

Un niveau est un fichier **JSON** décrivant des **vagues** d'ennemis, un **boss** et une **condition de fin**. Le comportement des monstres est défini par des scripts **Lua** déposés dans le dossier "".

---

## 2. Prérequis et compilation

Dépendances (récupérées par CMake via `FetchContent`) :

| Bibliothèque | Rôle |
|---|---|
| SDL3 | fenêtre, rendu, dialogues de fichiers |
| Dear ImGui (backends SDL3 / SDL_Renderer3) | interface |
| nlohmann/json | lecture et écriture des niveaux |

```bash
cmake -S . -B build
cmake --build build
./build/editor_test
```

> **Lancer depuis `mapEditor/`** : le dossier des scripts est cherché en relatif (`./content/scripts`).

---

## 3. Arborescence

```
mapEditor/
├── main.cpp
├── CMakeLists.txt
├── content/
│   ├── levels/          niveaux JSON
│   ├── scripts/         scripts Lua des monstres (palette)
│   └── sprites/
├── include/
│   ├── LevelEditor.hpp  fenêtre SDL, boucle principale
│   ├── HomeScene.hpp    menu d'accueil
│   ├── EditorScene.hpp  scène d'édition
│   ├── EditorUtils.hpp  constantes (tailles, couleurs)
│   └── jsonHandler.hpp  structures + lecture/écriture JSON
└── src/
    ├── LevelEditor.cpp
    ├── HomeScene.cpp
    └── EditorScene.cpp
```

---

## 4. Utilisation

### 4.1 Menu d'accueil

- **New** : crée un niveau vide et ouvre l'éditeur.
- **Load** : ouvre un dialogue système ; après le choix d'un `.json`, l'éditeur s'ouvre.

### 4.2 Interface de l'éditeur

```
┌─────────────────────────────────────────────────┐
│ Header : Back | Save | Save as | nom | (Unsaved)│
├──────────────────────────────────┬──────────────┤
│  Viewport (règle + vagues)       │  Palette     │
│                                  │  Monsters    │
├──────────────────────────────────┤              │
│  Wave properties (si sélection)  │              │
└──────────────────────────────────┴──────────────┘
```

Toutes les zones sont recalculées à chaque frame : l'interface s'adapte au redimensionnement de la fenêtre.

#### Header

| Élément | Action |
|---|---|
| **Back** | retour au menu (sans confirmation) |
| **Save** | enregistre dans le fichier courant, ou ouvre Save as si le niveau n'a jamais été enregistré |
| **Save as** | choix du fichier ; `.json` est toujours ajouté au nom saisi |
| **Champ de nom** | modifie le champ `name` du niveau |
| **(Unsaved)** | affiché s'il y a des modifications non enregistrées |

#### Palette Monsters (droite)

- **New wave** : crée une vague après la dernière (jamais dans la zone de départ).
- **Refresh** : rescanne le dossier des scripts.
- Un bouton par script `.lua`, à **glisser sur une vague** pour l'y ajouter. Déposer deux fois le même monstre augmente son `count`.

#### Viewport

- **Molette** : défilement horizontal.
- **Règle** : un repère tous les 500 unités.
- **Zone « Spawn »** grisée au début : aucune vague ne peut y commencer.
- **Vagues** : blocs colorés (nom + `monstre xN`). Clic = sélection (contour jaune), clic dans le vide = désélection.
- **Boss** : ligne rouge à `boss.trigger_x` (affichage seul).

#### Wave properties (en bas, si une vague est sélectionnée)

- **Name**, **Start** (minimum = zone de départ), **Length** (minimum 500).
- Liste des monstres : **count** modifiable (min. 1), bouton **X** pour retirer.
- **Delete wave**.

### 4.3 Raccourcis

Aucun raccourci clavier pour l'instant.

---

## 5. Format du fichier de niveau

```json
{
  "name": "Level 1",
  "background": "assets/backgrounds/space_01.png",
  "music": "assets/music/level1.ogg",
  "waves": [
    {
      "name": "Intro",
      "start_x": 1000,
      "length": 1000,
      "spawns": [
        { "preset": "", "sprite": "", "script": "content/scripts/snake.lua", "count": 4 }
      ]
    }
  ],
  "boss": {
    "trigger_x": 6000,
    "preset": "",
    "sprite": "",
    "script": "content/bosses/dobkeratops.lua"
  },
  "end_condition": { "type": "boss_defeated", "preset": "dobkeratops" }
}
```

| Champ | Type | Signification |
|---|---|---|
| `name` | string | nom du niveau |
| `background`, `music` | string | chemins des ressources (non éditables dans l'UI) |
| `waves[].name` | string | nom de la vague |
| `waves[].start_x` | int | début de la vague, en unités du niveau |
| `waves[].length` | int | longueur de la vague, en unités du niveau |
| `waves[].spawns[].script` | string | script Lua du monstre |
| `waves[].spawns[].count` | int | nombre de monstres de ce type dans la vague |
| `boss.trigger_x` | int | position où le boss se déclenche |
| `boss.script` | string | script Lua du boss |
| `end_condition.type` / `.preset` | string | condition de victoire (ex. `boss_defeated`) |

### Sémantique

- Les valeurs sont en **unités du niveau**, pas en pixels (conversion à l'affichage : `ZOOM = 0.15`).
- Pour chaque monstre d'une vague, le jeu tire une position dans `[start_x, start_x + length]`, le fait apparaître juste à droite de l'écran, à une hauteur aléatoire dans l'écran.
- `preset` et `sprite` sont conservés dans le fichier mais **plus utilisés** : l'apparence est gérée par le Lua.

---

## 6. Architecture du code

### 6.1 Classes

| Classe | Fichier | Rôle |
|---|---|---|
| `LevelEditor` | `LevelEditor.*` | crée la fenêtre SDL et le renderer, initialise ImGui, possède le `SceneManager` et le `levelPath` partagé, boucle principale |
| `HomeScene` | `HomeScene.*` | menu New / Load |
| `EditorScene` | `EditorScene.*` | toute l'édition |
| `jsonHandler` | `jsonHandler.hpp` | structures de données et conversion JSON |

### 6.2 Flux de données

1. `HomeScene` écrit le chemin choisi dans `levelPath` (partagé par référence avec `EditorScene`), puis `switchScene("Editor")`. **New** vide `levelPath`.
2. `EditorScene::onLoad()` charge le JSON si `levelPath` n'est pas vide, puis scanne les scripts.
3. À chaque frame : `update()` (traite une sauvegarde en attente), puis `render()`.

### 6.3 Structures (`jsonHandler.hpp`)

`spawn_t`, `wave_t`, `boss_t`, `endCondition_t`, `levelData_t`.

La macro `NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT` génère la conversion JSON dans les deux sens :

- le **nom d'un membre est exactement la clé JSON** ;
- une clé absente garde la valeur par défaut.

```cpp
levelData_t level = data.get<levelData_t>();   // chargement
nlohmann::json out = level;                    // sauvegarde
```

### 6.4 `EditorScene` en détail

| Fonction | Rôle |
|---|---|
| `updateLayout` | calcule header, palette, viewport à partir de la taille de la fenêtre |
| `DrawHeader` / `DrawMonsterPalette` / `DrawProperties` | panneaux ImGui à position et taille fixes |
| `DrawViewport` | orchestre navigation, dessin, clics |
| `handleNavigation` | défilement à la molette |
| `drawGrid` | fond, zone de départ, règle |
| `drawWaves` | dessine chaque vague, la rend cliquable et cible de dépôt |
| `drawBoss` | ligne du boss |
| `toScreen` | unités du niveau → pixels : `_vpMin.x + (x - _camX) * ZOOM` |
| `addWave` / `addSpawn` | modifications du niveau |
| `Save` / `SaveAs` / `OnSaveSelected` | sauvegarde |
| `LoadScript` | liste les `.lua` |

### 6.5 Principes ImGui utilisés

- **Mode immédiat** : l'interface est redéclarée à chaque frame ; un bouton est un simple `if`.
- **Fenêtres figées** (`FIXED_WINDOW`) repositionnées à chaque frame, d'où le responsive.
- **Viewport** dessiné avec la draw list d'arrière-plan, puis une fenêtre transparente par-dessus avec des `InvisibleButton` pour capter clics et dépôts.
- **Glisser-déposer** : source dans la palette (payload `MONSTER_SCRIPT` = chemin du script), cible sur chaque vague.

### 6.6 Sauvegarde asynchrone

Le dialogue SDL est non bloquant :

1. `SaveAs()` ouvre le dialogue.
2. Le callback `OnSaveSelected` range le chemin dans `_pendingSavePath`, **puis** lève `_hasPendingSave` (atomique).
3. `update()` voit le drapeau, ajoute `.json` et appelle `Save()` dans le thread principal.

Le callback ne touche jamais à l'interface ni aux données du niveau.

### 6.7 Constantes

Toutes les valeurs fixes (tailles, couleurs, pas, limites) sont dans `EditorUtils.hpp`.

---

## 7. Ajouter du contenu (game designer)

**Créer un niveau** : New, ajouter des vagues (New wave), glisser des monstres, régler Start et Length, Save as.

**Ajouter un monstre** : déposer un `.lua` dans `content/scripts/`, cliquer **Refresh** : il apparaît dans la palette. Aucune recompilation.

---

## 8. Limites connues et corrections à faire

1. **`LoadScript`** plante si `./content/scripts` n'existe pas, et pousse un `fs::path` dans un `vector<string>` : utiliser `std::error_code` et `.generic_string()`.
2. **`EditorUtils.hpp`** définit `PAYLOAD_MONSTER` (`const char *`) et des tableaux dans un header inclus par plusieurs fichiers : utiliser `inline constexpr` pour éviter les définitions multiples.
3. **Membres inutilisés** dans `EditorScene.hpp` : `_zoom` (remplacé par `ZOOM`) et `_saveMutex`.
4. **`onLoad`** ne réinitialise pas `_levelData`, `_selectedWave`, `_modified` : après Back puis New, l'ancien niveau reste. Ajouter la remise à zéro et appliquer `MIN_WAVE_LENGTH` aux vagues chargées.
5. **`jsonHandler.hpp`** : retirer `#include <asio/serial_port_base.hpp>`, attraper `parse_error` dans `Load`, supprimer ou implémenter `SaveLevel` (vide), retirer `preset` / `sprite` obsolètes.
6. **`SaveAs`** : déclarer `filters` en `static const` (le tableau doit survivre au dialogue). `SAFE_ZONE_SIZE` vaut 1000 dans le `.hpp`.
7. Pas de **confirmation avant Back**, pas d'**undo/redo**, pas de **raccourcis**, `_statusMessage` jamais affiché.
8. **Non éditable dans l'UI** : boss, `end_condition`, `background`, `music`. Un niveau créé avec New n'a donc pas de boss.
9. Pas de **contrôle de chevauchement** entre vagues, pas de **vérification** que les scripts référencés existent au chargement.
10. **`_buttons`** (ancien `ButtonList`) est encore rendu dans `render` sans utilité.

---

## 9. Pistes d'évolution

- Édition du boss et de la condition de fin.
- Déplacement et redimensionnement des vagues à la souris.
- Validation (chevauchements, scripts manquants, boss absent).
- Annuler / rétablir, raccourcis Ctrl+S.
- Aperçu des sprites.
- Documentation des scripts Lua (format de la table `monster`, fonctions `on_spawn` / `update`).