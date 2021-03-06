#include <iostream>
#include "tcp_server.hpp"
#include "tcp_connection.hpp"
#include "allocator_handler.hpp"

void tcp_server::start_accept()
{
  std::shared_ptr<tcp_connection> new_connection_ptr = tcp_connection::create_connection(acceptor_.get_io_service(), this);
  acceptor_.async_accept(new_connection_ptr->socket(),
                         make_allocation_handler(accepter_handler_memory_,
                                                 std::bind(&tcp_server::accept_handler,
                                                             this,
                                                             new_connection_ptr,
                                                             std::placeholders::_1)));
}

void tcp_server::accept_handler(std::shared_ptr<tcp_connection> connection_ptr, const boost::system::error_code &err)
{
  if(!err) {
    std::cout << "Client " << connection_ptr->socket().remote_endpoint().address().to_string() << ":" << connection_ptr->socket().remote_endpoint().port() << " connected" << std::endl;
    connection_list.insert(connection_ptr.get());
    connection_ptr->start();
    start_accept();
  } else {
    if (err == boost::asio::error::operation_aborted) {
      std::cerr << "Acceptor closed" << std::endl;
    } else {
      std::cerr << "Error on accepting new connection: " << err.message() << std::endl;
    }
  }
}

tcp_server::tcp_server(boost::asio::io_service &io_service) :
  acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080))
{
  start_accept();
}

std::unordered_set<tcp_connection *> &tcp_server::get_connection_list()
{
  return connection_list;
}

void tcp_server::broadcast_message(std::shared_ptr<std::vector<unsigned char>> message)
{
//  std::cout << "Sending message " << *message << " to " << connection_list.size() << " clients" << std::endl;
  std::unordered_set<tcp_connection*>::iterator connection_iterator = connection_list.begin();
  while(connection_iterator != connection_list.end()) {
    (*connection_iterator)->write(message);
    connection_iterator++;
  }
}

void tcp_server::stop()
{
  acceptor_.close();
  while(!connection_list.empty()) {
    (*connection_list.begin())->close();
  }
}

void tcp_server::clear()
{
  while(!connection_list.empty()) {
    (*connection_list.begin())->close();
  }
}

bool tcp_server::is_accepting()
{
  return acceptor_.is_open();
}

void tcp_server::unaccept()
{
  acceptor_.close();
}
