#include "udp_asyn.hpp"
#include <functional>

Udp_asyn::Udp_asyn(boost::asio::io_service & service, ushort id, ushort port) : _serv(service),
    _id(id), _port(port), _sock(_serv)
{
    udp::endpoint listen_endpoint(boost::asio::ip::address::from_string("0.0.0.0"), _port);
    _sock.open(listen_endpoint.protocol());
    _sock.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    _sock.bind(listen_endpoint);

    _sock.set_option(boost::asio::ip::multicast::join_group(id_to_address(_id)));
}

Udp_asyn::~Udp_asyn() { }

boost::asio::ip::address Udp_asyn::id_to_address(ushort id)
{
    std::string addr = "239.0." + std::to_string(id / 256) + "." + std::to_string(id % 256);
    return boost::asio::ip::address::from_string(addr);
}

void Udp_asyn::start_receive()
{
    using namespace std::placeholders;
    _sock.async_receive_from(boost::asio::buffer(_buff_mult), _remote_endpoint_mult,
        std::bind(&Udp_asyn::handle_rec, this, _1, _2));
}

void Udp_asyn::handle_rec(const boost::system::error_code& error, std::size_t bytes_transf)
{
    if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);
    if (on_msg) {
        std::string s;
        on_msg(s.append(_buff_mult.data(), bytes_transf), _remote_endpoint_mult.address());
    }
    start_receive();
}

void    Udp_asyn::broad_msg(const std::string &s)
{
    boost::system::error_code error;
    udp::socket socket(_serv);

    socket.open(udp::v4(), error);
    if (!error)
    {
        socket.set_option(udp::socket::reuse_address(true));
        socket.set_option(boost::asio::socket_base::broadcast(true));

        udp::endpoint senderEndpoint(boost::asio::ip::address_v4::broadcast(), _port);            

        using namespace std::placeholders;
        socket.async_send_to(boost::asio::buffer(s), senderEndpoint,
        std::bind(&Udp_asyn::handle_send, this, _1, _2));
        socket.close(error);
    }
}

void    Udp_asyn::multi_msg(const std::string &s, std::set<ushort> ids)
{
    udp::socket socket(_serv, udp::endpoint(id_to_address(_id), _port).protocol());        

    using namespace std::placeholders;
    for(auto add : ids)
    {
        socket.async_send_to(boost::asio::buffer(s), udp::endpoint(id_to_address(add), _port),
            std::bind(&Udp_asyn::handle_send, this, _1, _2));
    }
}

void Udp_asyn::handle_send(const boost::system::error_code& error, std::size_t bytes_transf) const
{
    if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);
}

void    Udp_asyn::set_on_msg(std::function< void (const std::string &s, boost::asio::ip::address ip) > fun)
{
    on_msg = fun;
}
