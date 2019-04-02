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
    boost::asio::io_service & _serv;
    ushort _id;
    ushort _port;
    udp::socket _sock;
    boost::array<char, SIZE_DATA> _buff;
    udp::endpoint _remote_endpoint;
    std::function<void (const std::string &s, boost::asio::ip::address ip)> on_msg;
    void handle_send(const boost::system::error_code& error, std::size_t bytes_transf) const;
    void handle_rec(const boost::system::error_code& error, std::size_t bytes_transf);
    boost::asio::ip::address id_to_address(ushort id);

public:
    void broad_msg(const std::string &s);
    void multi_msg(const std::string &s, std::set<ushort> ids);
    void start_receive();
    void set_on_msg(std::function< void (const std::string &s, boost::asio::ip::address ip) > fun);
    Udp_asyn(boost::asio::io_service & service, ushort id, ushort port);
    ~Udp_asyn();
};

#endif
