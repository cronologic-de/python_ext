# python_ext

## Overview
- [`setup.py`](./timetagger4ext/tools/setup.py) is used by python to create `setuptools` pakcage.

## Build the package
**Prerequisites**

1. Just make sure that the following packages are installed
```
<path-to-python-installation-folder>/pip install numpy setuptools wheel
```

**Steps**

cd to `python_ext\timetagger4ext\tools` and run:
```
python setup.py bdist_wheel sdist
```

It will build both the package binaries and source distribution.
Output:
- In directory: `python_ext\timetagger4ext\tools\dist`
- Wheel package: `crono_exts-0.1-cp311-cp311-win_amd64.whl`.
- Source: `crono_exts-0.1.tar.gz`

## Install the package 
cd to `python_ext\timetagger4ext\tools\dist`, run:
```
pip install crono_exts-0.1-cp311-cp311-win_amd64.whl
```
You should find:
```
Successfully installed crono-exts-0.1
```
## Next steps:
- Ensure that the extension APIs are exported, dll is available, and use them in a sample python app.
- Ensure all source is added to the source package.
- Fill `clasifiers` in `setup.py`.
- Try the package on `test.pypi`, could be using `twine`
  ```
  twine check dist/*
  twine upload dist/* -r testpypi
  ```
- Setup MSVC Projects for its users.
- Support Linux.
