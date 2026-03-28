#include "../src/tokenizer.h"
#include "../misc/ArrayList.h"
#include <stddef.h>
#include <stdio.h>

#define EXPECTED_TAGS(...) (TokenTag[]){__VA_ARGS__}
#define success "\x1b[38;5;40m"
#define failure "\x1b[38;5;160m"
#define reset  "\x1b[0m"
#define fail -1

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
	if (expected[0] == fail) {
		printf(failure"%s failed as expected\n"reset, name);
		return;
	}
	TokenArrayList arr = tokenizer_tokenize(str);
	size_t idx = 0;
	for (; expected[idx] != eof; ++idx) {
		if (arr[idx].tag != expected[idx]) {
			size_t start = arr[idx].start;
			size_t end = arr[idx].end;
			printf("%.*s`\n", end - start, str + start);
			printf("%s\n", tagNames[arr[idx].tag]);
			printf(failure"%s failed idx:%lu expected:%s got:%s\n"reset, name, idx, tagNames[expected[idx]], tagNames[arr[idx].tag]);
			ArrayList_free(arr);
			return;
		}
	}
	printf(success"%s succeded\n"reset, name);
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

	test("return statement simple",
		"return 42;",
		EXPECTED_TAGS(
			keyword_return,
			numliteral,
			semicolon,
			eof
		));

	test("return expression",
		"return a + b * c;",
		EXPECTED_TAGS(
			keyword_return,
			identifier,
			plus,
			identifier,
			asterix,
			identifier,
			semicolon,
			eof
		));

	test("nested struct access",
		"a.b.c.d",
		EXPECTED_TAGS(
			identifier,
			period,
			identifier,
			period,
			identifier,
			period,
			identifier,
			eof
		));

	test("array indexing",
		"arr[10]",
		EXPECTED_TAGS(
			identifier,
			left_bracket,
			numliteral,
			right_bracket,
			eof
		));

	test("array indexing expression",
		"arr[i+1]",
		EXPECTED_TAGS(
			identifier,
			left_bracket,
			identifier,
			plus,
			numliteral,
			right_bracket,
			eof
		));

	test("assignment with expression",
		"x = a + b * c;",
		EXPECTED_TAGS(
			identifier,
			assign,
			identifier,
			plus,
			identifier,
			asterix,
			identifier,
			semicolon,
			eof
		));

	test("compound assignments",
		"a += 1; b -= 2; c *= 3; d /= 4; e %= 5;",
		EXPECTED_TAGS(
			identifier,
			plus_equal,
			numliteral,
			semicolon,
			identifier,
			minus_equal,
			numliteral,
			semicolon,
			identifier,
			mul_equal,
			numliteral,
			semicolon,
			identifier,
			div_equal,
			numliteral,
			semicolon,
			identifier,
			mod_equal,
			numliteral,
			semicolon,
			eof
		));

	test("bitwise operations",
		"a & b | c ^ d ~e;",
		EXPECTED_TAGS(
			identifier,
			bit_and,
			identifier,
			bit_or,
			identifier,
			bit_xor,
			identifier,
			bit_not,
			identifier,
			semicolon,
			eof
		));

	test("comparison chain",
		"a < b <= c > d >= e == f != g;",
		EXPECTED_TAGS(
			identifier,
			less,
			identifier,
			less_equals,
			identifier,
			greater,
			identifier,
			greater_equals,
			identifier,
			equals,
			identifier,
			not_equals,
			identifier,
			semicolon,
			eof
		));

	test("function call with args",
		"foo(a,b,c)",
		EXPECTED_TAGS(
			identifier,
			left_brace,
			identifier,
			comma,
			identifier,
			comma,
			identifier,
			right_brace,
			eof
		));

	test("nested function calls",
		"foo(bar(baz()))",
		EXPECTED_TAGS(
			identifier,
			left_brace,
			identifier,
			left_brace,
			identifier,
			left_brace,
			right_brace,
			right_brace,
			right_brace,
			eof
		));

	test("empty struct",
		"struct A {}",
		EXPECTED_TAGS(
			keyword_struct,
			identifier,
			left_curly,
			right_curly,
			eof
		));

	test("empty enum",
		"enum E {}",
		EXPECTED_TAGS(
			keyword_enum,
			identifier,
			left_curly,
			right_curly,
			eof
		));

	test("empty union",
		"union U {}",
		EXPECTED_TAGS(
			keyword_union,
			identifier,
			left_curly,
			right_curly,
			eof
		));

	test("for loop one-liner",
		"for (mut i:int;i<10;i+=1) i=i+1;",
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
			identifier,
			assign,
			identifier,
			plus,
			numliteral,
			semicolon,
			eof
		));

	test("if one-liner",
		"if(a==b) a=b;",
		EXPECTED_TAGS(
			keyword_if,
			left_brace,
			identifier,
			equals,
			identifier,
			right_brace,
			identifier,
			assign,
			identifier,
			semicolon,
			eof
		));

	test("if else blocks",
		"if(a){b=1;}else{b=2;}",
		EXPECTED_TAGS(
			keyword_if,
			left_brace,
			identifier,
			right_brace,
			left_curly,
			identifier,
			assign,
			numliteral,
			semicolon,
			right_curly,
			keyword_else,
			left_curly,
			identifier,
			assign,
			numliteral,
			semicolon,
			right_curly,
			eof
		));

	test("string and identifier mix",
		"foo \"bar\" baz",
		EXPECTED_TAGS(
			identifier,
			stringliteral,
			identifier,
			eof
		));

	test("multiple pipes and math",
		"a()->b()+c()->d();",
		EXPECTED_TAGS(
			identifier,
			left_brace,
			right_brace,
			fn_pipe,
			identifier,
			left_brace,
			right_brace,
			plus,
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

	test("complex mixed expression",
		"(a+b)*(c-d)/(e%f);",
		EXPECTED_TAGS(
			left_brace,
			identifier,
			plus,
			identifier,
			right_brace,
			asterix,
			left_brace,
			identifier,
			minus,
			identifier,
			right_brace,
			div_op,
			left_brace,
			identifier,
			mod,
			identifier,
			right_brace,
			semicolon,
			eof
		));

	test("operator ambiguity torture",
		"<<= >>= == != <= >= && || ^^ & | ^ < > =",
		EXPECTED_TAGS(
			bitshift_left,
			assign,
			bitshift_right,
			assign,
			equals,
			not_equals,
			less_equals,
			greater_equals,
			logic_and,
			logic_or,
			logic_xor,
			bit_and,
			bit_or,
			bit_xor,
			less,
			greater,
			assign,
			eof
		));

	test("max munch hell",
		"a<<=b>>=c==d!=e<=f>=g&&h||i^^j",
		EXPECTED_TAGS(
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
			eof
		));

	test("keyword prefix collisions",
		"ifif elseelse forfor returnreturn fnfn mutmut constconst",
		EXPECTED_TAGS(
			identifier,
			identifier,
			identifier,
			identifier,
			identifier,
			identifier,
			identifier,
			eof
		));

	test("keywords glued to symbols",
		"if(a){return(b);}else{for(i=0;i<1;i+=1){}}",
		EXPECTED_TAGS(
			keyword_if,
			left_brace,
			identifier,
			right_brace,
			left_curly,
			keyword_return,
			left_brace,
			identifier,
			right_brace,
			semicolon,
			right_curly,
			keyword_else,
			left_curly,
			keyword_for,
			left_brace,
			identifier,
			assign,
			numliteral,
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
			eof
		));

	// TODO: CHECK THE BEHAVIOUR
	test("number edge boundaries",
		"0x 0o 0b 0xg 0b2 0o9",
		EXPECTED_TAGS(
			fail
		));

	test("dot vs number ambiguity",
		".1 1. . .a a.",
		EXPECTED_TAGS(
			period,
			numliteral,
			numliteral,
			period,
			period,
			identifier,
			identifier,
			period,
			eof
		));

	test("string escape chaos",
		"\"\" \"\\n\" \"\\\"\" \"a\\tb\"",
		EXPECTED_TAGS(
			stringliteral,
			stringliteral,
			stringliteral,
			stringliteral,
			eof
		));

	test("unterminated string fallback",
		"\"hello",
		EXPECTED_TAGS(
			stringliteral,
			eof
		));

	test("deeply interleaved brackets",
		"([{([{a}])}])",
		EXPECTED_TAGS(
			left_brace,
			left_bracket,
			left_curly,
			left_brace,
			left_bracket,
			left_curly,
			identifier,
			right_curly,
			right_bracket,
			right_brace,
			right_curly,
			right_bracket,
			right_brace,
			eof
		));

	test("array + call + field + pipe nightmare",
		"a[0].b(c,d)[1].e()->f()[g].h;",
		EXPECTED_TAGS(
			identifier,
			left_bracket,
			numliteral,
			right_bracket,
			period,
			identifier,
			left_brace,
			identifier,
			comma,
			identifier,
			right_brace,
			left_bracket,
			numliteral,
			right_bracket,
			period,
			identifier,
			left_brace,
			right_brace,
			fn_pipe,
			identifier,
			left_brace,
			right_brace,
			left_bracket,
			identifier,
			right_bracket,
			period,
			identifier,
			semicolon,
			eof
		));

	test("unary operator stacking nightmare",
		"!!!~~~!!a",
		EXPECTED_TAGS(
			logic_not,
			logic_not,
			logic_not,
			bit_not,
			bit_not,
			bit_not,
			logic_not,
			logic_not,
			identifier,
			eof
		));

	test("mixed unary and binary ambiguity",
		"-a*-b--c",
		EXPECTED_TAGS(
			minus,
			identifier,
			asterix,
			minus,
			identifier,
			minus,
			minus,
			identifier,
			eof
		));

	test("semicolon flooding",
		";;;;;",
		EXPECTED_TAGS(
			semicolon,
			semicolon,
			semicolon,
			semicolon,
			semicolon,
			eof
		));

	test("comma flooding",
		",,,,,",
		EXPECTED_TAGS(
			comma,
			comma,
			comma,
			comma,
			comma,
			eof
		));

	test("nested pipes and calls extreme",
		"a()->b()->c(d()->e())->f;",
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
			identifier,
			left_brace,
			right_brace,
			fn_pipe,
			identifier,
			left_brace,
			right_brace,
			right_brace,
			fn_pipe,
			identifier,
			semicolon,
			eof
		));

	test("whitespace pathological mix",
		"\t\n a\t=\n\tb \n+\t c ;\n",
		EXPECTED_TAGS(
			identifier,
			assign,
			identifier,
			plus,
			identifier,
			semicolon,
			eof
		));

	test("identifier vs number boundary",
		"a1 1a a1b2 123abc",
		EXPECTED_TAGS(
			identifier,
			numliteral,
			identifier,
			identifier,
			numliteral,
			identifier,
			eof
		));

	test("everything glued together no spaces",
		"fnx(a:int){returnx+y*z<<2>>1&&a||b^^c;}",
		EXPECTED_TAGS(
			identifier,
			left_brace,
			identifier,
			colon,
			identifier,
			right_brace,
			left_curly,
			identifier,
			plus,
			identifier,
			asterix,
			identifier,
			bitshift_left,
			numliteral,
			bitshift_right,
			numliteral,
			logic_and,
			identifier,
			logic_or,
			identifier,
			logic_xor,
			identifier,
			semicolon,
			right_curly,
			eof
		));

	test("extreme nesting with everything",
		"fn f(){if(a){for(mut i:int;i<3;i+=1){x=x+1;}}else{return;}}",
		EXPECTED_TAGS(
			keyword_fn,
			identifier,
			left_brace,
			right_brace,
			left_curly,
			keyword_if,
			left_brace,
			identifier,
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
			identifier,
			assign,
			identifier,
			plus,
			numliteral,
			semicolon,
			right_curly,
			right_curly,
			keyword_else,
			left_curly,
			keyword_return,
			semicolon,
			right_curly,
			right_curly,
			eof
		));

	test("ultra max munch chain",
		"<<<<=>>>=><<==>>==!=!===",
		EXPECTED_TAGS(
			bitshift_left,
			bitshift_left,
			assign,
			bitshift_right,
			greater_equals,
			greater,
			bitshift_left,
			equals,
			bitshift_right,
			equals,
			not_equals,
			not_equals,
			equals,
			eof
		));

	test("alternating operator hell",
		"+-*/%+-*/%+-*/%",
		EXPECTED_TAGS(
			plus,
			minus,
			asterix,
			div_op,
			mod,
			plus,
			minus,
			asterix,
			div_op,
			mod,
			plus,
			minus,
			asterix,
			div_op,
			mod,
			eof
		));

	test("identifier fragmentation",
		"a_b_c_d e__f g___h ____ i",
		EXPECTED_TAGS(
			identifier,
			identifier,
			identifier,
			identifier,
			identifier,
			eof
		));

	// TODO: CHECK THE BEHAVIOUR
	test("number fragmentation madness",
		"1.2.3.4 0x1x2 0b10102 00009",
		EXPECTED_TAGS(
			fail
		));

	test("string adjacency explosion",
		"\"a\"\"b\"\"c\"\"d\"\"e\"",
		EXPECTED_TAGS(
			stringliteral,
			stringliteral,
			stringliteral,
			stringliteral,
			stringliteral,
			eof
		));

	test("string + identifier glue",
		"\"a\"b\"c\"d\"e\"",
		EXPECTED_TAGS(
			stringliteral,
			identifier,
			stringliteral,
			identifier,
			stringliteral,
			eof
		));

	test("bracket type confusion",
		"({[)]})",
		EXPECTED_TAGS(
			left_brace,
			left_curly,
			left_bracket,
			right_brace,
			right_bracket,
			right_curly,
			eof
		));

	test("pipe vs minus vs greater ambiguity",
		"a-->b->c--->d",
		EXPECTED_TAGS(
			identifier,
			minus,
			fn_pipe,
			identifier,
			fn_pipe,
			identifier,
			minus,
			minus,
			fn_pipe,
			identifier,
			eof
		));

	test("long unary prefix chain",
		"!!!!!!!!!!~~~~~~~~~~a",
		EXPECTED_TAGS(
			logic_not,logic_not,logic_not,logic_not,logic_not,
			logic_not,logic_not,logic_not,logic_not,logic_not,
			bit_not,bit_not,bit_not,bit_not,bit_not,
			bit_not,bit_not,bit_not,bit_not,bit_not,
			identifier,
			eof
		));

	test("mixed unary binary collapse",
		"!-!-!-a*-!-b",
		EXPECTED_TAGS(
			logic_not,
			minus,
			logic_not,
			minus,
			logic_not,
			minus,
			identifier,
			asterix,
			minus,
			logic_not,
			minus,
			identifier,
			eof
		));

	test("extreme nesting depth",
		"((((((((((((a))))))))))))",
		EXPECTED_TAGS(
			left_brace,left_brace,left_brace,left_brace,left_brace,left_brace,
			left_brace,left_brace,left_brace,left_brace,left_brace,left_brace,
			identifier,
			right_brace,right_brace,right_brace,right_brace,right_brace,right_brace,
			right_brace,right_brace,right_brace,right_brace,right_brace,right_brace,
			eof
		));

	test("call + index interleave infinite style",
		"a()[0]()[1]()[2]()[3]",
		EXPECTED_TAGS(
			identifier,left_brace,right_brace,
			left_bracket,numliteral,right_bracket,
			left_brace,right_brace,
			left_bracket,numliteral,right_bracket,
			left_brace,right_brace,
			left_bracket,numliteral,right_bracket,
			left_brace,right_brace,
			left_bracket,numliteral,right_bracket,
			eof
		));

	test("dense keyword collisions",
		"ifelseforfnreturnmutconststructenumunion",
		EXPECTED_TAGS(
			identifier,
			eof
		));

	test("every operator no whitespace long chain",
		"a+b-c*d/e%f<<g>>h&i|j^k&&l||m^^n<o<=p>q>=r==s!=t",
		EXPECTED_TAGS(
			identifier,plus,identifier,minus,identifier,asterix,identifier,div_op,identifier,mod,identifier,
			bitshift_left,identifier,bitshift_right,identifier,
			bit_and,identifier,bit_or,identifier,bit_xor,identifier,
			logic_and,identifier,logic_or,identifier,logic_xor,identifier,
			less,identifier,less_equals,identifier,
			greater,identifier,greater_equals,identifier,
			equals,identifier,not_equals,identifier,
			eof
		));

	test("pathological semicolon + braces",
		"{;;;;{;;;;};;;;}",
		EXPECTED_TAGS(
			left_curly,
			semicolon,semicolon,semicolon,semicolon,
			left_curly,
			semicolon,semicolon,semicolon,semicolon,
			right_curly,
			semicolon,semicolon,semicolon,semicolon,
			right_curly,
			eof
		));

	test("assignment vs equality confusion",
		"a=b==c=d!=e",
		EXPECTED_TAGS(
			identifier,
			assign,
			identifier,
			equals,
			identifier,
			assign,
			identifier,
			not_equals,
			identifier,
			eof
		));

	// TODO: CHECK THE BEHAVIOUR
	test("float edge madness",
		"0. .0 00.00 1..2",
		EXPECTED_TAGS(
			fail
		));

	test("everything is adjacent chaos",
		"a(b)c[d]e{f}g.h,i;j:k",
		EXPECTED_TAGS(
			identifier,
			left_brace,identifier,right_brace,
			identifier,
			left_bracket,identifier,right_bracket,
			identifier,
			left_curly,identifier,right_curly,
			identifier,
			period,identifier,
			comma,
			identifier,
			semicolon,
			identifier,
			colon,
			identifier,
			eof
		));

	test("insane real-world style compression",
		"fn f(a:int){x=0;for(mut i:int;i<10;i+=1){x+=i;}return x;}",
		EXPECTED_TAGS(
			keyword_fn,
			identifier,
			left_brace,
			identifier,
			colon,
			identifier,
			right_brace,
			left_curly,
			identifier,
			assign,
			numliteral,
			semicolon,
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
			identifier,
			plus_equal,
			identifier,
			semicolon,
			right_curly,
			keyword_return,
			identifier,
			semicolon,
			right_curly,
			eof
		));

	test("fuzz stream 1",
		"fnx->a&&b||c^^d<<2>=e!=f+g*h;",
		EXPECTED_TAGS(
			identifier,
			fn_pipe,
			identifier,
			logic_and,
			identifier,
			logic_or,
			identifier,
			logic_xor,
			identifier,
			bitshift_left,
			numliteral,
			greater_equals,
			identifier,
			not_equals,
			identifier,
			plus,
			identifier,
			asterix,
			identifier,
			semicolon,
			eof
		));

	test("fuzz stream 2",
		"mutx:=[10]int;foo(bar)->baz^^qux;",
		EXPECTED_TAGS(
			identifier,
			colon,
			assign,
			left_bracket,
			numliteral,
			right_bracket,
			identifier,
			semicolon,
			identifier,
			left_brace,
			identifier,
			right_brace,
			fn_pipe,
			identifier,
			logic_xor,
			identifier,
			semicolon,
			eof
		));

	test("fuzz stream 3",
		"(((a+b)<<c)&d)|e^f&&g||h;",
		EXPECTED_TAGS(
			left_brace,
			left_brace,
			left_brace,
			identifier,
			plus,
			identifier,
			right_brace,
			bitshift_left,
			identifier,
			right_brace,
			bit_and,
			identifier,
			right_brace,
			bit_or,
			identifier,
			bit_xor,
			identifier,
			logic_and,
			identifier,
			logic_or,
			identifier,
			semicolon,
			eof
		));

	test("fuzz stream 4",
		"\"x\"y\"z\"+0x1f-0b10*0o7;",
		EXPECTED_TAGS(
			stringliteral,
			identifier,
			stringliteral,
			plus,
			numliteral,
			minus,
			numliteral,
			asterix,
			numliteral,
			semicolon,
			eof
		));

	test("fuzz stream 5",
		"a[1][2][3].b.c()[d]->e(f,g,h);",
		EXPECTED_TAGS(
			identifier,
			left_bracket,numliteral,right_bracket,
			left_bracket,numliteral,right_bracket,
			left_bracket,numliteral,right_bracket,
			period,
			identifier,
			period,
			identifier,
			left_brace,right_brace,
			left_bracket,
			identifier,
			right_bracket,
			fn_pipe,
			identifier,
			left_brace,
			identifier,comma,identifier,comma,identifier,
			right_brace,
			semicolon,
			eof
		));

	test("fuzz stream 6",
		"ifx&&y||z{a=b+c*d/e%f;}elseg",
		EXPECTED_TAGS(
			identifier,
			logic_and,
			identifier,
			logic_or,
			identifier,
			left_curly,
			identifier,
			assign,
			identifier,
			plus,
			identifier,
			asterix,
			identifier,
			div_op,
			identifier,
			mod,
			identifier,
			semicolon,
			right_curly,
			identifier,
			eof
		));

	test("fuzz stream 7",
		"0.1.2.3<<=4>>=5==6!=7<=8>=9;",
		EXPECTED_TAGS(
			invalid,
			bitshift_left,
			assign,
			numliteral,
			bitshift_right,
			assign,
			numliteral,
			equals,
			numliteral,
			not_equals,
			numliteral,
			less_equals,
			numliteral,
			greater_equals,
			numliteral,
			semicolon,
			eof
		));

	test("fuzz stream 8",
		"fn->fn->fn(a)->b()->c[d]<<e;",
		EXPECTED_TAGS(
			keyword_fn,
			fn_pipe,
			keyword_fn,
			fn_pipe,
			keyword_fn,
			left_brace,
			identifier,
			right_brace,
			fn_pipe,
			identifier,
			left_brace,
			right_brace,
			fn_pipe,
			identifier,
			left_bracket,
			identifier,
			right_bracket,
			bitshift_left,
			identifier,
			semicolon,
			eof
		));

	test("fuzz stream 9",
		"~!~!~!a+b-c*d/e%f^g|h&i;",
		EXPECTED_TAGS(
			bit_not,
			logic_not,
			bit_not,
			logic_not,
			bit_not,
			logic_not,
			identifier,
			plus,
			identifier,
			minus,
			identifier,
			asterix,
			identifier,
			div_op,
			identifier,
			mod,
			identifier,
			bit_xor,
			identifier,
			bit_or,
			identifier,
			bit_and,
			identifier,
			semicolon,
			eof
		));

	test("fuzz stream 10",
		"((fnx)(y))->z+((a*b)<<c);",
		EXPECTED_TAGS(
			left_brace,
			left_brace,
			identifier,
			right_brace,
			left_brace,
			identifier,
			right_brace,
			right_brace,
			fn_pipe,
			identifier,
			plus,
			left_brace,
			left_brace,
			identifier,
			asterix,
			identifier,
			right_brace,
			bitshift_left,
			identifier,
			right_brace,
			semicolon,
			eof
		));

	return 0;
}

