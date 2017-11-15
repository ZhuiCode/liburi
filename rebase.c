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

static int uri_rebase_nonhier_(URI *restrict reluri, const URI *restrict base);

/* Rebase reluri against the given base. If reluri is already absolute,
 * or base is NULL, this is a no-op. This function will modifiy reluri;
 * if this is not desirable, duplicate it first with uri_create_uri().
 */
int
uri_rebase(URI *restrict reluri, const URI *restrict base)
{
	URI abstemp;

	memset(&abstemp, 0, sizeof(URI));	
	if(!base || reluri->absolute)
	{
		/* Either no base provided (no-op), or reluri is already
		 * absolute.
		 */
		return 0;
	}
	if(!base->hier)
	{
		/* The base URI is non-hierarchical, which means we need to
		 * invoke an alternative rebasing strategy; this may result
		 * in NOT rebasing the relative URI, depending upon what it
		 * contains
		 */
		return uri_rebase_nonhier_(reluri, base);
	}
	if(uriAddBaseUriA(&(abstemp.uri), &(reluri->uri), &(base->uri)) != URI_SUCCESS)
	{
		/* Rebasing failed */
		return -1;
	}
	if(uriEqualsUriA(&(abstemp.uri), &(reluri->uri)) == URI_TRUE)
	{
		/* Rebasing didn't result in a new URI */
		uriFreeUriMembersA(&(abstemp.uri));
		return 0;
	}
	uri_postparse_(&abstemp);
	/* Free the resources used by reluri, replace its contents with that
	 * from absolute.
	 */
	uri_reset_(reluri);
	memcpy(reluri, &abstemp, sizeof(URI));
	return 0;
}

static int
uri_rebase_nonhier_(URI *restrict reluri, const URI *restrict base)
{
	/* Currently not possible */
#warning Handling non-hierarchical URIs is not yet supported
	errno = EPERM;
	return -1;
}
