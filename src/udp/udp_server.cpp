#include "udp_server.h"

#include <boost/bind/bind.hpp>

#include "utilities.h"

udp_server::udp_server(boost::asio::io_service& io_service)
    : m_sock(io_service, udp::endpoint(udp::v4(), 2333))
{
    session_receive();
}

void udp_server::session_receive()
{
    m_sock.async_receive_from(boost::asio::buffer(m_recv_buffer), m_remote_endpoint,
                              boost::bind(&udp_server::handle_receive, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
}

//FIXME check error code
void udp_server::handle_receive(const boost::system::error_code& e, std::size_t len)
{

    boost::asio::io_service io_service;
    boost::asio::io_service::strand io_strand(io_service);
    boost::asio::ip::tcp::resolver resolver(io_service);

    std::clog << __FUNCTION__ << __LINE__ << std::endl;
    std::clog << __FUNCTION__ << __LINE__ << m_remote_endpoint.address() << " "
              << m_remote_endpoint.port() << std::endl;
    resolver.async_resolve({m_remote_endpoint.address(), m_remote_endpoint.port()},
                           io_strand.wrap(
                               [this](const boost::system::error_code& ec,
                                      boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
                                   std::clog << __FUNCTION__ << __LINE__ << std::endl;
                                   if (!ec)
                                   {
                                       std::clog << __FUNCTION__ << __LINE__ << std::endl;
                                       std::for_each(endpoint_iterator, {}, [](auto& it) {
                                           std::clog << __FUNCTION__ << __LINE__ << std::endl;
                                           std::clog << it.endpoint().address() << std::endl;
                                       });
                                       std::clog << __FUNCTION__ << __LINE__ << std::endl;
                                   }
                                   else
                                   {
                                       std::clog << __FUNCTION__ << __LINE__ << std::endl;
                                       std::clog << "FONQRI: " << ec.message() << std::endl;
                                   }
                               }));

    std::string receive_message(m_recv_buffer.data(), len);
    if (receive_message == "login")
    {
        if (std::find(m_endpoint_list.begin(), m_endpoint_list.end(), m_remote_endpoint)
            != m_endpoint_list.end())
        {

            session_send(m_remote_endpoint, "You have already logged in.\n");
        }
        else
        {
            std::cout << "User login.\nAddress : " << m_remote_endpoint.address().to_string()
                      << std::endl;
            std::cout << "Port : " << m_remote_endpoint.port() << std::endl;
            m_endpoint_list.push_back(m_remote_endpoint);
            session_send(m_remote_endpoint, "login success.\n");
        }
    }
    else if (receive_message == "logout")
    {
        if (auto it = std::find(m_endpoint_list.begin(), m_endpoint_list.end(), m_remote_endpoint);
            it != m_endpoint_list.end())
        {
            m_endpoint_list.erase(it);
            std::cout << "User logout.\nAddress : " << m_remote_endpoint.address().to_string()
                      << std::endl;
            std::cout << "Port : " << m_remote_endpoint.port() << std::endl;
            session_send(m_remote_endpoint, "Logout success.\n");
        }
        else
        {
            session_send(m_remote_endpoint, "Logout failed, you have not logged in.\n");
        }
    }
    else if (receive_message == "list")
    {
        std::ostringstream message;
        int i = 0;
        for (auto iter : m_endpoint_list)
        {
            if (iter == m_remote_endpoint)
                message << "[" << i << "]" << iter.address().to_string() << ":" << iter.port()
                        << " (yourself)" << std::endl;
            else
                message << "[" << i << "]" << iter.address().to_string() << ":" << iter.port()
                        << std::endl;
            i++;
        }
        session_send(m_remote_endpoint, message.str());
    }
    else if (receive_message.compare(0, 5, "punch") == 0)
    {
        size_t punched_client = utilities::to_size_t(receive_message.c_str() + 6);

        std::ostringstream message;
        if (m_endpoint_list.size() > punched_client)
        {
            std::ostringstream peer_message;
            //udp::endpoint peer_endpoint(iter->address(), iter->port);
            peer_message << "PUNCH_REQUEST " << m_remote_endpoint.address().to_string() << ":"
                         << m_remote_endpoint.port() << std::endl;
            session_send(m_endpoint_list[punched_client], peer_message.str());
            message << "PUNCH_SUCCESS " << m_endpoint_list[punched_client].address().to_string()
                    << ":" << m_endpoint_list[punched_client].port() << std::endl;
        }
        else
        {
            message << "Punch failed, no such client.";
        }

        session_send(m_remote_endpoint, message.str());
    }
    else if (receive_message == "help")
    {
        session_send(m_remote_endpoint, "Command:\
                                       \n\thelp : Show this information.\
                                       \n\tlogin : Login p2p server to make you punchable.\
                                       \n\tlogout : Logout p2p server so that other client(s) won't find you.\
                                       \n\tlist: List all client(s) that have login.\
                                       \n\tpunch <client_number>: send punch request to remote client and start a p2p session\n");
    }
    else
    {
        session_send(m_remote_endpoint,
                     "Unknown command, please type 'help' to see more options.\n");
    }
    session_receive();
}

void udp_server::session_send(udp::endpoint& ep, std::string write_message)
{
    //std::getline(std::cin, _write_message);
    m_sock.async_send_to(boost::asio::buffer(write_message), ep,
                         boost::bind(&udp_server::handle_send, this,
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred));
}

void udp_server::handle_send(const boost::system::error_code& ec, std::size_t len)
{
    //session_send();
}
