#include "udp_asyn.hpp"
#include <thread>


void msg(const std::string &s, boost::asio::ip::address ip)
{
    std::cerr << "ip == " << ip << " >>> func res = " << s << " <<<" << std::endl;
}

int main()
{
    boost::asio::io_service serv;

    udp::endpoint receiver_endpoint(udp::v4(), 15);
    Udp_asyn s(serv, receiver_endpoint);

    s.set_on_msg(msg);

    std::thread t([&](){
        s.receive_async();
        serv.run();
    });

    std::string br = "this is broadcast";
    while (1)
    {
        s.broad_msg(br, 15);
        sleep(5);
    }
    
    #if 0 //test multi
    std::set<udp::endpoint> sp;
    sp.insert(udp::endpoint(udp::v4(), 2003));
    sp.insert(udp::endpoint(udp::v4(), 2001));
    sp.insert(udp::endpoint(udp::v4(), 15));
    sp.insert(udp::endpoint(boost::asio::ip::address::from_string("192.168.0.116"), 15));
    sp.insert(udp::endpoint(boost::asio::ip::address::from_string("192.168.0.184"), 15));
    sp.insert(udp::endpoint(boost::asio::ip::address::from_string("192.168.0.184"), 2001));
    s.multi_msg(sp, "multi testing");
    t.join();
    #endif

    return 0;
}
