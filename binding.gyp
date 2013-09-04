{
	'variables' : { 'foo':'<!(/bin/bash ./compile_libgit2.sh)>', },
	"targets": [
		{
			"target_name": "sgit",
			"sources": [ 
				"src/sgit.cc" 
			],
			"include_dirs": [
				"deps/libgit2/include",
				"/usr/loval/include"
			],
			"libraries": [
				"-L<!(pwd)/deps/libgit2/libgit2.so",
				"-lgit2",
				"-lboost_thread"
				
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
