# Change log

## 1.8.0 (unreleased)

Features:

  - Github Actions: automatic testing and preparing wheels for PyPI

## 1.7.0 (2014-08-13)

Security:

  - Fix for CVE-2013-0334, installing gems from an unexpected source -> [95f32s5b](http://www.google.com)

Features:

  - Gemfile `source` calls now take a block containing gems from that source -> [95f32s5b](http://www.google.com)
  - added the `:source` option to `gem` to specify a source -> [95f32s5b](http://www.google.com)

Fix:

  - warn on ambiguous gems available from more than one source -> [95f32s5b](http://www.google.com)

## < 0.0.5

Info:
  - Versions 0.0.5 and higher are based on somewhat different concepts 
    in comparison to earlier versions.
  - Algorithms remain the same.