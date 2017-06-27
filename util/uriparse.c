/* Author: Mo McRoberts <mo.mcroberts@bbc.co.uk>
 *
 * Copyright (c) 2015-2017 BBC
 */

/* Copyright 2012 Mo McRoberts.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <errno.h>

#include "liburi.h"

static const char *short_program_name = "uriparse";
static const char *prefix = "";
static const char *uristr;
static const char *basestr;
static int verbose;
static int printuri;
static int omitempty;
static int printquery = 1;

static void parseargs(int argc, char **argv);
static void usage(void);
static URI *parseuris(void);
static int printcomp(URI *uri, const char *name, const char *buf);
static int printesc(const char *string);
static int print_uri(URI *uri);
static int print_components(URI *uri);

int
main(int argc, char **argv)
{
	URI *uri;
	int r;

	setlocale(LC_ALL, "");
	parseargs(argc, argv);
	uri = parseuris();
	if(!uri)
	{
		return 1;
	}
	if(printuri)
	{
		r = print_uri(uri);	
	}
	else
	{
		r = print_components(uri);
	}
	uri_destroy(uri);
	return (r ? 1 : 0);
}


static void
parseargs(int argc, char **argv)
{
	const char *t;
	int ch;

	t = strrchr(argv[0], '/');
	if(t)
	{
		t++;
	}
	else
	{
		t = argv[0];
	}
	short_program_name = t;
	while((ch = getopt(argc, argv, "hvp:uo")) != -1)
	{
		switch(ch)
		{
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
		case 'v':
			verbose = 1;
			break;
		case 'p':
			prefix = optarg;
			break;
		case 'u':
			printuri = 1;
			break;
		case 'o':
			omitempty = 1;
			break;
		case 'q':
			printquery = 0;
			break;
		default:
			usage();
			exit(EXIT_FAILURE);
		}
	}
	if(argc - optind < 1 || argc - optind > 2)
	{
		usage();
		exit(EXIT_FAILURE);
	}
	argc -= optind;
	argv += optind;
	uristr = argv[0];	
	if(argc >= 2)
	{
		basestr = argv[1];
	}
	else
	{
		basestr = NULL;
	}
}

static void
usage(void)
{
	fprintf(stderr, "Parse a URI and print its components\n\n");
	fprintf(stderr, "Usage: %s [OPTIONS] URI [BASE]\n\n", short_program_name);
	fprintf(stderr, "OPTIONS is one or more of:\n"
			"  -h                  Display this usage message and exit\n"
			"  -v                  Produce verbose output\n"
			"  -p PREFIX           Prefix output variable names with PREFIX\n"
			"  -u                  Print the parsed URI instead of components\n"
		    "  -o                  Omit printing components which are absent\n"
			"  -q                  Omit print the query-string components\n");
}

static URI *
parseuris(void)
{
	URI *uri, *rel, *base;

	if(basestr)
	{
		base = uri_create_str(basestr, NULL);		
		if(!base)
		{
			fprintf(stderr, "%s: failed to parse URI '%s': %s\n", short_program_name, basestr, strerror(errno));
			return NULL;
		}
		rel = uri_create_str(uristr, NULL);
		if(!rel)
		{
			fprintf(stderr, "%s: failed to parse URI '%s': %s\n", short_program_name, uristr, strerror(errno));
			return NULL;
		}
		uri = uri_create_uri(rel, base);
		if(!uri)
		{
			fprintf(stderr, "%s: failed to resolve '%s' against '%s': %s\n", short_program_name, uristr, basestr, strerror(errno));
		}
		uri_destroy(base);
		uri_destroy(rel);
		return uri;
	}
	uri = uri_create_str(uristr, NULL);
	if(!uri)
	{
		fprintf(stderr, "%s: failed to parse URI '%s': %s\n", short_program_name, uristr, strerror(errno));
		return NULL;
	}
	return uri;
}

static int
printcomp(URI *uri, const char *name, const char *buf)
{
	(void) uri;
	
	if(!buf || !buf[0])
	{
		if(!omitempty)
		{
			printf("%s%s=''\n", prefix, name);
		}
		return 0;
	}	
	printf("%s%s=\"", prefix, name);
	printesc(buf);
	puts("\"");
	return 0;
}

static int
printesc(const char *string)
{
	const char *p;

	for(p = string; *p; p++)
	{
		switch(*p)
		{
		case '$':
		case '"':
		case '\\':
		case '`':
			putchar('\\');
			putchar(*p);
			break;
		default:
			if((unsigned char) *p < 32 || (unsigned char) *p > 127)
			{
				printf("\\%03o", *p);
				continue;
			}
			putchar(*p);
		}
	}
	return 0;
}

static int
print_uri(URI *uri)
{
	size_t len;
	char *buffer;

	len = uri_str(uri, NULL, 0);
	if(len == (size_t) -1)
	{
		fprintf(stderr, "%s: failed to recompose URI: %s\n", short_program_name, strerror(errno));
		return -1;
	}
	buffer = (char *) malloc(len);
	if(!buffer)
	{
		fprintf(stderr, "%s: failed to allocate %lu bytes: %s\n", short_program_name, (unsigned long) len, strerror(errno));
		return -1;
	}	
	len = uri_str(uri, buffer, len);
	if(len == (size_t) -1)
	{
		fprintf(stderr, "%s: failed to recompose URI: %s\n", short_program_name, strerror(errno));
		return -1;
	}
	puts(buffer);
	free(buffer);
	return 0;
}

static int
print_components(URI *uri)
{
	URI_INFO *info;
	size_t len, c;
	int r;

	info = uri_info(uri);
	if(!info)
	{
		fprintf(stderr, "%s: failed to obtain information about the URI: %s\n", short_program_name, strerror(errno));
		return 0;
	}
	r = 0;
	len = 0;
	if(printcomp(uri, "scheme", info->scheme) == -1)
	{
		r = -1;
	}
	if(printcomp(uri, "auth", info->auth) == -1)
	{
		r = -1;
	}
	if(printcomp(uri, "user", info->user) == -1)
	{
		r = -1;
	}
	if(printcomp(uri, "pass", info->pass) == -1)
	{
		r = -1;
	}
	if(info->port)
	{
		printf("%sport=%d\n", prefix, info->port);
	}
	if(printcomp(uri, "path", info->path) == -1)
	{
		r = -1;
	}
	if(printcomp(uri, "query", info->query) == -1)
	{
		r = -1;
	}
	if(printquery)
	{
		for(c = 0; info->params && info->params[c]; c += 2)
		{
			if(!info->params[c + 1][0])
			{
				if(!omitempty)
				{
					printf("%sparams[\"", prefix);
					printesc(info->params[c]);
					puts("\"]=''");
					continue;
				}
			}
			printf("%sparams[\"", prefix);
			printesc(info->params[c]);
			printf("\"]=\"");
			printesc(info->params[c + 1]);
			puts("\"");
		}
	}
	if(printcomp(uri, "fragment", info->fragment) == -1)
	{
		r = -1;
	}
	uri_info_destroy(info);
	return 0;
}
