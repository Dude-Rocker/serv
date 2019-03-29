#include "udp_asyn.hpp"
#include <functional>

Udp_asyn::Udp_asyn(boost::asio::io_service & service, udp::endpoint &ep) :
    serv(service), sock(serv, ep) { }

Udp_asyn::~Udp_asyn() { }

void Udp_asyn::receive_async()
{
    using namespace std::placeholders;
    sock.async_receive_from(boost::asio::buffer(buff), remote_endpoint,
        std::bind(&Udp_asyn::handle_rec, this, _1, _2));
}

void Udp_asyn::handle_rec(const boost::system::error_code& error, std::size_t bytes_transf)
{
    if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);
    if (on_msg) {
        std::string s;
        on_msg(s.append(buff.data(), bytes_transf), remote_endpoint.address());
    }
    receive_async();
}

void    Udp_asyn::broad_msg(const std::string &s, uint port)
{
    boost::system::error_code error;
    udp::socket socket(serv);

    socket.open(udp::v4(), error);
    if (!error)
    {
        socket.set_option(udp::socket::reuse_address(true));
        socket.set_option(boost::asio::socket_base::broadcast(true));

        udp::endpoint senderEndpoint(boost::asio::ip::address_v4::broadcast(), port);            

        socket.send_to(boost::asio::buffer(s), senderEndpoint);
        socket.close(error);
    }
}

void    Udp_asyn::multi_msg(const std::set<udp::endpoint> & points, const std::string &s)
{
    for(auto pts : points)
    {
        using namespace std::placeholders;
        sock.async_send_to(boost::asio::buffer(s), pts, 
        std::bind(&Udp_asyn::handle_mult, this, _1, _2));
    }
}

void Udp_asyn::handle_mult(const boost::system::error_code& error, std::size_t bytes_transf) const
{
    if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);
}

void    Udp_asyn::set_on_msg(std::function< void (const std::string &s, boost::asio::ip::address ip) > fun)
{
    on_msg = fun;
}
