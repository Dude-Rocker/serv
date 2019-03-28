#include "udp_asyn.hpp"
#include <thread>


void msg(const std::string &s)
{
    std::cerr << ">>> func res = " << s << " <<<" << std::endl;
}

int main()
{
    boost::asio::io_service serv;
    
    udp::endpoint receiver_endpoint(udp::endpoint(udp::v4(), 15));
    Udp_asyn s(serv, receiver_endpoint);
    // std::thread t([&](){
    //     s.asyn_start();
    // });

    // s.set_func(msg);
    std::string br = "are you hear me?";
    s.broad_msg(br);
    s.asyn_start();
    // t.join();

    return 0;
}
