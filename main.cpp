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
    boost::shared_ptr<std::string> test(new std::string("test"));
    for (;;) {
      std::this_thread::sleep_for(std::chrono::seconds(5));
      // TODO: lock
      std::cout << "Writing test string to " << server.get_connection_list().size() << " clients" << std::endl;
      std::list<tcp_connection*>::iterator connection_list_iterator = server.get_connection_list().begin();
      while (connection_list_iterator != server.get_connection_list().end()) {
        io_service.post(boost::bind(&tcp_connection::write, *connection_list_iterator, test));
        connection_list_iterator++;
      }
      // TODO: unlock
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
