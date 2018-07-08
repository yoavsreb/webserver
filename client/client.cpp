#include "RequestResponse.capnp.h"

#include <boost/program_options.hpp>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <fcntl.h>
#include <iostream>
#include <string>
namespace po = boost::program_options;

void createMessage(const std::string &path, const std::string &body,
                   const std::string &output) {
  ::capnp::MallocMessageBuilder message;
  webserver::Request::Builder request = message.initRoot<webserver::Request>();
  request.initHeaders(0);
  request.setPath(path);
  request.setBody(body);

  int fd = open(output.c_str(), O_TRUNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd > 0) {
    writeMessageToFd(fd, message);
    close(fd);
  } else {
    throw std::runtime_error{"Failed to create file"};
  }
}

/**
 * A helper utility to generate requests.
 */
int main(int argc, char **argv) {
  po::options_description desc{"Allowed Options"};
  desc.add_options()("help", "produce help message")(
      "path", po::value<std::string>(),
      "Path to resource")("body", po::value<std::string>(), "Body of payload")(
      "output", po::value<std::string>(), "path for output file");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (argc == 1 || vm.count("help")) {
    std::cout << desc;
  } else if (vm.count("path") && vm.count("output") && vm.count("body")) {
    auto path = vm["path"].as<std::string>();
    auto body = vm["body"].as<std::string>();

    auto outputFile = vm["output"].as<std::string>();
    createMessage(path, body, outputFile);
    std::cerr << "Wrote request to : " << outputFile << std::endl;
  }
  return 0;
}
