#include "udp_com.hpp"
#include <thread>


void msg(const std::string &s, boost::asio::ip::address ip)
{
    std::string res = "ip = " + ip.to_string() + " msg = " + s + "\n";
    std::cerr << res;
}

int main(int ac, char **av)
{
    if (ac < 3)
    {
        std::cerr << "usage ./<programm> <port> <...group>\n";
        return 0;
    }
    ushort port = std::atoi(av[1]);
    
    boost::asio::io_service io_service;

    std::set<ushort> group;
    for(size_t i = 2; i < ac; ++i)
    {
        group.insert(std::atoi(av[i]));
    }
    
    udp_com s(io_service, group, port);

    s.set_on_msg(msg);

    std::thread thr([&](){
        s.start_receive();
        io_service.run();
    });

    std::string cmd;
    std::string msg;
    ushort ad;
    while(std::cin >> cmd) {
        if (cmd == "exit" || !(std::cin >> msg)) {
            break ;
        } else if (cmd == "multicast" || cmd == "m:") {
            std::cin >> ad;
            s.send_msg_to_group(msg, ad);
        } else {
            std::cerr << "usage :\n <m:|multicast> <message> <group>\n   <exit> to exit\n";
        }
    }

    thr.join();

    return 0;
}
