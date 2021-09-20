#include "server.h"
#include "Socket.h"
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <future>
#include <thread>
#include <chrono>
#include "SSLSocket.h"
// #include <stropts.h>

int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

using namespace boost::asio;
using namespace boost::posix_time;

int Server::socketID = 1;
Server::Server():mAcceptor(mService, ip::tcp::endpoint(ip::tcp::v4(), 8890)),
    mSSLAcceptor(mService, ip::tcp::endpoint(ip::tcp::v4(), 8891)),
    mContext(boost::asio::ssl::context::sslv23) {
    pData = std::make_unique<uint8_t[]>(100);
    pBufReceive = std::make_unique<uint8_t[]>(max_size);
    mode = 0;
    mContext.set_options(
                boost::asio::ssl::context::default_workarounds
                | boost::asio::ssl::context::no_sslv2
                | boost::asio::ssl::context::single_dh_use);
    // mContext.set_password_callback(boost::bind(&server::get_password, this));
    mContext.use_certificate_chain_file("/var/irtm/certificate.pem");
    mContext.use_private_key_file("/var/irtm/key.pem", boost::asio::ssl::context::pem);
    // SSL_CTX_set_cipher_list(mContext.native_handle(), "ECDHE-RSA-AES128-GCM-SHA256");
    // mContext.use_tmp_dh_file("dh512.pem");
}
void Server::ack(){
    if(!response.empty()) {
        Response msgResponse = response.front();
        response.pop();
        msgResponse.msg.createResponse();
        std::cout << "Respond command mark: " << (uint16_t)msgResponse.msg.getHeader().commandMark << " with success" << std::endl;
        msgResponse.msg.print("Send.txt");
        auto it = mSockets.find(msgResponse.id);
        
        if(it != mSockets.end()) {
            it->second->async_send(boost::asio::buffer(msgResponse.msg.deserialize().get(), msgResponse.msg.getMessageLength()));
        }
    }
}

void Server::ack(Message msg){
    if(!response.empty()) {
        Response msgResponse = response.front();
        response.pop();
        std::cout << "Respond command mark: " << (uint16_t)msg.getHeader().commandMark << std::endl;
        msgResponse.msg.print("Send.txt");
        auto it = mSockets.find(msgResponse.id);
        if(it != mSockets.end()) {
            mSockets.find(msgResponse.id) ->second->async_send(boost::asio::buffer(msg.deserialize().get(), msg.getMessageLength()));
        }
    }
}

void Server::e_ack() {
    if(!response.empty()) {
        Response msgResponse = response.front();
        response.pop();
        msgResponse.msg.createResponse();
        std::cout << "Respond command mark: " << (uint16_t) msgResponse.msg.getHeader().commandMark<< " with error" <<std::endl;
        msgResponse.msg.getHeader().responseSign = 0x02;
        msgResponse.msg.setBCC(msgResponse.msg.calBCC());
        msgResponse.msg.print("Send.txt");
        auto it = mSockets.find(msgResponse.id);
        if(it != mSockets.end()) {
            it->second->async_send(boost::asio::buffer(msgResponse.msg.deserialize().get(), msgResponse.msg.getMessageLength()));
        }
    }
}
void Server::v_ack() {
    if(!response.empty()) {
        Response msgResponse = response.front();
        if(msgResponse.msg.getHeader().commandMark == 0x01) {
            response.pop();
            msgResponse.msg.createResponse();
            std::cout << "Respond command mark: " << (uint16_t)msgResponse.msg.getHeader().commandMark << " with VIN repetition" <<std::endl;
            msgResponse.msg.getHeader().responseSign = 0x03;
            msgResponse.msg.setBCC(msgResponse.msg.calBCC());
            msgResponse.msg.print("Send.txt");
            auto it = mSockets.find(msgResponse.id);
            if(it != mSockets.end()) {
                it->second->async_send(boost::asio::buffer(msgResponse.msg.deserialize().get(), msgResponse.msg.getMessageLength()));
            }
        } else {
            std::cout<<"Not login message\n";
        }
    }
}

void Server::start()
{
    std::cout<<"Start server \n";
    mCommHandler = std::make_shared<myCommHandler>(myCommHandler(*this));
    std::unique_ptr<Socket> pSocket = std::make_unique<Socket>(mService);
    pSocket->setCommHandler(mCommHandler.get());
    pSocket->setID(socketID);
    TCPAcceptorID = socketID;
    mAcceptor.async_accept(pSocket->getSocket(), boost::bind(&Server::onAccept, this, placeholders::error));
    mSockets.insert(std::pair<int, std::unique_ptr<ISocket>>(socketID, std::move(pSocket)));
    socketID += 1;
    std::unique_ptr<SSLSocket> pSSLSocket = std::make_unique<SSLSocket>(mService, mContext);
    pSSLSocket->setCommHandler(mCommHandler.get());
    pSSLSocket->setID(socketID);
    TLSAcceptorID = socketID;
    mSSLAcceptor.async_accept(pSSLSocket->getSocket(), boost::bind(&Server::onSSLAccept, this, placeholders::error));
    mSockets.insert(std::pair<int, std::unique_ptr<ISocket>>(socketID, std::move(pSSLSocket)));

}

void Server::onAccept(const boost::system::error_code& error) 
{
    if(!error){
        std::cout<<"Accepted TCP server!\n";
        auto it = mSockets.find(TCPAcceptorID)->second.get();
        // it->async_receive(max_size);
        it->async_receive(sizeof(Header));
        socketID += 1;
        std::unique_ptr<Socket> pSocket = std::make_unique<Socket>(mService);
        pSocket->setCommHandler(mCommHandler.get());
        pSocket->setID(socketID);
        TCPAcceptorID = socketID;
        mAcceptor.async_accept(pSocket->getSocket(), boost::bind(&Server::onAccept, this, placeholders::error));
        mSockets.insert(std::pair<int, std::unique_ptr<ISocket>>(socketID, std::move(pSocket)));
    } else {
        std::cout<<error.message()<<std::endl;
    }
}

void Server::onSSLAccept(const boost::system::error_code& error) 
{
    if(!error){
        std::cout<<"Accepted TLS server!\n";
        auto it = mSockets.find(TLSAcceptorID)->second.get();
        static_cast<SSLSocket*>(it)->async_handshake();
        socketID += 1;
        std::unique_ptr<SSLSocket> pSSLSocket = std::make_unique<SSLSocket>(mService, mContext);
        pSSLSocket->setCommHandler(mCommHandler.get());
        pSSLSocket->setID(socketID);
        TLSAcceptorID = socketID;
        mSSLAcceptor.async_accept(pSSLSocket->getSocket(), boost::bind(&Server::onSSLAccept, this, placeholders::error));
        mSockets.insert(std::pair<int, std::unique_ptr<ISocket>>(socketID, std::move(pSSLSocket)));
    } else {
        std::cout<<error.message()<<std::endl;
    }
}
void Server::runIO()
{
    mService.run();
}
void Server::stop()
{
    std::cout<<"Server stop\n";
}

void Server::setMode(int m)
{
    mode = m;
}

void Server::myCommHandler::onReceive(uint8_t* pData, uint32_t len, int id)
{
    std::lock_guard<std::mutex> lock(mApp.lockHandler);
    ISocket* socket = mApp.mSockets.find(id)->second.get();
    bool isTLSServer = false;
    if(dynamic_cast<SSLSocket*>(socket)) {
        isTLSServer = true;
    }
    try{
        Header header;
        Message msg;
        std::memcpy(&header, pData, len);
        header.dataLength = ntohs(header.dataLength);
        msg.setHeader(header);
        int32_t pDataUnitRev = 0;
        uint8_t* pDataUnit = new uint8_t[header.dataLength];
        while(pDataUnitRev < header.dataLength) {
            int32_t lenRev = 0;
            uint8_t* pData= socket->receive(header.dataLength - pDataUnitRev, lenRev);
            std::memcpy(pDataUnit + pDataUnitRev, pData, lenRev);
            pDataUnitRev += lenRev;
        }
        // uint8_t* pDataUnit = socket->receive(header.dataLength);
        msg.setData(pDataUnit);
        delete[] pDataUnit;
        uint8_t* bcc = socket->receive(1, pDataUnitRev);
        msg.setBCC(*bcc);
        // Message msg;
        // msg.serialize(pData, len);
        msg.display();
        msg.print("Receive.txt");
        if(msg.verify()) {
            switch(msg.getHeader().commandMark) {
            case 0x01:
                std::cout << "0x01 msg" << std::endl;
                msg.createResponse();
                if(mApp.mode == 0) {
                    msg.print("Send.txt");
                    socket->async_send(boost::asio::buffer(msg.deserialize().get(), msg.getMessageLength()));
                } else {
                    mApp.response.push(Response{id,msg});
                }
                break;
            case 0x02:
                std::cout << "0x02 msg" << std::endl;
                if(!isTLSServer) {
                    std::cout<<"Not TLS server, not respond\n";
                    break;
                }

                msg.createResponse();
                if(mApp.mode == 0) {
                    msg.print("Send.txt");
                    socket->async_send(boost::asio::buffer(msg.deserialize().get(), msg.getMessageLength()));
                } else {
                    mApp.response.push(Response{id,msg});
                }
                break;
            case 0x03:
                std::cout << "0x03 msg" << std::endl;
                if(!isTLSServer) {
                    std::cout<<"Not TLS server, not respond\n";
                    break;
                }
                msg.createResponse();
                if(mApp.mode == 0) {
                    msg.print("Send.txt");
                    socket->async_send(boost::asio::buffer(msg.deserialize().get(), msg.getMessageLength()));
                } else {
                    mApp.response.push(Response{id,msg});
                }
                break;
            case 0x04:
                std::cout << "0x04 msg" << std::endl;
                if(!isTLSServer) {
                    std::cout<<"Not TLS server, not respond\n";
                    break;
                }
                msg.createResponse();
                if(mApp.mode == 0) {
                    msg.print("Send.txt");
                    socket->async_send(boost::asio::buffer(msg.deserialize().get(), msg.getMessageLength()));
                } else {
                    mApp.response.push(Response{id,msg});
                }
                break;
            case 0x07:
                std::cout << "0x07 msg" << std::endl;
                msg.createResponse();
                if(mApp.mode == 0) {
                    msg.print("Send.txt");
                    socket->async_send(boost::asio::buffer(msg.deserialize().get(), msg.getMessageLength()));
                } else {
                    mApp.response.push(Response{id,msg});
                }
                break;
            default:
                std::cout << "Not identify" << std::endl;
            }
        } else {
            std::cout << "Verify msg failed" << std::endl;
        }
    } catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }
    // socket->async_receive(max_size);
    socket->async_receive(sizeof(Header));
}
void Server::myCommHandler::onSend(bool result, int id)
{
    std::lock_guard<std::mutex> lock(mApp.lockHandler);
}
void Server::myCommHandler::onDisconnected(int id)
{
    auto it = mApp.mSockets.find(id);
    if(it != mApp.mSockets.end()) {
        std::cout<<"Session disconnected"<<std::endl;
        boost::system::error_code ec;
        it->second->getSocket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        it->second->getSocket().close();
        // mApp.mSockets.erase(id);
        std::queue<struct Response> empty;
        mApp.response.swap(empty);
    } else {
        std::cout<<"Not a valid session"<<std::endl;
    }
}
int main(int argc, char** argv)
{
    Server server;
    std::future<void> f;
    if(argc > 1) {
        int mode = std::atoi(argv[1]);
        server.setMode(mode);
        if(mode == 1) {
            f = std::async(std::launch::async, [&server](){
                while (true) {
                    if(_kbhit()) {
                        char key = getchar();
                        if(key == 's') {
                            server.ack();
                        } else if (key == 'n') {
                            server.e_ack();
                        } else if (key == 'v') {
                            server.v_ack();
                        } else {
                            std::cout<<"Command not found";
                        }
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            });
        } else if (mode == 2) {
            f = std::async(std::launch::async, [&server](){
                while(true) {
                    std::string msg;
                    std::cin >> msg;
                    std::string GBTmsg;
                    int len;
                    len = msg.length();
                    for(int i = 0; i < len; i++) {
                        int item;
                        int first;
                        int second;
                        if(msg[i] >= '0' && msg[i] <= '9') {
                            first = msg[i] - '0';
                        } else {
                            first = msg[i] - 'a' + 10;
                        }
                        i++;
                        if(msg[i] >= '0' && msg[i] <= '9') {
                            second = msg[i] - '0';
                        } else {
                            second = msg[i] - 'a' + 10;
                        }
                        item = first * 16 + second;
                        GBTmsg += item;
                    }
                    Header header;
                    memcpy(&header, GBTmsg.c_str(), sizeof(header));
                    Message response;
                    response.setData((const uint8_t*)&GBTmsg[0] + sizeof(header));
                    response.setHeader(header);
                    response.setBCC(response.calBCC());
                    server.ack(response);
                }
            });
        }
    }
    server.start();
    server.runIO();
    f.get();
    return 0;
}
