#include "include/utils.h"

int __clzsi2(unsigned x)
{
	auto n = 1;
	if ((x & 0x0000FFFF) == 0) { n = n + 16; x = x >> 16; }
	if ((x & 0x000000FF) == 0) { n = n + 8; x = x >> 8; }
	if ((x & 0x0000000F) == 0) { n = n + 4; x = x >> 4; }
	if ((x & 0x00000003) == 0) { n = n + 2; x = x >> 2; }
	return n - (x & 1);
}