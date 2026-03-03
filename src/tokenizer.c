#include <stddef.h>
#include "tokenizer.h"

void tokenizerInit(TokenizerState *state, char *tokenizedData) {
	state->idx = 0;
	state->buffer = tokenizedData;
}

// TODO: refactor the function
/// returns the next token;
/// if the tokenizer reached the end, returns eof token;
Token TokenizerNext(TokenizerState *state) {
	Token token = {
		.tag = invalid,
		.start = state->idx
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
			token.tag = identifier;
			goto IDENTIFIER;
			break;

		// multiline and regular strings
		case '"':
			token.tag = stringliteral;
			goto STRINGLITERAL;
			break;

		case '1'...'9':
			token.tag = numberliteral;
			goto INTLITERAL;
			break;
	// onetime states
		// end of tokenized data
		case 0:
			token.tag = eof;
			token.end = ++(state->idx); // should test thourully, can contain off by 1 error
			goto END;
			break;

		case '.':
			token.end = ++(state->idx);
			token.tag = period;
			goto END;
			break;

		case ',':
			token.end = ++(state->idx);
			token.tag = comma;
			goto END;
			break;

		case ':':
			token.end = ++(state->idx);
			token.tag = colon;
			goto END;
			break;

		case ';':
			token.end = ++(state->idx);
			token.tag = semicolon;
			goto END;
			break;

		case '(':
			token.end = ++(state->idx);
			token.tag = left_brace;
			goto END;
			break;

		case ')':
			token.end = ++(state->idx);
			token.tag = right_brace;
			goto END;
			break;

		case '[':
			token.end = ++(state->idx);
			token.tag = left_bracket;
			goto END;
			break;

		case ']':
			token.end = ++(state->idx);
			token.tag = right_bracket;
			goto END;
			break;

		case '{':
			token.end = ++(state->idx);
			token.tag = left_curly;
			goto END;
			break;

		case '}':
			token.end = ++(state->idx);
			token.tag = right_curly;
			goto END;
			break;

		case '~':
			token.end = ++(state->idx);
			token.tag = bit_not;
			goto END;
			break;
	// can be one can be multiple
		case '0': goto ZERO; break;
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
			token.tag = numberliteral;
			goto ZERO;
			break;
		case '1'...'9':
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
			token.tag = div;
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
