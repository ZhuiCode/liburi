## Author: Mo McRoberts <mo.mcroberts@bbc.co.uk>
##
## Copyright 2017 BBC

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

dist_man3_MANS = liburi.3 \
	uri_create_str.3 uri_create_uri.3 uri_destroy.3 \
	uri_scheme.3 uri_auth.3 uri_host.3 uri_port.3 \
	uri_path.3 uri_query.3 uri_fragment.3 uri_portnum.3 \
	uri_absolute.3 uri_absolute_path.3 \
	uri_str.3

SUFFIXES = .3.xml .3 .1.xml .1

liburi.3: $(top_srcdir)/docs/liburi.3.xml
uri_create_str.3: $(top_srcdir)/docs/uri_create_str.3.xml
uri_create_uri.3: $(top_srcdir)/docs/uri_create_uri.3.xml
uri_destroy.3: $(top_srcdir)/docs/uri_destroy.3.xml
uri_scheme.3: $(top_srcdir)/docs/uri_scheme.3.xml
uri_auth.3: $(top_srcdir)/docs/uri_auth.3.xml
uri_host.3: $(top_srcdir)/docs/uri_host.3.xml
uri_port.3: $(top_srcdir)/docs/uri_port.3.xml
uri_path.3: $(top_srcdir)/docs/uri_path.3.xml
uri_query.3: $(top_srcdir)/docs/uri_query.3.xml
uri_fragment.3: $(top_srcdir)/docs/uri_fragment.3.xml
uri_portnum.3: $(top_srcdir)/docs/uri_portnum.3.xml
uri_absolute.3: $(top_srcdir)/docs/uri_absolute.3.xml
uri_absolute_path.3: $(top_srcdir)/docs/uri_absolute_path.3.xml
uri_str.3: $(top_srcdir)/docs/uri_str.3.xml

.1.xml.1:
	$(XML2MAN) $<
.3.xml.3:
	$(XML2MAN) $<
