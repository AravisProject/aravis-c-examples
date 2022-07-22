project('aravis-c-examples', 'c', version: '0.0.1')

aravis_dep = dependency('aravis-0.8')

examples = [
  '01-single-acquisition',
  '02-multiple-acquisition',
  '02-multiple-acquisition-signal',
  '03-camera-api',
  '04-camera-features',
]

foreach e: examples
  exe = executable (e, e + '.c', dependencies: aravis_dep)
endforeach
