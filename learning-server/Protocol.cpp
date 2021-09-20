#include "Protocol.h"
#include <iostream>
#include <fstream>
#include <bitset>
#include <cstring>
#include <arpa/inet.h>
#include <ctime>
#include <sys/time.h>
#include  <iomanip>
#include <chrono>
Message::Message()
{
}
Message::Message(Header mHeader, const uint8_t* pBuf)
{
    setHeader(mHeader);
    setData(pBuf);
}
Message::~Message()
{
}
void Message::setHeader(const Header header)
{
    mHeader = header;
}
Header& Message::getHeader()
{
    return mHeader;
}
void Message::setData(const uint8_t* pBuf)
{
    data.assign(pBuf, pBuf + mHeader.dataLength);
}
uint8_t Message::calBCC()
{
    uint8_t temp = 0;
    uint8_t* pHeader = (uint8_t*)&mHeader;
    for(int i = 0; i<24; i++ ) {
        // std::cout << std::setfill('0') << std::setw(2) << std::hex << (uint16_t)*(pHeader + i);
        temp ^= *(pHeader + i);
    }
    for(auto &i:data) {
        // std::cout << std::setfill('0') << std::setw(2) << std::hex << (uint16_t)i;
        temp ^= i;
    }
    // std::unique_ptr<uint8_t[]> ptr = deserialize();
    // uint8_t* p = ptr.get();
    // for(int i=0; i < getMessageLength() - 1; i++) {
    //     std::cout << std::setfill('0') << std::setw(2) << std::hex << (uint16_t)*(p + i);
    //     temp ^= *(p + i);
    // }
    // std::cout<<std::endl<<"BCC: "<<std::hex<<(uint16_t)temp<<std::endl;
    return temp;
}
void Message::setBCC(const uint8_t bcc)
{
    mBCC = bcc;
}
bool Message::verify()
{
    return mBCC == calBCC();
}
void Message::display()
{
    std::cout << "Start character: " << mHeader.startCharacter[0] << mHeader.startCharacter[1] << std::endl;
    std::cout << "Command Mark: " << std::hex <<(uint16_t)mHeader.commandMark << std::endl;
    std::cout << "Response Sign: " << std::hex << (uint16_t)mHeader.responseSign << std::endl;
    std::cout << "VIN: " << getVIN() << std::endl;
    std::cout << "Encrypt type: "<< std::hex << (uint16_t)mHeader.encrypType << std::endl;
    std::cout << "Data length: " << mHeader.dataLength <<std::endl;
    std::cout << "Data: ";
    // if(mHeader.commandMark != 0x02 && mHeader.commandMark != 0x03) { 
        for(auto &i:data) {
            std::cout << std::setfill('0') << std::setw(2) <<(uint16_t)i << '-';
        }
    // } 
    std::cout << std::endl << "BCC: " << std::bitset<8>(mBCC)<<std::endl;
}
void Message::print(const char* path)
{
    std::ofstream out(path, std::ios_base::app);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::tm* tm = localtime(&tv.tv_sec);
    out << "Time: " << tm->tm_year + 1900 << '/' <<tm->tm_mon + 1<< '/' <<tm->tm_mday<< ' ' << tm->tm_hour<< ':' \
    << tm->tm_min<< ':' << tm->tm_sec << '.' << (uint32_t)tv.tv_usec/1000 << std::endl;
    out << "Start character: " << mHeader.startCharacter[0] << mHeader.startCharacter[1] << std::endl;
    out << "Command Mark: " << std::hex << (uint16_t)mHeader.commandMark << std::endl;
    out << "Response Sign: " << std::hex << (uint16_t)mHeader.responseSign << std::endl;
    out << "VIN: " << getVIN() << std::endl;
    out << "Encrypt type: "<<std::hex<<(uint16_t)mHeader.encrypType << std::endl;
    out << "Data length: " << std::hex << mHeader.dataLength <<std::endl;
    out << "Data: ";
    for(auto &i:data) {
        out << std::setfill('0') << std::setw(2) <<(uint16_t)i << '-';
    }
    out << std::endl << "BCC: " << std::bitset<8>(mBCC)<<std::endl;
}
std::unique_ptr<uint8_t[]> Message::deserialize() {
    const uint16_t headerLen = sizeof(Header);
    const uint8_t bccLen = 1;
    std::unique_ptr<uint8_t[]> p = std::make_unique<uint8_t[]>(headerLen + mHeader.dataLength + bccLen);
    uint8_t* delegateP = p.get();
    mHeader.dataLength = htons(mHeader.dataLength);
    std::memcpy(delegateP, &mHeader, headerLen);
    mHeader.dataLength = ntohs(mHeader.dataLength);
    delegateP += headerLen;
    std::memcpy(delegateP, &data[0], mHeader.dataLength);
    delegateP += mHeader.dataLength;
    std::memcpy(delegateP, &mBCC, bccLen);
    return p;
}

uint32_t Message::getMessageLength() {
    return sizeof(Header) + mHeader.dataLength + 1;
}

void Message::serialize(uint8_t* pData, uint32_t len) {
    std::memcpy(&mHeader, pData, sizeof(Header));
    mHeader.dataLength = ntohs(mHeader.dataLength);
    setData(pData + sizeof(Header));
    mBCC = *(pData + sizeof(Header) + mHeader.dataLength);
}

std::string Message::getVIN() {
    std::string vin(mHeader.vin, mHeader.vin + 17);
    vin[17] = '\0';
    return vin;
}
void Message::createResponse() {
    mHeader.responseSign = 0x01;
    mHeader.dataLength = 6;
    data.clear();
    struct timeval now;
    gettimeofday(&now, NULL);
    uint8_t timezone = 8;
    std::chrono::system_clock::time_point utcTime = std::chrono::system_clock::from_time_t(now.tv_sec);
    utcTime += std::chrono::duration<int,std::ratio<60*60>>(timezone);
    time_t beijingTime = std::chrono::system_clock::to_time_t(utcTime);
    struct tm* tm = std::localtime(&beijingTime);
    data.push_back((tm->tm_year + 1900)%100);
    data.push_back(tm->tm_mon + 1);
    data.push_back(tm->tm_mday);
    data.push_back(tm->tm_hour);
    data.push_back(tm->tm_min);
    data.push_back(tm->tm_sec);
    mBCC = this->calBCC();
}