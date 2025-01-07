# Grammaire du langage draw++

Soit **G = ⟨T, N, S, P⟩** une grammaire définie comme suit :

T : éléments terminaux
N : éléments non terminaux
S : axiome
P : règles de production


// Contient tout les tokens pouvant etre générés
T = { TOKEN_DRAWCREATE_CURSOR, TOKEN_DRAWSET_POS, TOKEN_DRAWGO,TOKEN_DRAWSETX, TOKEN_DRAWSETY, TOKEN_DRAWSHOW_CURSOR,TOKEN_DRAWHIDE_CURSOR, TOKEN_DRAWCURSOR_COLOR, TOKEN_DRAWPEN_SIZE,
TOKEN_DRAWMOVE_FORWARD, TOKEN_DRAWMOVE_BACKWARD, TOKEN_DRAWPIVOT_LEFT,TOKEN_DRAWPIVOT_RIGHT, TOKEN_DRAWCIRCLE, TOKEN_DRAWDOT,TOKEN_DRAWARC, TOKEN_DRAWUPDATE, TOKEN_DRAWPENUP, TOKEN_DRAWPENDOWN,TOKEN_DRAWSHAPE, TOKEN_DRAWCLEAR_SCREEN,
TOKEN_DRAWV_,TOKEN_DRAWF, TOKEN_D_IF, TOKEN_D_ELSE, TOKEN_D_FOR, TOKEN_D_WHILE,TOKEN_DRAWT_SLEEP, TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE,TOKEN_RBRACE, TOKEN_COMMA, TOKEN_ASSIGN, TOKEN_SUP, TOKEN_INF,TOKEN_PLUS, TOKEN_MOINS, TOKEN_MULT, TOKEN_DIV, TOKEN_MSG,TOKEN_NBR, TOKEN_STR, TOKEN_VARIABLE, TOKEN_EOF }


N = { program, statement, block, statement_list, draw_stmt,setcolor_stmt, move_stmt, parameters, shape, condition,
loop, condition_expr, comparison_operator, value }

S = program

P =
  1. program → statement program | block program | ε

  2. statement → draw_stmt | setcolor_stmt | move_stmt | condition | loop

  3. block → TOKEN_LBRACE statement_list TOKEN_RBRACE

  4. statement_list → statement statement_list | ε

  5. draw_stmt → TOKEN_DRAWCREATE_CURSOR parameters TOKEN_EOF
                | TOKEN_DRAWGO TOKEN_EOF
                | TOKEN_DRAWSETX TOKEN_ASSIGN value TOKEN_EOF
                | TOKEN_DRAWSETY TOKEN_ASSIGN value TOKEN_EOF
                | TOKEN_DRAWMOVE_FORWARD TOKEN_EOF
                | TOKEN_DRAWMOVE_BACKWARD TOKEN_EOF
                | TOKEN_DRAWCIRCLE parameters TOKEN_EOF
                | TOKEN_DRAWDOT parameters TOKEN_EOF
                | TOKEN_DRAWSHAPE parameters TOKEN_EOF

  6. setcolor_stmt → TOKEN_DRAWCURSOR_COLOR TOKEN_ASSIGN TOKEN_STR TOKEN_EOF

  7. move_stmt → TOKEN_DRAWSET_POS TOKEN_LPAREN value TOKEN_COMMA value TOKEN_RPAREN TOKEN_EOF    **a revoir les possibles parametres de setpos**

  8. parameters → TOKEN_LPAREN TOKEN_NBR TOKEN_RPAREN | ε

  9. shape → TOKEN_DRAWCREATE_CURSOR | TOKEN_DRAWCIRCLE | TOKEN_DRAWDOT

  10. condition → TOKEN_D_IF TOKEN_LPAREN condition_expr TOKEN_RPAREN block (TOKEN_D_ELSE block | ε)

  11. loop → TOKEN_D_FOR TOKEN_LPAREN statement TOKEN_RPAREN block          **conditions du for a revoir**
            | TOKEN_D_WHILE TOKEN_LPAREN condition_expr TOKEN_RPAREN block

  12. condition_expr → TOKEN_VARIABLE comparison_operator value
                      | TOKEN_NBR comparison_operator TOKEN_NBR

  13. comparison_operator → TOKEN_SUP | TOKEN_INF | TOKEN_ASSIGN            **symboles de comparaisons a revoir (pas d'inf egale, sup egale et faut prendre que c'est "==" pas "=")**

  14. value → TOKEN_NBR | TOKEN_VARIABLE | TOKEN_STR

