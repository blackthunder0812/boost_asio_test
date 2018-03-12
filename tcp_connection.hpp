#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <memory>

class tcp_server;
class tcp_connection : public std::enable_shared_from_this<tcp_connection>
{
  public:
    static const unsigned int HEADER_SIZE = 8; // first 4 bytes: payload length, next 2 byte: signature: 0x55AA, next 1 byte: opcode, last byte: reserve
  private:
    std::array<unsigned char, HEADER_SIZE> read_header_buffer;
    boost::asio::ip::tcp::socket socket_;
    tcp_server *tcp_server_ptr;
    tcp_connection(boost::asio::io_service &io_service, tcp_server *tcp_server_ptr);
    void write_handler(const boost::system::error_code& err);
    void read_header();
    void read_header_handler (const boost::system::error_code& err);
    void read_payload(unsigned int payload_length);
    void read_payload_handler(std::shared_ptr<std::vector<unsigned char>> payload_ptr, const boost::system::error_code& err);
    void process_message(std::shared_ptr<std::vector<unsigned char>> payload_ptr);

  public:
    static std::shared_ptr<tcp_connection> create_connection(boost::asio::io_service &io_service, tcp_server *tcp_server_ptr);
    boost::asio::ip::tcp::socket& socket();
    void start();
    void write(std::shared_ptr<std::vector<unsigned char>> message);
    void close();
};

#endif // TCP_CONNECTION_HPP
