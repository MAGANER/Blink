#pragma once
#include<limits.h>
#include<SFML/Network.hpp>
namespace Blink
{
	inline bool can_accept_new_connection(int id)
	{
		return (id + 1) < INT_MAX;
	}

	struct RoomClient
	{
		int id;
		sf::TcpSocket* socket;
		int listner_port;
		RoomClient()
		{
			listner_port = -1;
			id = 0;
			socket = nullptr;
		}
		~RoomClient()
		{
			delete socket;
		}
	};
};