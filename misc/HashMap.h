#include <stddef.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/// using FNV-1a hashing algorithm
/// to increase randomness calculate twice the wanted size then xor the halves
uint32_t hash(char *key, size_t len) {
	uint64_t offsetBasis = 0xcbf29ce484222325;
	for (size_t idx = 0; idx < len; idx++) {
		offsetBasis ^= key[idx];
		offsetBasis *= 0x00000100000001b3;
	}

	uint32_t majorBits = offsetBasis >> 32;
	uint32_t minorBits = (uint32_t)offsetBasis;
	return majorBits ^ minorBits;
}

#ifdef PRECALC_HASHES

#define keywordsCount 10

char *keywords[] = {
	"if",
	"fn",
	"mut",
	"for",
	"enum",
	"else",
	"union",
	"const",
	"struct",
	"return",
};

// we precalculate the hashes to create switch state
int main() {
	for (int i = 0; i < keywordsCount; i++) {
		printf("%d\n", hash(keywords[i]));
	}
	return 0;
}
#endif // PRECALC_HASHES
