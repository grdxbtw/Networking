#pragma once
#ifdef _DEBUG
#include <iostream>
#define PRINT(MSG) std::cout << MSG << std::endl;    
#else
#define PRINT(MSG) 
#endif // _DEBUG

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
        uint32_t text_length_;  
        std::unique_ptr<char[]> buffer_; 


    public:
        using session_pointer = std::shared_ptr<Session>;

        static session_pointer create(boost::asio::io_context& ctx) 
        {
            return session_pointer(new Session(ctx)); 
        }

        tcp::socket& get_socket()
        {
            return socket_;
        }
        void start();

    private:
        Session(boost::asio::io_context& ctx) : socket_(ctx) {} 

        void DoRead();
        void DoReadText();

        void DoWrite(const std::string& message);
        void HandleMessage();
    };


    class ServerAsio
    {
    private:
        boost::asio::ip::tcp::acceptor acceptor_;
        boost::asio::io_context& ctx;
         
    public:
        ServerAsio(boost::asio::io_context& ioContext, uint16_t port) 
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
                        PRINT(socket.local_endpoint().port()); 
                        auto newConnection = Session::create(ctx);  
                        newConnection->get_socket() = std::move(socket); 
                        newConnection->start(); 
                    }

                    DoAccept();
                });
        }

	};
}

