typedef enum {
    // Mots-clés élémentaires
    TOKEN_DRAWCREATE_CURSOR,       // drawcreate_cursor
    TOKEN_DRAWSET_POS,             // drawsetpos
    TOKEN_DRAWGO,                  // drawgo
    TOKEN_DRAWSETX,                // drawsetx
    TOKEN_DRAWSETY,                // drawsety
    TOKEN_DRAWSHOW_CURSOR,         // drawshowcursor
    TOKEN_DRAWHIDE_CURSOR,         // drawhidecursor
    TOKEN_DRAWCURSOR_COLOR,        // drawcursorcolor
    TOKEN_DRAWPEN_SIZE,            // drawpensize
    TOKEN_DRAWMOVE_FORWARD,        // drawmoveforward
    TOKEN_DRAWMOVE_BACKWARD,       // drawmovebackward
    TOKEN_DRAWPIVOT_LEFT,          // drawpivotleft
    TOKEN_DRAWPIVOT_RIGHT,         // drawpivotright
    TOKEN_DRAWCIRCLE,              // drawcircle
    TOKEN_DRAWDOT,                 // drawdot
    TOKEN_DRAWARC,                 // drawarc
    TOKEN_DRAWUPDATE,              // drawupdate
    TOKEN_DRAWPENUP,               // drawpenup
    TOKEN_DRAWPENDOWN,             // drawpendown
    TOKEN_DRAWSHAPE,               // drawshape
    TOKEN_DRAWCLEAR_SCREEN,        // drawclearscreen

    // Mots-clés avancés
    TOKEN_DRAWV_,            // drawv_variable = value
    TOKEN_DRAWF,                   // drawf function
    TOKEN_D_IF,                    // d_if
    TOKEN_D_ELSE,                  // d_else
    TOKEN_D_FOR,                   // d_for
    TOKEN_D_WHILE,                 // d_while
    TOKEN_DRAWT_SLEEP,             // drawtsleep

    // Symboles
    TOKEN_LPAREN,                  // (
    TOKEN_RPAREN,                  // )
    TOKEN_LBRACE,                  // {
    TOKEN_RBRACE,                  // }
    TOKEN_COMMA,                   // ,
    TOKEN_ASSIGN,                  // =

    // Autres types
    TOKEN_NBR,                  // 123, 3.14
    TOKEN_STR,                  // "red"
    TOKEN_VARIABLE,                // drawv_myVar - nom associés au mot clé drawv_
    TOKEN_UNKNOWN,                 // Inconnus (erreurs de lexing)

    // Fin du fichier
    TOKEN_EOF                      // Fin de fichier
} TokenType;