init = executable('init',
	include_directories : headers,
	link_args : [ '-static' ],
	link_with : libinitcrt,
	sources : [
		'configuration.c',
		'kernelcmdline.c',
		'main.c',
		'mountutils.c',
		'switchroot.c',
	]
)
