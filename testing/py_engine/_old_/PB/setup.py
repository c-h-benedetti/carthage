from distutils.core import setup, Extension

module1 = Extension(
    'spam', # must match "PyInit_name" in the C code
    include_dirs = ['/usr/local/include'],
    library_dirs = ['/usr/local/lib'],
    # libraries = ['python3.6'],
    sources = ['python_bindings.c']
)

setup (name = 'testpb',
    version = '1.0',
    description = 'This is a course package',
    ext_modules = [module1]
)

# python3 setup.py build