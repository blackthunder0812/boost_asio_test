#include <iostream>
#include <boost/bind.hpp>
#include "tcp_server.hpp"

void tcp_server::start_accept()
{
  boost::shared_ptr<tcp_connection> new_connection_ptr = tcp_connection::create_connection(acceptor_.get_io_service());
  acceptor_.async_accept(new_connection_ptr->socket(),
                         boost::bind(&tcp_server::accept_handler,
                                     this,
                                     new_connection_ptr,
                                     boost::asio::placeholders::error));
}

void tcp_server::accept_handler(boost::shared_ptr<tcp_connection> connection_ptr, const boost::system::error_code &err)
{
  if(!err) {
    connection_list.push_back(connection_ptr);
    connection_ptr->start();
    start_accept();
  } else {
    std::cerr << "Error on accepting new connection: " << err.message() << std::endl;
  }
}

tcp_server::tcp_server(boost::asio::io_service &io_service) :
  acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080))
{
  start_accept();
}

std::list<boost::shared_ptr<tcp_connection> > &tcp_server::get_connection_list()
{
  return connection_list;
}
