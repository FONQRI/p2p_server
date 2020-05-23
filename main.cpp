#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "src/udp/udp_server.h"

int main(int argc, char* argv[])
{
    boost::asio::io_service io_service;
    udp_server server(io_service);
    io_service.run();
    return 0;
}
