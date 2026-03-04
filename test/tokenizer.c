#include "../src/tokenizer.h"
#include "../misc/ArrayList.h"
#include <stddef.h>
#include <stdio.h>

#define EXPECTED_TAGS(...) (TokenTag[]){__VA_ARGS__}

char *tagNames[] = {
	"eof",
	"invalid",
	"identifier",
	"stringliteral",
	"numliteral",
	// keywords:
	"keyword_const",
	"keyword_mut",
	"keyword_fn",
	"keyword_struct",
	"keyword_enum",
	"keyword_union",
	"keyword_return",
	"keyword_for",
	"keyword_if",
	"keyword_else",
	// operators:
	"fn_pipe",
	// *math
	"minus",
	"minus_equal",
	"plus",
	"plus_equal",
	"mul_equal",
	"div_op",
	"div_equal",
	"mod",
	"mod_equal",
	// *bit
	"bit_or",
	"bit_and",
	"bit_not",
	"bit_xor",
	"bitshift_left",
	"bitshift_right",
	// *logic
	"logic_or",
	"logic_and",
	"logic_not",
	"logic_xor",
	"less",
	"less_equals",
	"greater",
	"greater_equals",
	"equals",
	"not_equals",
	// symbols:
	"assign",
	"asterix",
	"period",
	"comma",
	"colon",
	"semicolon",
	"left_brace",
	"right_brace",
	"left_bracket",
	"right_bracket",
	"left_curly",
	"right_curly",
};

void test(char *name, char *str, TokenTag *expected) {
	TokenArrayList arr = tokenizer_tokenize(str);
	size_t idx = 0;
	for (; expected[idx] != eof; ++idx) {
		if (arr[idx].tag != expected[idx]) {
			size_t start = arr[idx].start;
			size_t end = arr[idx].end;
			printf("%.*s`\n", end - start, str + start);
			printf("%s\n", tagNames[arr[idx].tag]);
			printf("!!!! %s failed idx:%lu expected:%s got:%s\n", name, idx, tagNames[expected[idx]], tagNames[arr[idx].tag]);
			ArrayList_free(arr);
			return;
		}
	}
	printf("==== %s succeded\n", name);
	ArrayList_free(arr);
}

int main() {
	test("keywords",
		"fn if mut for else return const enum union else struct",
		EXPECTED_TAGS(
			keyword_fn,
			keyword_if,
			keyword_mut,
			keyword_for,
			keyword_else,
			keyword_return,
			keyword_const,
			keyword_enum,
			keyword_union,
			keyword_else,
			keyword_struct,
			eof,
		)
	);

	test("empty file", "", EXPECTED_TAGS(eof));

	test("identifier", "hello world12", EXPECTED_TAGS(identifier, identifier, eof));

	test("numliteral", "-1 123 0.1 000 0x1a 0o3 0b0", EXPECTED_TAGS(
		numliteral,
		numliteral,
		numliteral,
		numliteral,
		numliteral,
		numliteral,
		numliteral,
		eof
	));

	test("symbols", ";:,.(){}[]=*", EXPECTED_TAGS(
		semicolon,
		colon,
		comma,
		period,
		left_brace,
		right_brace,
		left_curly,
		right_curly,
		left_bracket,
		right_bracket,
		assign,
		asterix,
		eof
	));

	test("function declaration",
		"fn add(a: int, b: int) int;",
		EXPECTED_TAGS(
			keyword_fn,
			identifier,
			left_brace,
			identifier,
			colon,
			identifier,
			comma,
			identifier,
			colon,
			identifier,
			right_brace,
			identifier,
			semicolon,
			eof
	));

	test("function definition",
		"fn add(a: int, b: int) int { return a + b; }",
		EXPECTED_TAGS(
			keyword_fn,
			identifier,
			left_brace,
			identifier,
			colon,
			identifier,
			comma,
			identifier,
			colon,
			identifier,
			right_brace,
			identifier,
			left_curly,
			keyword_return,
			identifier,
			plus,
			identifier,
			semicolon,
			right_curly,
			eof
	));

	test("variable declaration",
		"const foo: int; mut bar: int;",
		EXPECTED_TAGS(
			keyword_const,
			identifier,
			colon,
			identifier,
			semicolon,
			keyword_mut,
			identifier,
			colon,
			identifier,
			semicolon,
			eof
	));

	test("variable definition",
		"const foo = 10; mut bar = 20;",
		EXPECTED_TAGS(
			keyword_const,
			identifier,
			assign,
			numliteral,
			semicolon,
			keyword_mut,
			identifier,
			assign,
			numliteral,
			semicolon,
			eof
	));

	test("struct definition",
		"struct Point { x: int, y: int, }",
		EXPECTED_TAGS(
			keyword_struct,
			identifier,
			left_curly,
			identifier,
			colon,
			identifier,
			comma,
			identifier,
			colon,
			identifier,
			comma,
			right_curly,
			eof
	));

	test("struct field access",
		"point.x",
		EXPECTED_TAGS(
			identifier,
			period,
			identifier,
			eof
	));

	test("array declaration",
		"mut arr: [10]int;",
		EXPECTED_TAGS(
			keyword_mut,
			identifier,
			colon,
			left_bracket,
			numliteral,
			right_bracket,
			identifier,
			semicolon,
			eof
	));

	test("enum definition",
		"enum Color { red, green, blue, }",
		EXPECTED_TAGS(
			keyword_enum,
			identifier,
			left_curly,
			identifier,
			comma,
			identifier,
			comma,
			identifier,
			comma,
			right_curly,
			eof
	));

	test("union definition",
		"union Data { i: int, f: float, }",
		EXPECTED_TAGS(
			keyword_union,
			identifier,
			left_curly,
			identifier,
			colon,
			identifier,
			comma,
			identifier,
			colon,
			identifier,
			comma,
			right_curly,
			eof
	));

	test("if else chain",
		"if (a < b) a = b; else if (a > b) a = b; else a = 0;",
		EXPECTED_TAGS(
			keyword_if,
			left_brace,
			identifier,
			less,
			identifier,
			right_brace,
			identifier,
			assign,
			identifier,
			semicolon,
			keyword_else,
			keyword_if,
			left_brace,
			identifier,
			greater,
			identifier,
			right_brace,
			identifier,
			assign,
			identifier,
			semicolon,
			keyword_else,
			identifier,
			assign,
			numliteral,
			semicolon,
			eof
	));

	test("for loop",
		"for (mut i: int; i < 10; i += 1) {}",
		EXPECTED_TAGS(
			keyword_for,
			left_brace,
			keyword_mut,
			identifier,
			colon,
			identifier,
			semicolon,
			identifier,
			less,
			numliteral,
			semicolon,
			identifier,
			plus_equal,
			numliteral,
			right_brace,
			left_curly,
			right_curly,
			eof
	));

	test("function piping",
		"foo() -> bar() -> baz();",
		EXPECTED_TAGS(
			identifier,
			left_brace,
			right_brace,
			fn_pipe,
			identifier,
			left_brace,
			right_brace,
			fn_pipe,
			identifier,
			left_brace,
			right_brace,
			semicolon,
			eof
	));

	test("string literal",
		"\"hello world\"",
		EXPECTED_TAGS(
			stringliteral,
			eof
	));

	test("multiline string literal",
		"\"hello\" \"world\"",
		EXPECTED_TAGS(
			stringliteral,
			stringliteral,
			eof
	));

	test("logic and bit ops",
		"a && b || c ^^ d & e | f ^ g << 1 >> 2;",
		EXPECTED_TAGS(
			identifier,
			logic_and,
			identifier,
			logic_or,
			identifier,
			logic_xor,
			identifier,
			bit_and,
			identifier,
			bit_or,
			identifier,
			bit_xor,
			identifier,
			bitshift_left,
			numliteral,
			bitshift_right,
			numliteral,
			semicolon,
			eof
	));

	test("keyword boundaries",
		"const constant const1 mut mutable fnx structy enumz unioned returnValue formless iffy elsex",
		EXPECTED_TAGS(
			keyword_const,
			identifier,
			identifier,
			keyword_mut,
			identifier,
			identifier,
			identifier,
			identifier,
			identifier,
			identifier,
			identifier,
			identifier,
			eof
	));

	test("operator chain no whitespace",
		"a+=b-=c*=d/=e%=f<<=g>>=h==i!=j<=k>=l&&m||n^^o&p|q^r<s>t",
		EXPECTED_TAGS(
			identifier,
			plus_equal,
			identifier,
			minus_equal,
			identifier,
			mul_equal,
			identifier,
			div_equal,
			identifier,
			mod_equal,
			identifier,
			bitshift_left,
			assign,
			identifier,
			bitshift_right,
			assign,
			identifier,
			equals,
			identifier,
			not_equals,
			identifier,
			less_equals,
			identifier,
			greater_equals,
			identifier,
			logic_and,
			identifier,
			logic_or,
			identifier,
			logic_xor,
			identifier,
			bit_and,
			identifier,
			bit_or,
			identifier,
			bit_xor,
			identifier,
			less,
			identifier,
			greater,
			identifier,
			eof
	));

	test("deep nesting",
		"fn f(a: int){if((a<10)&&((a+1)>0)){for(mut i:int;i<10;i+=1){}}}",
		EXPECTED_TAGS(
			keyword_fn,
			identifier,
			left_brace,
			identifier,
			colon,
			identifier,
			right_brace,
			left_curly,
			keyword_if,
			left_brace,
			left_brace,
			identifier,
			less,
			numliteral,
			right_brace,
			logic_and,
			left_brace,
			left_brace,
			identifier,
			plus,
			numliteral,
			right_brace,
			greater,
			numliteral,
			right_brace,
			right_brace,
			left_curly,
			keyword_for,
			left_brace,
			keyword_mut,
			identifier,
			colon,
			identifier,
			semicolon,
			identifier,
			less,
			numliteral,
			semicolon,
			identifier,
			plus_equal,
			numliteral,
			right_brace,
			left_curly,
			right_curly,
			right_curly,
			right_curly,
			eof
	));

	test("field call pipe madness",
		"a.b().c.d(e,f).g()->h().i.j();",
		EXPECTED_TAGS(
			identifier,
			period,
			identifier,
			left_brace,
			right_brace,
			period,
			identifier,
			period,
			identifier,
			left_brace,
			identifier,
			comma,
			identifier,
			right_brace,
			period,
			identifier,
			left_brace,
			right_brace,
			fn_pipe,
			identifier,
			left_brace,
			right_brace,
			period,
			identifier,
			period,
			identifier,
			left_brace,
			right_brace,
			semicolon,
			eof
	));

	test("numeric torture",
		"0 00 0000 0x0 0xdead 0o777 0b101010 1234567890 0.0001 999.999",
		EXPECTED_TAGS(
			numliteral,
			numliteral,
			numliteral,
			numliteral,
			numliteral,
			numliteral,
			numliteral,
			numliteral,
			numliteral,
			numliteral,
			eof
	));

	test("unary torture",
		"!~!-a + -b * !c;",
		EXPECTED_TAGS(
			logic_not,
			bit_not,
			logic_not,
			minus,
			identifier,
			plus,
			minus,
			identifier,
			asterix,
			logic_not,
			identifier,
			semicolon,
			eof
	));

	test("whitespace insanity",
		"\n\t  const\tfoo\n=\n123\t;\n",
		EXPECTED_TAGS(
			keyword_const,
			identifier,
			assign,
			numliteral,
			semicolon,
			eof
	));

	test("everything combined",
		"struct S{a:int,b:int,} fn main(){mut x:[10]int; x[0]=1; if(x[0]>=1&&x[0]<=10){x[0]+=1;} return x[0];}",
		EXPECTED_TAGS(
			keyword_struct,
			identifier,
			left_curly,
			identifier,
			colon,
			identifier,
			comma,
			identifier,
			colon,
			identifier,
			comma,
			right_curly,
			keyword_fn,
			identifier,
			left_brace,
			right_brace,
			left_curly,
			keyword_mut,
			identifier,
			colon,
			left_bracket,
			numliteral,
			right_bracket,
			identifier,
			semicolon,
			identifier,
			left_bracket,
			numliteral,
			right_bracket,
			assign,
			numliteral,
			semicolon,
			keyword_if,
			left_brace,
			identifier,
			left_bracket,
			numliteral,
			right_bracket,
			greater_equals,
			numliteral,
			logic_and,
			identifier,
			left_bracket,
			numliteral,
			right_bracket,
			less_equals,
			numliteral,
			right_brace,
			left_curly,
			identifier,
			left_bracket,
			numliteral,
			right_bracket,
			plus_equal,
			numliteral,
			semicolon,
			right_curly,
			keyword_return,
			identifier,
			left_bracket,
			numliteral,
			right_bracket,
			semicolon,
			right_curly,
			eof
	));

	return 0;
}

