# python_ext

## Build the package
1. Just make sure that the following packages are installed
```
<path-to-python-installation-folder>/pip install numpy setuptools wheel
```
2. cd to `python_ext\timetagger4ext\tools` and run:
```
python setup.py bdist_wheel sdist
```
It will build both the package binaries and source distribution.
Output:
- In directory: <path-to-python_ext>\timetagger4ext\tools\dist
- Wheel package: `crono_exts-0.1-cp311-cp311-win_amd64.whl`.
- Source: `crono_exts-0.1.tar.gz`

## Next steps:
- Install the package and make sure that the driver dll is installed on the system folder.
- Ensure that the extension APIs are exported, and use them in a sample python app.
- Ensure all source is added to the source package.
