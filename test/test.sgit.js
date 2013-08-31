var assert = require('assert');
var exec = require('exec-sync');
var fs = require('fs');
var sgit = require('../build/Release/sgit');

describe('Node-sgit', function() {
	it("should be able to create a new repository", function() {
		sgit.init_repository('./testrepo',function(err) {
			assert.equal(err,'');
		});	
	});
});

describe('Node-sgit', function() {
	it("should be able to commit a single file", function() {
		exec('echo "this is a test" > ./testrepo/test.c');
		sgit.commit_bypath('./testrepo','test.c','this is a test commit message',function(err) {
			//assert.equal(err,'');
			//exec('rm -rf ./testdir');
		});

		var out = exec('cd ./testrepo; /usr/bin/git log');
		assert.notStrictEqual(out.match(/.*this is a test commit message.*/g),null);
		exec('rm -rf ./testrepo');
	});
});
