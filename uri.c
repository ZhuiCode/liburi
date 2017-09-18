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

/* Create a new URI from an existing URI. If base is provided, the
 * new URI will be rebased against it.
 */
URI *
uri_create_uri(const URI *restrict source, const URI *restrict base)
{
	URI *uri;

	uri = uri_dup_(source);
	if(!uri)
	{
		return NULL;
	}
	if(base)
	{
		if(uri_rebase(uri, base))
		{
			uri_destroy(uri);
			return NULL;
		}
	}
	return uri;
}

/* Destroy a URI object */
int
uri_destroy(URI *uri)
{
	if(uri)
	{
		uri_reset_(uri);
		free(uri);
	}
	return 0;
}	

/* Internal: allocate a new URI object */
URI *
uri_create_(void)
{
	URI *p;
	p = (URI *) calloc(1, sizeof(URI));
	if(!p)
	{
		return NULL;
	}
	
	return p;
}

/* Internal: duplicate an existing URI object */
URI *
uri_dup_(const URI *src)
{
	URI *p;
	
	p = (URI *) calloc(1, sizeof(URI));
	if(!p)
	{
		return NULL;
	}
	/* Copy of each of the string properties */
#define URI_COPYSTR_(dest, src, member) \
	if((src)->member) \
	{ \
		(dest)->member = strdup((src)->member); \
		if(!(dest)->member) \
		{ \
			uri_destroy((dest)); \
			return NULL; \
		} \
	}
	URI_COPYSTR_(p, src, scheme);
	URI_COPYSTR_(p, src, auth);
	URI_COPYSTR_(p, src, user);
	URI_COPYSTR_(p, src, password);
	URI_COPYSTR_(p, src, hoststr);
	URI_COPYSTR_(p, src, portstr);
	URI_COPYSTR_(p, src, authority);
	URI_COPYSTR_(p, src, nss);
	URI_COPYSTR_(p, src, query);
	URI_COPYSTR_(p, src, fragment);
	URI_COPYSTR_(p, src, composed);
#undef URI_COPYSTR_
	/* Copy the host info */
	uri_hostdata_copy_(&(p->hostdata), &(src->hostdata));
	p->port = src->port;
	/* Copy the path */
	uri_path_copy_(p, src->pathfirst);
	/* Copy the flags */
	p->pathabs = src->pathabs;
	/* Now set the UriUri members to point to the new strings */
	uri_postparse_set_(p);
	return p;
}

/* Internal: safely free the contents of a URI object, so that they can be
 * replaced
 */
int
uri_reset_(URI *uri)
{
	UriPathSegmentA *seg, *next;

	uriFreeUriMembersA(&(uri->uri));
	free(uri->scheme);
	free(uri->auth);
	free(uri->user);
	free(uri->password);
	free(uri->hoststr);
	free(uri->hostdata.ip4);
	free(uri->hostdata.ip6);
	free((char *) uri->hostdata.ipFuture.first);
	free(uri->portstr);
	free(uri->authority);
	free(uri->nss);
	for(seg = uri->pathfirst; seg; seg = next)
	{
		next = seg->next;
		free((char *) seg->text.first);
		free(seg);
	}
	free(uri->query);
	free(uri->fragment);
	free(uri->composed);
	memset(uri, 0, sizeof(URI));
	return 0;
}
