#pragma once
#include <cstdint>
#include <boost/asio.hpp>
class ISocket
{
public:
    virtual ~ISocket(){}
    virtual void async_receive(uint32_t len) = 0;
    virtual void async_send(boost::asio::mutable_buffer mData) = 0;
    virtual uint8_t* receive(int32_t len, int32_t& rev) = 0;
    virtual uint32_t send(boost::asio::mutable_buffer mData) = 0;
    virtual boost::asio::ip::tcp::socket& getSocket() = 0;
    virtual void setID(int id) = 0;
};

