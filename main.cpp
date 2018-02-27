#include <iostream>
#include <exception>
#include <thread>
#include <boost/asio.hpp>
#include <tcp_server.hpp>

int main()
{
  try {
    boost::asio::io_service io_service;
    tcp_server server(io_service);
    std::thread io_service_thread([&](){io_service.run();});
    for (;;) {
      std::this_thread::sleep_for(std::chrono::seconds(5));
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
