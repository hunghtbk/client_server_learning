#pragma once
#include "ISocket.h"
#include "ICommHandler.h"
using namespace boost::asio;
class Socket:public ISocket
{
public:
    Socket(io_service& service);
    ~Socket();
    void async_receive(uint32_t len = maxsize);
    void async_send(boost::asio::mutable_buffer mData);
    uint8_t* receive(int32_t len, int32_t& rev);
    uint32_t send(boost::asio::mutable_buffer mData);
    ip::tcp::socket& getSocket();
    void setCommHandler(ICommHandler* const ptr);
    void setID(int id);
private:
    void onReceive(boost::system::error_code error, std::size_t bytes_transferred);
    void onSend(boost::system::error_code error, std::size_t bytes_transferred);
    void onReadUntil(const boost::system::error_code& e, std::size_t size);
private:
    static const uint32_t maxsize = 65535;
    io_service& mService;
    ip::tcp::socket mSocket;
    std::unique_ptr<uint8_t[]> pBufSend;
    std::unique_ptr<uint8_t[]> pBufReceive;
    uint64_t size;
    std::shared_ptr<ICommHandler> mCommHandler;
    int id;
    boost::asio::streambuf pStreamBuf;
};

