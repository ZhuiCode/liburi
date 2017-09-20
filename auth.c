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

/* 'auth' property accessors */

size_t
uri_auth(const URI *restrict uri, char *restrict buf, size_t buflen)
{
	if(!uri->auth)
	{
		if(buf && buflen)
		{
			*buf = 0;
		}
		return 0;
	}
	if(buf && buflen)
	{
		strncpy(buf, uri->auth, buflen - 1);
		buf[buflen - 1] = 0;
	}
	return strlen(uri->auth) + 1;
}

/* Return the auth as const string pointer */
const char *
uri_auth_str(const URI *uri)
{
	return uri->auth;
}

/* Return the auth as a newly-allocated string (which must be freed by
 * the caller)
 */
char *
uri_auth_stralloc(const URI *uri)
{
	if(!uri->auth)
	{
		errno = 0;
		return NULL;
	}
	return strdup(uri->auth);
}

/* Set a new auth (or remove it if newauth is NULL) */
int
uri_set_auth(URI *restrict uri, const char *restrict newauth)
{
	char *sbuf;
	
	if(newauth)
	{
		sbuf = strdup(newauth);
		if(!sbuf)
		{
			return -1;
		}
		newauth = sbuf;
	}
	free(uri->auth);
	uri->auth = sbuf;
	uri->uri.userInfo.first = sbuf;
	if(sbuf)
	{
		uri->uri.userInfo.afterLast = strchr(sbuf, 0);
	}
	else
	{
		uri->uri.userInfo.afterLast = NULL;
	}
	/* Reset the user and password components so that they will be
	 * re-parsed
	 */
	free(uri->user);
	free(uri->password);
	uri->user = NULL;
	uri->password = NULL;
	return 0;
}
