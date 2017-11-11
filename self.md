

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