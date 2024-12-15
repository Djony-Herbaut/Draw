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
