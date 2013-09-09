/*
	This file provides wrappers for the actual native bindings and performs additional validation of arguments 
	and other types of error and exception handling that are more cumbersome to do in C++. You should require
	this file rather than requiring the binary of the binding directly. 
*/

(function() {

fs = require("fs");
path = require("path");
assert = require("assert");
sgitNative = require('../build/Release/sgit.node');

sgit = module.exports = {};

sgit.init = init = (function(repo_path, callback) {
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
	return 0;
});

sgit.commit = commit = (function(path_to_file, message, callback) {
        if (typeof path_to_file !== 'string' && fs.existsSync(path_to_file)) {
                callback('Path to file argument supplied is not valid string or file does not exist.');
                return 1;
        }

        if (typeof message !== 'string') {
                callback('Message supplied is not valid string.');
                return 1;
        }

        if (typeof callback !== 'function') {
                callback('Callback function argument supplied is not valid function.');
                return 1;
        }

	var file_path;
	var repo_path;
	var path_parts = path_to_file.split(/\//);

	for (var i=path_parts.length-1; i>-1; i--) {
		var test_path = path_parts.slice(0,i);
		if (fs.existsSync(test_path.join('/') + '/.git')) {
			repo_path = test_path.join('/');
			file_path = path_parts.slice(i, path_parts.length).join('/');
			break;
		}
	}

	if (repo_path === 'undefined') {
                callback('Could not find git repository in file path provided.');
                return 1;

	}

        sgitNative.commit_bypath(repo_path,file_path,message,function(err,commit_id) {
                callback(err,commit_id);
        });
	return 0;
});

sgit.log = log = (function(repo_path, callback) {
        if (typeof repo_path !== 'string') {
                callback('Path argument supplied is not valid string.');
                return 1;
        }

        if (typeof callback !== 'function') {
                callback('Callback function argument supplied is not valid function.');
                return 1;
        }
        sgitNative.log_repository(repo_path, function(err,log) {
		var parsed_log = JSON.parse(log);
                callback(err,parsed_log);
        });
	return 0;
});



}).call(this);
