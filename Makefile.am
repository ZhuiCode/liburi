## Author: Mo McRoberts <mo.mcroberts@bbc.co.uk>
##
## Copyright 2017 BBC

## Copyright 2012 Mo McRoberts.
##
##  Licensed under the Apache License, Version 2.0 (the "License");
##  you may not use this file except in compliance with the License.
##  You may obtain a copy of the License at
##
##      http://www.apache.org/licenses/LICENSE-2.0
##
##  Unless required by applicable law or agreed to in writing, software
##  distributed under the License is distributed on an "AS IS" BASIS,
##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
##  See the License for the specific language governing permissions and
##  limitations under the License.

ACLOCAL_AMFLAGS = -I m4

DIST_SUBDIRS = uriparser t util m4 docs man

SUBDIRS = . t util m4 docs man

DISTCLEANFILES = liburi.pc liburi-uninstalled.pc

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = liburi.pc

doc_DATA = README.md LICENSE-2.0 TODO

pkgincludedir = $(includedir)/$(PACKAGE_NAME)

pkginclude_HEADERS = liburi.h URI.h

lib_LTLIBRARIES = liburi.la

liburi_la_SOURCES = p_liburi.h \
	uri.c parse.c unicode.c fspath.c rebase.c props.c recompose.c

liburi_la_CPPFLAGS = -I$(srcdir)/uriparser/include
liburi_la_LDFLAGS = -avoid-version -export-symbols-regex '^uri_[a-z]'
liburi_la_LIBADD = liburiparser.la

## Build a static version of liburiparser to embed within liburi

noinst_LTLIBRARIES = liburiparser.la

liburiparser_la_SOURCES = \
	uriparser/src/UriCommon.c \
	uriparser/src/UriCommon.h \
	uriparser/src/UriCompare.c \
	uriparser/src/UriEscape.c \
	uriparser/src/UriFile.c \
	uriparser/src/UriIp4.c \
	uriparser/src/UriIp4Base.c \
	uriparser/src/UriIp4Base.h \
	uriparser/src/UriNormalize.c \
	uriparser/src/UriNormalizeBase.c \
	uriparser/src/UriNormalizeBase.h \
	uriparser/src/UriParse.c \
	uriparser/src/UriParseBase.c \
	uriparser/src/UriParseBase.h \
	uriparser/src/UriQuery.c \
	uriparser/src/UriRecompose.c \
	uriparser/src/UriResolve.c \
	uriparser/src/UriShorten.c

liburiparser_la_CPPFLAGS = -I$(top_srcdir)/uriparser/include
liburiparser_la_LDFLAGS = -static

test:
	$(MAKE) check

## Maintainer tools

BRANCH ?= develop
DOCS_BRANCH ?= gh-pages
DEVELOP_SUBMODULES = m4
DOCS_SUBMODULES = doc

checkout:
	@for submod in $(DEVELOP_SUBMODULES) ; do \
		echo "+++ Checking out the latest version of $$submod on $(BRANCH)" >&2 ; \
		cd $$submod && \
		git checkout $(BRANCH) && \
		git pull && \
		$(MAKE) checkout BRANCH=$(BRANCH) || exit $? && \
		cd .. ; \
	done

reconf:
	(cd $(top_srcdir) && autoreconf -i ) && $(SHELL) $(top_builddir)/config.status --recheck && $(SHELL) $(top_builddir)/config.status
