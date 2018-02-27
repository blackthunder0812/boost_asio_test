#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP
#include <list>
#include <boost/asio.hpp>
#include "tcp_connection.hpp"

class tcp_server
{
  private:
    std::list<boost::shared_ptr<tcp_connection>> connection_list;
    boost::asio::ip::tcp::acceptor acceptor_;
    void start_accept();
    void accept_handler(boost::shared_ptr<tcp_connection> connection_ptr, const boost::system::error_code &err);
  public:
    tcp_server(boost::asio::io_service &io_service);
    std::list<boost::shared_ptr<tcp_connection>>& get_connection_list();
};

#endif // TCP_SERVER_HPP