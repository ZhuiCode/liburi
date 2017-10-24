/* Author: Mo McRoberts <mo.mcroberts@bbc.co.uk>
 *
 * Copyright 2015 BBC
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

static ssize_t uri_get_(const UriTextRangeA *restrict range, char *restrict buf, size_t bufsize);
static int uri_addch_(int ch, char *restrict *restrict buf, size_t *restrict buflen);
static int uri_info_parseauth_(URI_INFO *info);
static int uri_info_parseparams_(URI_INFO *info);
static int uri_info_param_add_(URI_INFO *info, const char *key, const char *value);

URI_INFO *
uri_info(const URI *uri)
{
	URI_INFO *p;
	char *buf;
	size_t buflen, r;

	buflen = 0;
#define getbuf(component) \
	r = uri_get_(&(uri->uri.component), NULL, 0);	  \
	if(r == (size_t) -1)							  \
	{												  \
		return NULL;								  \
	}												  \
	buflen += r;

	getbuf(scheme);
	getbuf(userInfo);
	getbuf(hostText);
	r = uri_path(uri, NULL, 0);
	if(r == (size_t) -1)
	{
		return NULL;
	}
	buflen += r;
	getbuf(query);
	getbuf(fragment);

#undef getbuf
	p = (URI_INFO *) calloc(1, sizeof(URI_INFO));
	if(!p)
	{
		return NULL;
	}
	buf = (char *) calloc(1, buflen);
	if(!buf)
	{
		free(p);
		return NULL;
	}
	p->internal.buffer = buf;
#define getbuf(component, member)						\
	r = uri_get_(&(uri->uri.component), buf, buflen);	\
	if(r)												\
	{													\
		p->member = buf;								\
		buf += r;										\
		buflen -= r;									\
	}

	getbuf(scheme, scheme);
	getbuf(userInfo, auth);
	getbuf(hostText, host);
	p->port = uri_portnum(uri);
	r = uri_path(uri, buf, buflen);
	if(r)
	{
		p->path = buf;
		buf += r;
		buflen -= r;
	}
	getbuf(query, query);
	getbuf(fragment, fragment);
#undef getbuf
	uri_info_parseauth_(p);
	uri_info_parseparams_(p);
	return p;
}

/* Free a URI_INFO structure */
int
uri_info_destroy(URI_INFO *info)
{
	free(info->internal.buffer);
	free(info->user);
	free(info->pass);
	free(info);
	return 0;
}

/* Obtain a parameter from a URI_INFO structure */
const char *
uri_info_get(URI_INFO *info, const char *key, const char *defval)
{
	size_t c;

	for(c = 0; info->params && info->params[c]; c += 2)
	{
		if(!strcmp(key, info->params[c]))
		{
			return info->params[c + 1];
		}
	}
	return defval;
}

/* Obtain a parameter from a URI_INFO structure */
intmax_t
uri_info_get_int(URI_INFO *info, const char *key, intmax_t defval)
{
	size_t c;

	for(c = 0; info->params && info->params[c]; c += 2)
	{
		if(!strcmp(key, info->params[c]))
		{
			return strtoimax(info->params[c + 1], NULL, 10);
		}
	}
	return defval;
}

/* Compare two URIs and test for equality */
int
uri_equal(const URI *a, const URI *b)
{
	return uriEqualsUriA(&(a->uri), &(b->uri));
}

static ssize_t
uri_get_(const UriTextRangeA *restrict range, char *restrict buf, size_t bufsize)
{
	size_t len, slen;

	if(!range->first)
	{
		if(buf && bufsize)
		{
			*buf = 0;
		}
		return 0;
	}
	if(range->afterLast)
	{
		len = range->afterLast - range->first;
	}
	else
	{
		len = strlen(range->first);
	}
	if(buf && bufsize)
	{
		/* Copy at most len or (bufsize - 1) characters, whichever
		 * is fewer.
		 */
		bufsize--;
		slen = bufsize < len ? bufsize : len;
		strncpy(buf, range->first, slen);
		buf[slen] = 0;
	}
	return len + 1;
}

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

/* Parse info->auth into info->user and info->pass */
static int
uri_info_parseauth_(URI_INFO *info)
{
	int n;
	const char *s;
	char *p;

	if(!info->auth)
	{
		return 0;
	}
	info->user = strdup(info->auth);
	if(!info->user)
	{
		return -1;
	}
	p = info->user;
	for(s = info->auth; *s && *s != ':'; s++)
	{
		if(*s == '%')
		{
			if(isxdigit(s[1]) && isxdigit(s[2]))
			{
				if(s[1] >= '0' && s[1] <= '9')
				{
					n = s[1] - '0';
				}
				else
				{
					n = 10 + tolower(s[1]) - 'a';
				}
				n <<= 4;
				if(s[2] >= '0' && s[2] <= '9')
				{
					n |= s[2] - '0';
				}
				else
				{
					n |= 10 + tolower(s[2]) - 'a';
				}
				*p = n;
				p++;
				s += 2;
				continue;
			}
		}
		*p = *s;
		p++;
	}
	*p = 0;
	if(*s == ':')
	{
		info->pass = strdup(s);
		if(!info->pass)
		{
			return -1;
		}
		p = info->pass;
		for(s++; *s; s++)
		{
			if(*s == '%')
			{
				if(isxdigit(s[1]) && isxdigit(s[2]))
				{
					if(s[1] >= '0' && s[1] <= '9')
					{
						n = s[1] - '0';
					}
					else
					{
						n = 10 + tolower(s[1]) - 'a';
					}
					n <<= 4;
					if(s[2] >= '0' && s[2] <= '9')
					{
						n |= s[2] - '0';
					}
					else
					{
						n |= 10 + tolower(s[2]) - 'a';
					}
					*p = n;
					p++;
					s += 2;
					continue;
				}
			}
			*p = *s;
			p++;
		}
		*p = 0;
	}
	return 0;
}

/* Parse info->query into info->params[] */
static int
uri_info_parseparams_(URI_INFO *info)
{
	const char *s, *t, *key, *value;
	char *qbuf, *p;
	char cbuf[3];

	if(!info->query)
	{
		return 0;
	}
	qbuf = (char *) calloc(1, strlen(info->query) + 1);
	if(!qbuf)
	{
		return -1;
	}
	p = qbuf;
	s = info->query;
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
			uri_info_param_add_(info, key, value);
		}
		if(t)
		{
			t++;
		}
		s = t;
	}
	free(qbuf);
	return 0;
}

static int
uri_info_param_add_(URI_INFO *info, const char *key, const char *value)
{
	char **p;
	size_t n;

	if(!value)
	{
		value = "";
	}
	if(info->internal.nparams + 1 >= info->internal.nalloc)
	{
		int i;

		/* Allocate in chunks of four parameters at a time plus sentinels */
		p = (char **) realloc(info->params, sizeof(char *) * (info->internal.nalloc + 5) * 2);
		if(!p)
		{
			return -1;
		}
		/* clear new memory; note that NULL may not == (void*)0, so don't use memset() */
		for(i = 0; i < 10; i++)
		{
		    *(p + (2*info->internal.nparams) + i) = NULL;
		}
		info->params = p;
		info->internal.nalloc += 4;
	}
	n = info->internal.nparams * 2;
	info->params[n] = strdup(key);
	info->params[n + 1] = strdup(value);
	if(!info->params[info->internal.nparams] || !info->params[info->internal.nparams + 1])
	{
		return -1;
	}
	info->internal.nparams++;
	return 0;
}
