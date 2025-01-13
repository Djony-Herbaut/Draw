#include "token.h"
#include <stdio.h>

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
    {TOKEN_DRAWF, "drawf"},
    {TOKEN_D_IF, "d_if"},
    {TOKEN_D_ELSE, "d_else"},
    {TOKEN_D_FOR, "d_for"},
    {TOKEN_D_WHILE, "d_while"},
    {TOKEN_DRAWT_SLEEP, "drawtsleep"},
    {TOKEN_DRAWV_, "drawv_variable déclaration"},
    {TOKEN_VARNBR, "variable de type nbr"},
    {TOKEN_VARSTR, "variable de type string"},
    {TOKEN_VARIABLE,"nom associé à la déclaration"},
    {TOKEN_STR, "String"},
    {TOKEN_NBR, "Nombre"},
    {TOKEN_EOF, "Token EOF"}
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
    {TOKEN_QUOTE, "\""},        // Quote 
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

// Fonction pour obtenir la chaîne correspondant au type de token
const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_DRAWCREATE_CURSOR: return "TOKEN_DRAWCREATE_CURSOR";
        case TOKEN_DRAWSET_POS: return "TOKEN_DRAWSET_POS";
        case TOKEN_DRAWGO: return "TOKEN_DRAWGO";
        case TOKEN_DRAWSETX: return "TOKEN_DRAWSETX";
        case TOKEN_DRAWSETY: return "TOKEN_DRAWSETY";
        case TOKEN_DRAWSHOW_CURSOR: return "TOKEN_DRAWSHOW_CURSOR";
        case TOKEN_DRAWHIDE_CURSOR: return "TOKEN_DRAWHIDE_CURSOR";
        case TOKEN_DRAWCURSOR_COLOR: return "TOKEN_DRAWCURSOR_COLOR";
        case TOKEN_DRAWPEN_SIZE: return "TOKEN_DRAWPEN_SIZE";
        case TOKEN_DRAWMOVE_FORWARD: return "TOKEN_DRAWMOVE_FORWARD";
        case TOKEN_DRAWMOVE_BACKWARD: return "TOKEN_DRAWMOVE_BACKWARD";
        case TOKEN_DRAWPIVOT_LEFT: return "TOKEN_DRAWPIVOT_LEFT";
        case TOKEN_DRAWPIVOT_RIGHT: return "TOKEN_DRAWPIVOT_RIGHT";
        case TOKEN_DRAWCIRCLE: return "TOKEN_DRAWCIRCLE";
        case TOKEN_DRAWDOT: return "TOKEN_DRAWDOT";
        case TOKEN_DRAWARC: return "TOKEN_DRAWARC";
        case TOKEN_DRAWUPDATE: return "TOKEN_DRAWUPDATE";
        case TOKEN_DRAWPENUP: return "TOKEN_DRAWPENUP";
        case TOKEN_DRAWPENDOWN: return "TOKEN_DRAWPENDOWN";
        case TOKEN_DRAWSHAPE: return "TOKEN_DRAWSHAPE";
        case TOKEN_DRAWCLEAR_SCREEN: return "TOKEN_DRAWCLEAR_SCREEN";
        case TOKEN_D_IF: return "TOKEN_D_IF";
        case TOKEN_D_ELSE: return "TOKEN_D_ELSE";
        case TOKEN_D_FOR: return "TOKEN_D_FOR";
        case TOKEN_D_WHILE: return "TOKEN_D_WHILE";
        case TOKEN_DRAWT_SLEEP: return "TOKEN_DRAWT_SLEEP";
        case TOKEN_LPAREN: return "TOKEN_LPAREN";
        case TOKEN_RPAREN: return "TOKEN_RPAREN";
        case TOKEN_COMMA: return "TOKEN_COMMA";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_ASSIGN: return "TOKEN_ASSIGN";
        case TOKEN_PLUS: return "TOKEN_PLUS";
        case TOKEN_MOINS: return "TOKEN_MOINS";
        case TOKEN_MULT: return "TOKEN_MULT";
        case TOKEN_DIV: return "TOKEN_DIV";
        case TOKEN_NBR: return "TOKEN_NBR";
        case TOKEN_VARNBR: return "TOKEN_VARNBR";
        case TOKEN_VARSTR: return "TOKEN_VARSTR";
        case TOKEN_STR: return "TOKEN_STR";
        case TOKEN_VARIABLE: return "TOKEN_VARIABLE";
        case TOKEN_EOF: return "TOKEN_EOF";
        default: return "TOKEN_UNKNOWN";
    }
} 

/* const char *get_token_string(TokenType type) {
    // Vérifier dans les mots-clés
    for (int i = 0; i < keywordsDictSize; i++) {
        if (keywords[i].type == type) {
            return keywords[i].name;
        }
    }

    // Vérifier dans les symboles
    for (int i = 0; i < symbolsDictSize; i++) {
        if (symbols[i].type == type) {
            return symbols[i].name;
        }
    }

    return "UNKNOWN"; // Par défaut, si non trouvé
}
 */
