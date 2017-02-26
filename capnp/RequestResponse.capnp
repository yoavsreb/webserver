@0xab55cd083bbe9680;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("webserver");


struct Header {
    name @0 : Text;
    value @1 : Text;
}
    
struct Request {
    verb @0 : Text;
    path @1 : Text;
    headers @2 : List(Header);
    body @3 : Text;
}

struct Respnose {
    statusCode @0 : UInt32;
    headers @1 : List(Header);
    body @2 : Text;
}

