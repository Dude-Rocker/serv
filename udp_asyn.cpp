#include "udp_asyn.hpp"
#include <functional>

Udp_asyn::Udp_asyn(boost::asio::io_service & service, udp::endpoint &ep) :
    serv(service), sock(serv, ep) { }

Udp_asyn::~Udp_asyn() { serv.stop(); }

void Udp_asyn::asyn_start()
{
    using namespace std::placeholders;
    sock.async_receive_from(boost::asio::buffer(buff), remote_endpoint, std::bind(&Udp_asyn::asyn_repl, this, _1, _2));
    // sock.async_receive_from(boost::asio::buffer(buff), remote_endpoint,
    // [&](const boost::system::error_code& error, std::size_t bytes_transf)
    // { asyn_repl(error, bytes_transf); });
    serv.run();
}

void Udp_asyn::asyn_repl(const boost::system::error_code& error, std::size_t bytes_transf)
{
    if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);
    if (on_msg) {
        std::string s;
        s.append(buff.data(), bytes_transf);
        on_msg(s);
    } else {
        buff[bytes_transf] = '\n';
        std::cerr.write(buff.data(), bytes_transf + 1);
    }
    asyn_start();
}

void    Udp_asyn::broad_msg(std::string &s)
{
    boost::system::error_code error;
    udp::socket socket(serv);

    socket.open(udp::v4(), error);
    if (!error)
    {
        socket.set_option(udp::socket::reuse_address(true));
        socket.set_option(boost::asio::socket_base::broadcast(true));

        udp::endpoint senderEndpoint(boost::asio::ip::address_v4::broadcast(), 15);            

        socket.send_to(boost::asio::buffer(s), senderEndpoint);
        socket.close(error);
    }
}

void    Udp_asyn::set_func(std::function< void (const std::string &s) > fun)
{
    on_msg = fun;
}
