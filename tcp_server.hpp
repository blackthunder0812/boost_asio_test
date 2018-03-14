#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <boost/asio.hpp>
#include <unordered_set>

class tcp_connection;
class tcp_server
{
  private:
    std::unordered_set<tcp_connection*> connection_list;
    boost::asio::ip::tcp::acceptor acceptor_;
    void start_accept();
    void accept_handler(std::shared_ptr<tcp_connection> connection_ptr, const boost::system::error_code &err);
  public:
    tcp_server(boost::asio::io_service &io_service);
    std::unordered_set<tcp_connection*>& get_connection_list();
    void broadcast_message(std::shared_ptr<std::vector<unsigned char>> message);
    void stop();
    void clear();
    bool is_accepting();
    void unaccept();
};

#endif // TCP_SERVER_HPP
