/* Author: Mo McRoberts <mo.mcroberts@bbc.co.uk>
 *
 * Copyright 2015-2016 BBC
 *
 * Copyright 2012 Mo McRoberts
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "p_tests.h"

static int testlen(const char *restrict file, const char *name, const URI *restrict uri, struct urimatch *restrict test, size_t (*fn)(const URI *restrict, char *restrict, size_t), size_t expected);

static int teststr(const char *restrict file, const char *name, const URI *restrict uri, char *restrict buf, size_t buflen, const char *restrict testuri, size_t (*fn)(const URI *restrict, char *restrict, size_t), const char *restrict expected);

int
test_urimatch(const char *restrict file, struct urimatch *restrict tests)
{
	URI *uri, *base;
	int c;
	int failed, didfail;
	char *buffer, *p;
	size_t bufsize, bl;
	int r;

	buffer = NULL;
	bufsize = 0;
	failed = 0;
	for(c = 0; tests[c].uri; c++)
	{
		base = NULL;
		bl = 0;
		if(tests[c].base)
		{
			base = uri_create_ustr((const unsigned char *) tests[c].base, NULL);
			bl = strlen(tests[c].base);
			if(!base)
			{
				fprintf(stderr, "%s: failed to construct base URI '%s': %s\n", file, tests[c].base, strerror(errno));
				failed++;
				continue;
			}
		}
		bl += strlen(tests[c].uri);
		uri = uri_create_ustr((const unsigned char *) tests[c].uri, base);
		if(!uri)
		{
			fprintf(stderr, "%s: failed to construct URI '%s': %s\n", file, tests[c].uri, strerror(errno));
			uri_destroy(base);
			failed++;
			continue;
		}
		bl = (bl * 3) + 1;
		if(bl > bufsize)
		{
			p = (char *) realloc(buffer, bl);
			if(!p)
			{
				fprintf(stderr, "%s: failed to resize buffer from %u to %u bytes: %s\n", file, (unsigned) bufsize, (unsigned) bl, strerror(errno));
				exit(2);
			}
			buffer = p;
			bufsize = bl;
		}
		didfail = 0;
#define TESTLEN(name, mask) \
		if(tests[c].testmask & mask) \
		{ \
			didfail += testlen(file, #name, uri, &(tests[c]), uri_##name, tests[c].name##len); \
		}
		TESTLEN(scheme, UM_SCHEME_LEN);
		TESTLEN(auth, UM_AUTH_LEN);
		TESTLEN(host, UM_HOST_LEN);
		TESTLEN(port, UM_PORT_LEN);
		TESTLEN(path, UM_PATH_LEN);
		TESTLEN(query, UM_QUERY_LEN);
		TESTLEN(fragment, UM_FRAGMENT_LEN);
#undef TESTLEN
		if(tests[c].testmask & UM_RECOMPOSED_LEN)
		{
			didfail += testlen(file, "recomposed string", uri, &(tests[c]), uri_str, tests[c].recomposedlen);
		}
		if(tests[c].testmask & UM_ABSOLUTE)
		{
			if((r = uri_absolute(uri)) != tests[c].absolute)
			{
				didfail++;
				if(tests[c].absolute)
				{
					fprintf(stderr, "%s: URI expected to be absolute, but return value was %d for %s\n", file, r, tests[c].uri);
				}
				else
				{
					fprintf(stderr, "%s: URI expected to not be absolute, but return value was %d for %s\n", file, r, tests[c].uri);
				}
			}
		}
		if(tests[c].testmask & UM_PORT)
		{
			if((r = uri_portnum(uri)) != tests[c].portnum)
			{
				didfail++;
				fprintf(stderr, "%s: parsed port number (%d) does not match expected port number (%d) for URI %s\n", file, r, tests[c].portnum, tests[c].uri);
			}
		}
#define TESTSTR(name, mask) \
		if(tests[c].testmask & mask) \
		{ \
			didfail += teststr(file, #name, uri, buffer, bufsize, tests[c].uri, uri_##name, tests[c].name); \
		}
		TESTSTR(scheme, UM_SCHEME);
		TESTSTR(auth, UM_AUTH);
		TESTSTR(host, UM_HOST);
		TESTSTR(port, UM_PORT);
		TESTSTR(path, UM_PATH);
		TESTSTR(query, UM_QUERY);
		TESTSTR(fragment, UM_FRAGMENT);
#undef TESTSTR
		if(tests[c].testmask & UM_RECOMPOSED)
		{
			didfail += teststr(file, "recomposed string", uri, buffer, bufsize, tests[c].uri, uri_str, tests[c].recomposed);
		}
		if(didfail)
		{
			failed++;
		}
		uri_destroy(uri);
		uri_destroy(base);
	}
	free(buffer);
	return failed ? FAIL : PASS;
}

int
test_recomposed(const char *restrict file, const char *restrict src, const char *restrict expected, URI *restrict base)
{
	URI *uri;
	char *buffer;
	size_t buflen;
	int r;
	
	uri = uri_create_ustr((const unsigned char *) src, base);
	if(!uri)
	{
		fprintf(stderr, "%s: failed to construct URI <%s>: %s\n", file, src, strerror(errno));
		return HARDERR;
	}
	buflen = (strlen(src) * 3) + 1;
	buffer = (char *) calloc(1, buflen);
	if(!buffer)
	{
		fprintf(stderr, "%s: failed to allocate buffer for result of recomposing <%s>\n", file, src);
		uri_destroy(uri);
		return HARDERR;
	}
	r = teststr(file, "recomposed string", uri, buffer, buflen, src, uri_str, expected);
	free(buffer);
	uri_destroy(uri);
	return r == 0 ? PASS : FAIL;
}

int
test_urimatch_simple(const char *restrict file, struct urimatch_simple *restrict tests)
{
	URI *uri, *base;
	int failed;
	char *buffer;
	size_t c;

	failed = 0;
	for(c = 0; tests[c].uri; c++)
	{
		base = NULL;
		if(tests[c].base)
		{
			base = uri_create_ustr((const unsigned char *) tests[c].base, NULL);
			if(!base)
			{
				fprintf(stderr, "%s: failed to construct base URI '%s': %s\n", file, tests[c].base, strerror(errno));
				failed++;
				continue;
			}
		}
		uri = uri_create_ustr((const unsigned char *) tests[c].uri, base);
		if(!uri)
		{
			fprintf(stderr, "%s: failed to construct URI '%s': %s\n", file, tests[c].uri, strerror(errno));
			uri_destroy(base);
			failed++;
			continue;
		}
		buffer = uri_stralloc(uri);
		if(!buffer)
		{
			fprintf(stderr, "%s: failed to obtain re-composed URI for '%s': %s\n", file, tests[c].uri, strerror(errno));
			failed++;
		}
		else if(strcmp(buffer, tests[c].expected))
		{
			fprintf(stderr, "%s: re-composed URI <%s> does not match expected string '%s'\n", file, buffer, tests[c].expected);
			if(base)
			{
				free(buffer);
				buffer = uri_stralloc(base);
				fprintf(stderr, "%s: base was <%s>\n", file, buffer);
			}
			else
			{
				fprintf(stderr, "%s: (no base URI provided)\n", file);
			}
			failed++;		  
		}
		else
		{
			fprintf(stderr, "%s: OK: <%s>: matched '%s'\n", file, tests[c].uri, tests[c].expected);
		}
		free(buffer);
		uri_destroy(uri);
		uri_destroy(base);
	}
	return failed == 0 ? PASS : FAIL;
}

static int
testlen(const char *restrict file, const char *name, const URI *restrict uri, struct urimatch *restrict test, size_t (*fn)(const URI *restrict, char *restrict, size_t), size_t expected)
{
	size_t r;

	r = fn(uri, NULL, 0);
	if(r != expected)
	{
		fprintf(stderr, "%s: unexpected buffer size requested for %s (%u, expected %u) for URI '%s'\n", file, name, (unsigned) r, (unsigned) expected, test->uri);
		return 1;
	}
	return 0;
}

static int
teststr(const char *restrict file, const char *name, const URI *restrict uri, char *restrict buffer, size_t buflen, const char *restrict testuri, size_t (*fn)(const URI *restrict, char *restrict, size_t), const char *restrict expected)
{
	size_t r;

	memset(buffer, 0, buflen);
	r = fn(uri, buffer, buflen);
	if(r == (size_t) -1)
	{
		fprintf(stderr, "%s: <%s>: failed to obtain %s\n", file, testuri, name);
		return 1;
	}
	if(r == 0 && expected)
	{
		fprintf(stderr, "%s: <%s>: expected %s '%s', but result was NULL\n", file, testuri, name, expected);
		return 1;
	}
	if(r && !expected)
	{
		fprintf(stderr, "%s: <%s>: expected NULL %s, but result was '%s'\n", file, testuri, name, buffer);
		return 1;
	}
	if(!expected)
	{
		return 0;
	}
	if(strcmp(buffer, expected))
	{
		fprintf(stderr, "%s: <%s>: expected '%s' for %s, but result was '%s'\n", file, testuri, expected, name, buffer);
		return 1;
	}
	fprintf(stderr, "%s: OK: <%s>: matched '%s' for %s\n", file, testuri, expected, name);
	return 0;
}
