#include "udp_com.hpp"
#include <functional>
#include <utility>

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
    int size_fr = sizeof(frame);
    frame fr;
    std::memcpy(&fr, m_buff.data(), size_fr);
    if (fr.preamble == PREAM) {
        std::string key = m_remote_endpoint.address().to_string() + ":" + std::to_string(fr.frame_id);

        if (std::find_if(m_map[key].begin(), m_map[key].end(), [&fr](rec_frame &rec_fr) {
            return fr.packeg_id == rec_fr.packeg_id;
            }) == m_map[key].end())
        {
            rec_frame r_fr;
            r_fr.packeg_id = fr.packeg_id;
            r_fr.full_pack = fr.full_pack;
            r_fr.msg.append(m_buff.data() + size_fr, bytes_transf - size_fr);
            m_map[key].push_back(r_fr);
        }

        if (m_map[key].size() == m_map[key][0].full_pack) {
            std::string res;
            for(size_t i = 0; i < m_map[key].size(); ++i)
            {
                res += std::find_if(m_map[key].begin(), m_map[key].end(), [&i](rec_frame &rec_fr) {
                    return i == rec_fr.packeg_id; })->msg;
            }
            
            if (m_on_msg) {
                m_on_msg(res, m_remote_endpoint.address());
            }
        }
    }
    start_receive();
}

void    udp_com::send_msg_to_group(const std::string &s, ushort group)
{
    using namespace std::placeholders;
    size_t size = s.size();
    frame fr;
    uint8_t size_struct = sizeof(fr);
    uint16_t size_piece = SIZE_DATA - size_struct;
    fr.preamble = PREAM;
    fr.frame_id = m_frame_id++;
    fr.full_pack = size / size_piece + (size % size_piece ? 1 : 0);

    for (uint16_t i = 0; i < size; i += size_piece) {
        fr.packeg_id = i / size_piece;
        std::string buf;
        buf.append(reinterpret_cast<char *>(&fr), size_struct);
        int transf = (size_piece < size - i ? SIZE_DATA : size - i + size_struct);
        buf.append(s.data() + i, transf);
        m_socket.async_send_to(boost::asio::buffer(buf.data(), transf), udp::endpoint(id_to_address(group),
            m_port), std::bind(&udp_com::handle_send, this, _1, _2));
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
