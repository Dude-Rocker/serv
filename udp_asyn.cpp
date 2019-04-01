#include "udp_asyn.hpp"
#include <functional>

Udp_asyn::Udp_asyn(boost::asio::io_service & service, udp::endpoint &ep) :
    serv(service), sock(serv, ep), sock_mult(serv) { }

Udp_asyn::~Udp_asyn() { }

void Udp_asyn::start_receive()
{
    using namespace std::placeholders;
    sock.async_receive_from(boost::asio::buffer(buff), remote_endpoint,
        std::bind(&Udp_asyn::handle_rec, this, _1, _2));
}

void Udp_asyn::mult_receive(const boost::asio::ip::address& listen_address,
      const boost::asio::ip::address& multicast_address, uint port)
{
    udp::endpoint listen_endpoint(listen_address, port);
    sock_mult.open(listen_endpoint.protocol());
    sock_mult.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    sock_mult.bind(listen_endpoint);

    sock_mult.set_option(
        boost::asio::ip::multicast::join_group(multicast_address));
        
    mult_start();
}

void Udp_asyn::mult_start()
{
    using namespace std::placeholders;
    sock_mult.async_receive_from(boost::asio::buffer(buff_mult), remote_endpoint_mult,
        std::bind(&Udp_asyn::handle_mult_rec, this, _1, _2));
}

void Udp_asyn::handle_mult_rec(const boost::system::error_code& error, std::size_t bytes_transf)
{
    if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);
    if (on_msg) {
        std::string s;
        on_msg(s.append(buff_mult.data(), bytes_transf), remote_endpoint_mult.address());
    }
    mult_start();
}

void Udp_asyn::handle_rec(const boost::system::error_code& error, std::size_t bytes_transf)
{
    if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);
    if (on_msg) {
        std::string s;
        on_msg(s.append(buff.data(), bytes_transf), remote_endpoint.address());
    }
    start_receive();
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

        using namespace std::placeholders;
        socket.async_send_to(boost::asio::buffer(s), senderEndpoint,
        std::bind(&Udp_asyn::handle_send, this, _1, _2));
        socket.close(error);
    }
}

void    Udp_asyn::multi_msg(const std::string &s, udp::endpoint ep)
{
    udp::socket socket(serv, ep.protocol());        

    using namespace std::placeholders;
    socket.async_send_to(boost::asio::buffer(s), ep,
        std::bind(&Udp_asyn::handle_send, this, _1, _2));
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
