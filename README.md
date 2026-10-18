# R-Type

Table of contents:
1. **[dependencies](#dependencies)**
2. **[git clone](#cloning-the-repository)**
3. **[Building](#building)**
4. **[how to launch](#how-to-launch)**
5. **[other doc](#other-doc)**


# dependencies
- cmake
- ninja-build
- curl
- zip
- unzip
- tar
- pkg-config
- vcpkg (installed during [git clone](#cloning-the-repository))


# cloning the repository

```sh
git clone --recurse-submodules https://github.com/Pereira-Romeo/R-Type
```
`--recurse-submodules` clones vcpkg at the same time

if you already cloned but without the submodules flag, you can run:
```sh
git submodule update --init
```

# building

you need only one of these 2 commands:
```sh
#for linux:
cmake --workflow --preset linux-release
#for windows:
cmake --workflow --preset windows-release
```

# How to launch

after **[building](#building)**,
on linux:
```sh
#to start the server in debug:
./r-type_server.sh
#to start the client in debug:
./r-type_client.sh

#to start the release server:
RTYPE_PRESET=linux-release ./r-type_server.sh
#to start the release client:
RTYPE_PRESET=linux-release ./r-type_client.sh
```


# Other doc

**[Technical choices](./doc/technical_choices.md)**

