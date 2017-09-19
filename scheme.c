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

/* 'scheme' property accessors */

size_t
uri_scheme(const URI *restrict uri, char *restrict buf, size_t buflen)
{
	if(!uri->scheme)
	{
		if(buf && buflen)
		{
			*buf = 0;
		}
		return 0;
	}
	if(buf && buflen)
	{
		strncpy(buf, uri->scheme, buflen - 1);
		buf[buflen - 1] = 0;
	}
	return strlen(uri->scheme) + 1;
}

/* Return the scheme as const string pointer */
const char *
uri_scheme_str(const URI *uri)
{
	return uri->scheme;
}

/* Return the scheme as a newly-allocated string (which must be freed by
 * the caller)
 */
char *
uri_scheme_stralloc(const URI *uri)
{
	return strdup(uri->scheme);
}

/* Set a new scheme (or remove it if newscheme is NULL) */
int
uri_set_scheme(URI *restrict uri, const char *restrict newscheme)
{
	char *sbuf;
	
	if(newscheme)
	{
		sbuf = strdup(newscheme);
		if(!sbuf)
		{
			return -1;
		}
		newscheme = sbuf;
	}
	free(uri->scheme);
	uri->scheme = sbuf;
	uri->uri.scheme.first = sbuf;
	if(sbuf)
	{
		uri->uri.scheme.afterLast = strchr(sbuf, 0);
	}
	else
	{
		uri->uri.scheme.afterLast = NULL;
	}
	return 0;
}
