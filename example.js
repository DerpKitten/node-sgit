var sgit = require('./lib/sgit');
var fs = require("fs");
sgit.init('./sandbox',function (err) {
	if (err) {
		console.log(err);
	}

	//create file to commit
	if (!fs.existsSync('./sandbox/newfolder')) {
		fs.mkdirSync('./sandbox/newfolder');
	}
	fs.writeFileSync ('./sandbox/newfolder/test.c','test stuff');
	console.log("Created Repository");

	//commit test file to repo
	sgit.commit('./sandbox/newfolder/test.c','here is my commit message',function (err,commit_id) {
		if (err) {
			console.log(err);
		}
		console.log("commit id: " + commit_id);
	
		//run log on newly created repo with newly added commit			
		sgit.log('./sandbox',function(err,log) {
			if (err) {
				console.log(err);
			}
			console.log(log);
		});
	});
});
