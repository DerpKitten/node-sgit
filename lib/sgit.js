/*
	This file provides wrappers for the actual native bindings and performs additional validation of arguments 
	and other types of error and exception handling that are more cumbersome to do in C++. You should require
	this file rather than requiring the binary of the binding directly. 
*/

(function() {

fs = require("fs");
sgitNative = require('./build/Release/sgit');

if (env === "Debug") (require("segfault-handler")).registerHandler();

sgit = module.exports = {};

  sgit.Repository = Repository = (function() {
    function Repository(nativeRepo) {
      var index, _priv;
      if (!(nativeRepo instanceof NativeRepository)) {
        throw new Error("Don't construct me, see sgit.(open|init)Repository");
      }
      _priv = _createPrivate(this);
      _priv["native"] = nativeRepo;
      _immutable(this, nativeRepo).set("bare").set("path").set("workDir", "workingDirectory").set("remotes").set("references").set("submodules");
      index = new Index(nativeRepo.index);
      _immutable(this, {
        index: index
      }).set("index");
    }


  sgit.openRepository = function() {
    /*
    	Opens a local Git repository with the given `path`. Calls `cb` once
    	a :class:`sgit::Repository` has been opened.
    */
    var cb, path, _ref;
    _ref = args({
      path: {
        type: "string"
      },
      cb: {
        type: "function"
      }
    }), path = _ref[0], cb = _ref[1];
    return bindings.openRepository(path, _wrapCallback(cb, function(repo) {
      return cb(null, new Repository(repo));
    }));
  };

  sgit.initRepository = function() {
    /*
    	Creates a new Git repository at `path`, and calls `cb` providing the new 
    	:class:`sgit::Repository` when operation has completed.
    
    	If `bare` is specified as true the repository will be created without a
    	working directory. For more information see (TODO: link to page describing
    	bare repositories.)
    */
    var bare, cb, path, _ref;
    _ref = args({
      path: {
        type: "string"
      },
      bare: {
        type: "bool",
        "default": false
      },
      cb: {
        type: "function"
      }
    }), path = _ref[0], bare = _ref[1], cb = _ref[2];
    return bindings.initRepository(path, bare, _wrapCallback(cb, function(repo) {
      return cb(null, new Repository(repo));
    }));
  };

}).call(this);
