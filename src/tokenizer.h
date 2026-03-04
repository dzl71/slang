#include <stddef.h>

#ifndef TOKENIZER_H
#define TOKENIZER_H

typedef struct TokenizerState {
	size_t idx;
	char *buffer;
} TokenizerState;

typedef enum TokenTag {
	eof,
	invalid,
	identifier,
	stringliteral,
	numliteral,
	// keywords:
	keyword_const,
	keyword_mut,
	keyword_fn,
	keyword_struct,
	keyword_enum,
	keyword_union,
	keyword_return,
	keyword_for,
	keyword_if,
	keyword_else,
	// operators:
	fn_pipe,
	// *math
	minus,	
	minus_equal,
	plus,
	plus_equal,
	mul_equal,
	div_op,
	div_equal,
	mod,
	mod_equal,
	// *bit
	bit_or,
	bit_and,
	bit_not,
	bit_xor,
	bitshift_left,
	bitshift_right,
	// *logic
	logic_or,
	logic_and,
	logic_not,
	logic_xor,
	less,
	less_equals,
	greater,
	greater_equals,
	equals,
	not_equals,
	// symbols:
	assign,
	asterix,
	period,
	comma,
	colon,
	semicolon,
	left_brace,
	right_brace,
	left_bracket,
	right_bracket,
	left_curly,
	right_curly,
} TokenTag;

typedef struct Token {
	TokenTag tag;
	size_t start;
	size_t end;
} Token;

typedef Token* TokenArrayList;

void tokenizer_init(TokenizerState *state, char *tokenizedData);

/// returns the next token;
/// if the tokenizer reached the end, returns eof token;
Token tokenizer_next(TokenizerState *state);

TokenTag keywordIdentify(size_t start, size_t end, char *buffer);

TokenArrayList tokenizer_tokenize(char *tokenizedData);

#endif //TOKENIZER_H
