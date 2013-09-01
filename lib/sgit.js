/*
	This file provides wrappers for the actual native bindings and performs additional validation of arguments 
	and other types of error and exception handling that are more cumbersome to do in C++. You should require
	this file rather than requiring the binary of the binding directly. 
*/

(function() {

fs = require("fs");
assert = require("assert");
sgitNative = require('../build/Release/sgit.node');

sgit = module.exports = {};

sgit.init_repository = init_repository = (function(repo_path, callback) {
	if (typeof repo_path !== 'string') {
		callback('Path argument supplied is not valid string.');
		return 1;
	}
 
	if (typeof callback !== 'function') {
		callback('Callback function argument supplied is not valid function.');
		return 1;
	} 

	sgitNative.init_repository(repo_path, function(err,repo) {
		callback(err,repo);
	});
});


}).call(this);
