//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    explicit session(tcp::socket socket)
        : socket_(std::move(socket))
    {}

    void start()
    {
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());

        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                [this, self](boost::system::error_code ec, std::size_t length) {
                                    if (!ec)
                                    {
                                        std::cerr << std::this_thread::get_id() << std::endl;
                                        std::cerr << std::string(data_, length) << std::endl;
                                        do_write(length);
                                    }
                                });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        for (size_t i = 0; i < 2; i++)
        {
            std::clog << length - i * 3 << std::endl;
            boost::asio::async_write(socket_, boost::asio::buffer(data_, length - i * 3),
                                     [this, self](boost::system::error_code ec,
                                                  std::size_t /*length*/) {
                                         if (!ec)
                                         {
                                             do_read();
                                         }
                                     });
        }
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            tcp::endpoint remote_ep = socket.remote_endpoint();
            boost::asio::ip::address remote_ad = remote_ep.address();
            std::string s = remote_ad.to_string();
            auto port = remote_ep.port();
            std::cerr << __FUNCTION__ << " " << __LINE__ << "port:  " << port << std::endl;
            std::cerr << __FUNCTION__ << " " << __LINE__ << "address: " << s << std::endl;

            if (!ec)
            {
                std::make_shared<session>(std::move(socket))->start();
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }
        std::clog << std::atoi(argv[1]) << std::endl;

        boost::asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
