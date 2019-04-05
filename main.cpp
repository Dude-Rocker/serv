#include "udp_com.hpp"
#include <thread>
#include <fstream>

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
    
    udp_com commun(io_service, group, port);

    commun.set_on_msg(msg);
    commun.start_receive();

    std::thread thr([&](){
        io_service.run();
    });

    std::string cmd;
    ushort ad;

    while(std::cin >> cmd) {
        std::string msg;
        if (cmd == "exit") {
            break ;
        } else if (cmd == "multicast" || cmd == "m:") {
            std::cin >> msg;
            std::cin >> ad;
            commun.send_msg_to_group(msg, ad);
        } else if (cmd == "add") {
            std::cin >> ad;            
            commun.add_group(ad);
        } else if (cmd == "del") {
            std::cin >> ad;
            commun.del_group(ad);
        } else if (cmd == "port") {
            msg = "this port is " + std::to_string(commun.get_port());
            std::cerr << msg << std::endl;
        } else if (cmd == "r:" || cmd == "read") {
            std::cin >> msg;
            std::ifstream is(msg);
            std::cin >> ad;
            if (is.is_open()) {
                msg = "";
                std::getline(is, msg, '\0');
                commun.send_msg_to_group(msg, ad);
            } else {
                std::cerr << "cant read " << msg << std::endl;
            }
        } else if (cmd == "address") {
            std::set<ushort> ads = commun.get_addrs();
            msg = "group contain:";
            for(ushort ad : ads)
            {
                msg += " " + std::to_string(ad);
            }
            msg += " adresses";
            std::cerr << msg << std::endl;
        } else {
            std::cerr << "usage :\n\t<m:|multicast> <'message'> <'group'>\n\
\t<add|del> <'group'>\n\t<r:|read> <file> <'group'>\n\t<address|port>\n\t<exit> to exit\n";
        }
    }
    
    io_service.stop();
    thr.join();

    return 0;
}
