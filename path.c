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

static int uri_addch_(int ch, char *restrict *restrict buf, size_t *restrict buflen);

/* 'path' property accessors */

size_t
uri_path(const URI *restrict uri, char *restrict buf, size_t buflen)
{
	size_t total, len;
	char *bp;
	UriPathSegmentA *p;
	
	if(!uri->uri.pathHead && !uri->uri.absolutePath)
	{
		if(buf && buflen)
		{
			*buf = 0;
		}
		return 0;
	}
	total = 0;
	bp = buf;
	if(uri_absolute_path(uri))
	{
		uri_addch_('/', &buf, &buflen);
		total++;
	}
	if(buf && buflen)
	{
		*buf = 0;
	}
	for(p = uri->pathfirst; p; p = p->next)
	{
		if(p != uri->pathfirst)
		{
			uri_addch_('/', &buf, &buflen);
			total++;
		}
		if(!p->text.first)
		{
			return (size_t) -1;
		}
		if(!p->text.first[0])
		{
			continue;
		}
		len = strlen(p->text.first) + 1;
		if(buf)
		{
			strncpy(buf, p->text.first, buflen - 1);
			buf[buflen - 1] = 0;
		}
		len--;
		total += len;
		if(buflen < len)
		{
			buflen = 0;
			buf = NULL;
		}
		else
		{
			if(buf)
			{
				buf += len;
			}
			buflen -= len;
		}
	}
	return total + 1;
}

/* Return 1 if the URI's path is absolute, 0 otherwise */
int
uri_absolute_path(const URI *uri)
{
	return uri->pathabs || (uri->absolute && !uri->hier) || uri->hoststr;
}

/* Add a single character to buf, provided it has space */
static int
uri_addch_(int ch, char *restrict *restrict buf, size_t *restrict buflen)
{
	if(*buf && *buflen)
	{
		**buf = ch;
		(*buf)++;
		**buf = 0;
		(*buflen)--;
	}
	return 1;
}
