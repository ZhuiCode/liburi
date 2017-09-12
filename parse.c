/* Author: Mo McRoberts <mo.mcroberts@bbc.co.uk>
 *
 * Copyright 2015-2017 BBC
 */

/*
 * Copyright 2012 Mo McRoberts.
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

#include "p_liburi.h"

static const char *uri_schemeend_(const char *str);
static int uri_parse_nonhier_(URI *restrict dest, const char *uristr);

static char *uri_range_copy_(UriTextRangeA *range);
static int uri_range_set_(UriTextRangeA *range, const char *src, UriBool owner);

/* Create a URI from a 7-bit ASCII string, which we consider to be the
 * native form.
 * Use uri_create_str(), uri_create_wstr(), or uri_create_ustr() if the
 * source string is not plain ASCII.
 */
URI *
uri_create_ascii(const char *restrict str, const URI *restrict base)
{
	URI *uri;
	UriParserStateA state;
	const char *t;
	
	uri = uri_create_();
	if(!uri)
	{
		return NULL;
	}
	/* Deal with non-hierarchical URIs properly:
	 * Scan the string for the end of the scheme, If the character immediately
	 * following the colon is not a forward-slash, we consider the URI
	 * non-hierarchical and parse it accordingly.
	 */
	t = uri_schemeend_(str);
	if(t && t[0] && t[1] != '/')
	{
		/* A scheme is present and the first character after the colon
		 * is not slash
		 */
		uri->hier = 0;
		if(uri_parse_nonhier_(uri, str))
		{
			uri_destroy(uri);
			return NULL;
		}
	}
	else
	{
		uri->hier = 1;
		state.uri = &(uri->uri);
		if(uriParseUriA(&state, str) != URI_SUCCESS)
		{
			uri_destroy(uri);
			return NULL;
		}
	}
	if(uri_postparse_(uri) || uri_rebase(uri, base))
	{
		uri_destroy(uri);
		return NULL;
	}
	return uri;
}

/* Internal: find the first character after the URI scheme; this function will
 * therefore return either NULL, or a pointer to a colon.
 */
static const char *
uri_schemeend_(const char *str)
{
	for(; str && *str; str++)
	{
		if(*str == ':')
		{
			return str;
		}
		/* These characters cannot appear within a scheme, and so
		 * indicate that the scheme is absent.
		 */
		if(*str == '@' || *str == '/' || *str == '%')
		{
			break;
		}
	}
	return NULL;
}

/* Internal: parse a non-hierarchical URI
 * This is a URI in the form:
 *
 * scheme ':' authority ':' namespace-specific + '?' query + '#' + fragment
 *
 * The namespace-specific segment (NSS) is considered to be an opaque string.
*/
static int
uri_parse_nonhier_(URI *restrict dest, const char *uristr)
{
	return -1;
}

/* Internal: perform post-parsing normalisation and manipulation of a URI */
int
uri_postparse_(URI *uri)
{
	uriNormalizeSyntaxA(&(uri->uri));
	/* Copy the UriUriA text ranges into new buffers, then set the
	 * ranges to point back to our new buffers. This means that the uriparser
	 * APIs will still work on our UriUriA object, but we can manipulate the
	 * components as needed.
	 */
	uri->scheme = uri_range_copy_(&(uri->uri.scheme));
	uri_range_set_(&(uri->uri.scheme), uri->scheme, uri->uri.owner);
	uri->auth = uri_range_copy_(&(uri->uri.userInfo));
	uri_range_set_(&(uri->uri.userInfo), uri->auth, uri->uri.owner);
	uri->hoststr = uri_range_copy_(&(uri->uri.hostText));
	uri_range_set_(&(uri->uri.hostText), uri->hoststr, uri->uri.owner);
	uri->portstr = uri_range_copy_(&(uri->uri.portText));
	uri_range_set_(&(uri->uri.portText), uri->portstr, uri->uri.owner);
	uri->query = uri_range_copy_(&(uri->uri.query));
	uri_range_set_(&(uri->uri.query), uri->query, uri->uri.owner);
	uri->fragment = uri_range_copy_(&(uri->uri.fragment));
	uri_range_set_(&(uri->uri.fragment), uri->fragment, uri->uri.owner);
	/* Copy the path data */
	/* Copy the host data */
	/* Parse the port number, if present */
	uri->uri.owner = URI_FALSE;
	return 0;
}

/* Internal: copy a UriTextRange to a newly-allocated buffer */
static char *
uri_range_copy_(UriTextRangeA *range)
{
	size_t l;
	char *buf;
	
	if(range->first && range->afterLast)
	{
		l = range->afterLast - range->first + 1;
	}
	else if(range->first)
	{
		l = strlen(range->first) + 1;
	}
	else
	{
		errno = 0;
		return NULL;
	}
	if(!l)
	{
		errno = 0;
		return NULL;
	}
	buf = (char *) malloc(l);
	if(!buf)
	{
		return NULL;
	}
	strncpy(buf, range->first, l);
	buf[l - 1] = 0;
	fprintf(stderr, "copied buffer [%s]\n", buf);
	return buf;
}

/* Internal: free a UriTextRange if it's owned by the UriUri, and then
 * set it to point to the supplied null-terminated string. Note that
 * uri->owner must be set to URI_FALSE after calling this or else
 * uriFreeUriMembersA() will free heap blocks it doesn't own.
 * src may be NULL. owner must be uri->owner.
 */
static int
uri_range_set_(UriTextRangeA *range, const char *src, UriBool owner)
{
	/* If the UriUri owns the range, and it's present, free it
	 * before replacing. The owner flag must be cleared on the UriUri
	 * instance after calling this function.
	 */
	if(owner && range->first && range->afterLast != range->first)
	{
		free((URI_CHAR *) range->first);
	}
	/* Set the range to point to the entire length of src (provided src
	 * is non-NULL)
	 */
	range->first = src;
	if(src)
	{
		range->afterLast = strchr(src, 0);
	}
	else
	{
		range->afterLast = NULL;
	}
	return 0;
}
