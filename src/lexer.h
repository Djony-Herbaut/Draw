#ifndef LEXER_H
#define LEXER_H

#include "token.h"

#define MAX_TOKENS 5000

// Prototype de la fonction principale du lexer
int tokenize(const char *source_code, Token *tokens);

#endif // LEXER_H
