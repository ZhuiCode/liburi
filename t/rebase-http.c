/* Author: Mo McRoberts <mo.mcroberts@bbc.co.uk>
 *
 * Copyright 2014-2017 BBC
 *
 * Copyright 2012 Mo McRoberts
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

#include "lib/p_tests.h"

/* Parse a selection of URIs and test the results */

static struct urimatch_simple tests[] = {
	{ "test.html", "http://user:pass@www.example.com:8080/battery/staple/horse.correct?foo=bar&baz=zap#fragment",
	  "http://user:pass@www.example.com:8080/battery/staple/test.html"
	},
	{ "//www2.example.com/donkey-1234", "http://user:pass@www.example.com:8080/battery/staple/horse.correct?foo=bar&baz=zap#fragment",
	  "http://www2.example.com/donkey-1234"
	},
	{ "#section2", "http://user:pass@www.example.com:8080/battery/staple/horse.correct?foo=bar&baz=zap#fragment",
	  "http://user:pass@www.example.com:8080/battery/staple/horse.correct?foo=bar&baz=zap#section2",
	},

	
	{ NULL, NULL, NULL }
};

int
main(void)
{
	return test_urimatch_simple(__FILE__, tests);
}
