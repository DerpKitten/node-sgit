{
	'targets': [
		{
			'target_name': 'sgit',
			'sources': [
				'src/sgit.cc',
				'src/repository.cc',
				'src/baton.cc',
			],
			'include_dirs': [
				'deps/v8-convert',
				'deps/libgit2/include'
			],

			'libraries': [
				'-L<!(pwd)/deps/libgit2/build',
				'-lgit2'
			],

			'cflags': [
				'-Wall'
			],

			'ldflags': [
				'-Wl,-rpath,\$$ORIGIN/../../deps/libgit2/build'
			],

			'conditions': [
				['OS=="mac"', {
					'xcode_settings': {
						'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
					}
				}]
			]
		}
	]
}
