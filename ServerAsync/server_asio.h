#pragma once
#include <iostream>
#include <string>

#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace Net_asio
{
	using boost::asio::ip::tcp;   
	namespace pt = boost::property_tree;

    // A reference-counted non-modifiable buffer class.
    class shared_const_buffer
    {
    public:
        // Construct from a std::string.
        explicit shared_const_buffer(const std::string& data)
            : data_(new std::vector<char>(data.begin(), data.end())),
            buffer_(boost::asio::buffer(*data_))
        {
        }

        // Implement the ConstBufferSequence requirements.
        typedef boost::asio::const_buffer value_type;
        typedef const boost::asio::const_buffer* const_iterator;
        const boost::asio::const_buffer* begin() const { return &buffer_; }
        const boost::asio::const_buffer* end() const { return &buffer_ + 1; }

    private:
        std::shared_ptr<std::vector<char> > data_;
        boost::asio::const_buffer buffer_;
    };

    class Session : public std::enable_shared_from_this<Session>
    {
    private:
        tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
        //boost::asio::mutable_buffer buffer_; 


    public:
        using tcp_pointer = std::shared_ptr<Session>;

        static tcp_pointer create(boost::asio::io_context& ctx)
        {
            return tcp_pointer(new Session(ctx));
        }

        tcp::socket& get_socket()
        {
            return socket_;
        }
        void start();

    private:
        Session(boost::asio::io_context& ctx) : socket_(ctx) {} 

        void DoRead();
        void DoWrite(const std::string& message);
        void HandleMessage(std::size_t length, boost::system::error_code& ec);
    };


    class ServerAsio
    {
    private:
        boost::asio::ip::tcp::acceptor acceptor_;
        boost::asio::io_context& ctx;

    public:
        ServerAsio(boost::asio::io_context& ioContext, short port)
            : ctx(ioContext), acceptor_(ioContext, tcp::endpoint(tcp::v4(), port))
        {
            DoAccept();
        }

    private:
        void DoAccept()
        {
            acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket)
                {
                    if (!ec)
                    {
                        auto newConnection = Session::create(ctx);  
                        newConnection->get_socket() = std::move(socket); 
                        newConnection->start(); 
                    }

                    DoAccept();
                });
        }

	};
}

