#include "GraphicalDecentralysedServerClient.h"
using namespace Blink;

void GraphicalDecentralysedServerClient::run_in_window()
{
	if (should_send)
	{
		send_text_to_all(text_to_send);
		text_to_send.clear();
		should_send = false;
	}
	bool should_resend_clients_info = true;
	if (listener.accept(*entering_socket) == sf::Socket::Done)
	{
		auto check_addres = [&](vector<IpAddress>& addresses)
		{
			return is_addres_allowed(addresses, entering_socket->getRemoteAddress());
		};

		if (check_addres(allowed) &&
			can_accept_new_connection(client_counter))
		{
			//send password because it was hashed
			Packet pack;
			pack << password;
			entering_socket->send(pack);

			string some_shit = get_raw_message(*entering_socket);
			if (some_shit == "motherfucker") should_resend_clients_info = false;

			//don't resend if you connect to user
			//who already was connected to another one
			if (should_resend_clients_info)
				send_clients_info(clients, entering_socket);


			//send password to provide user an ability to come in again later
			send_auto_message(*entering_socket, "room password:" + password + "\n don't forget it!");

			//save address, because client can log out and came back
			//so save it, because ip is added to DB, but not to allowed vector
			allowed.push_back(entering_socket->getRemoteAddress());
			make_client(clients, client_counter, entering_socket, PORT);
			entering_socket = new TcpSocket();
		}
		else
		{
			check_access(*entering_socket, allowed);
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
		save_offline_clients();
		listener.close();
		socket->disconnect();
		
		return;
	}
}
void GraphicalDecentralysedServerClient::save_offline_clients()
{
	for (auto& client : offline_clients)
	{
		add_offline_client(room_name, client);
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