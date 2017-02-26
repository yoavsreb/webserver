#include "gtest/gtest.h"
#include "RequestResponse.capnp.h"

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <boost/filesystem.hpp>


TEST(RequestResponse_Capnp, test1) {
    ::capnp::MallocMessageBuilder message;
    webserver::Request::Builder request = message.initRoot<webserver::Request>();
    ::capnp::List<webserver::Header>::Builder headers = request.initHeaders(1);
    auto header = headers[0];
    header.setName("Hello");
    header.setValue("World");
    request.setPath("/");
    request.setBody("body");
    
    boost::filesystem::path temp = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    {   
        std::cout << "Opening file in : " << temp << std::endl;
        int fd = open(temp.c_str(), O_TRUNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (fd > 0) {
            writeMessageToFd(fd, message);
            close(fd);
        } else {
            std::cerr << "Failed to open file: " << temp << " Error code: " << errno << std::endl;
            FAIL();
        }
    }
    
    {
        int fd = open(temp.c_str(), O_RDONLY, S_IRUSR);
        if (fd > 0) {
            ::capnp::StreamFdMessageReader message(fd);
            auto rdr = message.getRoot<webserver::Request>();
            EXPECT_EQ(std::string{"/"}, std::string(rdr.getPath().cStr()));
            EXPECT_EQ(std::string{"body"}, std::string(rdr.getBody().cStr()));
            EXPECT_EQ(1, rdr.getHeaders().size());
            close(fd);
        } else {
            std::cerr << "Failed to open file: " << temp << " Error code: " << errno << std::endl;
            FAIL();
        }
    }
}

