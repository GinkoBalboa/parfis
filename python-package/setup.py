import sys
import os
import setuptools
from platform import system

# You can't use `pip install .` as pip copies setup.py to a temporary
# directory, parent directory is no longer reachable (isolated build) .
CURRENT_DIR = os.path.abspath(os.path.dirname(__file__))
sys.path.insert(0, CURRENT_DIR)

def lib_name() -> str:
    '''Return platform dependent shared object name.'''
    if system() == 'Linux' or system().upper().endswith('BSD'):
        name = 'libpafis.so'
    elif system() == 'Windows':
        name = 'pafis.dll'
    return name

if __name__ == '__main__':
    
    # Supported commands:
    # From internet:
    #   pip install parfis

    # From source tree `xgboost/python-package`:
    #   python setup.py bdist_wheel && pip install <wheel-name>

    with open("README.md", "r") as fh:
        long_description = fh.read()

    setuptools.setup(
        name="parfis",
        version="0.0.5-dev",
        author="Ginko Balboa",
        author_email="ginkobalboa3@gmail.com",
        description="Particles and field simulator",
        packages=setuptools.find_packages(include=['parfis', 'parfis.*']),
        #package_data={"parfis": ['data/config/api_1.ini']},
        include_package_data=True,
        platforms=["Windows 10 x64"],
        # extras_require={'plotting': ['matplotlib'],
        #                 'data': ['numpy'],
        #                 'testing':['unittest']},
        long_description=long_description,
        long_description_content_type="text/markdown",
        url="https://github.com/GinkoBalboa/parfis",
        classifiers=[
            "Development Status :: 1 - Planning",
            "Intended Audience :: Science/Research",
            "Programming Language :: Python :: 3.7",
            "Programming Language :: Python :: 3.8",
            "Programming Language :: Python :: 3.9",
            "Programming Language :: Python :: 3.10",
            "Programming Language :: C++",
            "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
            "Operating System :: Microsoft :: Windows :: Windows 10",
            "Operating System :: POSIX :: Linux",
            "Topic :: Scientific/Engineering :: Physics",
        ],
        python_requires='>=3.7',
        zip_safe=False,
    )