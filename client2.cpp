#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>

using boost::asio::ip::udp;

int main()
{
    boost::asio::io_service serv;
    
    udp::endpoint receiver_endpoint(udp::endpoint(udp::v4(), 15));

    udp::socket socket(serv);
    socket.open(udp::v4());

    // boost::array<char, 1024> buff;
    // boost::system::error_code err;
    // std::cerr << "try\n";
    // size_t t = socket.receive_from(boost::asio::buffer(buff), receiver_endpoint, 0, err);
    // std::cerr << "catch\n";
    // std::cout.write(buff.data(), t);
    for(size_t i = 0; i < 60; i++)
    {
        std::string s = "hi my friend " + std::to_string(i);
        socket.send_to(boost::asio::buffer(s), receiver_endpoint);
        usleep(300000);
    }

    return 0;
}
