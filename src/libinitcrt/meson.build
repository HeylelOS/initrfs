libinitcrt = static_library('initcrt',
	include_directories : headers,
	sources : [
		join_paths(host_machine.cpu_family(), 'crt0.s'),
		join_paths(host_machine.cpu_family(), 'syscalls.s'),
		'ctype.c',
		'dirent.c',
		'err.c',
		'errno.c',
		'stdio.c',
		'stdlib.c',
		'string.c',
		'sys/mount.c',
		'unistd.c',
	]
)
