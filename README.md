# python_ext

## Overview
- [`setup.py`](./timetagger4ext/tools/setup.py) is used by python to create `setuptools` pakcage.
- As per `setup.py` and project settings:
| Module | `crono_exts.timetagger4vector` |
| Python Installation folder in `vcxproj` | `C:\Dev\Python311` |
| Python folder on the Windows Path | `C:\Dev\Python311` |

## Build the package from Command Line
### Prerequisites
1. Just make sure that the following packages are installed
```
<path-to-python-installation-folder>/pip install numpy setuptools wheel
```

2. You would need Visual Studio 2022 (not VS Code), you would need to enable C++ and python development in the Installer.  
3. To configure it for your environment you would not to set the paths for your install , mentioned in the following document:
[Write C++ extensions for Python - Visual Studio (Windows) | Microsoft Learn](https://learn.microsoft.com/en-us/visualstudio/python/working-with-c-cpp-python-in-visual-studio?view=vs-2022).

### Steps
cd to `python_ext\timetagger4ext\tools` and run:
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
- Ensure all source is added to the source package.


# `ReadOut` App

## Overview
- Sample application that uses the extension.
- Uses `crono_exts.timetagger4vector` extension.

## Create MSVS Project

### Prerequisites
- Check prerequisites of the extension.

### Steps
- Create "Python Application" project from Visual Studio.
- If not created, then create an environment from python installation: Right click `TimeTagger4ReadOut -> Python Environments` then `Add Environment` then select the underlying environment from `Base interpreter`, assuming called `env1`.
- Install `numpy`: Right click `TimeTagger4ReadOut -> Python Environments -> env1` then `Manage Python Packages...`, then enter `numpy` and install it.
- Install our extension: Right click `TimeTagger4ReadOut -> Python Environments -> env1` then `Open Interactive Window`, then:
  ```
  pip install <path-to-whl>
  ```


# Extras

## Which Python Installation to Use on Windows for MSVS
Here are trials with the results, just to avoid in future any one has problems, last one works successfully:

### 1 - Using **bundled python**
Installing `numpy` on Visual Studio bundled python:
```
C:\Program Files (x86)\Microsoft Visual Studio\Shared\Python39_64>.\scripts\pip install numpy
````
This installed it on:
```
C:\Users\basse\AppData\Roaming\Python\Python39\site-packages
```

**Conclusion**

* A bit annoying setting the paths, and is an old version, trying with the latest python versions.

### 2 - Using **latest python version 3.12**
- Installing latest python: 3.12.
- When setting it as an environment in Visual Studio, a warning that this "This python version is not supported by Visual Studio".
- No problem with python immediately previous version, it's supported.

**Conclusion**

* Not supported by MSVS.

### 3 - Using an **external python 3.11.9**
- Installing python version 3.11.9, asking installer to set it in the environment variables.
- Select it as an environment in Visual Studio Project: Solution Explorer -> TimeTagger4ReadOut -> Python Environments -> Add Environment. Select it from "Base interpreter".
- Install `numpy` from the project environment. 
    - Right click 'env (Ptyhon 3.11(64-bit))' in the Solution Explorer
    - Click "Manage Python Packages"
    - Enter "numpy", as it to install it.
    - Installing numpy on the python environment (C:\Dev\Python311) externally somehow doesn't avail it for the project, the above way works well.
- Update link and include paths in the project to `C:\Dev\Python311`.
- Build all succeeds.
- When running, `import numpy as np` is OK, but:
  line:
  ```
  import timetagger4vector as tt4v
  ```
  Caused a run-time error
  ```
  DLL load failed while importing timetagger4vector: The specified module could not be found.
  ```
  - I copied `xtdc4_driver_64.dll` & `timetagger4vector.pyd` to the .py folder (F:\Work\crono-all\timetagger4python\TimeTagger4ReadOut) and it worked.

**Conclusion**
 
* Used successfully.