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
    boost::asio::io_service & serv;
    boost::array<char, SIZE_DATA> buff;
    udp::endpoint remote_endpoint;
    std::function<void (const std::string &s, boost::asio::ip::address ip)> on_msg;
    udp::socket sock;
    void handle_rec(const boost::system::error_code& error, std::size_t bytes_transf);

public:
    void broad_msg(const std::string &s, int port);
    void multi_msg(const std::set<udp::endpoint> & set_points, const std::string &s);
    void receive_async();
    void set_func(std::function< void (const std::string &s, boost::asio::ip::address ip) > fun);
    Udp_asyn(boost::asio::io_service & service, udp::endpoint &ep);
    ~Udp_asyn();
};

#endif
