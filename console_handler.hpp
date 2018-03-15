#ifndef CONSOLE_HANDLER_HPP
#define CONSOLE_HANDLER_HPP

#include <boost/asio.hpp>
#include "handler_memory.hpp"

class tcp_server;
class console_handler
{
  private:
    boost::asio::posix::stream_descriptor input_stream_;
    boost::asio::streambuf read_buffer_;
    void read();
    void read_handler(const boost::system::error_code& err, size_t byte_read);
    tcp_server* tcpserver;
    handler_memory console_read_handler_memory_;
  public:
    console_handler(boost::asio::io_service &io_service, tcp_server *tcpserver);
};

#endif // CONSOLE_HANDLER_HPP
