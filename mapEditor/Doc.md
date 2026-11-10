# R-Type Map Editor : Documentation

## 1. Présentation

### 1.1 Qu'est-ce que c'est ?

Le **Map Editor** est un éditeur de niveaux graphique pour le jeu R-Type. Il permet de **construire un niveau visuellement** puis de l'enregistrer dans un fichier **JSON**, que le jeu lit ensuite pour faire apparaître les ennemis.

### 1.2 À quoi sert-il ?

Il répond au besoin du Track #3 du sujet : fournir aux utilisateurs un outil pour créer du contenu **sans écrire de C++** et **sans recompiler** le jeu.

Concrètement, il évite d'éditer à la main un fichier JSON : on place des vagues, on y glisse des monstres, on règle des valeurs, on sauvegarde.

### 1.3 Ce qu'on peut faire avec

| Action | Détail |
|---|---|
| Créer ou ouvrir un niveau | menu d'accueil : **New** / **Load** |
| Nommer le niveau | champ de nom dans le header |
| Créer des vagues | bouton **New wave** (placées après la dernière, jamais dans la zone de départ) |
| Ajouter des monstres | glisser-déposer d'un script `.lua` de la palette vers une vague (déposer deux fois augmente le `count`) |
| Régler une vague | nom, début (`Start`), longueur (`Length`), nombre de chaque monstre |
| Supprimer | monstre d'une vague (bouton **X**) ou vague entière (**Delete wave**) |
| Naviguer | défilement horizontal à la molette, règle graduée tous les 500 unités |
| Sauvegarder | **Save** / **Save as** (extension `.json` ajoutée automatiquement) |
| Ajouter un monstre au jeu | déposer un `.lua` dans `content/scripts/` puis **Refresh** |

L'interface est **responsive** : header, palette et viewport sont recalculés à chaque frame selon la taille de la fenêtre.

---

## 2. Principe de fonctionnement

```
 Game designer
      │  (souris : glisser-déposer, champs)
      ▼
 ┌─────────────┐   sauvegarde   ┌──────────────┐   lecture   ┌──────────┐
 │ Map Editor  │ ─────────────▶ │ niveau .json │ ──────────▶ │   Jeu    │
 │             │ ◀───────────── │              │             │          │
 └─────────────┘    chargement  └──────────────┘             └──────────┘
        │ scanne
        ▼
  content/scripts/*.lua  (un script = un type de monstre)
```

- Le **niveau** (JSON) décrit *quoi* apparaît et *où dans le niveau*.
- Le **script Lua** décrit *comment* est un monstre (apparence, vitesse, comportement).
- Les ennemis apparaissent **à droite de l'écran**, à une hauteur aléatoire, et leur position dans la vague est tirée au hasard dans `[start_x, start_x + length]`.

---

## 3. Le système JSON

### 3.1 Pourquoi du JSON ?

Format texte standard, lisible par un humain, modifiable sans recompiler, versionnable dans git et lisible par n'importe quel langage (donc par le jeu, le serveur et l'éditeur).

### 3.2 Structure d'un niveau

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
        { "script": "assets/scripts/enemy.lua", "count": 4 }
      ]
    }
  ],
  "boss": { "trigger_x": 6000, "script": "assets/scripts/boss.lua" },
  "end_condition": { "type": "boss_defeated", "preset": "dobkeratops" }
}
```

| Champ | Signification |
|---|---|
| `name`  | nom du niveau |
| `background`, `music`  | chemins des ressources |
| `waves[].name`  | nom de la vague |
| `waves[].start_x`  | début de la vague (unités du niveau) |
| `waves[].length`  | longueur de la vague (unités du niveau) |
| `waves[].spawns[].script`  | script Lua du monstre |
| `waves[].spawns[].count`  | nombre de monstres de ce type |
| `boss.trigger_x`  | position de déclenchement du boss |
| `end_condition.type`  | condition de victoire |

### 3.3 Règles

- Les valeurs sont en **unités du niveau**, pas en pixels (l'éditeur affiche avec `ZOOM = 0.15`).
- Une zone de départ (début du niveau) est **sans ennemi**.
- Le fichier est **tolérant** : une clé absente prend sa valeur par défaut.

---

## 4. Le parseur JSON (nlohmann/json)

Le **parsing** transforme le texte du fichier en structures C++ utilisables, et inversement à la sauvegarde.

### 4.1 Les structures

`spawn_t`, `wave_t`, `boss_t`, `endCondition_t`, `levelData_t` (fichier `jsonHandler.hpp`).

### 4.2 La conversion automatique

```cpp
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(wave_s, name, start_x, length, spawns)
```

Cette macro génère la conversion dans les deux sens. Le **nom d'un membre C++ est exactement la clé JSON**, et une clé absente garde la valeur par défaut. Ensuite :

```cpp
levelData_t level = data.get<levelData_t>();   // chargement
nlohmann::json out = level;                    // sauvegarde
file << out.dump(4);                           // écriture indentée
```

### 4.3 Gestion d'erreurs

Un JSON invalide ou un mauvais type lève une exception, attrapée pour renvoyer un niveau vide plutôt que de planter.

---

## 5. Technologies utilisées

| Technologie | Rôle |
|---|---|
| **SDL3** | fenêtre, rendu, entrées, dialogues de fichiers |
| **Dear ImGui** | interface (panneaux, boutons, champs, glisser-déposer) |
| **nlohmann/json** | lecture et écriture des niveaux |

### 5.1 Dear ImGui

Bibliothèque d'interface en **mode immédiat** : l'interface est redéclarée à chaque frame dans le code, un bouton est un simple `if (ImGui::Button("Save"))`. Pas d'objets widget à gérer, pas de synchronisation entre l'interface et les données : un champ modifie directement la variable. C'est le standard des outils et éditeurs de jeux.

### 5.2 nlohmann/json

Bibliothèque JSON **header-only** pour C++ moderne. Le JSON se manipule comme un conteneur (`j["waves"]`), et la conversion vers des structures est automatique grâce aux macros.

---

## 6. Comparatifs

**Échelle** : de 1 (faible) à 5 (très bien adapté **à notre projet**). Les critères sont ceux qui comptent ici : éditeur d'outils, équipe étudiante, petits fichiers, SDL3, intégration CMake.

### 6.1 Interface : Dear ImGui et alternatives

**Échelle** : de 1 (faible) à 5 (très bien adapté **à notre projet**).

| Critère (besoin du projet) | **Dear ImGui** | Qt Widgets | Nuklear | SDL maison (`ButtonList`) |
|---|:-:|:-:|:-:|:-:|
| Rapidité de mise en place | 5 | 2 | 3 | 2 |
| Backend SDL3 prêt | 5 | 2 | 3 | 5 |
| Widgets d'édition (champs, listes, drag & drop) | 5 | 5 | 3 | 1 |
| Intégration CMake / FetchContent | 4 | 2 | 4 | 5 |
| Légèreté (dépendance) | 5 | 1 | 5 | 5 |
| Documentation et communauté | 5 | 5 | 3 | 1 |
| Adapté aux outils internes | 5 | 4 | 3 | 2 |
| Apparence « jeu » finie | 2 | 4 | 3 | 4 |
| **Total (/40)** | **36** | 25 | 27 | 25 |

**Forces d'ImGui** : très rapide à écrire, lié directement aux données, léger, backend SDL3 officiel, énorme communauté.

**Faiblesses** : look « outil » peu personnalisable, interface redessinée à chaque frame, pas fait pour un menu de jeu final.

**Conclusion** : meilleur choix pour un éditeur ; `ButtonList` reste suffisant pour le menu d'accueil.

### 6.2 JSON : nlohmann/json et alternatives

| Critère (besoin du projet) | **nlohmann/json** | RapidJSON | simdjson | Boost.JSON |
|---|:-:|:-:|:-:|:-:|
| Facilité d'utilisation | 5 | 2 | 2 | 3 |
| Conversion JSON ↔ structures | 5 | 1 | 1 | 4 |
| Écriture (sauvegarde) | 5 | 3 | 1 | 4 |
| Intégration CMake (header-only) | 5 | 4 | 3 | 2 |
| Gestion d'erreurs claire | 4 | 3 | 3 | 4 |
| Documentation et communauté | 5 | 4 | 3 | 3 |
| Performance | 2 | 5 | 5 | 4 |
| **Total (/40)** | **36** | 27 | 23 | 29 |

**Forces de nlohmann** : syntaxe naturelle, macros de conversion (chargement et sauvegarde presque sans code), header-only, messages d'erreur précis, très répandu.
**Faiblesses** : compilation plus lente, parsing moins rapide que RapidJSON/simdjson, messages d'erreur de compilation très longs, ordre des clés alphabétique à l'écriture (sauf `ordered_json`), clés inconnues perdues avec la macro.
**Conclusion** : la performance n'a aucun poids ici (fichiers de quelques Ko lus de temps en temps), donc la facilité d'usage prime.

### 6.3 Format de données : JSON et alternatives

| Critère | **JSON** | Binaire |
|---|:-:|:-:|
| Lisible et éditable à la main | 4 | 1 |
| Support C++ (bibliothèques) | 5 | 3 |
| Structures imbriquées (vagues, spawns) | 5 | 5 |
| Compact | 3 | 5 |
| **Total (/25)** | **22** | 16 | 15 |

---

## 7. Architecture du code

| Fichier | Rôle |
|---|---|
| `HomeEditorMenu` | menu New / Load |
| `EditorScene` | toute l'édition (header, palette, viewport, propriétés) |
| `EditorUtils.hpp` | constantes (tailles, couleurs) |
| `JsonHandler.hpp` | structures et lecture/écriture JSON |

**Flux** : `HomeEditorMenu` écrit le chemin choisi dans `levelPath` (partagé par référence) → `EditorScene::onLoad()` charge le JSON puis scanne les scripts → chaque frame : `update()` puis `render()`.

**Sauvegarde asynchrone** : le dialogue SDL ne bloque pas. Son callback range le chemin et lève un drapeau atomique ; `update()` effectue la sauvegarde dans le thread principal.

---

## 8. Ajouter du contenu (game d esigner)

- **Créer un niveau** : New, ajouter des vagues, glisser des monstres, régler Start et Length, Save as.
- **Ajouter un monstre** : déposer un `.lua` dans `content/scripts/`, cliquer **Refresh**. Aucune recompilation.

