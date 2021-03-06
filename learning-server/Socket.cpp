#include "Socket.h"
#include <memory>
#include <boost/bind.hpp>
#include <iostream>

Socket::Socket(io_service& service):mService(service), mSocket(mService) {
    pBufReceive = std::make_unique<uint8_t[]>(maxsize);
    pBufSend = std::make_unique<uint8_t[]>(maxsize);
}
Socket::~Socket() {
    std::cout<<"Socket destructor\n";
}

void Socket::async_receive(uint32_t len) {
    size = len;
    mSocket.async_receive(boost::asio::buffer(pBufReceive.get(),len), boost::bind(&Socket::onReceive,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
    // boost::asio::async_read_until(mSocket, pStreamBuf,"##",boost::bind(&onReadUntil,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
}

void Socket::async_send(boost::asio::mutable_buffer mData) {
    std::size_t s1 = boost::asio::buffer_size(mData);
    char* p1 = boost::asio::buffer_cast<char*>(mData);
    std::memcpy(pBufSend.get(), p1, s1);
    mSocket.async_send(boost::asio::buffer(pBufSend.get(),s1), boost::bind(&Socket::onSend, this, boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
}

uint8_t* Socket::receive(int32_t len, int32_t& rev) {
    rev = mSocket.receive(boost::asio::buffer(pBufReceive.get(), len));
    std::cout<<"Byte sync receive: "<<rev<<std::endl;
    return pBufReceive.get();
}

uint32_t Socket::send(boost::asio::mutable_buffer mData) {
    return 1;
}

ip::tcp::socket& Socket::getSocket() {
    return mSocket;
}

void Socket::setCommHandler(ICommHandler* const ptr) {
    mCommHandler = std::shared_ptr<ICommHandler>(ptr);
}

void Socket::setID(int mid)
{
    this->id = mid;
}

void Socket::onReceive(boost::system::error_code error, std::size_t bytes_transferred) {
    std::cout << "Byte received: "<<bytes_transferred<<std::endl;
    if(!error) {
        mCommHandler->onReceive(pBufReceive.get(), bytes_transferred, id);
        // mSocket.async_receive(boost::asio::buffer(pBufReceive.get(),size), boost::bind(&onReceive,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));  
    } else {
        std::cout << error.message() <<std::endl;
        mCommHandler->onDisconnected(id);
    }
}

void Socket::onReadUntil(const boost::system::error_code& error, std::size_t size) {
    std::cout << "Byte received: "<<size<<std::endl;
    if(!error) {
        // mCommHandler->onReceive(pBufReceive.get(), bytes_transferred, id);
        boost::asio::async_read_until(mSocket, pStreamBuf,"##",boost::bind(&Socket::onReadUntil,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
    } else {
        std::cout << error.message() <<std::endl;
        mCommHandler->onDisconnected(id);
    }
}

void Socket::onSend(boost::system::error_code error, std::size_t bytes_transferred) {
    std::cout << "Byte sent: "<<std::dec <<bytes_transferred<<std::endl;
    if(!error) {
        mCommHandler->onSend(true, id);
    } else {
        mCommHandler->onSend(false, id);
    }
}
