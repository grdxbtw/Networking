#include "server_asio.h"
#include "../../cpp_projects/newCalculator/newCalculator/Kalkulator.h"

namespace Net_asio
{
	void Session::start() 
	{
		DoRead();
	}

	void Session::DoRead()
	{	
		auto self = shared_from_this();	
		boost::asio::async_read(  
			socket_,  
			boost::asio::buffer(&text_length_, sizeof(text_length_)), 
			[this, self](boost::system::error_code ec, std::size_t /*length*/) { 
				if (!ec) {
					text_length_ = ntohl(text_length_);  
					DoReadText(); 
				} 
			});

	}

	void Session::DoReadText()
	{
		auto self(shared_from_this());
		buffer_ = std::make_unique<char[]>(text_length_);
		boost::asio::async_read(
			socket_,
			boost::asio::buffer(buffer_.get(), text_length_),
			[this, self](boost::system::error_code ec, std::size_t /*length*/) {
				if (!ec) {
					std::string text(buffer_.get(), text_length_);
					PRINT("Received: " << text);
					self->HandleMessage();
				}
			});
	}

	void Session::DoWrite(const std::string& message) 
	{
		auto self = shared_from_this();
		boost::asio::async_write(socket_, boost::asio::buffer(message.c_str(),message.size()),  
			[self](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec) {
					self->DoRead();
				}
			});
	}

	void Session::HandleMessage()
	{
		
		Kalkulator<double> k;
		std::string receivedMessage(buffer_.get(),text_length_); 
		k.Parsing(receivedMessage);
		uint32_t text_length = receivedMessage.size(); 
		text_length = htonl(text_length); 
		auto self = shared_from_this(); 
		boost::asio::async_write(socket_, boost::asio::buffer(&text_length, sizeof(text_length)),
			[self, receivedMessage](const boost::system::error_code& er, std::size_t)
			{
				if (!er)
					self->DoWrite(receivedMessage);
			});

	}

}
