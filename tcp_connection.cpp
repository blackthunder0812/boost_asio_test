#include <iostream>
#include "tcp_connection.hpp"
#include <boost/endian/conversion.hpp>
#include "tcp_server.hpp"

tcp_connection::tcp_connection(boost::asio::io_service &io_service, tcp_server *tcp_server_ptr) :
  socket_(io_service),
  tcp_server_ptr(tcp_server_ptr)
{

}

void tcp_connection::write(boost::shared_ptr<std::string> message)
{
  boost::asio::async_write(socket_,
                           boost::asio::buffer(message->data(), message->length()),
                           boost::bind(&tcp_connection::write_handler,
                                       shared_from_this(),
                                       boost::asio::placeholders::error));
}

void tcp_connection::close()
{
  tcp_server_ptr->get_connection_list().erase(this);
  socket_.cancel();
}

void tcp_connection::write_handler(const boost::system::error_code &err)
{
  if (err) {
    if (err != boost::asio::error::eof) {
      if (err == boost::asio::error::operation_aborted) {
        tcp_server_ptr->get_connection_list().erase(this);
        socket_.close();
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
  if(!err) {
    unsigned int payload_length = boost::endian::big_to_native(*(unsigned int*)(read_header_buffer.data()));
//    std::cout << "Payload length: " << payload_length << std::endl;
    if (payload_length == 0) { // heartbeat package
      read_header();
    } else {
      read_payload(payload_length);
    }
  } else {
    if (err != boost::asio::error::eof) {
      if (err == boost::asio::error::operation_aborted) {
        tcp_server_ptr->get_connection_list().erase(this);
        socket_.close();
      } else {
        std::cerr << "Error reading payload size from client " << socket_.remote_endpoint().address().to_string() << ":" << socket_.remote_endpoint().port() << " : " << err.message() << std::endl;
      }
    } else {
      std::cout << "Connection to " << socket_.remote_endpoint().address().to_string() << ":" << socket_.remote_endpoint().port() << " closed" << std::endl;
    }
  }
}

void tcp_connection::read_payload(unsigned int payload_length)
{
  boost::shared_ptr<char> payload_ptr(new char[payload_length]);
  boost::asio::async_read(socket_,
                          boost::asio::buffer(payload_ptr.get(), payload_length),
                          boost::bind(&tcp_connection::read_payload_handler,
                                      shared_from_this(),
                                      payload_ptr,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::read_payload_handler(boost::shared_ptr<char> payload_ptr, const boost::system::error_code &err, size_t byte_transfered)
{
  if(!err) {
    process_message(payload_ptr, byte_transfered);
    read_header();
  } else {
    if (err != boost::asio::error::eof) {
      if (err == boost::asio::error::operation_aborted) {
        tcp_server_ptr->get_connection_list().erase(this);
        socket_.close();
      } else {
        std::cerr << "Error reading from client " << socket_.remote_endpoint().address().to_string() << ":" << socket_.remote_endpoint().port() << " : " << err.message() << std::endl;
      }
    } else {
      std::cout << "Connection to " << socket_.remote_endpoint().address().to_string() << ":" << socket_.remote_endpoint().port() << " closed" << std::endl;
    }
    tcp_server_ptr->get_connection_list().erase(this);
    socket_.close();
  }
}

void tcp_connection::process_message(boost::shared_ptr<char> payload_ptr, size_t byte_transfered)
{
  for (size_t i = 0; i < byte_transfered; i++) {
    std::cout << payload_ptr.get()[i];
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
