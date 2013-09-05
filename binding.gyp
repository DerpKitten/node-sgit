{
	'variables' : { 'foo':'<!(/bin/bash ./compile_libgit2.sh)>', },
	"targets": [
		{
			"target_name": "sgit",
			"sources": [ 
				"src/sgit.cc" 
			],
			"include_dirs": [
				"deps/libgit2/include"
			],
			"libraries": [
				"-L<!(pwd)/deps/libgit2/libgit2.so",
				"-lgit2"	
			],
			'cflags': [
				'-Wall'
			],
			'ldflags': [
                                '-Wl,-rpath,\$$ORIGIN/../../deps/libgit2'
                        ]
		}
	],
}
