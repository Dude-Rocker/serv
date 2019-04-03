#ifndef UDP_COM
#define UDP_COM

#include <iostream>
#include <boost/asio.hpp>
#include <set>
#include <array>

using boost::asio::ip::udp;
#define SIZE_DATA 1024

class udp_com
{

public:
    udp_com(boost::asio::io_service & service, std::set<ushort> & group, ushort port);
    ~udp_com();

    void send_msg_to_group(const std::string &s, ushort group);
    void start_receive();
    void set_on_msg(std::function< void (const std::string &s, boost::asio::ip::address ip) > fun);

	ushort get_port();

private:
    boost::asio::io_service & m_io_service;
    ushort m_port;
	boost::asio::ip::address m_multicast_address;
    udp::socket m_socket;
    udp::endpoint m_remote_endpoint;
	std::array<char, SIZE_DATA> m_buff;
    std::function<void (const std::string &s, boost::asio::ip::address ip)> m_on_msg;
    void handle_send(const boost::system::error_code& error, std::size_t bytes_transf) const;
    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transf);
    boost::asio::ip::address id_to_address(ushort id);

};

#endif
