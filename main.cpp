#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

/**
   * @brief  Convert const char* to size_t
   * @note   When there is an error it returns the maximum of size_t
   * @param  *number: const char*
   * @retval size_t
   */
size_t to_size_t(const char* number)
{
    size_t sizeT;
    std::istringstream iss(number);
    iss >> sizeT;
    if (iss.fail())
    {
        return std::numeric_limits<size_t>::max();
    }
    else
    {
        return sizeT;
    }
}

using boost::asio::ip::udp;

class udp_server
{
public:
    explicit udp_server(boost::asio::io_service& io_service)
        : _sock(io_service, udp::endpoint(udp::v4(), 2333))
    {
        session_receive();
    }
    void session_receive();
    void handle_receive(const boost::system::error_code& ec, std::size_t len);
    void session_send(udp::endpoint& ep, std::string write_message);
    void handle_send(const boost::system::error_code& ec, std::size_t len);

private:
    udp::socket _sock;
    boost::array<char, 256> _recv_buffer;
    std::string _write_message;
    std::vector<udp::endpoint> _endpoint_list;
    udp::endpoint _remote_endpoint; //current endpoint
};

void udp_server::session_receive()
{
    _sock.async_receive_from(boost::asio::buffer(_recv_buffer), _remote_endpoint,
                             boost::bind(&udp_server::handle_receive, this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}

void udp_server::handle_receive(const boost::system::error_code&, std::size_t len)
{
    std::string receive_message(_recv_buffer.data(), len);
    if (strcmp(receive_message.c_str(), "login") == 0)
    {
        if (std::find(_endpoint_list.begin(), _endpoint_list.end(), _remote_endpoint)
            != _endpoint_list.end())
        {

            session_send(_remote_endpoint, "You have already logged in.\n");
        }
        else
        {
            std::cout << "User login.\nAddress : " << _remote_endpoint.address().to_string()
                      << std::endl;
            std::cout << "Port : " << _remote_endpoint.port() << std::endl;
            _endpoint_list.push_back(_remote_endpoint);
            session_send(_remote_endpoint, "login success.\n");
        }
    }
    else if (strcmp(receive_message.c_str(), "logout") == 0)
    {
        if (auto it = std::find(_endpoint_list.begin(), _endpoint_list.end(), _remote_endpoint);
            it != _endpoint_list.end())
        {
            _endpoint_list.erase(it);
            std::cout << "User logout.\nAddress : " << _remote_endpoint.address().to_string()
                      << std::endl;
            std::cout << "Port : " << _remote_endpoint.port() << std::endl;
            session_send(_remote_endpoint, "Logout success.\n");
        }
        else
        {
            session_send(_remote_endpoint, "Logout failed, you have not logged in.\n");
        }
    }
    else if (strcmp(receive_message.c_str(), "list") == 0)
    {
        std::ostringstream message;
        int i = 0;
        for (auto iter : _endpoint_list)
        {
            if (iter == _remote_endpoint)
                message << "[" << i << "]" << iter.address().to_string() << ":" << iter.port()
                        << " (yourself)" << std::endl;
            else
                message << "[" << i << "]" << iter.address().to_string() << ":" << iter.port()
                        << std::endl;
            i++;
        }
        session_send(_remote_endpoint, message.str());
    }
    else if (strncmp(receive_message.c_str(), "punch", 5) == 0)
    {
        std::clog << __FUNCTION__ << __LINE__ << std::endl;

        size_t punched_client = to_size_t(receive_message.c_str() + 6);

        std::ostringstream message;
        if (_endpoint_list.size() > punched_client)
        {
            std::ostringstream peer_message;
            //udp::endpoint peer_endpoint(iter->address(), iter->port);
            peer_message << "PUNCH_REQUEST " << _remote_endpoint.address().to_string() << ":"
                         << _remote_endpoint.port() << std::endl;
            session_send(_endpoint_list[punched_client], peer_message.str());
            message << "PUNCH_SUCCESS " << _endpoint_list[punched_client].address().to_string()
                    << ":" << _endpoint_list[punched_client].port() << std::endl;
        }
        else
        {
            message << "Punch failed, no such client.";
        }

        session_send(_remote_endpoint, message.str());
    }
    else if (strcmp(receive_message.c_str(), "help") == 0)
    {
        session_send(_remote_endpoint, "Command:\
                                       \n\thelp : Show this information.\
                                       \n\tlogin : Login p2p server to make you punchable.\
                                       \n\tlogout : Logout p2p server so that other client(s) won't find you.\
                                       \n\tlist: List all client(s) that have login.\
                                       \n\tpunch <client_number>: send punch request to remote client and start a p2p session\n");
    }
    else
    {
        session_send(_remote_endpoint,
                     "Unknown command, please type 'help' to see more options.\n");
    }
    session_receive();
}

void udp_server::session_send(udp::endpoint& ep, std::string write_message)
{
    //std::getline(std::cin, _write_message);
    _sock.async_send_to(boost::asio::buffer(write_message), ep,
                        boost::bind(&udp_server::handle_send, this, boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));
}

void udp_server::handle_send(const boost::system::error_code& ec, std::size_t len)
{
    //session_send();
}

int main(int argc, char* argv[])
{

    boost::asio::io_service io_service;
    udp_server server(io_service);
    io_service.run();
    return 0;
}
