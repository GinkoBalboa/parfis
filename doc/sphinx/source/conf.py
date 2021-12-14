# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

# The full version, including alpha/beta/rc tags
version = 'v0.0.5-dev'
git_tag = 'v0.0.5-dev-5-ge700f34'
git_rev = 'e700f34+'

project = 'Parfis'
author = 'Ginko Balboa'
copyright = f'2021, by {author}, {git_tag}'

# Uncomment bellow block of code if you want to use |variable| in .rst files

# variables_to_export = [
#     "version",
#     "git_tag",
#     "git_rev"
# ]
# frozen_locals = dict(locals())
# rst_epilog = '\n'.join(map(lambda x: f".. |{x}| replace:: {frozen_locals[x]}", variables_to_export))
# del frozen_locals

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.autosectionlabel',
    'sphinx.ext.todo',
    'sphinx.ext.autodoc',
    'sphinx.ext.napoleon',
    'sphinx_last_updated_by_git',
    'sphinx.ext.mathjax',
    'breathe'
    #'nbsphinx'
]

breathe_domain_by_extension = {"h": "cpp"}
breathe_default_project = "Parfis"
breathe_projects = { "Parfis": "./doc/doxygen/xml" }

# Display todos by setting to True
todo_include_todos = True

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 
                    '**.ipynb_checkpoints']

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#

html_theme = 'sphinx_rtd_theme'

html_context = {
'display_github': True,  # Add 'Edit on Github' link instead of 'View page source'
'last_updated': True,
'commit': False
}

rst_prolog = """
:github_url: https://github.com/GinkoBalboa/parfis
"""

html_title = 'Parfis'

master_doc = 'index'

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
# html_static_path = ['static']
html_copy_source = False

# Enable numref
numfig = True
