import sys
import os
import time
import setuptools
import subprocess
from platform import system

# You can't use `pip install .` as pip copies setup.py to a temporary
# directory, parent directory is no longer reachable (isolated build) .
CURRENT_DIR = os.path.abspath(os.path.dirname(__file__))
sys.path.insert(0, CURRENT_DIR)

if __name__ == '__main__':
    
    # Uploading new release:
    # - Check upload artifacts when running action, run actions
    # - Downlad artifact wheels on your local machine
    # - Create empty folder and put all *.whl files in the same folder
    # - From that folder call:
    #     twine upload ./*
    
    # Supported commands:
    # From internet:
    #   pip install parfis
    # Locally for testing (from parfis/python-packate dir)
    #   pip install -e . --force-reinstall -v
    # Upload to test.pypi
    #   twine upload --repository testpypi dist/*
    # Istall from test.pypi
    #   pip install --force-reinstall -i https://test.pypi.org/simple/ parfis

    with open("README.md", "r") as fh:
        long_description = fh.read()

    # If tag.txt is supplied use that (for testing purposes only)
    if os.path.isfile("tag.txt") == False:
        subprocess.Popen("git describe --tags --abbrev=0 > tag.txt", shell=True)

    while os.path.isfile("tag.txt") == False:
        time.sleep(1)

    tagName = "0.0.0"
    with open('tag.txt', 'r') as file:
        tagName = file.read().rstrip()

    print(f"Tag name is: {tagName}")
    setuptools.setup(
        name="parfis",
        version=tagName,
        author="Ginko Balboa",
        author_email="ginkobalboa3@gmail.com",
        description="Particles and field simulator",
        packages=setuptools.find_packages(include=['parfis', 'parfis.*']),
        #package_data={"parfis": ['data/config/api_1.ini']},
        include_package_data=True,
        # extras_require={'plotting': ['matplotlib'],
        #                 'data': ['numpy'],
        #                 'testing':['unittest']},
        long_description=long_description,
        long_description_content_type="text/markdown",
        project_urls={"github": "https://github.com/GinkoBalboa/parfis",
                      "documentation": "https://www.parfis.com"},
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
