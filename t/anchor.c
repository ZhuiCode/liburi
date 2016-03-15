/* http://trac.webkit.org/browser/trunk/LayoutTests/fast/url/anchor.html */

#include "lib/p_tests.h"

static const char *tests[] = {
	"http://www.example.com/#hello, world", "http://www.example.com/#hello, world",
	"http://www.example.com/#Â©", "http://www.example.com/#Â©",
	"http://www.example.com/#𐌀ss", "http://www.example.com/#𐌀ss",
	"http://www.example.com/#%41%a", "http://www.example.com/#%41%a",
	"http://www.example.com/#\u0d800\u597d", "http://www.example.com/#�好",
	"http://www.example.com/#a\uFDD0", "http://www.example.com/#a",
	"http://www.example.com/#asdf#qwer", "http://www.example.com/#asdf#qwer",
	"http://www.example.com/##asdf", "http://www.example.com/##asdf",
	"http://www.example.com/#a\nb\rc\td", "http://www.example.com/#abcd",
	NULL
};

int
main(void)
{
	size_t c;
	const char *src;
	int r;
	
	r = 0;
	for(c = 0; tests[c]; c++)
	{
		src = tests[c];
		if(!src)
		{
			break;
		}
		c++;
		if(test_recomposed(__FILE__, src, tests[c], NULL))
		{
			r = FAIL;
		}
	}
	return r;
}
