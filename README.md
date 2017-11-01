


### FAQ 

+ __Dependencies__
    + [Asio](http://think-async.com/Asio)
    + [Catch](https://github.com/philsquared/Catch)
    + [nlohmann/json](https://github.com/nlohmann/json)
    + [OpenSSL](https://github.com/openssl/openssl)
    + [CMake](https://github.com/Kitware/CMake)
+ __C++17__
    + flags `-std=c++1z -stdlib=libc++`
    + compiled with `clang-802.0.42` on `macOS`
+ __Build__
    ```sh
    cmake -H. -Bbuild -Wno-dev 
    cmake --build build -- -j5
    ```
+ __Run__
    ```sh 
    ./bin/server
    ```

### A simple `Http` library

+ Partial implementation (as much as needed) of HTTP/1.1
    + Request parsing and response writing
    + Uri percent encoding/decoding
+ Compact-trie based router to handle 
    + request-response cycle 
    + middleware stuff
+ HTTPS with Asio's wrapper around OpenSSL 
+ Middlewares
    + CORS

### Todos

- [ ] chunked transfer encoding
- [ ] Multithreaded support 
- [ ] async file serving 
- [ ] body parser



### Resources

+ [Asio doc](http://think-async.com/Asio/asio-1.10.6/doc/)
+ [HTTP/1.1 standard](https://www.w3.org/Protocols/rfc2616/rfc2616.html)
+ [mozilla doc on HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP/)
+ [Google CPP style guide](https://google.github.io/styleguide/cppguide.html#Header_Files)


### QA

+ configure vscode include path for `c_cpp_properties.json` with `gcc -xc++ -E -v -`
+ [custom asio service impl discuz](https://stackoverflow.com/questions/23887056/trying-to-understand-boost-asio-custom-service-implementationls)
+ [explanation on `std::streambuf`](http://en.cppreference.com/w/cpp/io/basic_streambuf)
+ [test HTTPS with curl on mac](https://github.com/curl/curl/issues/283)
+ [go.http](https://golang.org/pkg/net/http/)
+ [stackoverflow: setup server crt key for asio](https://stackoverflow.com/questions/6452756/exception-running-boost-asio-ssl-example)
+ [stackoverflow: how to create signed certificate](https://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl)
+ [stackoverflow: self-signed certificate with config file](https://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl)
+ [sprint.io on CORS](https://spring.io/understanding/CORS)


```sh 
openssl req -config req.cnf -new -x509 -sha256 -newkey rsa:2048 -keyout key.pem -days 365 -out cert.pem
```