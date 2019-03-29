#include <boost/asio.hpp>

using boost::asio::ip::udp;

int main()
{
    boost::asio::io_service serv;
    
    udp::endpoint receiver_endpoint(udp::endpoint(udp::v4(), 15));

    udp::socket socket(serv, udp::endpoint(udp::v4(), 2003));
    // socket.open(udp::v4());

    for(size_t i = 0; i < 30; i++)
    {
        std::string s = "Hello serv " + std::to_string(i);
        socket.send_to(boost::asio::buffer(s), receiver_endpoint);
        usleep(600000);
    }

    return 0;
}
