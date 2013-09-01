/*
	This file provides wrappers for the actual native bindings and performs additional validation of arguments 
	and other types of error and exception handling that are more cumbersome to do in C++. You should require
	this file rather than requiring the binary of the binding directly. 
*/

(function() {

fs = require("fs");
sgitNative = require('./build/Release/sgit');

sgit = module.exports = {};

}).call(this);
