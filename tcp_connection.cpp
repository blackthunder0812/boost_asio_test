#include <iostream>
#include "tcp_connection.hpp"
#include <boost/endian/conversion.hpp>
#include <vector>
#include "tcp_server.hpp"

tcp_connection::tcp_connection(boost::asio::io_service &io_service, tcp_server *tcp_server_ptr) :
  socket_(io_service),
  tcp_server_ptr(tcp_server_ptr)
{

}

void tcp_connection::write(boost::shared_ptr<std::vector<unsigned char>> message)
{
  boost::asio::async_write(socket_,
                           boost::asio::buffer(*message),
                           boost::bind(&tcp_connection::write_handler,
                                       shared_from_this(),
                                       boost::asio::placeholders::error));
}

void tcp_connection::close()
{
  tcp_server_ptr->get_connection_list().erase(this);
  socket_.close();
}

void tcp_connection::write_handler(const boost::system::error_code &err)
{
  if (err) {
    if (err != boost::asio::error::eof) {
      if (err == boost::asio::error::operation_aborted) {
        std::cerr << "Operation canceled" << std::endl;
      } else {
        std::cerr << "Error writing to client " << socket_.remote_endpoint().address().to_string() << ":" << socket_.remote_endpoint().port() << " : " << err.message() << std::endl;
      }
    } else {
      std::cout << "Connection to " << socket_.remote_endpoint().address().to_string() << ":" << socket_.remote_endpoint().port() << " closed" << std::endl;
    }
    tcp_server_ptr->get_connection_list().erase(this);
    socket_.close();
  }
}

void tcp_connection::read_header()
{
  boost::asio::async_read(socket_,
                          boost::asio::buffer(read_header_buffer),
                          boost::bind(&tcp_connection::read_header_handler,
                                      shared_from_this(),
                                      boost::asio::placeholders::error));
}

void tcp_connection::read_header_handler(const boost::system::error_code &err)
{
  if(err) {
    if (err != boost::asio::error::eof) {
      if (err == boost::asio::error::operation_aborted) {
        std::cerr << "Operation canceled" << std::endl;
      } else {
        std::cerr << "Error reading payload size from client " << socket_.remote_endpoint().address().to_string() << ":" << socket_.remote_endpoint().port() << " : " << err.message() << std::endl;
      }
    } else {
      std::cout << "Connection to " << socket_.remote_endpoint().address().to_string() << ":" << socket_.remote_endpoint().port() << " closed" << std::endl;
    }
    tcp_server_ptr->get_connection_list().erase(this);
    socket_.close();
  } else {
    if (read_header_buffer.data()[4] != 0x55 || read_header_buffer.data()[5] != 0xAA) {
      std::cerr << "Invalid signature: " << std::hex << read_header_buffer.data()[4] << read_header_buffer.data()[5] << std::endl;
      tcp_server_ptr->get_connection_list().erase(this);
      socket_.close();
    } else {
      unsigned int payload_length = boost::endian::big_to_native(*(unsigned int*)(read_header_buffer.data()));
      std::cout << "Payload length: " << payload_length << std::endl;
      if (payload_length == 0) { // heartbeat package: 0x00 0x00 0x00 0x00 0x55 0xAA 0x00 0x00
        read_header();
      } else {
        read_payload(payload_length);
      }
    }
  }
}

void tcp_connection::read_payload(unsigned int payload_length)
{

  boost::shared_ptr<std::vector<unsigned char>> payload_ptr(new std::vector<unsigned char>(payload_length));
  boost::asio::async_read(socket_,
                          boost::asio::buffer(*payload_ptr),
                          boost::bind(&tcp_connection::read_payload_handler,
                                      shared_from_this(),
                                      payload_ptr,
                                      boost::asio::placeholders::error));
}

void tcp_connection::read_payload_handler(boost::shared_ptr<std::vector<unsigned char>> payload_ptr, const boost::system::error_code &err)
{
  if(err) {
    if (err != boost::asio::error::eof) {
      if (err == boost::asio::error::operation_aborted) {
        std::cerr << "Operation canceled" << std::endl;
      } else {
        std::cerr << "Error reading from client " << socket_.remote_endpoint().address().to_string() << ":" << socket_.remote_endpoint().port() << " : " << err.message() << std::endl;
      }
    } else {
      std::cout << "Connection to " << socket_.remote_endpoint().address().to_string() << ":" << socket_.remote_endpoint().port() << " closed" << std::endl;
    }
    tcp_server_ptr->get_connection_list().erase(this);
    socket_.close();
  } else {
    process_message(payload_ptr);
    read_header();
  }
}

void tcp_connection::process_message(boost::shared_ptr<std::vector<unsigned char>> payload_ptr)
{
  size_t payload_size = payload_ptr->size();
  for (size_t i = 0; i < payload_size; i++) {
    std::cout << (*payload_ptr)[i];
  }
  std::cout << std::endl;
}

boost::shared_ptr<tcp_connection> tcp_connection::create_connection(boost::asio::io_service &io_service, tcp_server *tcp_server_ptr)
{
  return boost::shared_ptr<tcp_connection>(new tcp_connection(io_service, tcp_server_ptr));
}

boost::asio::ip::tcp::socket &tcp_connection::socket()
{
  return socket_;
}

void tcp_connection::start()
{
  read_header();
}
