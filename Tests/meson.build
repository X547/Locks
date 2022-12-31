project('LocksTests', 'cpp', version: '1.0')

compiler = meson.get_compiler('cpp')

LocksProj = subproject('Locks')
Locks = LocksProj.get_variable('Locks')

libbe = compiler.find_library('be')


executable('Tests', sources: ['Tests.cpp'], dependencies: [Locks, libbe], install: true)
executable('Tests2', sources: ['Tests2.cpp'], dependencies: [Locks], install: true)
