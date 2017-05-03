[![Build Status](https://travis-ci.org/bitprim/bitprim-server?branch=master)](https://travis-ci.org/bitprim/bitprim-server)

# Bitprim Server

*Bitprim Server*

Make sure you have installed [bitprim-core](https://github.com/bitprim/bitprim-core), [bitprim-database](https://github.com/bitprim/bitprim-database), [bitprim-blockchain](https://github.com/bitprim/bitprim-blockchain), [bitprim-consensus](https://github.com/bitprim/bitprim-consensus) (optional), [bitprim-network](https://github.com/bitprim/bitprim-network), [bitprim-node](https://github.com/bitprim/bitprim-node) and [bitprim-protocol](https://github.com/bitprim/bitprim-protocol) beforehand according to its build instructions.

```
$ git clone https://github.com/bitprim/bitprim-server.git
$ cd bitprim-server
$ mkdir build
$ cd build
$ cmake .. -DENABLE_TESTS=OFF -DWITH_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-std=c++11" 
$ make -j2
$ sudo make install
```

bitprim-server is now installed in `/usr/local/`.
