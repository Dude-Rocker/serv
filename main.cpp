#include "udp_asyn.hpp"
#include <thread>


void msg(const std::string &s, boost::asio::ip::address ip)
{
    std::cerr << "ip == " << ip << "  >>> func res = " << s << " <<<" << std::endl;
}

int main()
{
    boost::asio::io_service serv;
    
    udp::endpoint receiver_endpoint(udp::v4(), 15);
    Udp_asyn s(serv, receiver_endpoint);
    std::thread t([&](){
        s.receive_async();
        serv.run();
    });

    s.set_func(msg);
    std::string br = "are you hear me?";
    s.broad_msg(br, 15);
    sleep(2);
    br = "yes i hear myself!";
    s.broad_msg(br, 15);
    sleep(3);
    std::set<udp::endpoint> sp;

    sp.insert(udp::endpoint(udp::v4(), 2003));
    sp.insert(udp::endpoint(udp::v4(), 2001));
    s.multi_msg(sp, "multi testing");
    t.join();

    return 0;
}
