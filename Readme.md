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

# IDE Draw++

## Description
**IDE Draw++** is a simple text-based Integrated Development Environment (IDE) built using **GTK+ 3**. It allows users to:

- Create, open, and save text files.
- Display logs and console output.
- Visualize graphical content in a dedicated drawing area.

This project demonstrates basic usage of GTK for creating graphical user interfaces and integrates features like a text editor, file handling, and a drawing area.

---

## Features
- **Text Editor**: Edit and manage text files with basic functionalities.
- **Console/Logs**: View logs, errors, and outputs.
- **Drawing Area**: Dedicated space for graphical content or visual outputs.
- **Menu Bar**: Easy access to file operations such as "New", "Open", and "Save".

---

## Prerequisites
To compile and run the project, you need the following libraries and tools:

- **GTK+ 3** (development files)
- **gcc** (GNU Compiler Collection)
- **pkg-config** (to manage GTK dependencies)

---

## Installation on Linux

1. Install the required libraries and tools:
   ```bash
   sudo apt update
   sudo apt install libgtk-3-dev gcc pkg-config
   ```

2. Clone the project repository:
   ```bash
   git clone <repository-url>
   cd <project-folder>
   ```

3. Compile the project:
   ```bash
    gcc -o ide ide.c lexer.c token.c `pkg-config --cflags --libs gtk+-3.0`
   ```

4. Run the executable:
   ```bash
   ./ide
   ```

---

## Usage
- **New File**: Clear the editor to start a new file.
- **Open File**: Load a text file into the editor.
- **Save File**: Save the content of the editor to a file.
- **Console/Logs**: Monitor logs and messages.
- **Drawing Area**: Visualize graphical content.

---
