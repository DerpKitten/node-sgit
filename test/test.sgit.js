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

	it("should be able to commit a single file", function() {
		exec('echo "this is a test" > ./testrepo/test.c');
		sgit.commit_bypath('./testrepo','test.c','this is a test commit message',function(err) {
			sgit.log('./testrepo',function(err,log) {
				log = JSON.parse(log);
				var firstEntry = "";
				for (var i in log) {
					firstEntry = i;
					break;
				}
				assert(log[firstEntry].message,'this is a test commit message');
			});
		});
	});

        it("should be able to log history of a repo", function() {
                exec('echo "this is a test 2 editing this file" > ./testrepo/test.c');
                sgit.commit_bypath('./testrepo','test.c','this is a revised commit message',function(err) {
                        sgit.log('./testrepo',function(err,log) {
                                log = JSON.parse(log);
                                var loop = 0;
                                for (var i in log) {
					if (loop == 0) {
                                		assert(log[i].message,'this is a test commit message');
					}
					else if (loop == 1) {
                                		assert(log[i].message,'this is a revised commit message');
					}	
                                        loop++;
                                }
                        });
                });

        });

        it("clean up after itself", function() {
		exec("rm -rf ./testrepo");
		assert.strictEqual(fs.existsSync('./testrepo'),false);
	});
});
