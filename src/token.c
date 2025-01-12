#include "token.h"

// Définition des mots-clés
TokenDictionnary keywords[] = {
   {TOKEN_DRAWCREATE_CURSOR, "drawcreate_cursor"},
    {TOKEN_DRAWSET_POS, "drawsetpos"},
    {TOKEN_DRAWGO, "drawgo"},
    {TOKEN_DRAWSETX, "drawsetx"},
    {TOKEN_DRAWSETY, "drawsety"},
    {TOKEN_DRAWSHOW_CURSOR, "drawshowcursor"},
    {TOKEN_DRAWHIDE_CURSOR, "drawhidecursor"},
    {TOKEN_DRAWCURSOR_COLOR, "drawcursorcolor"},
    {TOKEN_DRAWPEN_SIZE, "drawpensize"},
    {TOKEN_DRAWMOVE_FORWARD, "drawmoveforward"},
    {TOKEN_DRAWMOVE_BACKWARD, "drawmovebackward"},
    {TOKEN_DRAWPIVOT_LEFT, "drawpivotleft"},
    {TOKEN_DRAWPIVOT_RIGHT, "drawpivotright"},
    {TOKEN_DRAWCIRCLE, "drawcircle"},
    {TOKEN_DRAWDOT, "drawdot"},
    {TOKEN_DRAWARC, "drawarc"},
    {TOKEN_DRAWUPDATE, "drawupdate"},
    {TOKEN_DRAWPENUP, "drawpenup"},
    {TOKEN_DRAWPENDOWN, "drawpendown"},
    {TOKEN_DRAWSHAPE, "drawshape"},
    {TOKEN_DRAWCLEAR_SCREEN, "drawclearscreen"},
    {TOKEN_DRAWV_, "drawv_variable"},
    {TOKEN_VARNBR, "nbr"},
    {TOKEN_VARSTR, "string"},
    {TOKEN_DRAWF, "drawf"},
    {TOKEN_D_IF, "d_if"},
    {TOKEN_D_ELSE, "d_else"},
    {TOKEN_D_FOR, "d_for"},
    {TOKEN_D_WHILE, "d_while"},
    {TOKEN_DRAWT_SLEEP, "drawtsleep"}
};

const int keywordsDictSize = sizeof(keywords) / sizeof(keywords[0]);

// Définition des symboles
TokenDictionnary symbols[] = {
    {TOKEN_LPAREN, "("},         // Parenthèse ouvrante
    {TOKEN_RPAREN, ")"},         // Parenthèse fermante
    {TOKEN_LBRACE, "{"},         // Accolade ouvrante
    {TOKEN_RBRACE, "}"},         // Accolade fermante
    {TOKEN_LBRACKET, "["},       // Crochet ouvrant
    {TOKEN_RBRACKET, "]"},       // Crochet fermant
    {TOKEN_COMMA, ","},          // Virgule
    {TOKEN_SEMICOLON, ";"},      // Point-virgule (C)
    {TOKEN_ASSIGN, "="},         // Assignation
    {TOKEN_PLUS, "+"},           // Addition
    {TOKEN_MOINS, "-"},          // Soustraction
    {TOKEN_MULT, "*"},           // Multiplication
    {TOKEN_DIV, "/"},            // Division
    {TOKEN_POW, "**"},           // Puissance (Python)
    {TOKEN_INF, "<"},            // Inférieur à
    {TOKEN_SUP, ">"},            // Supérieur à
    {TOKEN_LEQ, "<="},           // Inférieur ou égal
    {TOKEN_GEQ, ">="},           // Supérieur ou égal
    {TOKEN_EQ, "=="},            // Égalité
    {TOKEN_NEQ, "!="},           // Différent (C et Python)
    {TOKEN_AND, "and"},           // ET logique (C)
    {TOKEN_OR, "or"},            // OU logique (C)
    {TOKEN_NOT, "not"},            // Négation logique (C)
};

const int symbolsDictSize = sizeof(symbols) / sizeof(symbols[0]);
