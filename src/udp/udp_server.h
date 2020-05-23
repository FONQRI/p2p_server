#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <list>
#include <string>

using boost::asio::ip::udp;

class udp_server
{
public:
    explicit udp_server(boost::asio::io_service& io_service);
    void session_receive();
    void handle_receive(const boost::system::error_code& e, std::size_t len);
    void session_send(udp::endpoint& ep, std::string write_message);
    void handle_send(const boost::system::error_code& ec, std::size_t len);

private:
    udp::socket m_sock;
    boost::array<char, 256> m_recv_buffer;
    std::string m_write_message;
    std::vector<udp::endpoint> m_endpoint_list;
    udp::endpoint m_remote_endpoint; //current endpoint
};

#endif // UDP_SERVER_H
