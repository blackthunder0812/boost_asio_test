#include "console_handler.hpp"
#include "tcp_server.hpp"
#include <iostream>
#include <boost/bind.hpp>
#include <boost/endian/conversion.hpp>
#include <vector>
#include "tcp_connection.hpp"

void console_handler::read()
{
  boost::asio::async_read_until(input_stream_,
                                read_buffer_,
                                '\n',
                                boost::bind(&console_handler::read_handler,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
}

void console_handler::read_handler(const boost::system::error_code &err, size_t byte_read)
{
  if (err) {
    std::cerr << "Error reading from stdin: " << err.message() << std::endl;
  } else {
    std::string console_command(boost::asio::buffers_begin(read_buffer_.data()),
                                boost::asio::buffers_begin(read_buffer_.data()) + byte_read);
    std::cout << "Buffer size: " << read_buffer_.size() << " bytes. Read: " << byte_read << " bytes: " << console_command << std::endl;
    read_buffer_.consume(byte_read);

    if (console_command.compare("exit\n") == 0) {
      tcpserver->stop();
      return;
    } else if (console_command.compare("clear\n") == 0) {
      tcpserver->clear(); // close all connections but keep accepting new connections
    } else if (console_command.compare("info\n") == 0) {
      std::cout << "Connection list: " << std::endl;
      for(auto conn : tcpserver->get_connection_list()) {
        std::cout << conn->socket().remote_endpoint().address().to_string() << ":" << conn->socket().remote_endpoint().port() << std::endl;
      }
    } else if (console_command.compare("hb\n") == 0) {
      boost::shared_ptr<std::vector<unsigned char>> data(new std::vector<unsigned char>(tcp_connection::HEADER_SIZE));
      *(unsigned int*)((*data).data()) = 0;
      (*data).at(4) = 0x55; (*data).at(5) = 0xAA;
      tcpserver->broadcast_message(data);
    } else {
      unsigned int data_len = tcp_connection::HEADER_SIZE + byte_read;
      boost::shared_ptr<std::vector<unsigned char>> data(new std::vector<unsigned char>(data_len));
      *(unsigned int*)((*data).data()) = boost::endian::native_to_big((unsigned int)byte_read);
      (*data).at(4) = 0x55; (*data).at(5) = 0xAA;
      memcpy((*data).data() + tcp_connection::HEADER_SIZE, console_command.data(), byte_read);
      tcpserver->broadcast_message(data);
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
