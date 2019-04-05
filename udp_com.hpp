#ifndef UDP_COM
#define UDP_COM

#include <iostream>
#include <boost/asio.hpp>
#include <set>
#include <array>
#include <map>

using boost::asio::ip::udp;

#define SIZE_DATA 1024 //don`t use buff lesser than 512
#define TIME_SEC 5
#define PREAM 55001

class udp_com
{

public:

    udp_com(boost::asio::io_service & service, std::set<ushort> & group, ushort port);
    ~udp_com();

    void send_capture(const std::string & st, ushort group);
    void send_msg_to_group(const std::string &s, ushort group);
    void start_receive();
    void set_on_msg(std::function< void (const std::string &s, boost::asio::ip::address ip) > fun);
    void add_group(ushort add);
    void del_group(ushort add);

	ushort get_port();
	std::set<ushort> get_addrs();

private:

    struct frame
    {
        uint16_t frame_id;
        uint16_t packeg_id;
        uint16_t full_pack;
        uint16_t preamble;
    };

    struct rec_frame
    {
        uint16_t packeg_id;
        uint16_t full_pack;
        std::string msg;
    };

    std::string m_img;
    boost::asio::io_service & m_io_service;
    ushort m_port;
	boost::asio::ip::address m_multicast_address;
    udp::socket m_socket;
	std::set<ushort> m_addr;
    udp::endpoint m_remote_endpoint;
	std::array<char, SIZE_DATA> m_buff;
    std::function<void (const std::string &s, boost::asio::ip::address ip)> m_on_msg;
    void handle_send(const boost::system::error_code& error, std::size_t bytes_transf) const;
    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transf);
    boost::asio::ip::address id_to_address(ushort id);
    uint8_t m_frame_id = 0;
    std::map<std::string, std::vector<rec_frame>> m_map;

};

#endif
