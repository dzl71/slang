#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "../misc/ArrayList.h"
#include "../misc/HashMap.h"
#include "tokenizer.h"

void tokenizer_init(TokenizerState *state, char *tokenizedData) {
	state->idx = 0;
	state->buffer = tokenizedData;
}

TokenTag keywordIdentify(char *keyword, size_t len) {
	switch (hash(keyword, len)) {
		case -1108668895:
			return keyword_if;
		case -1109815566:
			return keyword_fn;
		case 279730432:
			return keyword_mut;
		case 577479304:
			return keyword_for;
		case -861930528:
			return keyword_enum;
		case 739881296:
			return keyword_else;
		case -776288134:
			return keyword_union;
		case 2083172026:
			return keyword_const;
		case 1535229688:
			return keyword_struct;
		case -222620452:
			return keyword_return;
		default:
			return identifier;
	}
}

// TODO: refactor the function
/// returns the next token;
/// if the tokenizer reached the end, returns eof token;
Token tokenizer_next(TokenizerState *state) {
	Token token = {
		.tag = invalid,
	};

	// using goto labels to increase cache hits (inspired by zig source code)
	START: 
	switch (state->buffer[state->idx]) {
	// looping states
		// skip wihite space
		case ' ': case '\t': case '\n': case '\r':
			state->idx++;
			goto START;
			break;
			
		// the ... syntax is gcc and clnag specific
		case 'A'...'Z': case 'a'...'z': case '_':
			token.start = state->idx;
			goto IDENTIFIER;
			break;

		// multiline and regular strings
		case '"':
			token.start = state->idx;
			token.tag = stringliteral;
			goto STRINGLITERAL;
			break;

		case '1'...'9':
			token.start = state->idx;
			token.tag = numliteral;
			goto INTLITERAL;
			break;
	// onetime states
		// end of tokenized data
		case 0:
			token.start = state->idx;
			token.end = ++(state->idx); // should test thourully, can contain off by 1 error
			token.tag = eof;
			goto END;
			break;

		case '.':
			token.start = state->idx;
			token.end = ++(state->idx);
			token.tag = period;
			goto END;
			break;

		case ',':
			token.start = state->idx;
			token.end = ++(state->idx);
			token.tag = comma;
			goto END;
			break;

		case ':':
			token.start = state->idx;
			token.end = ++(state->idx);
			token.tag = colon;
			goto END;
			break;

		case ';':
			token.start = state->idx;
			token.end = ++(state->idx);
			token.tag = semicolon;
			goto END;
			break;

		case '(':
			token.start = state->idx;
			token.end = ++(state->idx);
			token.tag = left_brace;
			goto END;
			break;

		case ')':
			token.start = state->idx;
			token.end = ++(state->idx);
			token.tag = right_brace;
			goto END;
			break;

		case '[':
			token.start = state->idx;
			token.end = ++(state->idx);
			token.tag = left_bracket;
			goto END;
			break;

		case ']':
			token.start = state->idx;
			token.end = ++(state->idx);
			token.tag = right_bracket;
			goto END;
			break;

		case '{':
			token.start = state->idx;
			token.end = ++(state->idx);
			token.tag = left_curly;
			goto END;
			break;

		case '}':
			token.start = state->idx;
			token.end = ++(state->idx);
			token.tag = right_curly;
			goto END;
			break;

		case '~':
			token.start = state->idx;
			token.end = ++(state->idx);
			token.tag = bit_not;
			goto END;
			break;
	// can be one can be multiple
		case '0':
			token.start = state->idx;
			token.tag = numliteral;
			goto ZERO;
			break;
		case '-': goto MINUS; break;
		case '+': goto PLUS; break;
		case '*': goto ASTERIX; break;
		case '/': goto DIV; break;
		case '%': goto MOD; break;
		case '<': goto LEFTANGLE; break;
		case '>': goto RIGHTANGLE; break;
		case '=': goto EQUAL; break;
		case '!': goto NOT; break;
		case '|': goto OR; break;
		case '&': goto AND; break;
		case '^': goto XOR; break;

		default: // encountered invalid character
			token.start = state->idx;
			token.end = ++(state->idx);
			token.tag = invalid;
			goto END;
			break;
	}

	MINUS:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '>':
			token.end = ++(state->idx);
			token.tag = fn_pipe;
			goto END;
			break;
		case '=':
			token.end = ++(state->idx);
			token.tag = minus_equal;
			goto END;
			break;
		case '0':
			token.tag = numliteral;
			goto ZERO;
			break;
		case '1'...'9':
			token.tag = numliteral;
			goto INTLITERAL;
			break;
		default:
			token.end = state->idx;
			token.tag = minus;
			goto END;
			break;
	}

	PLUS:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '=':
			token.end = ++(state->idx);
			token.tag = plus_equal;
			goto END;
			break;
		default:
			token.end = state->idx;
			token.tag = plus;
			goto END;
			break;
	}

	ASTERIX:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '=':
			token.end = ++(state->idx);
			token.tag = mul_equal;
			goto END;
			break;
		default:
			token.end = state->idx;
			token.tag = asterix;
			goto END;
			break;
	}

	DIV:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '=':
			token.end = ++(state->idx);
			token.tag = div_equal;
			goto END;
			break;
		default:
			token.end = state->idx;
			token.tag = div_op;
			goto END;
			break;
	}

	MOD:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '=':
			token.end = ++(state->idx);
			token.tag = mod_equal;
			goto END;
			break;
		default:
			token.end = state->idx;
			token.tag = mod;
			goto END;
			break;
	}

	LEFTANGLE:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '<':
			token.end = ++(state->idx);
			token.tag = bitshift_left;
			goto END;
			break;
		case '=':
			token.end = ++(state->idx);
			token.tag = less_equals;
			goto END;
			break;
		default:
			token.end = state->idx;
			token.tag = less;
			goto END;
			break;
	}

	RIGHTANGLE:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '>':
			token.end = ++(state->idx);
			token.tag = bitshift_right;
			goto END;
			break;
		case '=':
			token.end = ++(state->idx);
			token.tag = greater_equals;
			goto END;
			break;
		default:
			token.end = state->idx;
			token.tag = greater;
			goto END;
			break;
	}

	EQUAL:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '=':
			token.end = ++(state->idx);
			token.tag = equals;
			goto END;
			break;
		default:
			token.end = state->idx;
			token.tag = assign;
			goto END;
			break;
	}

	NOT:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '=':
			token.end = ++(state->idx);
			token.tag = not_equals;
			goto END;
			break;
		default:
			token.end = state->idx;
			token.tag = logic_not;
			goto END;
			break;
	}

	OR:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '|':
			token.end = ++(state->idx);
			token.tag = logic_or;
			goto END;
			break;
		default:
			token.end = state->idx;
			token.tag = bit_or;
			goto END;
			break;
	}

	AND:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '&':
			token.end = ++(state->idx);
			token.tag = logic_and;
			goto END;
			break;
		default:
			token.end = state->idx;
			token.tag = bit_and;
			goto END;
			break;
	}

	XOR:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '^':
			token.end = ++(state->idx);
			token.tag = logic_xor;
			goto END;
			break;
		default:
			token.end = state->idx;
			token.tag = bit_xor;
			goto END;
			break;
	}

	ZERO:
	// TODO: refactor, decide how the numbers tag should be represented
	state->idx++;
	switch (state->buffer[state->idx]) {
		case 'b': goto BINARY; break;
		case 'x': goto HEX; break;
		case 'o': goto OCT; break;
		case '.': goto FLOAT; break;
		case '0'...'9': case '_': goto INTLITERAL; break;
		default:
			token.end = state->idx;
			goto END;
			break;
	}

	BINARY:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '0': case '1': case '_':
			goto BINARY;
			break;
		case '2'...'9':
			token.tag = invalid;
			goto BINARY;
			break;
		default:
			token.end = state->idx;
			goto END;
			break;
	}

	HEX:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '0'...'9': case 'a'...'f': case 'A'...'F': case '_':
			goto HEX;
			break;
		default:
			token.end = state->idx;
			goto END;
			break;
	}

	OCT:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '0'...'7': case '_':
			goto OCT;
			break;
		case '8': case '9':
			token.tag = invalid;
			goto OCT;
			break;
		default:
			token.end = state->idx;
			goto END;
			break;
	}

	FLOAT:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '0'...'9':  case '_': 
			goto FLOAT;
			break;
		case '.': // float cannot contain 2 periods
			token.tag = invalid;
			goto FLOAT;
			break;
		default:
			token.end = state->idx;
			goto END;
			break;
	}


	INTLITERAL:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '0'...'9':  case '_': 
			goto INTLITERAL;
			break;
		case '.':
			goto FLOAT;
			break;
		default:
			token.end = state->idx;
			goto END;
			break;
	}

	IDENTIFIER:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case 'A'...'Z': case 'a'...'z': case '_': case '0'...'9':
			goto IDENTIFIER;
			break;
		default:
			token.end = state->idx;
			token.tag = keywordIdentify(state->buffer + token.start, token.end - token.start);
			goto END;
			break;
	}

	STRINGLITERAL:
	state->idx++;
	switch (state->buffer[state->idx]) {
		case '"':
			token.end = ++(state->idx);
			goto END;
			break;
		case '\n':
			// TODO: decide what to do in this case
			break;
		// skip the nex character ???? shuld it even be implemented ????
		// case '\\':
		// 	   state->idx++;
		// 	   goto STRINGLITERAL;
		default:
			goto STRINGLITERAL;
			break;
	}

	END:
	return token;
}

TokenArrayList tokenizer_tokenize(char *tokenizedData) {
	TokenizerState state;
	tokenizer_init(&state, tokenizedData);
	TokenArrayList arr = NULL;
	ArrayList_init(arr);
	if (!arr); // handle init error

	Token token;
	do { // makes so the eof token is added into the arraylist
		token = tokenizer_next(&state);
		size_t prevLen = ArrayList_len(arr);
		ArrayList_push(arr, token);
		if (prevLen == ArrayList_len(arr)); // TODO: handle ralloc failure

	} while (token.tag != eof);

	return arr;
}

