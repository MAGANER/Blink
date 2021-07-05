#ifndef SOCKET_RUNNER_H
#define SOCKET_RUNNER_H
#include"masesk/EasySocket.hpp"
#include<functional>
namespace Blink
{
	using namespace std;
	using namespace masesk;
namespace
{
	void run_socket(function<void()> fn)
	{
		try
		{
			fn();
		}
		catch (socket_error_exception& e)
		{
			cout << "ooops! " << e.what() << endl;
			return;
		}
	}
};
};
#endif SOCKET_RUNNER_H