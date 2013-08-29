# Gitteh `v0.17.2`

[![build status](https://secure.travis-ci.org/libgit2/node-sgit.png)](http://travis-ci.org/libgit2/node-sgit)

## Installation

sgit requires `node 0.10`, `CMake 2.6` and `gcc`. Installation via NPM:

	npm install gitteh

## What?

Simple node bindings to the [libgit2](http://libgit2.github.com) C library that permit creating repositories, initializing repositories, commiting to them quickly and easily, pulling versions of files and pulling a history of revisions. 

sgit right now supports:

* creating repositories locally
* initializing repositories locally
* commit support is underway and will be completed shortly

## License

sgit is available under the MIT License. See the LICENSE file for more information.

## Contributing

To contribute please create a fork on github and make a pull request with your changes.

### Building

Next, you need to install gitteh's dependencies and build it. In gitteh's root directory, run:

    npm install
    ./node_modules/.bin/node-gyp configure
    ./node_modules/.bin/node-gyp build

### Installing Manually

Copy the parent directory of the now built module to the node_modules directory of your project. Include the module using:
    sgit = require('sgit');
