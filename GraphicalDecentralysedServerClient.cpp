#include "GraphicalDecentralysedServerClient.h"
using namespace Blink;

void GraphicalDecentralysedServerClient::run_in_window()
{
	//receive_input_and_send_message_to_all(clients);
	if (should_send)
	{
		send_text_to_all(text_to_send);
		text_to_send.clear();
		should_send = false;
	}

	bool should_resend_clients_info = true;
	if (listener.accept(*entering_socket) == sf::Socket::Done)
	{
		auto check1 = [&](vector<IpAddress>& addresses)
		{
			return is_addres_allowed(addresses, entering_socket->getRemoteAddress());
		};
		auto check2 = [&](vector<int>& ports)
		{
			return is_port_allowed(ports, entering_socket->getLocalPort());
		};

		if (check1(allowed) &&
			check2(allowed_ports) &&
			true &&
			can_accept_new_connection(client_counter))
		{
			// send room name, because room name from file link is hashed
			auto data = room_name + "+" + password;
			send_message(*entering_socket, data, MessageType::RoomName);

			//retrieve port, cos it's needed to create clients' info
			int listner_port = -1;
			string some_shit = get_raw_message(*entering_socket);
			if (some_shit == "motherfucker") should_resend_clients_info = false;
			else listner_port = atoi(some_shit.c_str());

			//don't resend if you connect to user
			//who already was connected to another one
			if (should_resend_clients_info)
				send_clients_info(clients, entering_socket);

			make_client(clients, client_counter, entering_socket, listner_port);


			//clear it, because there is no need to keep this data anymore
			for (size_t i = 0; i < allowed.size(); i++)
			{
				auto curr = allowed[i];
				if (curr.toString() == entering_socket->getRemoteAddress().toString())
				{
					allowed.erase(allowed.begin() + i);
				}
			}
			entering_socket = new TcpSocket();
		}
		else
		{
			check_access(*entering_socket, allowed, allowed_ports);
		}
	}

	//receive message, show it and resend
	for (auto& client : clients)
		NetBase::get_and_save_message(*client->socket, received_messages);

	//info about another clients
	process_received_clients_info();

	if (timer.getElapsedTime().asSeconds() > 10.0f)
	{
		check_offline_clients();
		timer.restart();
	}

	//update_clients(clients);
	if (should_disconnect())
	{
		//save offline clients
		//because if you don't do that you lost all data  about potential
		//clients
		cout << offline_clients.size() << "is offline clients!" << endl;
		for (auto& client : offline_clients)
		{
			add_offline_client(room_name, client.first.toString(), client.second);
		}

		return;
	}
}
void GraphicalDecentralysedServerClient::send_text_to_all(const string& text)
{
	for (auto& client : clients)
		send_message(*client->socket, text);
}
bool GraphicalDecentralysedServerClient::has_message_to_show()
{
	return !received_messages.empty();
}
message_store* GraphicalDecentralysedServerClient::get_message_to_show()
{
	auto result = received_messages.front();
	received_messages.pop_front();
	return result;
}