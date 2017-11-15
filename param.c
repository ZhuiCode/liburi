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

static int uri_param_parse_(URI *uri);
static int uri_param_changed_(URI *uri);
static int uri_param_add_internal_(URI *restrict uri, const char *restrict key, const char *restrict value);
static int uri_param_remove_internal_(URI *restrict uri, const char *restrict key);

const char *
uri_param_str(URI *restrict uri, const char *restrict key)
{
	size_t c;

	if(!uri->params)
	{
		/* Parse the parameters if we haven't already */
		if(uri_param_parse_(uri))
		{
			return NULL;
		}
	}
	/* XXX should we actually iterate backwards in order to return the
	 * most recently-added value in multi-value parameters?
	 */
	for(c = 0; uri->params && c < uri->nparams * 2; c += 2)
	{
		if(!strcmp(key, uri->params[c]))
		{
			return uri->params[c + 1];
		}
	}
	return NULL;
}

char *
uri_param_stralloc(URI *restrict uri, const char *restrict key)
{
	const char *value;

	value = uri_param_str(uri, key);
	if(!value)
	{
		return NULL;
	}
	return strdup(value);
}

intmax_t
uri_param_int(URI *restrict uri, const char *restrict key)
{
	const char *value;

	value = uri_param_str(uri, key);
	if(!value)
	{
		return -1;
	}
	return strtoimax(value, NULL, 10);	
}

/* Remove all parameters */
int
uri_param_reset(URI *restrict uri)
{
	uri_param_reset_internal_(uri);
	uri_param_changed_(uri);
	return 0;
}

/* Add a parameter by appending to the parameter list; has no impact upon
 * parameters which already have values.
 */
int
uri_param_add(URI *restrict uri, const char *restrict key, const char *restrict value)
{
	int r;

	/* Add the parameter, then reset uri->query (and uri->uri.query) so that it
	 * will be re-composed by uri_str() or uri_info()
	 */
	r = uri_param_add_internal_(uri, key, value);
	if(r)
	{
		return r;
	}
	uri_param_changed_(uri);
	return 0;
}

/* Set a parameter, replacing any existing values that it may have */
int
uri_param_set(URI *restrict uri, const char *restrict key, const char *restrict value)
{
	int r;

	r = uri_param_remove_internal_(uri, key);
	if(r)
	{
		return r;
	}
	if(value)
	{
		r = uri_param_add(uri, key, value);
		if(r)
		{
			return r;
		}
	}
	uri_param_changed_(uri);
	return 0;
}

/* Reset uri->params because uri->query has been modified */
int
uri_param_reset_internal_(URI *restrict uri)
{
	size_t c;

	for(c = 0; uri->params && c < uri->nparams * 2; c++)
	{
		free(uri->params[c]);
	}
	free(uri->params);
	uri->params = NULL;
	uri->nparams = 0;
	uri->nparamalloc = 0;
	return 0;
}

/* Parse the query-string into a key-value set */
static int
uri_param_parse_(URI *uri)
{
	const char *s, *t, *key, *value;
	char *qbuf, *p;
	char cbuf[3];

	if(!uri->query)
	{
		return 0;
	}
	uri_param_reset_internal_(uri);
	qbuf = (char *) calloc(1, strlen(uri->query) + 1);
	if(!qbuf)
	{
		return -1;
	}
	p = qbuf;
	s = uri->query;
	while(s)
	{
		key = p;
		value = NULL;
		t = strchr(s, '&');
		while(*s &&(!t || s < t))
		{
			if(*s == '=')
			{
				*p = 0;
				p++;
				s++;
				value = p;
				continue;
			}
			if(*s == '%')
			{
				if(isxdigit(s[1])  && isxdigit(s[2]))
				{
					cbuf[0] = s[1];
					cbuf[1] = s[2];
					cbuf[2] = 0;
					*p = (char) ((unsigned char) strtol(cbuf, NULL, 16));
					p++;
					s += 3;
					continue;
				}
			}
			*p = *s;
			p++;
			s++;
		}
		*p = 0;
		p++;
		if(value)
		{
			uri_param_add_internal_(uri, key, value);
		}
		if(t)
		{
			t++;
		}
		s = t;
	}
	free(qbuf);
	/* We don't invoke uri_param_changed_() here because although we have
	 * now parsed the query-string to our parameter set, they've not been
	 * modified, so discarding the original strings would be premature.
	 */
	return 0;
}

static int
uri_param_changed_(URI *restrict uri)
{
	/* uri->params has been modified, which means that uri->query has been
	 * invalidated. Free it to trigger it being reconstituted when it's
	 * next required
	 */
	free(uri->query);
	uri->query = NULL;
	uri->uri.query.first = NULL;
	uri->uri.query.afterLast = NULL;
	free(uri->composed);
	uri->composed = NULL;
	return 0;
}

/* Add a new parameter to the list, ignoring any which may already exist with
 * the same name, expanding the size of the parameter list as needed
 */
static int
uri_param_add_internal_(URI *restrict uri, const char *key, const char *value)
{
	char **p;
	size_t n;

	if(!value)
	{
		value = "";
	}
	n = uri->nparams * 2;
	if(uri->nparams + 1 >= uri->nparamalloc)
	{
		p = (char **) realloc(uri->params, (uri->nparamalloc + URI_PARAM_ALLOC_SLOTS) * sizeof(char *) * 2);
		if(!p)
		{
			return -1;
		}
		memset(p + n, 0, (URI_PARAM_ALLOC_SLOTS * sizeof(char *)) * 2);
		uri->params = p;
		uri->nparamalloc += URI_PARAM_ALLOC_SLOTS;
	}
	uri->params[n] = strdup(key);
	uri->params[n + 1] = strdup(value);
	if(!uri->params[n] || !uri->params[n + 1])
	{
		free(uri->params[n]);
		uri->params[n] = NULL;
		free(uri->params[n + 1]);
		uri->params[n + 1] = NULL;
		return -1;
	}
	uri->nparams++;
	return 0;
}

/* Remove all instances of the parameter named 'key' */
static int
uri_param_remove_internal_(URI *restrict uri, const char *key)
{
	size_t c;

	for(c = 0; c < uri->nparams * 2; )
	{
		if(!strcmp(uri->params[c], key))
		{
			free(uri->params[c]);
			free(uri->params[c + 1]);
			memmove(&(uri->params[c]), &(uri->params[c + 2]), sizeof(char *) * ((uri->nparams * 2) - c - 2));
			uri->nparams--;
			memset(&(uri->params[uri->nparams * 2]), 0, sizeof(char *) * 2);
			continue;
		}
		c += 2;
	}
	return 0;
}
