#include "udp_com.hpp"
#include <functional>

udp_com::udp_com(boost::asio::io_service & service, std::set<ushort> & group, ushort port) : m_io_service(service),
    m_port(port), m_socket(m_io_service), m_addr(group)
{
    udp::endpoint listen_endpoint(boost::asio::ip::address::from_string("0.0.0.0"), m_port);
    m_socket.open(listen_endpoint.protocol());
    m_socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    m_socket.bind(listen_endpoint);

    for(ushort add : m_addr)
    {
        m_socket.set_option(boost::asio::ip::multicast::join_group(id_to_address(add)));
    }
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
    m_socket.async_receive_from(boost::asio::buffer(m_buff), m_remote_endpoint,
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

void    udp_com::send_msg_to_group(const std::string &s, ushort group)
{
    using namespace std::placeholders;
    m_socket.async_send_to(boost::asio::buffer(s), udp::endpoint(id_to_address(group), m_port),
        std::bind(&udp_com::handle_send, this, _1, _2));
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

void udp_com::add_group(ushort add)
{
    if (m_addr.find(add) == m_addr.end()) {
        m_socket.set_option(boost::asio::ip::multicast::join_group(id_to_address(add)));
        m_addr.insert(add);
    }
}

void udp_com::del_group(ushort add)
{
    if (m_addr.find(add) != m_addr.end()) {
        m_socket.set_option(boost::asio::ip::multicast::leave_group(id_to_address(add)));
        m_addr.erase(add);
    }
}

ushort udp_com::get_port()
{
    return m_port;
}

std::set<ushort> udp_com::get_addrs()
{
    return m_addr;
}
