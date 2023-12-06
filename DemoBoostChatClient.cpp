//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "chat_message.hpp"
#include "ZAUtilities.h"
#include "ZADataFormat.h"

static int iReceivedMsg = 0;
static unsigned short serverPortNum = 4105;
static std::atomic<bool> isConnected;
using namespace za::net;

typedef std::deque<chat_message> chat_message_queue;
static std::thread clientThread;
static boost::asio::io_context io_context;
static boost::asio::ip::tcp::resolver resolver(io_context);
std::string serverIP = "10.125.1.175";
void clientConnectServer(std::string serverIPArg);
void sendCMD(std::string msg);
class chat_client
{
public:


    chat_client(boost::asio::io_context& io_context)
        : io_context_(io_context),
        socket_(io_context)
    {
        std::cout << "Client at reception desk" << std::endl;

    }
    void open(const boost::asio::ip::tcp::resolver::results_type& endpoints)
    {
        do_connect(endpoints);
    }
    void write(const chat_message& msg)
    {
        boost::asio::post(io_context_,
            [this, msg]()
            {
                bool write_in_progress = !write_msgs_.empty();
                write_msgs_.push_back(msg);
                if (!write_in_progress)
                {
                    do_write();
                }
            });
    }
    void close()
    {
        boost::asio::post(io_context_, [this]() { socket_.close(); });
        isConnected = false;
    }
    void clear_write()
    {
        write_msgs_.clear();
    }

private:
    void do_connect(const boost::asio::ip::tcp::resolver::results_type& endpoints)
    {
        std::cout << "Client checking booking information at reception desk" << std::endl;
        boost::asio::async_connect(socket_, endpoints,
            [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint)
            {
                if (!ec)
                {
                    std::cout << "Client[" << socket_.local_endpoint() << "] at reception desk[" << socket_.remote_endpoint() << "][ACCEPTED]" << std::endl;
                    isConnected = true;
                    do_read_header();
                }
                else
                {
                    isConnected = false;
                }

            });


    }

    void do_read_header()
    {
        boost::asio::async_read(socket_,
            boost::asio::buffer(read_msg_.data(), chat_message::header_length),
            [this](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec && read_msg_.decode_header())
                {
                    do_read_body();
                }
                else
                {
                    isConnected = false;
                    socket_.close();
                }
            });
    }

    void do_read_body()
    {
        read_msg_.body_length(cnt::ZA_SIZE);
        boost::asio::async_read(socket_,
            boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
            [this](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    char buffer[cnt::ZA_SIZE];
                    std::memcpy(buffer, read_msg_.body(), cnt::ZA_SIZE);
                    std::cout << "Client got the message[";
                    std::string msgStr = std::string(read_msg_.body() + cnt::ZA_HEADER_SIZE, cnt::ZA_BODY_SIZE);
                    std::cout << msgStr;
                    //std::cout.write(read_msg_.body(), cnt::ZA_SIZE);
                    std::cout << "] from the server";
                    std::cout << std::endl;


                    if (read_msg_.body()[cnt::ZA_PACKET::HEADER_1] == cnt::ZA_HEADER_1::HEADER_1_1)
                    {

                    }
                    else if (read_msg_.body()[cnt::ZA_PACKET::HEADER_1] == cnt::ZA_HEADER_1::HEADER_1_2)
                    {

                    }
                    else if (read_msg_.body()[cnt::ZA_PACKET::HEADER_2] == cnt::ZA_HEADER_2::HEADER_2_1)
                    {

                    }
                    else if (read_msg_.body()[cnt::ZA_PACKET::HEADER_2] == cnt::ZA_HEADER_2::HEADER_2_2)
                    {

                    }
                    else if (read_msg_.body()[cnt::ZA_PACKET::HEADER_3] == cnt::ZA_HEADER_3::HEADER_3_1)
                    {

                    }
                    else if (read_msg_.body()[cnt::ZA_PACKET::HEADER_3] == cnt::ZA_HEADER_3::HEADER_3_2)
                    {

                    }



                    do_read_header();
                }
                else
                {
                    isConnected = false;
                    socket_.close();
                }
            });

    }

    void do_write()
    {
        boost::asio::async_write(socket_,
            boost::asio::buffer(write_msgs_.front().data(),
                write_msgs_.front().length()),
            [this](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    std::cout << "Client sent a message to the server" << std::endl;
                    std::cout << "Message[" << std::string(write_msgs_.front().data(), write_msgs_.front().length()) << "]" << std::endl;
                    write_msgs_.pop_front();
                    if (!write_msgs_.empty())
                    {
                        do_write();
                    }
                }
                else
                {
                    isConnected = false;
                    socket_.close();
                }
            });
    }

private:
    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::socket socket_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;
};

static chat_client client(io_context);
static boost::asio::high_resolution_timer timer(io_context);

int main(int argc, char* argv[])
{

    clientConnectServer(serverIP);
    timer.expires_from_now(std::chrono::seconds(10));
    timer.wait();
    sendCMD("USA");

    timer.expires_from_now(std::chrono::seconds(15));
    timer.wait();
    sendCMD("CHINA");

    timer.expires_from_now(std::chrono::seconds(20));
    timer.wait();
    sendCMD("ITALY");

    timer.expires_from_now(std::chrono::seconds(25));
    timer.wait();
    sendCMD("GERMANY");

    timer.expires_from_now(std::chrono::seconds(30));
    timer.wait();
    sendCMD("DUBAI");

    clientThread.join();
    return 0;
}


void clientConnectServer(std::string serverIPArg)
{
    std::thread threadLoc;
    try
    {
        std::string serverRawIP = serverIP;
        boost::system::error_code ec;
        boost::asio::ip::address serverBoostIP = boost::asio::ip::address::from_string(serverRawIP, ec);
        if (ec.value())
        {
            std::cerr << ec.value() << ". Message: " << ec.message();
            return;
        }

        boost::asio::ip::tcp::endpoint serverBoostIPPort(serverBoostIP, serverPortNum);
        std::cout << "Client on " << serverBoostIPPort << "\n";
        auto serverBoostIPPortResolvedByOS = resolver.resolve(serverBoostIPPort);
        client.open(serverBoostIPPortResolvedByOS);

        clientThread = std::thread([&]() { io_context.run(); });

    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

}
//void sendCMD(std::string ctx, std::string ctxSub1, std::string ctxSub2, std::string msgArg)
void sendCMD(std::string msgArg)
{
    char buffer[cnt::ZA_SIZE];

    //if (ctx == "country")
    //{
    //    buffer[cnt::ZA_PACKET::HEADER_1] = cnt::ZA_HEADER_1::HEADER_1_1;
    //}
    //else if (ctx == "continent")
    //{
    //    buffer[cnt::ZA_PACKET::HEADER_1] = cnt::ZA_HEADER_1::HEADER_1_2;
    //}

    //if (ctxSub1 == "population")
    //{
    //    buffer[cnt::ZA_PACKET::HEADER_2] = cnt::ZA_HEADER_2::HEADER_2_1;
    //}
    //else if (ctxSub1 == "language")
    //{
    //    buffer[cnt::ZA_PACKET::HEADER_2] = cnt::ZA_HEADER_2::HEADER_2_2;
    //}

    //if (ctxSub2 == "number")
    //{
    //    buffer[cnt::ZA_PACKET::HEADER_3] = cnt::ZA_HEADER_3::HEADER_3_1;
    //}
    //else if (ctxSub2 == "dominant")
    //{
    //    buffer[cnt::ZA_PACKET::HEADER_3] = cnt::ZA_HEADER_3::HEADER_3_2;
    //}

    //if (buffer[cnt::ZA_PACKET::HEADER_1] == cnt::ZA_HEADER_1::HEADER_1_1 &&
    //    buffer[cnt::ZA_PACKET::HEADER_2] == cnt::ZA_HEADER_2::HEADER_2_1 &&
    //    buffer[cnt::ZA_PACKET::HEADER_3] == cnt::ZA_HEADER_3::HEADER_3_1)
    //    {

    //    }


    //else if()
    buffer[cnt::ZA_PACKET::HEADER_1] = cnt::ZA_HEADER_1::HEADER_1_1;
    buffer[cnt::ZA_PACKET::HEADER_2] = cnt::ZA_HEADER_2::HEADER_2_1;
    buffer[cnt::ZA_PACKET::HEADER_3] = cnt::ZA_HEADER_3::HEADER_3_1;

    buffer[cnt::ZA_PACKET::BODY_1] = msgArg[0];
    buffer[cnt::ZA_PACKET::BODY_2] = msgArg[1];
    buffer[cnt::ZA_PACKET::BODY_3] = msgArg[2];
    buffer[cnt::ZA_PACKET::BODY_4] = msgArg[3];
    buffer[cnt::ZA_PACKET::BODY_5] = msgArg[4];

    chat_message msg;
    msg.body_length(cnt::ZA_SIZE);
    std::memcpy(msg.body(), buffer, cnt::ZA_SIZE);
    msg.encode_header();
    client.write(msg);
}