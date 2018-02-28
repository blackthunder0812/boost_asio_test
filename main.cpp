#include <iostream>
#include <exception>
#include <thread>
#include <list>
#include <string>
#include <boost/asio.hpp>
#include "tcp_server.hpp"
#include "console_handler.hpp"

int main()
{
  try {
    boost::asio::io_service io_service;
    tcp_server server(io_service);
    console_handler console_handler(io_service, &server);
    io_service.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
