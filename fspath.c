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

/* Create a file: URI for the current working directory */
URI *
uri_create_cwd(void)
{
	char *pathbuf;
	URI *uri;
	
	pathbuf = (char *) calloc(1, 7 + PATH_MAX + 2);
	if(!pathbuf)
	{
		return NULL;
	}
	strcpy(pathbuf, "file://");
	if(!getcwd(&(pathbuf[7]), PATH_MAX + 1))
	{
		free(pathbuf);
		return NULL;
	}
	pathbuf[strlen(pathbuf)] = '/';
	/* XXX encode special characters in the path */
	uri = uri_create_str(pathbuf, NULL);
	free(pathbuf);
	return uri;
}
