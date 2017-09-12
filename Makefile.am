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

doc_DATA = \
	index.html \
	uri_create_str.3.html uri_create_uri.3.html uri_destroy.3.html \
	uri_scheme.3.html uri_auth.3.html uri_host.3.html uri_port.3.html \
	uri_path.3.html uri_query.3.html uri_fragment.3.html uri_portnum.3.html \
	uri_absolute.3.html uri_absolute_path.3.html \
	uri_str.3.html

SUFFIXES = .3.xml .3.html .1.xml .1.html

.1.xml.1.html:
	$(XML2HTML) $< > $@

.3.xml.3.html:
	$(XML2HTML) $< > $@
