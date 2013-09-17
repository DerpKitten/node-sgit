var assert = require('assert');
var exec = require('exec-sync');
var fs = require('fs');
var nsgit = require('../build/Release/sgit');
var sgit = require('../lib/sgit');
var sleep = require('../node_modules/node-sleep');

describe('Node-native-sgit', function() {
	it("should be able to create a new repository", function(done) {
		nsgit.init_repository('./testrepo',function(err) {
			assert.equal(err,'');
			done();
		});	
	});
	it("should be able to commit a single file", function(done) {
		exec('echo "this is a test" > ./testrepo/test.c');
		nsgit.commit_bypath('./testrepo','test.c','this is a test commit message',function(err) {
			nsgit.log_repository('./testrepo',function(err,log) {
				log = JSON.parse(log);
				var firstEntry = "";
				for (var i in log) {
					firstEntry = i;
					break;
				}
				assert(log[firstEntry].message,'this is a test commit message');
				done();
			});
		});
	});

	it("should be able to commit a single file in a subdirectory", function(done) {
                exec('mkdir ./testrepo/newfolder');
                exec('echo "this is a subfolder test" > ./testrepo/newfolder/test.c');
                nsgit.commit_bypath('./testrepo','newfolder/test.c','this is a test commit message for subfolder',function(err) {
                        nsgit.log_repository('./testrepo',function(err,log) {
                                log = JSON.parse(log);
                                var entry = "";
                                for (var i in log) {
                                        entry = i;
                                        break;
                                }
                                assert(log[entry].message,'this is a test commit message for subfolder');
				done();
                        });
                });
        });

        it("should be able to log history of a repo", function(done) {
                exec('echo "this is a test 2 editing this file" > ./testrepo/test.c');
                nsgit.commit_bypath('./testrepo','test.c','this is a revised commit message',function(err) {
                        nsgit.log_repository('./testrepo',function(err,log) {
                                log = JSON.parse(log);
                                var loop = 0;
                                for (var i in log) {
					if (loop == 0) {
                                		assert(log[i].message,'this is a test commit message');
					}
					else if (loop == 1) {
                                		assert(log[i].message,'this is a revised commit message');
					}	
					else if (loop == 3) {
                                		assert(log[i].message,'this is a test commit message for subfolder');
					}	
                                        loop++;
                                }
				done();
                        });
                });

        });

       	it("should clean up after itself", function() {
		exec("rm -rf ./testrepo");
		assert.strictEqual(fs.existsSync('./testrepo'),false);
	});
});
describe('Node-sgit', function() {
	it("should be able to create a new repository", function(done) {
		sgit.init('./testrepo',function(err) {
			assert.equal(err,'');
			done();
		});	
	});

	it("should be able to commit a single file", function(done) {
		exec('echo "this is a test" > ./testrepo/test.c');
		sgit.commit('./testrepo/test.c','this is a test commit message',function(err) {
			sgit.log('./testrepo',function(err,log) {
				var firstEntry = "";
				for (var i in log) {
					firstEntry = i;
					break;
				}
				assert(log[firstEntry].message,'this is a test commit message');
				done();
			});
		});
	});

	it("should be able to commit a single file in a subdirectory", function(done) {
                exec('mkdir ./testrepo/newfolder');
                exec('echo "this is a subfolder test" > ./testrepo/newfolder/test.c');
                sgit.commit('./testrepo/newfolder/test.c','this is a test commit message for subfolder',function(err) {
                        sgit.log('./testrepo',function(err,log) {
                                var entry = "";
                                for (var i in log) {
                                        entry = i;
                                        break;
                                }
                                assert(log[entry].message,'this is a test commit messagefor subfolder');
				done();
                        });
                });
        });


        it("should be able to log history of a repo", function(done) {
                exec('echo "this is a test 2 editing this file" > ./testrepo/test.c');
                sgit.commit('./testrepo/test.c','this is a revised commit message',function(err) {
                        sgit.log('./testrepo',function(err,log) {
                                var loop = 0;
                                for (var i in log) {
					if (loop == 0) {
                                		assert(log[i].message,'this is a test commit message');
					}
					else if (loop == 2) {
                                		assert(log[i].message,'this is a revised commit message');
					}	
					else if (loop == 3) {
                                		assert(log[i].message,'this is a test commit message for subfolder');
					}	
                                        loop++;
                                }
				done();
                        });
                });

        });

        it("should clean up after itself", function() {
		exec("rm -rf ./testrepo");
		assert.strictEqual(fs.existsSync('./testrepo'),false);
	});
});
