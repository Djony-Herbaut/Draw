Ce dépôt contient le code de notre projet Draw++, un langage de programmation ayant pour but de faciliter les dessins en utilisant turtle (bibliothèque Python) comme base pour la grammaire de notre langage.

## Instructions élémentaires ##

**Créer un curseur :** drawcreate_cursor()

**Positionner un curseur :** drawsetpos(x, y), drawgo(x, y), drawsetx(x), drawsety(y)

**Rendre le curseur visible/invisible :** drawshowcursor(), drawhidecursor()

**Couleur du curseur :** drawcursorcolor(colorname)

**Épaisseur du trait :** drawpensize(width)

**Faire avancer le curseur :** drawmoveforward(distance), drawmovebackward(distance)

**Faire pivoter le curseur :** drawpivotleft(angle), drawpivotright(angle)

**Dessiner un segment :** (automatique via forward, goto avec stylo abaissé : **drawpendown()** )

**Dessiner un carré :** (composé avec boucle for, ou avec des fonctions)

**Dessiner un cercle :** drawcircle(radius)

**Dessiner un point :** drawdot(size, color)

**Dessiner un arc :** drawarc(radius, extent)

**Animer un dessin :** drawspeed(speed), drawtracer(n, delay), drawupdate()

**Lever ou abaisser le stylo :** drawpenup(), drawpendown()

**Changer la forme du curseur :** drawshape(shape)

**Effacer l’écran :** drawclearsreen()

## Instructions évoluées ##

**Assignation de valeurs :** drawv_”nom variable” = valeur

**Instructions de bloc :** Définir des fonctions avec drawf nom_fonction() {  }

**Instructions conditionnelles :** d_if condition {, d_else {

**Boucles répétitives :** d_for variable in iterable {, d_while 
condition{

**Boucles imbriquées :** Combinaison de boucles for et while

**Gestion du temps :** drawtsleep(seconds)

##  Description du Fonctionnement de la Fonction tokenize dans lexer.C ##

**Initialisation :** Initialise le compteur de tokens, le pointeur sur le code source, le numéro de ligne et le numéro de colonne.

**Boucle principale :** Continue tant qu'il reste des caractères et qu'on n'a pas atteint la limite de tokens.

**Ignorer les espaces et commentaires :** Appelle skip_whitespace_and_comments.

**Gestion des nombres :** Si le caractère courant est un chiffre, extrait le nombre complet, crée un token TOKEN_NBR et stocke la valeur dans lexeme.

**Gestion des identifiants/variables et mots clés :** Si le caractère courant est une lettre ou un _, extrait l'identifiant complet, vérifie s'il s'agit d'un mot-clé avec is_keyword. Si c'est un mot clé, le type du token est le type du mot clé, sinon le type du token est TOKEN_VARIABLE.

**Gestion des symboles :** Si le caractère courant est un symbole, crée un token avec le type de symbole correspondant.

**Gestion des erreurs :** Si aucun des cas précédents ne correspond, signale une erreur lexicale.

**Gestion du token EOF :** Ajoute un token de fin de fichier à la fin.

**Retourne le nombre de tokens :** Retourne le nombre total de tokens générés.

...

# Draw++

Draw++ est une interface graphique interactive permettant d'interpréter des commandes de dessin, de valider leur syntaxe et de générer des graphiques sur un canvas externe basé sur Tkinter.

---

## Fonctionnalités principales

### Éditeur de texte
- Permet à l'utilisateur de saisir des commandes de dessin dans un format structuré.
- Exemples de commandes supportées :
  ```
  drawcircle 50
  drawgo 100 200
  ```

### Menu interactif
- **Options disponibles :**
  - **Nouveau** : Efface l'éditeur.
  - **Ouvrir** : Charge un fichier contenant des commandes.
  - **Sauvegarder** : Enregistre le contenu de l'éditeur.
  - **Exécuter Lexer** : Analyse le contenu pour générer une liste de tokens.
  - **Exécuter Parser** : Valide la syntaxe des tokens et exécute le dessin si valide.

### Console de logs
- Affiche les messages d'erreur, de débogage et les étapes du processus (lexage, parsing, exécution).

### Canvas Tkinter
- Interprète les commandes validées et génère des graphiques interactifs (cercles, lignes, points, etc.).

---

## Structure de l'IDE

### Composants de l'interface graphique

#### Fenêtre principale
- Créée à l'aide de **GTK+**.
- Comprend :
  - Un éditeur de texte.
  - Une console pour les logs.
  - Une barre de menu.

#### Barre de menu
- Créée dans la fonction `create_menu`.
- Les options du menu sont connectées à des callbacks définis dans `ide.c`.

#### Éditeur de texte
- Utilise un `GtkTextView` pour permettre à l'utilisateur d'éditer des commandes.
- Son contenu est analysé lors de l'exécution du lexer.

#### Console de logs
- Utilise un autre `GtkTextView` pour afficher les messages du programme.
- Les messages sont ajoutés via la fonction `log_to_console`.

#### Canvas Tkinter
- Lancé dans un thread séparé via la fonction `run_tk_canvas`.
- Gère l'exécution des commandes validées.

---

## Fonctionnement de l'IDE

### 1. Exécution du Lexer

- **Action :** L'utilisateur clique sur "Exécuter Lexer" dans le menu.
- **Processus :**
  - Le texte de l'éditeur est récupéré et analysé via la fonction `tokenize`.
  - Les tokens générés sont stockés dans `global_tokens`.
  - Les tokens sont affichés dans la console pour débogage.

### 2. Exécution du Parser

- **Action :** L'utilisateur clique sur "Exécuter Parser" dans le menu.
- **Processus :**
  - La liste des tokens (`global_tokens`) est analysée via `parse_program` (définie dans `parser.h`).
  - Si la syntaxe est valide, un thread est créé pour lancer le canvas.

### 3. Lancement du Canvas

- **Action :** L'IDE passe les tokens au canvas via `run_tk_canvas`.
- **Processus :**
  - Chaque token est interprété par `execute_token`.
  - Les commandes graphiques correspondantes sont générées et exécutées via **Tkinter**.

---

## Dépendances externes

L'IDE utilise plusieurs bibliothèques externes :
- **GTK+** : Pour créer l'interface graphique.
- **Python 3** : Pour gérer le canvas Tkinter.
- **Pthreads** : Pour exécuter le canvas dans un thread séparé.

---


## Commande de compilation

---
1. Install the required libraries and tools:
   ```bash
   sudo apt update
   sudo apt install libgtk-3-dev gcc pkg-config python3-tk
   ```

Utilisez la commande suivante pour compiler l'IDE :
```bash
gcc -o ide ide.c read_tokens.c lexer.c token.c parser.c \
    `pkg-config --cflags --libs gtk+-3.0` -I/usr/include/python3.12 -lpython3.12 -lpthread -Wall -Wextra -g
```

---

## Exemple d'utilisation

1. **Saisissez le contenu suivant dans l'éditeur :**
   ```
   drawcircle(50);
   ```

2. **Cliquez sur "Exécuter Lexer" :**
   - La console affiche les tokens générés :
     ```
  Token 0 : Type = 13, Lexeme = "drawcircle"
  Token 1 : Type = 28, Lexeme = "("
  Token 2 : Type = 52, Lexeme = "50"
  Token 3 : Type = 29, Lexeme = ")"
  Token 4 : Type = 35, Lexeme = ";"
  Token 5 : Type = 58, Lexeme = "EOF"
     ```

3. **Cliquez sur "Exécuter Parser" :**
   - Si valide, la console indique :
     ```
     Syntaxe valide : lancement du dessin...
     ```
   - Un canvas s'ouvre et affiche les formes demandées.

---

## Résumé des fonctions principales dans `ide.c`

| Fonction         | Description                                                                 |
|------------------|-----------------------------------------------------------------------------|
| `create_menu`    | Crée la barre de menu et initialise ses options.                            |
| `setup_editor`   | Initialise l'éditeur de texte et son buffer.                                |
| `setup_console`  | Initialise la console de logs.                                              |
| `log_to_console` | Affiche les erreurs renvoyés par les divers composants dans la console.     |
| `on_run_lexer`   | Exécute le lexer, génère les tokens et les affiche dans la console.         |
| `on_run_parser`  | Valide les tokens via `parse_program` et lance le canvas si valide.         |
| `run_tk_canvas`  | Interprète les tokens et exécute les commandes graphiques dans Tkinter.     |

---

## Fichiers utilisés dans l'IDE

- **`read_tokens.c`** : Fournit des utilitaires pour manipuler les tokens.
- **`lexer.c`** : Contient la fonction `tokenize` pour générer des tokens.
- **`parser.c`** : Définit `parse_program` et d'autres outils pour analyser la syntaxe.
- **`token.c`** : Définit les types de tokens et leur correspondance avec les commandes.

---

## Prochaines étapes

- Ajout de nouvelles commandes graphiques.
- Validation syntaxique avancée (conditions, boucles, etc.).
- Personnalisation du canvas (couleurs, épaisseurs de traits, etc.).



## READ_TOKENS

Le fichier "read_tokens.c" permet de récupérer dans un fichier texte les tokens des fonctions écrites par l'utilisateur dans l'IDE ainsi que ses paramètres. Ce fichier texte se trouve ici : /output/tokens.txt

1. Compiler le fichier 
   gcc -o read_tokens read_tokens.c -I/usr/include/python3.12 -lpython3.12

2. Le lancer
   ./read_tokens

