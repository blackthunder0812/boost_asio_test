#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class tcp_server;
class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
  private:
    boost::array<char, sizeof(unsigned int)> read_header_buffer;
    boost::asio::ip::tcp::socket socket_;
    tcp_server *tcp_server_ptr;
    tcp_connection(boost::asio::io_service &io_service, tcp_server *tcp_server_ptr);
    void write_handler(const boost::system::error_code& err);
    void read_header();
    void read_header_handler (const boost::system::error_code& err);
    void read_payload(unsigned int payload_length);
    void read_payload_handler(boost::shared_ptr<char> payload_ptr, const boost::system::error_code& err, size_t byte_transfered);
    void process_message(boost::shared_ptr<char> payload_ptr, size_t byte_transfered);

  public:
    static boost::shared_ptr<tcp_connection> create_connection(boost::asio::io_service &io_service, tcp_server *tcp_server_ptr);
    boost::asio::ip::tcp::socket& socket();
    void start();
    void write(boost::shared_ptr<std::string> message);
    void close();
};

#endif // TCP_CONNECTION_HPP
