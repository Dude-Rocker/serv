#include "udp_asyn.hpp"
#include <thread>


void msg(const std::string &s, boost::asio::ip::address ip)
{
    std::string res = "ip = " + ip.to_string() + " msg = " + s + "\n";
    std::cerr << res;
}

int main()
{
    boost::asio::io_service serv;

    udp::endpoint receiver_endpoint(udp::v4(), 15);
    Udp_asyn s(serv, receiver_endpoint);

    s.set_on_msg(msg);

    std::thread t([&](){
        s.start_receive();
        serv.run();
    });

    std::thread t2([&](){
        while (1)
        {
            s.broad_msg("this is broadcast", 15);
            sleep(5);
        }
    });
    
    udp::endpoint mult(boost::asio::ip::address::from_string("239.255.0.1"), 1200);

    std::thread t4([&](){
        s.mult_receive(boost::asio::ip::address::from_string("0.0.0.0"), boost::asio::ip::address::from_string("239.255.0.1"), 1200);
        serv.run();
    });

    std::thread t3([&](){
        for(size_t i = 0; i < 100; i++)
        {
            s.multi_msg("this is multicast", mult);
            sleep(5);
        }
    });
    t3.join();
    t.join();
    t2.join();
    t4.join();

    return 0;
}
