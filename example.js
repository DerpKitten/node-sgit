var sgit = require('./lib/sgit');

sgit.init_repository('./sandbox',function (err, repo) {
	if (err) {
		console.log(err);
	}
	console.log(repo);
});

sgit.commit_bypath('./sandbox/newfolder/test.c','here is my commit message',function (err) {
	if (err) {
		console.log(err);
	}
});

sgit.log('./sandbox',function(err,log) {
	if (err) {
		console.log(err);
	}
	console.log(log);
});
