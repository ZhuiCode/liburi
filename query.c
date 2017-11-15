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

/* 'query' property accessors */

size_t
uri_query(const URI *restrict uri, char *restrict buf, size_t buflen)
{
	if(!uri->query)
	{
		if(buf && buflen)
		{
			*buf = 0;
		}
		return 0;
	}
	if(buf && buflen)
	{
		strncpy(buf, uri->query, buflen - 1);
		buf[buflen - 1] = 0;
	}
	return strlen(uri->query) + 1;
}

/* Return the query as const string pointer */
const char *
uri_query_str(const URI *uri)
{
	return uri->query;
}

/* Return the query as a newly-allocated string (which must be freed by
 * the caller)
 */
char *
uri_query_stralloc(const URI *uri)
{
	if(!uri->query)
	{
		errno = 0;
		return NULL;
	}
	return strdup(uri->query);
}

/* Set a new query (or remove it if newquery is NULL) */
int
uri_set_query(URI *restrict uri, const char *restrict newquery)
{
	char *sbuf;

	sbuf = NULL;
	if(newquery)
	{
		sbuf = strdup(newquery);
		if(!sbuf)
		{
			return -1;
		}
		newquery = sbuf;
	}
	free(uri->query);
	uri->query = sbuf;
	uri->uri.query.first = sbuf;
	if(sbuf)
	{
		uri->uri.query.afterLast = strchr(sbuf, 0);
	}
	else
	{
		uri->uri.query.afterLast = NULL;
	}
	return 0;
}
