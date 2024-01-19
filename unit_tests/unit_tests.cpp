#define BOOST_TEST_MODULE Net_Calculator 
 
#include <boost/test/unit_test.hpp> 
#include <boost/test/data/test_case.hpp> 
#include <boost/test/data/monomorphic.hpp> 

#include "../ServerSelect/Server.h"
#include "../Client/Client_class.h"


BOOST_AUTO_TEST_SUITE(server_test)

BOOST_AUTO_TEST_CASE(first)
{
	NetServer::Server s("127.0.0.1", 27015);
	BOOST_TEST(s.start()); 
	Net::Client c1("127.0.0.1", 27015);
	Net::Client c2("127.0.0.1", 27015);
	Net::Client c3("127.0.0.1", 27015);
	BOOST_TEST(c1.connect_to_server());
	BOOST_TEST(c2.connect_to_server()); 
	BOOST_TEST(c3.connect_to_server());

	std::this_thread::sleep_for(std::chrono::seconds(5));  
}

BOOST_AUTO_TEST_CASE(second)
{
	NetServer::Server s("127.0.0.1", 27015); 
	BOOST_TEST(s.start()); 
	Net::Client c1("127.0.0.1", 27015); 
	BOOST_TEST(c1.connect_to_server()); 
	BOOST_TEST(c1.send_str("1+3")); 

	std::string res; 
	BOOST_TEST(c1.recv_str(res)); 
	std::this_thread::sleep_for(std::chrono::seconds(5));
}

BOOST_AUTO_TEST_SUITE_END()