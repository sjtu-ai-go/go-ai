# go-ai
[![Build Status](https://travis-ci.org/sjtu-ai-go/go-ai.svg)](https://travis-ci.org/sjtu-ai-go/go-ai)
[![GNU3 License](https://img.shields.io/github/license/sjtu-ai-go/go-ai.svg)](https://github.com/sjtu-ai-go/go-ai/blob/master/LICENSE)

Toy Go game AI

## Setup

### Clone

```
git clone --recursive {{Git address of this repository (you can find it by clicking the green "Clone or Download" button)}}
```

### Install Vagrant

Vagrant is a middleware between virtual machine(provided by VirtualBox in this repo) and user. It can be installed on Windows, Mac and Linux, which bridges the difference between host and developing environment.

[Windows installation guide](https://blog.smdcn.net/article/1308.html); [Official guide](https://www.vagrantup.com/docs/installation/). Note that Vagrant itself is not a virtual machine provider, thus you need to install one(VirtualBox recommended) before installing Vagrant.


After installation, run `vagrant up` under this directory(may take long time because it downloads the whole Ubuntu image).

Check whether you can log into this box by `vagrant ssh`(on Linux/Mac) or the way metioned in Windows Installation guide.

The virtual machine run in the box shares current directory in `/vagrant`, which means the common workflow is like this:
- Edit files on your host machine
- `vagrant ssh` or anything else to get into the box and cd `/vagrant`
- Compile it and run tests

This virtual machine also maps port 8080 to host port 8080.

### Build

#### Build inside vagrant(recommended for dev/test)
Building&Runninginside the vagrant box is recommended for development and testing, since the `Vagrantfile` unifies the environment.

Log into the box. Then:

```bash
cd /vagrant  # you should see code in this directory
mkdir build  # Temporary directory
cd build
cmake -DCMake_Build_Type=Debug ..     # This step will generate Makefile with debug flags
# Or: cmake -DCMake_Build_Type=Release .. 
make -j4     # Compile with 4 threads

# After above steps binaries should be built under build/

# To run tests:
ctest
```

It is not recommended to benchmark built binaries inside virtual machine due to performance.
#### Build with Xnix or Mingw64/Cygwin on windows
Almost the same as above. Install `libboost-all-dev`, `cmake`, `g++`(>=5) or `clang++` first.

Cygwin builds may suffer performance loss.

#### Build with MSVC(Untested)
CMake could transform `CMakeLists.txt` into MSVC solution file `sln`:

- Create a `_build` directory
- Run `cmake .. -G "Visual Studio 10 Win64"` under `_build` (Replace VS version by yours)
- Open generated `sln` file with your MSVC, then compile


### Keep updated

#### Keep Vagrant image updated
```
vagrant provision
```

#### Keep submodules updated
```
git submodule update
```
