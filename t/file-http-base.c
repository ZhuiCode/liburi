/* http://trac.webkit.org/browser/trunk/LayoutTests/fast/url/file-http-base.html */

#include "lib/p_tests.h"

static const char *baseuri = "http://example.com/mock/path";

static const char *tests[] = {
	"file:c:\\\\foo\\\\bar.html", "file:///C:/foo/bar.html",
	"  File:c|////foo\\\\bar.html", "file:///C:////foo/bar.html",
	"file:", "file:///",
	"file:UNChost/path", "file://unchost/path",
	"c:\\\\foo\\\\bar", "file:///C:/foo/bar",
	"C|/foo/bar", "file:///C:/foo/bar",
	"/C|\\\\foo\\\\bar", "file:///C:/foo/bar",
	"//C|/foo/bar", "file:///C:/foo/bar",
	"//server/file", "file://server/file",
	"\\\\\\\\server\\\\file", "file://server/file",
	"/\\\\server/file", "file://server/file",
	"file:c:foo/bar.html", "file:///C:/foo/bar.html",
	"file:/\\\\/\\\\C:\\\\\\\\//foo\\\\bar.html", "file:///C:////foo/bar.html",
	"file:///foo/bar.txt", "file:///foo/bar.txt",
	"FILE:/\\\\/\\\\7:\\\\\\\\//foo\\\\bar.html", "file://7:////foo/bar.html",
	"file:filer/home\\\\me", "file://filer/home/me",
	"file:///C:/foo/../../../bar.html", "file:///C:/bar.html",
	"file:///C:/asdf#\\xc2", "file:///C:/asdf#\\xc2",
	"file:///C:/asdf#\xc2", "file:///C:/asdf#\xc2",
	"file:///home/me", "file:///home/me",
	"file:c:\\\\foo\\\\bar.html", "file:///c:/foo/bar.html",
	"file:c|//foo\\\\bar.html", "file:///c%7C//foo/bar.html",
	"//", "file:///",
	"///", "file:///",
	"///test", "file:///test",
	"file://test", "file://test/",
	"file://localhost",  "file://localhost/",
	"file://localhost/", "file://localhost/",
	"file://localhost/test", "file://localhost/test",
	NULL
};

int
main(void)
{
	size_t c;
	const char *src;
	int r;
	URI *base;
	
	base = uri_create_str(baseuri, NULL);
	if(!base)
	{
		fprintf(stderr, "%s: failed to parse base URI <%s>\n", __FILE__, baseuri);
	}
	r = 0;
	for(c = 0; tests[c]; c++)
	{
		src = tests[c];
		if(!src)
		{
			break;
		}
		c++;
		if(test_recomposed(__FILE__, src, tests[c], base))
		{
			r = FAIL;
		}
	}
	uri_destroy(base);
	return r;
}
