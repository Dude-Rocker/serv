#include "udp_asyn.hpp"
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
    uint id = std::atoi(av[2]);
    
    boost::asio::io_service serv;

    Udp_asyn s(serv, id, port);

    s.set_on_msg(msg);

    std::thread t3([&](){
        s.start_receive();
        serv.run();
    });

    // std::thread t2([&](){
    //     while (1)
    //     {
    //         s.broad_msg("this is broadcast 1");
    //         sleep(5);
    //     }
    // });

    std::string cmd;
    std::string msg;
    ushort ad;
    std::cerr << "enter command: ";
    while(std::cin >> cmd) {
        if (cmd == "exit" || !(std::cin >> msg)) {
            break ;
        } else if (cmd == "broadcast" || cmd == "b:") {
            s.broad_msg(msg);
        } else if (cmd == "multicast" || cmd == "m:") {
            std::set<ushort> ids;
            while ((std::cin >> ad) && ad)
                ids.insert(ad);
            s.multi_msg(msg, ids);
        } else {
            std::cerr << "usage <command> <message> [id]\ncommand:\n  b: | broadcast\n  m: | multicast\n";
        }
        std::cerr << "enter command: ";
    }
    

    t3.join();
    // t2.join();

    return 0;
}
