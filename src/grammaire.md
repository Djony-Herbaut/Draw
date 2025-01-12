# Grammaire du langage draw++

Soit **G = ⟨T, N, S, P⟩** une grammaire définie comme suit :

T : éléments terminaux
N : éléments non terminaux
S : axiome
P : règles de production


**Contient tout les tokens pouvant etre générés**

    T = { TOKEN_DRAWCREATE_CURSOR, TOKEN_DRAWSET_POS, TOKEN_DRAWGO,TOKEN_DRAWSETX, TOKEN_DRAWSETY, TOKEN_DRAWSHOW_CURSOR,TOKEN_DRAWHIDE_CURSOR, TOKEN_DRAWCURSOR_COLOR, TOKEN_DRAWPEN_SIZE,
    TOKEN_DRAWMOVE_FORWARD, TOKEN_DRAWMOVE_BACKWARD, TOKEN_DRAWPIVOT_LEFT,TOKEN_DRAWPIVOT_RIGHT, TOKEN_DRAWCIRCLE, TOKEN_DRAWDOT,TOKEN_DRAWARC, TOKEN_DRAWUPDATE, TOKEN_DRAWPENUP, TOKEN_DRAWPENDOWN,TOKEN_DRAWSHAPE, TOKEN_DRAWCLEAR_SCREEN,
    TOKEN_DRAWV_,TOKEN_DRAWF, TOKEN_D_IF, TOKEN_D_ELSE, TOKEN_D_FOR, TOKEN_D_WHILE,TOKEN_DRAWT_SLEEP,
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE,TOKEN_RBRACE, TOKEN_COMMA,TOKEN_SEMICOLON, TOKEN_VARNBR, TOKEN_VARSTR, TOKEN_ASSIGN, TOKEN_SUP, TOKEN_INF, TOKEN_LEQ, TOKEN_GEQ, TOKEN_EQ, TOKEN_NEQ, TOKEN_AND, TOKEN_OR, TOKEN_NOT,TOKEN_PLUS, TOKEN_MOINS, TOKEN_MULT, TOKEN_DIV, TOKEN_POW TOKEN_NBR, TOKEN_STR, TOKEN_VARIABLE, TOKEN_EOF }

**Elements non terminaux afin de generer les regles**
N = { program, statement, block, statement_list, draw_stmt, setcolor_stmt, move_stmt, parameters, shape, condition,
loop, condition_expr, comparison_operator, value, var_decl }


**axiome**
S = program

## Legende

ε : mot vide
program : Programme principal.
statement : Déclaration d'une action (dessiner, déplacer, définir une couleur).
draw_stmt : Commande pour dessiner une forme.
setcolor_stmt : Commande pour définir une couleur.
move_stmt : Commande pour déplacer le curseur.
parameters : Paramètres supplémentaires (dimensions, coordonnées).
block : Groupe d'instructions encadré par des accolades {}.
statement_list : Liste de déclarations ou instructions dans un bloc.
condition : Structure de contrôle conditionnelle (if et else).
loop : Structure de contrôle répétitive (while).
condition_expr représente l'expression conditionnelle globale, qui peut inclure des "ou" (TOKEN_OR).
condition_term représente les "et" (TOKEN_AND) au sein d'une expression.
condition_factor permet de gérer les négations (TOKEN_NOT) et les conditions simples.
comparison_operator : Opérateurs de comparaison (ex. ==, !=, <, >).
value : Peut être une constante numérique (number) ou une variable (variable).

**regles de production**
P =
  1. program → statement program | block program | TOKEN_EOF

  2. statement → draw_stmt | setcolor_stmt | move_stmt | condition | loop | var_decl

  3. block → TOKEN_LBRACE statement_list TOKEN_RBRACE

  4. statement_list → statement statement_list | ε

  5. draw_stmt → TOKEN_DRAWCREATE_CURSOR parameters TOKEN_SEMICOLON
                | TOKEN_DRAWGO TOKEN_SEMICOLON
                | TOKEN_DRAWSETTOKEN_ASSIGN value TOKEN_SEMICOLON
                | TOKEN_DRAWSETY TOKEN_ASSIGN value TOKEN_SEMICOLON
                | TOKEN_DRAWSETX TOKEN_ASSIGN value TOKEN_SEMICOLON
                | TOKEN_DRAWSHOW_CURSOR TOKEN_SEMICOLON
                | TOKEN_DRAWHIDE_CURSOR TOKEN_SEMICOLON
                | TOKEN_DRAWCURSOR_COLOR parameters TOKEN_SEMICOLON
                | TOKEN_DRAWPEN_SIZE parameters TOKEN_SEMICOLON
                | TOKEN_DRAWPIVOT_LEFT TOKEN_SEMICOLON
                | TOKEN_DRAWPIVOT_RIGHT TOKEN_SEMICOLON
                | TOKEN_DRAWMOVE_FORWARD TOKEN_SEMICOLON
                | TOKEN_DRAWMOVE_BACKWARD TOKEN_SEMICOLON
                | TOKEN_DRAWCIRCLE parameters TOKEN_SEMICOLON
                | TOKEN_DRAWDOT parameters TOKEN_SEMICOLON
                | TOKEN_DRAWARC paremeters TOKEN_SEMICOLON
                | TOKEN_DRAWUPDATE parameters TOKEN_SEMICOLON
                | TOKEN_DRAWPENUP TOKEN_SEMICOLON
                | TOKEN_DRAWPENDOWN TOKEN_SEMICOLON
                | TOKEN_DRAWSHAPE parameters TOKEN_SEMICOLON
                | TOKEN_DRAWCLEAR_SCREEN TOKEN_SEMICOLON
                | TOKEN_DRAWT_SLEEP parameters TOKEN_SEMICOLON

  6. setcolor_stmt → TOKEN_DRAWCURSOR_COLOR TOKEN_ASSIGN TOKEN_STR TOKEN_SEMICOLON

  7. move_stmt → TOKEN_DRAWSET_POS TOKEN_LPAREN value TOKEN_COMMA value TOKEN_RPAREN TOKEN_SEMICOLON    

  8. parameters → TOKEN_LPAREN value TOKEN_RPAREN | ε

  10. condition → TOKEN_D_IF TOKEN_LPAREN condition_expr TOKEN_RPAREN block (TOKEN_D_ELSE block | ε)

  11. loop → TOKEN_D_FOR TOKEN_LPAREN statement TOKEN_RPAREN block          
            | TOKEN_D_WHILE TOKEN_LPAREN condition_expr TOKEN_RPAREN block

  12. condition_expr → condition_term
                      | condition_expr TOKEN_OR condition_term

  13. condition_term → condition_factor
                      | condition_term TOKEN_AND condition_factor

  14. condition_factor → TOKEN_NOT condition_factor
                        | simple_condition

  15. simple_condition → TOKEN_VARIABLE comparison_operator value
                        | TOKEN_NBR comparison_operator TOKEN_NBR

  16. comparison_operator → TOKEN_SUP 
                            | TOKEN_GEQ
                            | TOKEN_INF 
                            | TOKEN_LEQ 
                            | TOKEN_EQ
                            | TOKEN_NEQ

  17. value → TOKEN_NBR | TOKEN_VARIABLE | TOKEN_STR

  18. var_decl → TOKEN_VARNBR TOKEN_VARIABLE TOKEN_ASSIGN value TOKEN_SEMICOLON
                | TOKEN_VARSTR TOKEN_VARIABLE TOKEN_ASSIGN TOKEN_STR TOKEN_SEMICOLON


