#pragma once
#include"Server.h"
#include"ConnectionChecker.hpp"
namespace Blink
{
class DecentralysedServerClient:public Server
{
private:
	bool _cant_connect = false;
	string conn_port, conn_ip;
	bool connecting = false;


	TcpSocket* socket = nullptr;
public:
	DecentralysedServerClient(command_hash& commands,
							  const string& password,
							  const string& room_name,
							  const string& user_name,
							  const string& db_key,
							  const string& db_name,
							  bool inherited = false);
	~DecentralysedServerClient();


	bool _run();
	bool cant_connect() { return _cant_connect; };

	void set_ip_and_port_to_connect(const string& ip, const string& port);
	void is_connecting(bool flag);
	void set_key_iv(const encr::AES::key_iv& key_iv);
private:
	void make_client(list<RoomClient*>& clients, 
					 int& client_counter,
					 TcpSocket* socket,
					 int listner_port);
	void send_clients_info(list<RoomClient*>& clients,
						   TcpSocket* socket);
};
};

