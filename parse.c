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

static char *uri_range_copy_(const UriTextRangeA *range);
static int uri_range_set_(UriTextRangeA *restrict range, const char *restrict src);
static int uri_path_copy_ref_(UriPathSegmentA **head, UriPathSegmentA **tail, const UriPathSegmentA *src);

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
	 * following the colon is not a slash, we consider the URI
	 * non-hierarchical and parse it accordingly.
	 */
	t = uri_schemeend_(str);
	if(t && t[0] && t[1] != '/' && t[1] != '\\')
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
	if(uri_postparse_(uri))
	{
		uri_destroy(uri);
		return NULL;
	}
	if(uri_rebase(uri, base))
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
		if(*str == '@' || *str == '/' || *str == '%' || *str == '\\')
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
	errno = EPERM;
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
	 *
	 * The UriUriA's owner flag will be set to false, indicating that with the
	 * exception of the ip4 and ip6 structs within its hostData, it does not
	 * own any of the memory its text ranges point at.
	 */
	uri->scheme = uri_range_copy_(&(uri->uri.scheme));
	uri->auth = uri_range_copy_(&(uri->uri.userInfo));
	uri->hoststr = uri_range_copy_(&(uri->uri.hostText));
	uri->portstr = uri_range_copy_(&(uri->uri.portText));
	uri->query = uri_range_copy_(&(uri->uri.query));
	uri->fragment = uri_range_copy_(&(uri->uri.fragment));
	/* Copy the path data */
	if(uri_path_copy_(uri, uri->uri.pathHead))
	{
		return -1;
	}
	/* Copy the host data */
	if((uri->hostdata.ipFuture.first = uri_range_copy_(&(uri->uri.hostData.ipFuture))))
	{
		uri->hostdata.ipFuture.afterLast = strchr(uri->hostdata.ipFuture.first, 0);
	}
	if(uri_hostdata_copy_(&(uri->hostdata), &(uri->uri.hostData)))
	{
		return -1;
	}
	/* Parse the port number, if present */
	if(uri->portstr)
	{
		uri->port = atoi(uri->portstr);
		if(uri->port < 1 || uri->port > 65535)
		{
			uri->port = 0;
		}
	}
	return uri_postparse_set_(uri);
}

/* Replace the members of a UriUriA with pointers to our own buffers
 * excepting ip4 and ip6 within the hostData member, which is always
 * duplicated.
 */
int
uri_postparse_set_(URI *uri)
{
	uriFreeUriMembersA(&(uri->uri));
	uri->uri.owner = URI_FALSE;
	uri_range_set_(&(uri->uri.scheme), uri->scheme);
	uri_range_set_(&(uri->uri.userInfo), uri->auth);
	uri_range_set_(&(uri->uri.hostText), uri->hoststr);
	uri_range_set_(&(uri->uri.portText), uri->portstr);
	uri_range_set_(&(uri->uri.query), uri->query);
	uri_range_set_(&(uri->uri.fragment), uri->fragment);
	uri_range_set_(&(uri->uri.hostData.ipFuture), uri->hostdata.ipFuture.first);
	uri_hostdata_copy_(&(uri->uri.hostData), &(uri->hostdata));
	uri_path_copy_ref_(&(uri->uri.pathHead), &(uri->uri.pathTail), uri->pathfirst);
	uri->uri.absolutePath = (uri->pathabs ? URI_TRUE : URI_FALSE);
	return 0;
}

/* Internal: copy a UriHostData struct, allocating memory as needed */
int
uri_hostdata_copy_(struct UriHostDataStructA *restrict dest, const struct UriHostDataStructA *restrict src)
{	
	if(src->ip4)
	{
		dest->ip4 = (UriIp4 *) calloc(1, sizeof(UriIp4));
		if(!dest->ip4)
		{
			return -1;
		}
		memcpy(dest->ip4, src->ip4, sizeof(UriIp4));
	}
	if(src->ip6)
	{
		dest->ip6 = (UriIp6 *) calloc(1, sizeof(UriIp6));
		if(!dest->ip6)
		{
			return -1;
		}
		memcpy(dest->ip6, src->ip6, sizeof(UriIp6));
	}
	return 0;
}

/* Internal: copy a UriPath into our URI object */
int
uri_path_copy_(URI *uri, const UriPathSegmentA *head)
{
	UriPathSegmentA *seg, *prev;

	prev = NULL;
	uri->pathabs = (int) uri->uri.absolutePath;
	for(; head; head = head->next)
	{
		seg = (UriPathSegmentA *) calloc(1, sizeof(UriPathSegmentA));
		if(!seg)
		{
			return -1;
		}
		seg->text.first = uri_range_copy_(&(head->text));
		if(seg->text.first)
		{
			seg->text.afterLast = strchr(seg->text.first, 0);
		}
		if(prev)
		{
			prev->next = seg;
		}
		else
		{
			uri->pathfirst = seg;
		}
		prev = seg;
	}
	uri->pathlast = prev;
	uri->pathcur = uri->pathfirst;
	return 0;
}

/* Internal: create a new chain of path segments referencing the
 * strings in the source segment; this is used because even if
 * UriUriA::owner is false, the segments themselves will still be
 * freed by uriFreeUriMembersA().
 */
static int
uri_path_copy_ref_(UriPathSegmentA **head, UriPathSegmentA **tail, const UriPathSegmentA *src)
{
	UriPathSegmentA *seg, *prev;

	prev = NULL;
	for(; src; src = src->next)
	{
		seg = (UriPathSegmentA *) calloc(1, sizeof(UriPathSegmentA));
		if(!seg)
		{
			return -1;
		}
		seg->text.first = src->text.first;
		seg->text.afterLast = src->text.afterLast;
		if(prev)
		{
			prev->next = seg;
		}
		else
		{
			*head = seg;
		}
		prev = seg;
	}
	*tail = prev;
	return 0;
}

/* Internal: copy a UriTextRange to a newly-allocated buffer */
static char *
uri_range_copy_(const UriTextRangeA *range)
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
	return buf;
}

/* Internal: free a UriTextRange if it's owned by the UriUri, and then
 * set it to point to the supplied null-terminated string. Note that
 * uri->owner must be set to URI_FALSE after calling this or else
 * uriFreeUriMembersA() will free heap blocks it doesn't own.
 * src may be NULL. owner must be uri->owner.
 */
static int
uri_range_set_(UriTextRangeA *range, const char *src)
{
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
