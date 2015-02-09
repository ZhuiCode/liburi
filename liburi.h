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

#ifndef LIBURI_H_
# define LIBURI_H_                      1

# include <sys/types.h>

typedef struct uri_struct URI;

typedef struct uri_info_struct URI_INFO;

/* Note that excepting the 'internal' member, URI_INFO can be safely
 * modified by the calling application if it's convenient to do so.
 */
struct uri_info_struct
{
	void *internal;
	char *scheme;
	char *auth;
	char *host;
	int port;
	char *path;
	char *query;
	char *fragment;
};

# if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L) && !defined(restrict)
#  define restrict
# endif

# if defined(__cplusplus)
extern "C" {
# endif

	/* Create a URI from a string and optional base URI */
	URI *uri_create_str(const char *restrict uristr, const URI *restrict uri);
	
	/* Create a URI from an existing URI and optional base URI */
	URI *uri_create_uri(const URI *restrict source, const URI *restrict base);

	/* Create a file: URI from the current working directory */
	URI *uri_create_cwd(void);

	/* Destroy a URI, freeing the resources it uses */
	int uri_destroy(URI *uri);

	/* Return 1 if the URI specified is absolute, 0 if it is relative */
	int uri_absolute(const URI *uri);

	/* Return 1 if the path in the URI specified is absolute, 0 if it is
	 * relative
	 */
	int uri_absolute_path(const URI *uri);
	
	/* Copy the URI's scheme into the buffer provided */
	size_t uri_scheme(const URI *restrict uri, char *restrict buf, size_t buflen);

	/* Copy the URI's authentication information into the buffer provided */
	size_t uri_auth(const URI *restrict uri, char *restrict buf, size_t buflen);

	/* Copy the URI's hostname into the buffer provided */
	size_t uri_host(const URI *restrict uri, char *restrict buf, size_t buflen);

	/* Copy the URI's port into the buffer provided */
	size_t uri_port(const URI *restrict uri, char *restrict buf, size_t buflen);

	/* Copy the URI's path into the buffer provided */
	size_t uri_path(const URI *restrict uri, char *restrict buf, size_t buflen);

	/* Copy the URI's query string into the buffer provided */
	size_t uri_query(const URI *restrict uri, char *restrict buf, size_t buflen);

	/* Copy the URI's fragment into the buffer provided */
	size_t uri_fragment(const URI *restrict uri, char *restrict buf, size_t buflen);

	/* Return the URI's port number, if present, as an integer */
	int uri_portnum(const URI *uri);

	/* Copy the whole URI, as a string, into the buffer provided */
	size_t uri_str(const URI *restrict uri, char *restrict buf, size_t buflen);

	/* Allocate a new string using malloc() and copy the URI into it */
	char *uri_stralloc(const URI *restrict uri);

	/* Copy the various parts of the URI to a URI_INFO structure */
	URI_INFO *uri_info(const URI *uri);
	
	/* Free a URI_INFO structure */
	int uri_info_destroy(URI_INFO *info);

	/* Compare two URIs and test for equality */
	int uri_equal(const URI *a, const URI *b);
	
# if defined(__cplusplus)
}
# endif

#endif /*!LIBURI_H_*/
