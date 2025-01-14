# Grammaire du langage draw++

Soit **G = ⟨T, N, S, P⟩** une grammaire définie comme suit :

T : éléments terminaux
N : éléments non terminaux
S : axiome
P : règles de production

**Contient tout les tokens pouvant etre générés**

    T = { TOKEN_DRAWCREATE_CURSOR, TOKEN_DRAWSET_POS, TOKEN_DRAWGO,TOKEN_DRAWSETX, TOKEN_DRAWSETY, TOKEN_DRAWSHOW_CURSOR,TOKEN_DRAWHIDE_CURSOR, TOKEN_DRAWCURSOR_COLOR, 
    TOKEN_DRAWPEN_SIZE,TOKEN_DRAWMOVE_FORWARD, TOKEN_DRAWMOVE_BACKWARD, TOKEN_DRAWPIVOT_LEFT,TOKEN_DRAWPIVOT_RIGHT, TOKEN_DRAWCIRCLE, TOKEN_DRAWDOT,TOKEN_DRAWARC, TOKEN_DRAWUPDATE, 
    TOKEN_DRAWPENUP, TOKEN_DRAWPENDOWN,TOKEN_DRAWSHAPE, TOKEN_DRAWCLEAR_SCREEN,

    TOKEN_DRAWV_,TOKEN_DRAWF, TOKEN_D_IF, TOKEN_D_ELSE, TOKEN_D_FOR, TOKEN_D_WHILE,TOKEN_DRAWT_SLEEP,

    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE,TOKEN_RBRACE, TOKEN_QUOTE, TOKEN_COMMA,TOKEN_SEMICOLON, TOKEN_VARNBR, TOKEN_VARSTR, TOKEN_ASSIGN, TOKEN_SUP, TOKEN_INF, TOKEN_LEQ, 
    TOKEN_GEQ, TOKEN_EQ, TOKEN_NEQ, TOKEN_AND, TOKEN_OR, TOKEN_NOT,TOKEN_PLUS, TOKEN_MOINS, TOKEN_MULT, TOKEN_DIV, TOKEN_POW TOKEN_NBR, TOKEN_STR, TOKEN_VARIABLE, TOKEN_NEWLINE, TOKEN_EOF }

**Elements non terminaux afin de generer les regles**
N = {ε, program, statement, block, statement_list, draw_stmt, setcolor_stmt, move_stmt, parameters, shape, condition,
loop, condition_expr, comparison_operator, value, var_decl, string_literal }

**axiome**

S = program

## Legende

ε : Mot vide.
program : Programme principal, constituant l'ensemble de l'application ou du script.
statement : Déclaration d'une action, comme dessiner, déplacer, ou définir une couleur.
draw_stmt : Commande pour dessiner une forme (par exemple, drawcircle, drawline).
setcolor_stmt : Commande pour définir une couleur, souvent utilisée avant de dessiner (par exemple, setcolor(255, 0, 0)).
move_stmt : Commande pour déplacer le curseur ou l'outil de dessin (par exemple, move(100, 100)).
parameters : Paramètres supplémentaires pour les commandes de dessin, comme les dimensions ou les coordonnées (par exemple, 100, 200 dans drawcircle(100, 200)).
block : Un ensemble d'instructions regroupées dans un bloc de code, généralement délimité par des accolades {}.
statement_list : Liste des déclarations ou instructions dans un bloc, souvent une séquence de statement dans un block.
condition : Structure de contrôle conditionnelle, comme if et else.
loop : Structure de contrôle répétitive, comme les boucles for et while.
value : Constante numérique, chaîne de caractères ou variable utilisée dans une expression.
string_literal : Chaîne de caractères, généralement délimitée par des guillemets (par exemple, "hello").
var_decl : Déclaration d'une variable, associée à un type et un nom (par exemple, int x;, string msg;).

**regles de production**
P =

  1. program → statement TOKEN_NEWLINE program | block TOKEN_NEWLINE program | TOKEN_EOF

  2. statement → draw_stmt | setcolor_stmt | move_stmt | condition | loop | var_decl

  3. block → TOKEN_LBRACE statement_list TOKEN_RBRACE

  4. statement_list → statement TOKEN_NEWLINE statement_list | ε

  5. draw_stmt → TOKEN_DRAWCREATE_CURSOR parameters TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWGO TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWSET TOKEN_ASSIGN value TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWSETY TOKEN_ASSIGN value TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWSETX TOKEN_ASSIGN value TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWSHOW_CURSOR TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWHIDE_CURSOR TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWCURSOR_COLOR parameters TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWPEN_SIZE parameters TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWPIVOT_LEFT TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWPIVOT_RIGHT TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWMOVE_FORWARD TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWMOVE_BACKWARD TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWCIRCLE parameters TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWDOT parameters TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWARC parameters TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWUPDATE parameters TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWPENUP TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWPENDOWN TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWSHAPE parameters TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWCLEAR_SCREEN TOKEN_SEMICOLON TOKEN_NEWLINE
            | TOKEN_DRAWT_SLEEP parameters TOKEN_SEMICOLON TOKEN_NEWLINE


  6. setcolor_stmt → TOKEN_DRAWSETCOLOR TOKEN_LPAREN string_literal TOKEN_RPAREN TOKEN_SEMICOLON


  7. move_stmt → TOKEN_DRAWSET_POS TOKEN_LPAREN value TOKEN_COMMA value TOKEN_RPAREN TOKEN_SEMICOLON    

  8. parameters → TOKEN_LPAREN value (TOKEN_COMMA, value)* TOKEN_RPAREN | ε

  condition → TOKEN_D_IF TOKEN_LPAREN condition_expr TOKEN_RPAREN block TOKEN_NEWLINE 
            (TOKEN_D_ELSE block TOKEN_NEWLINE | ε)


  loop → TOKEN_D_FOR TOKEN_LPAREN statement TOKEN_RPAREN block TOKEN_NEWLINE
       | TOKEN_D_WHILE TOKEN_LPAREN condition_expr TOKEN_RPAREN block TOKEN_NEWLINE


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

  17. value → TOKEN_NBR | TOKEN_VARIABLE | string_litteral

  18. var_decl → TOKEN_VARNBR TOKEN_VARIABLE TOKEN_ASSIGN TOKEN_NBR TOKEN_SEMICOLON
                | TOKEN_VARSTR TOKEN_VARIABLE TOKEN_ASSIGN string_litteral TOKEN_SEMICOLON

  19. string_literal → TOKEN_QUOTE TOKEN_STR TOKEN_QUOTE



