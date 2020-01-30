# Coro Examples

This project created for demonstration purposes only. It contains examples of how to use coroutines.

## Build requirements

python 2.7, cmake >=3.5, qt5, msvc15

### Build process

Setup repo for build:

```
git clone --recursive https://github.com/Supremist/CoroExamples # clone, recursively init submodules
cd CoroExamples/cppcoro
export PATH="$PATH:/d/Python27:/d/Python27/Scripts" # enable python 2
./cake.bat release=debug architecture=x64 lib/build.cake # build cppcoro
```

Then you can configure it with cmake.



