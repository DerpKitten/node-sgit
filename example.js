var sgit = require('./build/Release/sgit');

/*sgit.init_repository('./sandbox',function (err, repo) {
	if (err) {
		console.log(err);
	}
	console.log(repo);
});*/

/*sgit.commit_bypath('./sandbox','test.c','here is my commit message',function (err) {
	if (err) {
		console.log(err);
	}
});*/

sgit.log('./',function(err,log) {
	if (err) {
		console.log(err);
	}
	//console.log(JSON.parse(log));
	console.log(log);
});
