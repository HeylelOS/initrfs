project('initrfs', 'c',
	default_options : [ 'c_std=c2x' ],
	license : 'BSD-3-Clause'
)

add_project_arguments(
	'-nostdinc', '-ffreestanding',
	'-Wno-unused-command-line-argument', # Assembly warnings are obnoxious
	language : 'c'
)
add_project_link_arguments('-nostdlib', language : 'c')

###########
# Targets #
###########

subdir('include')
subdir('src/mkinitrfs')
subdir('src/libinitcrt')
subdir('src/init')

########
# Test #
########

subdir('test')
