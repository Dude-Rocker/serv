#include "udp_com.hpp"
#include <thread>


void msg(const std::string &s, boost::asio::ip::address ip)
{
    std::string res = "ip = " + ip.to_string() + " msg = " + s + "\n";
    std::cerr << res;
}

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "usage ./<programm> <port> <id>\n";
        return 0;
    }
    ushort port = std::atoi(av[1]);
    ushort id = std::atoi(av[2]);
    
    boost::asio::io_service serv;

    udp_com s(serv, id, port);

    s.set_on_msg(msg);

    std::thread t3([&](){
        s.start_receive();
        serv.run();
    });

    std::string cmd;
    std::string msg;
    ushort ad;
    while(std::cin >> cmd) {
        if (cmd == "exit" || !(std::cin >> msg)) {
            break ;
        } else if (cmd == "multicast" || cmd == "m:") {
            std::set<ushort> ids;
            while ((std::cin >> ad) && ad)
                ids.insert(ad);
            s.send_msg_to_ids(msg, ids);
        } else {
            std::cerr << "usage <m:|multicast> <message> <...id> <0> \nor <exit> to exit\n";
        }
    }

    t3.join();

    return 0;
}
