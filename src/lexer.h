#ifndef LEXER_H
#define LEXER_H

#include "token.h"

#define MAX_TOKENS 5000

// Prototypes pour rendre is_keyword et is_symbol disponibles dans ide.c
int tokenize(const char *source_code, Token *tokens);
void skip_whitespace_and_comments(const char **current_char);
TokenType is_keyword(const char *word);
TokenType is_symbol(char c);
TokenType is_number(const char *word);

#endif // LEXER_H
