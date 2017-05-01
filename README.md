# liburi

A library for parsing URIs and IRIs.

[![Current build status][travis]](https://travis-ci.org/bbcarchdev/liburi)
[![Apache 2.0 licensed][license]](#license)
![Implemented in C][language]
[![Follow @RES_Project][twitter]](https://twitter.com/RES_Project)

[liburi](https://github.com/bbcarchdev/liburi) is a simple interface for parsing
URIs. Under the hood, the actual URI parsing is handled by a bundled version of
[uriparser](http://uriparser.sourceforge.net/)—liburi aims to provide
an API which is easier to work with than uriparser's own.

This software was developed as part of the [Research & Education Space project](https://bbcarchdev.github.io/res/).

## Table of Contents

* [Requirements](#requirements)
* [Information for BBC Staff](#information-for-bbc-staff)
* [Building from source](#building-from-source)
* [Contributing](#contributing)
* [License](#license)

## Requirements

There are no special build requirements; a working C compiler and build
environment will be sufficient.

liburi has not been ported to non-Unix-like environments, although it should build inside Cygwin.

See also the additional requirements when [building from source](#building-from-source).

## Information for BBC Staff

This is an open source project which is actively maintained and developed
by a team within Design and Engineering. Please bear in mind the following:—

* Bugs and feature requests **must** be filed in [GitHub Issues](https://github.com/bbcarchdev/liburi/issues): this is the authoratitive list of backlog tasks.
* Issues with the label [triaged](https://github.com/bbcarchdev/liburi/issues?q=is%3Aopen+is%3Aissue+label%3Atriaged) are 
* [Forking](https://github.com/bbcarchdev/liburi/fork) is encouraged! See the “[Contributing](#contributing)” section.
* Under **no** circumstances may you commit directly to this repository, even if you have push permission in GitHub.
* If you’re joining the development team, contact *“Archive Development Operations”* in the GAL to request access to GitLab (although your line manager should have done this for you in advance).

## Building from source

You will need git, automake, autoconf and libtool. You'll also need a
DocBook 5 and DocBook-XSL toolchain (i.e., xsltproc and the stylesheets)
if you wish to rebuild the documentation. Both liburi and uriparser have
minimal external dependencies.

    $ git clone git://github.com/bbcarchdev/liburi.git
    $ cd liburi
    $ git submodule update --init --recursive
    $ autoreconf -i
    $ ./configure --prefix=/some/path
    $ make
    $ make check
    $ sudo make install

## Contributing

To contribute to liburi, [fork this repository](https://github.com/bbcarchdev/liburi/fork) and commit your changes to the
`develop` branch.

For larger changes, you should create a feature branch with
a meaningful name, for example one derived from the [issue number](https://github.com/bbcarchdev/liburi/issues/).

Once you are satisfied with your contribution, open a pull request and describe
the changes you’ve made.

## License

**uriparser** is licensed under the [New BSD license](http://uriparser.git.sourceforge.net/git/gitweb.cgi?p=uriparser/uriparser;a=blob;f=COPYING).

* Copyright © 2007 Weijia Song - <songweijia@gmail.com>
* Copyright © 2007 Sebastian Pipping - <sebastian@pipping.org>

**liburi** is licensed under the terms of the [Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0)

* Copyright © 2012 Mo McRoberts
* Copyright © 2014-2017 BBC

[travis]: https://img.shields.io/travis/bbcarchdev/liburi.svg
[license]: https://img.shields.io/badge/license-Apache%202.0-blue.svg
[language]: https://img.shields.io/badge/implemented%20in-C-yellow.svg 
[twitter]: https://img.shields.io/twitter/url/http/shields.io.svg?style=social&label=Follow%20@RES_Project
