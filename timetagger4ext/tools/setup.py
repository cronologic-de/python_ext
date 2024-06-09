import os
import numpy
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext as build_ext_orig
from setuptools.command.install import install as install_orig
from setuptools.command.develop import develop as develop_orig

class BuildExt(build_ext_orig):
    def build_extensions(self):
        compiler_type = self.compiler.compiler_type
        opts = []
        if compiler_type == 'msvc':
            opts.append('/EHsc')
        else:
            opts.append('-std=c++11')
        for ext in self.extensions:
            ext.extra_compile_args = opts
        build_ext_orig.build_extensions(self)

class CustomInstallCommand(install_orig):
    def run(self):
        install_orig.run(self)
        self._post_install()

    def _post_install(self):
        self._copy_dll()

    def _copy_dll(self):
        dll_src = os.path.abspath('../lib/xtdc4_driver_64.dll')
        dll_dst = os.path.join(self.install_lib, 'crono_exts', 'xtdc4_driver_64.dll')
        self.copy_file(dll_src, dll_dst)

class CustomDevelopCommand(develop_orig):
    def run(self):
        develop_orig.run(self)
        self._post_develop()

    def _post_develop(self):
        self._copy_dll()

    def _copy_dll(self):
        dll_src = os.path.abspath('../lib/xtdc4_driver_64.dll')
        dll_dst = os.path.join(self.install_lib, 'crono_exts', 'xtdc4_driver_64.dll')
        self.copy_file(dll_src, dll_dst)

# Define the extension module
extension_mod = Extension(
    'crono_exts.timetagger4vector',     # From "PyMODINIT_FUNC PyInit_timetagger4vector"
    sources=['../src/crono_exts/timetagger4ext.cpp'], 
    include_dirs=[
        numpy.get_include(),     # Include the NumPy headers
        os.path.abspath('../include'),  # Include the additional ../include directory
    ],
    language='c++',                  # Language of the source file(s)
    libraries=['xtdc4_driver_64'],      # Add your external libraries here, no extension
    library_dirs=[os.path.abspath('../lib')], # Directory of the external libraries
)

# Setup function
setup(
    name='crono_exts',             # Name of the package
    version='0.1',
    description='A sample C++ extension using CPython',
    package_dir={'': '../src'},         # Root directory for the package
    packages=['crono_exts'],       # List of packages to include
    ext_modules=[extension_mod],     # List of extension modules
    cmdclass={
        'build_ext': BuildExt,
        'install': CustomInstallCommand,
        'develop': CustomDevelopCommand,
    },
    package_data={
        'crono_exts': ['xtdc4_driver_64.dll'],
    },
    include_package_data=True,
    install_requires=[
        'numpy>=1.21.0',  # Specify the required version of NumPy
    ],
)
