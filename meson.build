project('Locks', 'cpp', version: '1.0')

compiler = meson.get_compiler('cpp')

LocksLib = static_library(
	'Locks',
	sources: [
		'Mutex.cpp',
		'RecursiveLock.cpp',
		'ConditionVariable.cpp',
		'Sem.cpp',
		'Timer.cpp',
	],
	include_directories: [
		'Locks',
		'/boot/system/develop/headers/private/shared',
		'/boot/system/develop/headers/private/system',
		'/boot/system/develop/headers/private/system/arch/x86_64',
	]
)

Locks = declare_dependency(
	link_with : LocksLib,
	include_directories: [
		'.',
		'/boot/system/develop/headers/private/shared'
	],
)
