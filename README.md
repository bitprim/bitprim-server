[![Build Status](https://travis-ci.org/bitprim/bitprim-server?branch=master)](https://travis-ci.org/bitprim/bitprim-server)

# Bitprim Server

*Bitprim Server*

**License Overview**

All files in this repository fall under the license specified in [COPYING](https://github.com/libbitcoin/libbitcoin-server/blob/master/COPYING). The project is licensed as [AGPL with a lesser clause](https://wiki.unsystem.net/en/index.php/Libbitcoin/License). It may be used within a proprietary project, but the core library and any changes to it must be published on-line. Source code for this library must always remain free for everybody to access.

**About Libbitcoin**

The libbitcoin toolkit is a set of cross platform C++ libraries for building bitcoin applications. The toolkit consists of several libraries, most of which depend on the foundational [libbitcoin](https://github.com/libbitcoin/libbitcoin) library. Each library's repository can be cloned and built using common [Automake](http://www.gnu.org/software/automake) instructions.

**About Libbitcoin Server**

A full Bitcoin peer-to-peer node, Libbitcoin Server is also a high performance blockchain query server. It can be built as a single portable executable for Linux, OSX or Windows and is available for download as a signed single executable for each. It is trivial to deploy, just run the single process and allow it about two days to synchronize the Bitcoin blockchain.

Libbitcoin Server exposes a custom query TCP API built based on the [ZeroMQ](http://zeromq.org) networking stack. It supports server, and optionally client, identity certificates and wire encryption via [CurveZMQ](http://curvezmq.org) and the [Sodium](http://libsodium.org) cryptographic library.

**About Bitprim**

The Bitprim toolkit is a set of cross platform C++ libraries for building bitcoin applications. The toolkit consists of several libraries, most of which depend on the foundational [bitprim-core](https://github.com/bitprim/bitprim-core) library. Each library's repository can be cloned and built using common [automake](http://www.gnu.org/software/automake) 1.14+ instructions. 

## Installation

A minimal bitprim build requires boost and libsecp256k1. The [bitprim/secp256k1](https://github.com/bitprim/secp256k1) repository is forked from [bitcoin-core/secp256k1](https://github.com/bitcoin-core/secp256k1) in order to control for changes and to incorporate the necessary Visual Studio build. The original repository can be used directly but recent changes to the public interface may cause build breaks. The `--enable-module-recovery` switch is required.

Detailed instructions are provided below.
  * [Debian/Ubuntu](#debianubuntu)
  * [Macintosh](#macintosh)
  * [Windows](#windows)

### Debian/Ubuntu

Bitprim requires a C++11 compiler, currently minimum [GCC 4.8.0](https://gcc.gnu.org/projects/cxx0x.html) or Clang based on [LLVM 3.5](http://llvm.org/releases/3.5.0/docs/ReleaseNotes.html).

To see your GCC version:
```sh
$ g++ --version
```
```
g++ (Ubuntu 4.8.2-19ubuntu1) 4.8.2
Copyright (C) 2013 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```
If necessary, upgrade your compiler as follows:
```sh
$ sudo apt-get install g++-4.8
$ sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50
$ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50
$ sudo update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-4.8 50
```
Next install the [build system](http://wikipedia.org/wiki/GNU_build_system) (Automake minimum 1.14) and git:
```sh
$ sudo apt-get install build-essential autoconf automake libtool pkg-config git 
```
Next install the [Boost](http://www.boost.org) (minimum 1.56.0) development package:
```sh
$ sudo apt-get install libboost-all-dev
```
Next download the [install script](https://github.com/libbitcoin/libbitcoin-server/blob/version3/install.sh) and enable execution:
```sh
$ wget https://raw.githubusercontent.com/libbitcoin/libbitcoin-server/version3/install.sh
$ chmod +x install.sh
```
Next install the [ZeroMQ](http://zeromq.org/) (minimum 4.2.0) development package:
```sh
$ git clone https://github.com/zeromq/libzmq
$ cd libzmq
$ ./autogen.sh && ./configure && make -j 4
$ make check
$ sudo make install
$ sudo ldconfig
```

Next install the [Protobuff](https://github.com/google/protobuf) (minimum 3.0.0) development package:
```sh
$ sudo apt-get install autoconf automake libtool curl make g++ unzip
$ git clone https://github.com/google/protobuf
$ cd protobuf
$ ./autogen.sh 
$ ./configure
$ make
$ make check
$ sudo make install
$ sudo ldconfig
```

Next install the [CMake](https://cmake.org/) (minimum 3.7.0-rc1) development package:
```sh
$ wget https://cmake.org/files/v3.7/cmake-3.7.0-rc1.tar.gz
$ tar -xvzf cmake-3.7.0-rc1.tar.gz
$ cd cmake-3.7.0-rc1
$ ./bootstrap
$ make 
$ sudo make install
$ sudo ln -s /usr/local/bin/cmake /usr/bin/cmake
```

Finally, install bitprim-build:
```sh
$ git clone --recursive https://github.com/bitprim/bitprim-build/
$ cd bitprim-build
$ mkdir build
$ cd build
$ cmake .. -G "Unix Makefiles" -DENABLE_TESTS=OFF -DWITH_TESTS=OFF -DWITH_TOOLS=OFF -DCMAKE_BUILD_TYPE=Release
$ make
$ sudo make install
$ sudo ldconfig
```
Bitprim is now installed in `/usr/local/`.

### Macintosh

The OSX installation differs from Linux in the installation of the compiler and packaged dependencies. Bitprim supports both [Homebrew](http://brew.sh) and [MacPorts](https://www.macports.org) package managers. Both require Apple's [Xcode](https://developer.apple.com/xcode) command line tools. Neither requires Xcode as the tools may be installed independently.

Bitprim compiles with Clang on OSX and requires C++11 support. Installation has been verified using Clang based on [LLVM 3.5](http://llvm.org/releases/3.5.0/docs/ReleaseNotes.html). This version or newer should be installed as part of the Xcode command line tools.

To see your Clang/LLVM  version:
```sh
$ clang++ --version
```
```
Apple LLVM version 6.0 (clang-600.0.54) (based on LLVM 3.5svn)
Target: x86_64-apple-darwin14.0.0
Thread model: posix
```
If required update your version of the command line tools as follows:
```sh
$ xcode-select --install
```

#### Using Homebrew

First install Homebrew. Installation requires [Ruby](https://www.ruby-lang.org/en) and [cURL](http://curl.haxx.se), which are pre-installed on OSX.
```sh
$ ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```
You may encounter a prompt to install the Xcode command line developer tools, in which case accept the prompt.

Next install the [build system](http://wikipedia.org/wiki/GNU_build_system) (Automake minimum 1.14) and [wget](http://www.gnu.org/software/wget):
```sh
$ brew install autoconf automake libtool pkgconfig wget
```
Next install the [Boost](http://www.boost.org) (1.56.0 or newer) development package:
```sh
$ brew install boost
```
Next download the [install script](https://github.com/libbitcoin/libbitcoin-server/blob/version3/install.sh) and enable execution:
```sh
$ wget https://raw.githubusercontent.com/libbitcoin/libbitcoin-server/version3/install.sh
$ chmod +x install.sh
```
TODO: ZeroMQ Mac Install
```
Next install the [Protobuff](https://github.com/google/protobuf) (minimum 3.0.0) development package:
```
TODO: Protobuff Mac Install
```
Next download the [install script](https://github.com/libbitcoin/libbitcoin-server/blob/version3/install.sh) and enable execution:
```sh
$ wget https://raw.githubusercontent.com/libbitcoin/libbitcoin-server/version3/install.sh
$ chmod +x install.sh
```
TODO: Mac Install
```
Bitprim is now installed in `/usr/local/`.

#### Using MacPorts

First install [MacPorts](https://www.macports.org/install.php).

Next install the [build system](http://wikipedia.org/wiki/GNU_build_system) (Automake minimum 1.14) and [wget](http://www.gnu.org/software/wget):
```sh
$ sudo port install autoconf automake libtool pkgconfig wget
```
Next install the [Boost](http://www.boost.org) (1.56.0 or newer) development package. The `-` options remove MacPort defaults that are not Boost defaults:
```sh
$ sudo port install boost -no_single -no_static -python27
```
Next install the [ZeroMQ](http://zeromq.org/) (minimum 4.2.0) development package:
```
TODO: ZeroMQ MacPorts Install
```
Next install the [Protobuff](https://github.com/google/protobuf) (minimum 3.0.0) development package:
```
TODO: Protobuff MacPorts Install
```
Finally install bitprim:
```
TODO: MacPorts Install
```
Bitprim is now installed in `/usr/local/`.

#### Notes

The install script itself is commented so that the manual build steps for each dependency can be inferred by a developer.

* [Visual Studio 2013 Express](https://www.microsoft.com/en-us/download/details.aspx?id=44914)
* [November 2013 CTP Compiler](http://www.microsoft.com/en-us/download/details.aspx?id=41151)
* See also: [CTP Compiler installation issue](http://stackoverflow.com/a/34548651/1172329)

The build script clones, builds and installs two unpackaged repositories, namely:

- [bitprim/secp256k1](https://github.com/bitprim/secp256k1)
- [bitprim/bitprim-core](https://github.com/bitprim/bitprim-core)

The script builds from the head of their `version4` and `version2` branches respectively. The `master` branch is a staging area for changes. The version branches are considered release quality.

* Packages maintained by [sergey.shandar](http://www.nuget.org/profiles/sergey.shandar)
   * [boost](http://www.nuget.org/packages/boost)
   * [boost\_chrono-vc120](http://www.nuget.org/packages/boost_chrono-vc120)
   * [boost\_date\_time-vc120](http://www.nuget.org/packages/boost_date_time-vc120)
   * [boost\_filesystem-vc120](http://www.nuget.org/packages/boost_filesystem-vc120)
   * [boost\_iostreams-vc120](http://www.nuget.org/packages/boost_iostreams-vc120)
   * [boost\_log-vc120](http://www.nuget.org/packages/boost_log-vc120)
   * [boost\_program\_options-vc120](http://www.nuget.org/packages/boost_program_options-vc120)
   * [boost\_regex-vc120](http://www.nuget.org/packages/boost_regex-vc120)
   * [boost\_system-vc120](http://www.nuget.org/packages/boost_system-vc120)
   * [boost\_thread-vc120](http://www.nuget.org/packages/boost_thread-vc120)
   * [boost\_unit\_test\_framework-vc120](http://www.nuget.org/packages/boost_unit_test_framework-vc120)
* Packages maintained by [evoskuil](http://www.nuget.org/profiles/evoskuil)
   * [libzmq\_vc120](http://www.nuget.org/packages/libzmq_vc120)
   * [secp256k1\_vc120](http://www.nuget.org/packages/secp256k1_vc120)

#### Build Libbitcoin Projects

To build Libbitcoin Server you must also download and build its **libbitcoin dependencies**, as these are not yet packaged. The builds can be performed manually (from within Visual Studio) or using the `buildall.bat` script provided in the `builds\msvc\build\` subdirectory of each repository. The scripts automatically download the required NuGet packages.

> Tip: The `buildall.bat` scripts build *all* valid configurations. The build time can be significantly reduced by disabling all but the desired configuration in the `buildbase.bat` of each project.

Build these solutions in order:

1. [libbitcoin/libbitcoin](https://github.com/libbitcoin/libbitcoin)
2. [libbitcoin/libbitcoin-consensus](https://github.com/libbitcoin/libbitcoin-consensus)
2. [libbitcoin/libbitcoin-database](https://github.com/libbitcoin/libbitcoin-database)
2. [libbitcoin/libbitcoin-blockchain](https://github.com/libbitcoin/libbitcoin-blockchain)
2. [libbitcoin/libbitcoin-network](https://github.com/libbitcoin/libbitcoin-network)
2. [libbitcoin/libbitcoin-node](https://github.com/libbitcoin/libbitcoin-node)
2. [libbitcoin/libbitcoin-protocol](https://github.com/libbitcoin/libbitcoin-protocol)
3. [libbitcoin/libbitcoin-server](https://github.com/libbitcoin/libbitcoin-server)

> The libbitcoin dynamic (DLL) build configurations do not compile, as the exports have not yet been fully implemented. These are currently disabled in the build scripts but you will encounter numerous errors if you build then manually.

Configuration options are exposed in the Visual Studio property pages.

#### Optional: Build Everything

The non-boost packages above are all sourced from GitHub repositories maintained using the same [Visual Studio template](https://github.com/evoskuil/visual-studio-template) as the libbitcoin libraries. If so desired each of these can be built locally, in the same manner as the libbitcoin libraries above. This allows you to avoid using the pre-built NuGet packages. The repositories for each dependency are as follows:

* Cryptography
   * [libbitcoin/secp256k1](https://github.com/libbitcoin/secp256k1)
* Zero Message Queue
   * [zeromq/libzmq](https://github.com/zeromq/libzmq)

This change is properly accomplished by disabling the "NuGet Dependencies" in the Visual Studio properties user interface for each libbitcoin project and then importing the `.import.props` file(s) for the corresponding dependencies.

#### NuGet Repository
NuGet packages are downloaded to a local file systems repository. By default the [NuGet Package Manager](http://visualstudiogallery.msdn.microsoft.com/27077b70-9dad-4c64-adcf-c7cf6bc9970c) uses a repository path within the solution. This can complicate source control and results in multiple repositories across solutions.

A better configuration is to centralize the NuGet repository outside of your `git` directory, for example:

```
-me
    -git
        nuget.config
        -libbitcoin
        -libbitcoin-consensus
        -libbitcoin-database
        -libbitcoin-blockchain
        -libbitcoin-network
        -libbitcoin-node
        -libbitcoin-protocol
        -libbitcoin-server
            -builds
                -msvc
                    -vs2013
                        -bitcoin-server
                            bitcoin-server.vcxproj
                            packages.config
                        libbitcoin-server.sln
    -nuget
        repositories.config
        +boost.1.57.0.0
        +boost_chrono-vc120.1.57.0.0
        +boost_date_time-vc120.1.57.0.0
        +boost_filesystem-vc120.1.57.0.0
        +boost_iostreams-vc120.1.57.0.0
        +boost_log-vc120.1.57.0.0
        +boost_program_options-vc120.1.57.0.0
        +boost_regex-vc120.1.57.0.0
        +boost_system-vc120.1.57.0.0
        +boost_thread-vc120.1.57.0.0
        +boost_unit_test_framework-vc120.1.57.0.0
        +libzmq_vc120.4.2.2.0
        +secp256k1_vc120.0.1.0.13
```

If properly configured the NuGet Package Manager will share this NuGet repository across all solutions within the `git` directory. There are three steps required in this configuration:

* Create a `nuget` directory as a sibling to your `git` directory.
* Create a `nuget.config` file in the root of your `git` directory.
* Ensure there are no other `nuget.config` files in your `git` directory.

The `nuget.config` should have the [documented structure](http://docs.nuget.org/docs/reference/nuget-config-settings) and should refer to the relative `nuget` directory `..\nuget` as follows:

```xml
<configuration>
  <config>
    <!-- Allows you to install the NuGet packages in the specified folder,
    instead of the default "$(Solutiondir)\Packages" folder. -->
    <add key="repositoryPath" value="..\nuget" />
  </config>
  <solution>
    <!-- Disable source control integration for the "Packages" folder. -->
    <add key="disableSourceControlIntegration" value="true" />
  </solution>
  <packageRestore>
    <!-- Allow NuGet to download missing packages -->
    <add key="enabled" value="false" />
    <!-- Automatically check for missing packages during build in Visual Studio -->
    <add key="automatic" value="false" />
  </packageRestore>
  <packageSources>
    <!-- Allows you to specify the list of sources to be used while looking for packages. -->
    <add key="NuGet official package source" value="https://nuget.org/api/v2/" />
  </packageSources>
  <disabledPackageSources>
    <!-- "DisabledPackageSources" has the list of sources which are currently disabled. -->
  </disabledPackageSources>
  <activePackageSource>
    <!-- "ActivePackageSource" points to the currently active source. 
    Specifying "(Aggregate source)" as the source value would imply that
    all the current package sources except for the disabled ones are active. -->
    <add key="All" value="(Aggregate source)"  />
  </activePackageSource>
  <packageSourceCredentials>
    <!-- Allows you to set the credentials to access the given package source. -->
    <!-- <feedName>
    <add key="Username" value="foobar" />
    <add key="ClearTextPassword" value="secret" />
    </feedName> -->
  </packageSourceCredentials>
</configuration>
```

With this configuration in place you should experience the following behavior. When you open one of these Visual Studio projects and then open the Package Manager, you may be informed that there are missing packages. Upon authorizing download of the packages you will see them appear in the `nuget` directory. You will then be able to compile the project(s).
