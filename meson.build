project('patchcreator', 'c',
  version: '1.0',
  default_options: ['warning_level=2', 'c_std=c11'])

# Include directory
#inc = include_directories('include')
lzma_dep = dependency('liblzma', required: true)

createpatch_sources = files(
  'src/createpatch.c'
)

makepatch_sources = files(
  'src/makepatch.c'
)

# Executable target
executable('makepatch', 'createpatch.c',
  dependencies: lzma_dep)

executable('createpatch', 'makepatch.c',
  dependencies: lzma_dep)

