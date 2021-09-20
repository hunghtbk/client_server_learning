#include "SSLSocket.h"
#include <memory>
#include <boost/bind.hpp>
#include <iostream>
#include "Protocol.h"

SSLSocket::SSLSocket(io_service& service, boost::asio::ssl::context& context):mService(service), mSocket(mService, context) {
    pBufReceive = std::make_unique<uint8_t[]>(maxsize);
    pBufSend = std::make_unique<uint8_t[]>(maxsize);
    bHandshaked = false;
}
SSLSocket::~SSLSocket() {
    
}

void SSLSocket::async_receive(uint32_t len) {
    size = len;
    mSocket.async_read_some(boost::asio::buffer(pBufReceive.get(),size), boost::bind(&SSLSocket::onReceive,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
}

void SSLSocket::async_handshake() {
    mSocket.async_handshake(boost::asio::ssl::stream_base::server,
                boost::bind(&SSLSocket::onHandShake, this,
                boost::asio::placeholders::error));
}
void SSLSocket::async_send(boost::asio::mutable_buffer mData) {
    std::size_t s1 = boost::asio::buffer_size(mData);
    char* p1 = boost::asio::buffer_cast<char*>(mData);
    std::memcpy(pBufSend.get(), p1, s1);
    mSocket.async_write_some(boost::asio::buffer(pBufSend.get(),s1), boost::bind(&SSLSocket::onSend, this, boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
}
uint8_t* SSLSocket::receive(int32_t len, int32_t& rev) {
    rev = mSocket.read_some(boost::asio::buffer(pBufReceive.get(), len));
    std::cout<<"Byte sync receive: "<<rev<<std::endl;
    return pBufReceive.get();
}

uint32_t SSLSocket::send(boost::asio::mutable_buffer mData) {
    return 1;
}

boost::asio::ip::tcp::socket& SSLSocket::getSocket() {
    return mSocket.next_layer();
}

void SSLSocket::setCommHandler(ICommHandler* const ptr) {
    mCommHandler = std::shared_ptr<ICommHandler>(ptr);
}

void SSLSocket::setID(int mid)
{
    this->id = mid;
}

void SSLSocket::onReceive(boost::system::error_code error, std::size_t bytes_transferred) {
    std::cout << "Byte received: "<<bytes_transferred<<std::endl;
    // static bool login = false;
    if(!error) {
        // if(!login) {
            mCommHandler->onReceive(pBufReceive.get(), bytes_transferred, id);
            // login = true;
        // }
        // mSocket.async_read_some(boost::asio::buffer(pBufReceive.get(),size), boost::bind(&onReceive,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));  
    } else {
        std::cout << error.message() <<std::endl;
        mCommHandler->onDisconnected(id);
        bHandshaked = false;
    }
}
void SSLSocket::onHandShake(boost::system::error_code error) {
    std::cout<<"On Handshake callback "<< error.message() << std::endl;
    if(!error) {
        bHandshaked = true;
        async_receive(sizeof(Header));
    } else {
        mCommHandler->onDisconnected(id);
        bHandshaked = false;
    }
}
void SSLSocket::onReadUntil(const boost::system::error_code& error, std::size_t size) {
    std::cout << "Byte received: "<<size<<std::endl;
    if(!error) {
        // mCommHandler->onReceive(pBufReceive.get(), bytes_transferred, id);
        boost::asio::async_read_until(getSocket(), pStreamBuf,"##",boost::bind(&SSLSocket::onReadUntil,this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
    } else {
        std::cout << error.message() <<std::endl;
        mCommHandler->onDisconnected(id);
        bHandshaked = false;
    }
}

void SSLSocket::onSend(boost::system::error_code error, std::size_t bytes_transferred) {
    std::cout << "Byte sent: "<<std::dec <<bytes_transferred<<std::endl;
    if(!error) {
        mCommHandler->onSend(true, id);
    } else {
        mCommHandler->onSend(false, id);
    }
}
