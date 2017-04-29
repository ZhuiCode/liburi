# liburi

A library for parsing URIs.

[![Current build status][travis]](https://travis-ci.org/bbcarchdev/liburi)
[![Apache 2.0 licensed][license]](#license)
![Implemented in C][language]
[![Follow @RES_Project][twitter]](https://twitter.com/RES_Project)

## Introduction

[liburi](https://github.com/bbcarchdev/liburi) is a simple interface for parsing
URIs. Under the hood, the actual URI parsing is handled by
[uriparser](http://uriparser.sourceforge.net/) -- liburi aims to provide
an API which is easier to work with than uriparser's own.

liburi provides:

* The ability to parse [IRIs](http://tools.ietf.org/html/rfc3987)
* Accessor methods for each parsed URI component and information about the URIs

## Building from git

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

To contribute to liburi, fork this repository and commit your changes to the
`develop` branch. For larger changes, you should create a feature branch with
a meaningful name, for example one derived from the [issue number](https://github.com/bbcarchdev/liburi/issues/).

Once you are satisfied with your contribution, open a pull request and describe
the changes you’ve made.

## License

**liburi** is licensed under the terms of the [Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0)

* Copyright © 2012 Mo McRoberts
* Copyright © 2014-2017 BBC

**uriparser** is licensed under the [New BSD license](http://uriparser.git.sourceforge.net/git/gitweb.cgi?p=uriparser/uriparser;a=blob;f=COPYING).

* Copyright © 2007, Weijia Song <songweijia@gmail.com>
* Copyright © 2007, Sebastian Pipping <sebastian@pipping.org>

[travis]: https://img.shields.io/travis/bbcarchdev/liburi.svg
[license]: https://img.shields.io/badge/license-Apache%202.0-blue.svg
[language]: https://img.shields.io/badge/Language-C-yellow.svg 
[twitter]: https://img.shields.io/twitter/url/http/shields.io.svg?style=social&label=Follow%20@RES_Project
