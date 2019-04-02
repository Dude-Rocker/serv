#include "udp_com.hpp"
#include <functional>

udp_com::udp_com(boost::asio::io_service & service, ushort id, ushort port) : m_io_service(service),
    m_id(id), m_port(port), m_multicast_address(id_to_address(m_id)), m_sock_listen(m_io_service),
    m_sock_send(m_io_service, udp::endpoint(m_multicast_address, m_port).protocol())
{
    udp::endpoint listen_endpoint(boost::asio::ip::address::from_string("0.0.0.0"), m_port);
    m_sock_listen.open(listen_endpoint.protocol());
    m_sock_listen.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    m_sock_listen.bind(listen_endpoint);

    m_sock_listen.set_option(boost::asio::ip::multicast::join_group(m_multicast_address));

    m_buff.resize(SIZE_DATA);

}

udp_com::~udp_com() { }

boost::asio::ip::address udp_com::id_to_address(ushort id)
{
    std::string addr = "239.0." + std::to_string(id / 256) + "." + std::to_string(id % 256);
    return boost::asio::ip::address::from_string(addr);
}

void udp_com::start_receive()
{
    using namespace std::placeholders;
    m_sock_listen.async_receive_from(boost::asio::buffer(m_buff), m_remote_endpoint,
        std::bind(&udp_com::handle_receive, this, _1, _2));
}

void udp_com::handle_receive(const boost::system::error_code& error, std::size_t bytes_transf)
{
    if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);
    if (m_on_msg) {
        std::string s;
        m_on_msg(s.append(m_buff.data(), bytes_transf), m_remote_endpoint.address());
    }
    start_receive();
}

void    udp_com::send_msg_to_ids(const std::string &s, std::set<ushort> ids)
{
    using namespace std::placeholders;
    for(auto add : ids)
    {
        m_sock_send.async_send_to(boost::asio::buffer(s), udp::endpoint(id_to_address(add), m_port),
            std::bind(&udp_com::handle_send, this, _1, _2));
    }
}

void udp_com::handle_send(const boost::system::error_code& error, std::size_t bytes_transf) const
{
    if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);
}

void    udp_com::set_on_msg(std::function< void (const std::string &s, boost::asio::ip::address ip) > fun)
{
    m_on_msg = fun;
}

boost::asio::ip::address udp_com::get_multicast_address()
{
    return m_multicast_address;
}
ushort udp_com::get_id()
{
    return m_id;
}

ushort udp_com::get_port()
{
    return m_port;
}
