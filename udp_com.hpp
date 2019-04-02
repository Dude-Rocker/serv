#ifndef UDP_COM
#define UDP_COM

#include <iostream>
#include <boost/asio.hpp>
#include <set>

using boost::asio::ip::udp;
#define SIZE_DATA 1024

class udp_com
{

public:
    udp_com(boost::asio::io_service & service, ushort id, ushort port);
    ~udp_com();

    void send_msg_to_ids(const std::string &s, std::set<ushort> ids);
    void start_receive();
    void set_on_msg(std::function< void (const std::string &s, boost::asio::ip::address ip) > fun);

	ushort get_id();
	ushort get_port();
	boost::asio::ip::address get_multicast_address();

private:
    boost::asio::io_service & m_io_service;
    ushort m_id;
    ushort m_port;
	boost::asio::ip::address m_multicast_address;
    udp::socket m_sock_listen;
    udp::socket m_sock_send;
	std::string m_buff;
    udp::endpoint m_remote_endpoint;
    std::function<void (const std::string &s, boost::asio::ip::address ip)> m_on_msg;
    void handle_send(const boost::system::error_code& error, std::size_t bytes_transf) const;
    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transf);
    boost::asio::ip::address id_to_address(ushort id);

};

#endif
