{
	'variables' : { 'foo':'<!(/bin/bash ./compile_libgit2.sh)>', },
	"targets": [
		{
			"target_name": "sgit",
			"sources": [ 
				"src/sgit.cc", 
				"src/init.cc",
				"src/commit.cc",
				"src/log.cc"
			],
			"include_dirs": [
				"deps/libgit2/include"
			],
			"libraries": [
				"-L<!(pwd)/deps/libgit2/libgit2.so",
				"-lgit2"	
			],
			'cflags': [
				'-g -Wall'
			],
			'ldflags': [
                                '-Wl,-rpath,\$$ORIGIN/../../deps/libgit2'
                        ]
		}
	],
}
