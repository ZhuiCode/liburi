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

/* 'host' property accessors */

size_t
uri_host(const URI *restrict uri, char *restrict buf, size_t buflen)
{
	if(!uri->hoststr)
	{
		if(buf && buflen)
		{
			*buf = 0;
		}
		return 0;
	}
	if(buf && buflen)
	{
		strncpy(buf, uri->hoststr, buflen - 1);
		buf[buflen - 1] = 0;
	}
	return strlen(uri->hoststr) + 1;
}

/* Return the host as const string pointer */
const char *
uri_host_str(const URI *uri)
{
	return uri->hoststr;
}

/* Return the host as a newly-allocated string (which must be freed by
 * the caller)
 */
char *
uri_host_stralloc(const URI *uri)
{
	if(!uri->hoststr)
	{
		errno = 0;
		return NULL;
	}
	return strdup(uri->hoststr);
}

/* Set a new host (or remove it if newhost is NULL) */
int
uri_set_host(URI *restrict uri, const char *restrict newhost)
{
	char *sbuf;
	
	if(newhost)
	{
		sbuf = strdup(newhost);
		if(!sbuf)
		{
			return -1;
		}
		newhost = sbuf;
	}
	free(uri->hoststr);
	uri->hoststr = sbuf;
	uri->uri.hostText.first = sbuf;
	if(sbuf)
	{
		uri->uri.hostText.afterLast = strchr(sbuf, 0);
	}
	else
	{
		uri->uri.hostText.afterLast = NULL;
	}
	return 0;
}
