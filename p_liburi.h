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

#ifndef P_LIBURI_H_
# define P_LIBURI_H_                    1

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <wchar.h>
# include <errno.h>
# include <limits.h>

# include "uriparser/Uri.h"

struct uri_info_internal_data_struct
{
	char *buffer;
	size_t nparams;
	size_t nalloc;
};

# include "URI.h"

struct uri_struct
{
	/* A uriparser URI instance */
	UriUriA uri;
	/* Any of the below members may be NULL to indicate absence */
	/* The URI scheme, e.g., 'http' */
	char *scheme;
	/* Authentication data - e.g., 'user:secret' */
	char *auth;
	/* The username portion of auth */
	char *user;
	/* The password portion of auth */
	char *password;
	/* The hostname, as a string */
	char *hoststr;
	/* The hostname as a structure */
	struct UriHostDataStructA hostdata;
	/* The port, as a string */
	char *portstr;
	/* The port parsed as an unsigned integer */
	unsigned int port;
	/* The combined auth, hoststr and portstr comprising the authority or
	 * namespace identifier
	 */
	char *authority;
	/* The namespace-specific segment of a non-hierarchical URI */
	char *nss;
	/* The path; the first segment points to the start of the buffer; the
	 * remaining segments in the list point to sections within that same
	 * buffer.
	 */
	UriPathSegmentA *pathfirst;
	UriPathSegmentA *pathlast;
	/* Is the path absolute? */
	int pathabs;
	/* The current path pointer (used with uri_peek(), uri_consume() and
	 * uri_rewind())
	 */
	UriPathSegmentA *pathcur;
	/* The query-string */
	char *query;
	/* The fragment identifier */
	char *fragment;
	/* The complete URI in normalised form */
	char *composed;
	/* Is this URI absolute? */
	int absolute;
	/* Is this URI hierarchical? (http, file and ftp are; urn, tag and 
	 * about aren't)
	*/
	int hier;
};

URI *uri_create_(void);
URI *uri_dup_(const URI *src);
int uri_reset_(URI *uri);
int uri_postparse_(URI *uri);
int uri_postparse_set_(URI *uri);

int uri_hostdata_copy_(struct UriHostDataStructA *restrict dest, const struct UriHostDataStructA *restrict src);
int uri_path_copy_(URI *dest, const UriPathSegmentA *head);

#endif /*!P_LIBURI_H_*/
