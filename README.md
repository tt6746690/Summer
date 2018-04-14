
# Theros

> Theros is a light-wieght web microframework sitting on shoulders of Asio

#### Features

+ Partial implementation of HTTP/1.1
+ HTTPS support with Asio's wrapper around OpenSSL
+ Compact trie based router
    + Routing path pattern matching
    + Variadic callables
+ Encoding/Decoding Utilities 
    + base64 
    + sha256
+ Extensible with Middlewares
    + CORS 


#### FAQ

+ __Dependencies__
    + __Project__ 
        + [Asio](http://think-async.com/Asio)
        + [OpenSSL](https://github.com/openssl/openssl)
        + [nlohmann/json](https://github.com/nlohmann/json)
    + __Test/Build__
        + [Catch](https://github.com/philsquared/Catch)
        + [CMake](https://github.com/Kitware/CMake)
+ __C++17__
    + compiled with `clang-900.0.38` on `macOS`
+ __Compilation__ 
    ```sh 
    cmake -H. -Bbuild -Wno-dev
    cmake --build build -- -j4
    ```
+ __Test__
    ```sh 
    ./bin/testing
    ```


#### Todos

- [ ] custom allocator 
- [ ] time/space profiler 
- [ ] re-work structure of message 
- [ ] chunked transfer encoding
- [ ] multithreaded support 
- [ ] async file serving 
- [ ] body parser

