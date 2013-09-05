var sgit = require('./lib/sgit');
var fs = require("fs");

sgit.init_repository('./sandbox',function (err, repo) {
	if (err) {
		console.log(err);
	}
	console.log("Created Repository: " + repo);
});

//create file to commit
if (!fs.existsSync('./sandbox/newfolder')) {
	fs.mkdirSync('./sandbox/newfolder');
}
fs.writeFileSync ('./sandbox/newfolder/test.c','test stuff');

sgit.commit_bypath('./sandbox/newfolder/test.c','here is my commit message',function (err,commit_id) {
	if (err) {
		console.log(err);
	}
	console.log("commit id: " + commit_id);
});

sgit.log('./sandbox',function(err,log) {
	if (err) {
		console.log(err);
	}
	console.log(log);
});
