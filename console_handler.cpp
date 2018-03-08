#include "console_handler.hpp"
#include "tcp_server.hpp"
#include <iostream>
#include <boost/bind.hpp>

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
    std::string data(boost::asio::buffers_begin(read_buffer_.data()),
                     boost::asio::buffers_begin(read_buffer_.data()) + byte_read);
    std::cout << "Buffer size: " << read_buffer_.size() << " bytes. Read: " << byte_read << " bytes: " << data << std::endl;
    read_buffer_.consume(byte_read);

    if (data.compare("exit\n") == 0) {
      tcpserver->stop();
    } else if (data.compare("clear\n") == 0) {
      tcpserver->clear();
      read();
    } else {
      // send all message to clients
      tcpserver->broadcast_message(data);
      read();
    }
  }
}

console_handler::console_handler(boost::asio::io_service &io_service,
                                 tcp_server *tcpserver) :
  input_stream_(io_service, STDIN_FILENO),
  tcpserver(tcpserver)
{
  read();
}
