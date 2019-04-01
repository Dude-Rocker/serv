#ifndef UDP_ASYN
#define UDP_ASYN

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <set>

using boost::asio::ip::udp;
#define SIZE_DATA 1024

class Udp_asyn
{
private:
    boost::array<char, SIZE_DATA> buff;
    udp::endpoint remote_endpoint;
    boost::array<char, SIZE_DATA> buff_mult;
    udp::endpoint remote_endpoint_mult;
    boost::asio::io_service & serv;
    std::function<void (const std::string &s, boost::asio::ip::address ip)> on_msg;
    udp::socket sock;
    udp::socket sock_mult;
    void handle_rec(const boost::system::error_code& error, std::size_t bytes_transf);
    void handle_send(const boost::system::error_code& error, std::size_t bytes_transf) const;
    void handle_mult_rec(const boost::system::error_code& error, std::size_t bytes_transf);
    void mult_start();



public:
    void broad_msg(const std::string &s, uint port);
    void multi_msg(const std::string &s, udp::endpoint ep);
    void start_receive();
    void mult_receive(const boost::asio::ip::address& listen_address,
      const boost::asio::ip::address& multicast_address, uint port);
    void set_on_msg(std::function< void (const std::string &s, boost::asio::ip::address ip) > fun);
    Udp_asyn(boost::asio::io_service & service, udp::endpoint &ep);
    ~Udp_asyn();
};

#endif
