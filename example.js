var sgit = require('./build/Release/sgit');

sgit.init_repository('./sandbox',function (err, repo) {
	if (err) {
		console.log(err);
	}
	console.log(repo);
});

