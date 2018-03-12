#include "console_handler.hpp"
#include "tcp_server.hpp"
#include <iostream>
#include <boost/endian/conversion.hpp>
#include <vector>
#include "tcp_connection.hpp"
#include <string>
#include <functional>
#include "demangle.h"

void console_handler::read()
{
  boost::asio::async_read_until(input_stream_,
                                read_buffer_,
                                '\n',
                                std::bind(&console_handler::read_handler,
                                            this,
                                            std::placeholders::_1,
                                            std::placeholders::_2));
}

void console_handler::read_handler(const boost::system::error_code &err, size_t byte_read)
{
  if (err) {
    std::cerr << "Error reading from stdin: " << err.message() << std::endl;
  } else {
    unsigned int buffer_size = tcp_connection::HEADER_SIZE + byte_read;
    std::shared_ptr<std::vector<unsigned char>> command(new std::vector<unsigned char>());
    command->reserve(buffer_size);
    boost::asio::buffers_iterator<boost::asio::const_buffer, unsigned char> buffer_iter = boost::asio::buffers_iterator<boost::asio::const_buffer, unsigned char>::begin(read_buffer_.data());

    command->assign(tcp_connection::HEADER_SIZE, 0x00);
    for (size_t i = tcp_connection::HEADER_SIZE; i < buffer_size; i++) {
      command->emplace_back(*buffer_iter++);
    }
    read_buffer_.consume(byte_read);

    if (memcmp("exit\n", &(*command)[tcp_connection::HEADER_SIZE], byte_read) == 0) {
      tcpserver->stop();
      return;
    } else if (memcmp("clear\n", &(*command)[tcp_connection::HEADER_SIZE], byte_read) == 0) {
      tcpserver->clear(); // close all connections but keep accepting new connections
    } else if (memcmp("unaccept\n", &(*command)[tcp_connection::HEADER_SIZE], byte_read) == 0) {
      tcpserver->unaccept(); // keep connecting new connections but don't allow new connection
    } else if (memcmp("info\n", &(*command)[tcp_connection::HEADER_SIZE], byte_read) == 0) {
      if (tcpserver->is_accepting()) {
        std::cout << "Server is accepting" << std::endl;
      } else {
        std::cout << "Server is NOT accepting" << std::endl;
      }
      if (tcpserver->get_connection_list().empty()) {
        std::cout << "No client" << std::endl;
      } else {
        std::cout << "Connection list: " << std::endl;
        for(auto conn : tcpserver->get_connection_list()) {
          std::cout << conn->socket().remote_endpoint().address().to_string() << ":" << conn->socket().remote_endpoint().port() << std::endl;
        }
      }
    } else if (memcmp("hb\n", &(*command)[tcp_connection::HEADER_SIZE], byte_read) == 0) {
      if (tcpserver->get_connection_list().empty()) {
        std::cout << "No client" << std::endl;
      } else {
        *(unsigned int*)((*command).data()) = 0;
        (*command)[4] = 0x55; (*command)[5] = 0xAA;
        tcpserver->broadcast_message(command);
      }
    } else {
      if (tcpserver->get_connection_list().empty()) {
        std::cout << "No client" << std::endl;
      } else {
        *(unsigned int*)((*command).data()) = boost::endian::native_to_big((unsigned int)byte_read);
        (*command)[4] = 0x55; (*command)[5] = 0xAA;
        tcpserver->broadcast_message(command);
      }
    }

    read();
  }
}

console_handler::console_handler(boost::asio::io_service &io_service,
                                 tcp_server *tcpserver) :
  input_stream_(io_service, STDIN_FILENO),
  tcpserver(tcpserver)
{
  read();
}
