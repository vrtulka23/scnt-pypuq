import os
import subprocess
import sys
import setuptools
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from pathlib import Path

def load_env_file(file_path):
    with open(file_path) as file:
        for line in file:
            if line.strip() and not line.startswith('#'):
                line = line.replace("export","")
                key, value = line.split('=', 1)
                value = value.replace("\"", "")
                os.environ[key.strip()] = value.strip()
                print(key,value)

load_env_file("settings.env")

class CMakeBuild(build_ext):
    def run(self):
        # Ensure CMake is installed
        try:
            subprocess.check_call(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = Path(self.get_ext_fullpath(ext.name)).parent.resolve()
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}",
            f"-DPYTHON_EXECUTABLE={sys.executable}",
            f"-DCODE_VERSION={os.environ['CODE_VERSION']}",
        ]
        
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp)
        subprocess.check_call(['cmake', '--build', '.'], cwd=self.build_temp)

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        super().__init__(name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

setup(
    name='pypuq',
    version=os.environ['CODE_VERSION'],
    author=os.environ['CODE_AUTHOR'],
    description=os.environ['CODE_DESCRIPTION'],
    url='https://github.com/vrtulka23/scnt-pypuq',
    project_urls={
        "Documentation": "https://vrtulka23.github.io/scnt-docs/",
    },
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    ext_modules=[CMakeExtension('pypuq')],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
    packages=setuptools.find_packages(),
    include_package_data=True,
    classifiers=[
        'Programming Language :: C',
        'Programming Language :: C++',
        'Intended Audience :: Education',
        'Intended Audience :: Science/Research',
        'Intended Audience :: Developers',
        'Topic :: Scientific/Engineering :: Physics',
        'Topic :: Scientific/Engineering :: Chemistry',
        'Topic :: Scientific/Engineering :: Astronomy',
        'Topic :: Scientific/Engineering :: Mathematics',
        'Topic :: Software Development :: Libraries :: Application Frameworks',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'Programming Language :: Python :: Implementation :: CPython',
    ],
)
