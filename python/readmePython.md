Le fichier "python test en temps réel" permet d'utiliser les commandes en temps réel (comme son nom l'indique) et le fichier "python le fichier officiel" c'est la liste des fonctions de manière plus statique


Liste des Commandes Reconnaissables :


Commande	           | Description	                                       | Exemple
-----------------------|-------------------------------------------------------|-------------------
quit	               | Quitte le programme	                               | quit
pos <x> <y>	           | Positionne le curseur (dessine si stylo abaissé)	   | pos 200 300
go <x> <y>	           | Déplace le curseur sans dessiner	                   | go 100 200
setx <x>	           | Déplace le curseur sur l'axe X uniquement	           | setx 400
sety <y>	           | Déplace le curseur sur l'axe Y uniquement	           | sety 300
forward <distance>	   | Avance le curseur de la distance donnée	           | forward 50
backward <distance>	   | Recule le curseur de la distance donnée	           | backward 50
upward (distance)      | Monte le curseur de la distance donnée                | upward 50
downward (distance)    | Descend le curseur de la distance donnée              | downward 50
showcursor	           | Affiche le curseur	                                   | showcursor
hidecursor	           | Masque le curseur	                                   | hidecursor
cursorcolor <color>	   | Change la couleur du curseur et du trait	           | cursorcolor red
penup	               | Lève le stylo	                                       | penup
pendown	               | Abaisse le stylo	                                   | pendown
pensize <width>	       | Change l'épaisseur du trait	                       | pensize 5
circle <radius>	       | Dessine un cercle	                                   | circle 50
dot <size> <color>	   | Dessine un point	                                   | dot 20 blue
arc <radius> <extent>  | Dessine un arc	                                       | arc 50 180
pivotleft <angle>	   | Pivote le curseur à gauche	                           | pivotleft 45
pivotright <angle>	   | Pivote le curseur à droite	                           | pivotright 45
clear	               | Efface tout le canvas	                               | clear
speed <speed>	       | Change la vitesse (à implémenter)	                   | speed 10
tracer <n> <delay>	   | Contrôle le traçage (à implémenter)	               | tracer 1 100
update	               | Force la mise à jour de l'écran	                   | update


Remarques :

si on rajoute une commande ne pas oublier de la mettre dans le listenInstruction.
des erreures sont écrites dans la console si jamais y a un soucis (rudimentaire mais on a déjà ça au moins)
les dessins se font instant donc la fct speed est inutile on devrait la retirer je pense 
la fct tracer bah j'ai pas compris
on devrait aussi regarder pour rendre le nom de commande plus claire peut être (pos et go peuvent porter à confusion)
