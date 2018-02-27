#include <iostream>
#include <exception>
#include <thread>
#include <list>
#include <string>
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
      std::cout << "Writing null string to all clients" << std::endl;
      boost::shared_ptr<std::string> test(new std::string(""));
      std::list<boost::shared_ptr<tcp_connection>>::iterator connection_list_iterator = server.get_connection_list().begin();
      while (connection_list_iterator != server.get_connection_list().end()) {
        (*connection_list_iterator)->write(test);
        connection_list_iterator++;
      }
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
