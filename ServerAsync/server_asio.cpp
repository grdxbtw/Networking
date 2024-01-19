#include "server_asio.h"
#include "../../cpp_projects/newCalculator/newCalculator/Kalkulator.h"

#ifdef _DEBUG
#define PRINT(MSG) std::cout << MSG << std::endl;  
#else
#define PRINT(MSG) 
#endif // _DEBUG

namespace Net_asio
{
	void Session::start() 
	{
		DoRead();
	}

	void Session::DoRead()
	{	
		auto self = shared_from_this();
		
		socket_.async_read_some(boost::asio::buffer(data_), 
			[self](boost::system::error_code ec, std::size_t length) 
			{
				//if(!ec) 
					self->HandleMessage(length,ec); 
				
			});

	}

	void Session::DoWrite(const std::string& message) 
	{
		auto self = shared_from_this();
		boost::asio::async_write(socket_, shared_const_buffer(message), 
			[self](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec) {
					self->DoRead();
				}
			});
	}

	void Session::HandleMessage(std::size_t length,boost::system::error_code &ec)
	{
		
		Kalkulator<double> k;
		//std::string receivedMessage(data_);
		std::cout << "Received message: " << length << std::endl;
		//k.Parsing(receivedMessage);

		//DoWrite(receivedMessage);  
	}

}
