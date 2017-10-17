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

static size_t uri_wctoutf8_(int *dest, wchar_t ch);
static size_t uri_encode_8bit_(char *dest, unsigned char ch);
static size_t uri_encode_wide_(char *dest, wchar_t ch);
static size_t uri_widebytes_(const char *uristr, size_t nbytes);
static int uri_preprocess_(char *restrict buf, const char *restrict uristr, size_t nbytes);
static int uri_preprocess_utf8_(char *restrict buf, const unsigned char *restrict uristr, size_t nbytes);

/* Create a URI from a wide-character Unicode string */
URI *
uri_create_wstr(const wchar_t *restrict wstr, const URI *restrict base)
{
	/* XXX We should do this via a custom UTF-8-only wcstombs() */
}

/* Create a URI from a UTF-8-encoded string; any non-ASCII characters
 * will be percent-encoded
 */
URI *
uri_create_ustr(const unsigned char *restrict ustr, const URI *restrict base)
{
	const unsigned char *t;
	char *buf;
	size_t l, needed;
	URI *uri;

	/* Determine the required buffer size, accounting for percent-encoding
	 * of non-printable and non-ASCII characters
	 */
	l = strlen((const char *) ustr);
	needed = l + 1;
	for(t = ustr; *t; t++)
	{
		if(*t < 33 || *t > 127)
		{
			needed += 2;
		}
	}
	buf = (char *) calloc(1, needed);
	if(!buf)
	{
		return NULL;
	}
	if(uri_preprocess_utf8_(buf, ustr, l))
	{
		free(buf);
		return NULL;
	}
	uri = uri_create_ascii(buf, base);
	free(buf);
	return uri;
}

/* Create a URI from a string in the current locale */
URI *
uri_create_str(const char *restrict uristr, const URI *restrict base)
{
	char *buf;
	URI *uri;
	size_t l, numwide;

	/* XXX We should do this via mbstowcs() and then uri_create_wstr() */
	l = strlen(uristr) + 1;
	numwide = uri_widebytes_(uristr, l);
	buf = (char *) malloc(l + numwide * 3);
	if(!buf)
	{
		return NULL;
	}
	if(uri_preprocess_(buf, uristr, l))
	{
		free(buf);
		return NULL;
	}
	uri = uri_create_ascii(buf, base);
	free(buf);
	return uri;
}


/* Encode ch as UTF-8, storing it in dest[0..3] and returning the number
 * of octets stored. Because this is a convenience function used by
 * uri_encode_wide_(), dest is an array of ints, rather than unsigned chars.
 */
static size_t
uri_wctoutf8_(int *dest, wchar_t ch)
{
	if(ch < 0x7f)
	{
		dest[0] = ch;
		return 1;
	}
	if(ch < 0x07ff)
	{
		/* 110aaaaa 10bbbbbb */
		dest[0] = 0xc0 | ((ch & 0x0007c0) >>  6);
		dest[1] = 0x80 | (ch & 0x00003f);
		return 2;
	}
	if(ch < 0xffff)
	{
		/* 1110aaaa 10bbbbbb 10cccccc */
		dest[0] = 0xe0 | ((ch & 0x00f000) >> 12);
		dest[1] = 0x80 | ((ch & 0x000fc0) >> 6);
		dest[2] = 0x80 | (ch & 0x00003f);
		return 3;
	}
	/* 11110aaa 10bbbbbb 10cccccc 10dddddd */
	dest[0] = 0xf0 | ((ch & 0x1c0000) >> 18);
	dest[1] = 0x80 | ((ch & 0x03f000) >> 12);
	dest[2] = 0x80 | ((ch & 0x000fc0) >>  6);
	dest[3] = 0x80 | (ch & 0x00003f);
	return 4;
}

/* Encode an 8-bit character as a percent-encoded sequence */
static size_t
uri_encode_8bit_(char *dest, unsigned char ch)
{
	static const char hexdig[16] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};
	
	*dest = '%';
	dest++;
	*dest = hexdig[ch >> 4];
	dest++;
	*dest = hexdig[ch & 15];
	dest++;
	return 3;
}

/* Encode a Unicode wide-character as a sequence of percent-encoded
 * UTF-8.
 */
static size_t
uri_encode_wide_(char *dest, wchar_t ch)
{
	static const char hexdig[16] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};

	int utf8[6];
	size_t l, c;
	
	l = uri_wctoutf8_(utf8, ch);
	for(c = 0; c < l; c++)
	{
		*dest = '%';
		dest++;
		*dest = hexdig[utf8[c] >> 4];
		dest++;
		*dest = hexdig[utf8[c] & 15];
		dest++;
	}
	return l * 3;
}

/* Scan the URI string for wide characters and return the maximum storage
 * needed for their UTF-8 encoding
 */
static size_t
uri_widebytes_(const char *uristr, size_t nbytes)
{
	wchar_t ch;
	int r;
	const char *p;
	size_t numwide;

	mbtowc(&ch, NULL, 0);
	numwide = 0;
	for(p = uristr; *p;)
	{
		r = mbtowc(&ch, p, nbytes);
		if(r <= 0)
		{
			return (size_t) -1;
		}		
		if(ch < 33 || ch > 127)
		{
			/* Account for the full 6 bytes of UTF-8: we can't assume that
			 * the source string (and hence the return value of mbtowc()) is
			 * itself UTF-8, as it's locale-dependent.
			 */
			numwide += 6;
		}
		p += r;
	}
	return numwide;
}

/*
 * Map a potential IRI to a URI (see section 3.1 of RFC3987), converting
 * from locale-specific multibyte encoding to wide characters as we do
 */
static int
uri_preprocess_(char *restrict buf, const char *restrict uristr, size_t nbytes)
{
	wchar_t ch;
	char *bp;
	int r;

	/* Reset the multibyte shift state */
	mbtowc(&ch, NULL, 0);
	r = 0;
	for(bp = buf; nbytes && *uristr;)
	{
		/* Convert the next character sequence into a wide character */
		r = mbtowc(&ch, uristr, nbytes);
		if(r <= 0)
		{
			return -1;
		}		
		if(ch < 33 || ch > 127)
		{
			/* If the character is outside of the ASCII printable range,
			 * replace it with a percent-encoded UTF-8 equivalent
			 */
			bp += uri_encode_wide_(bp, ch);
		}
		else
		{
			*bp = ch;
			bp++;
		}
		uristr += r;
		nbytes -= r;
	}
	*bp = 0;
	return 0;
}

/*
 * Map a potential IRI to a URI (see section 3.1 of RFC3987), percent-encoding
 * UTF-8 characters as we do
 */
static int
uri_preprocess_utf8_(char *restrict buf, const unsigned char *restrict uristr, size_t nbytes)
{
	unsigned char ch;
	char *bp;
	int r;

	/* Reset the multibyte shift state */
	r = 0;
	for(bp = buf; nbytes && *uristr;)
	{
		/* Convert the next character sequence into a wide character */
		ch = *uristr;
		if(ch < 33 || ch > 127)
		{
			/* If the character is outside of the ASCII printable range,
			 * replace it with a percent-encoded UTF-8 equivalent
			 */
			bp += uri_encode_8bit_(bp, ch);
		}
		else
		{
			*bp = ch;
			bp++;
		}
		uristr++;
		nbytes--;
	}
	*bp = 0;
	return 0;
}
