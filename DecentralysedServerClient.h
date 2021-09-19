#pragma once
#include"Server.h"
#include"ConnectionChecker.hpp"
#include<random>
#include<set>
namespace Blink
{
class DecentralysedServerClient:public Server
{
private:
	bool _cant_connect = false;
	string conn_port, conn_ip;
	bool connecting = false;
	bool connect_to_saved_clients = false;

	TcpSocket* socket = nullptr;

	list<RoomClient*> clients;

	//TODO::unite these vectors into vector of pairs
	vector<IpAddress> allowed;
	vector<int> allowed_ports;


	//they already were connected
	typedef pair<IpAddress, int> ip_port;
	

	int client_counter = 0;
public:
	DecentralysedServerClient(command_hash& commands,
							  const string& password,
							  const string& room_name,
							  const string& user_name,
							  const string& db_key,
							  const string& db_name,
							  bool connecting_with_conflink_command,
							  bool inherited = false);
	~DecentralysedServerClient();


	bool _run();
	bool cant_connect() { return _cant_connect; };

	void set_ip_and_port_to_connect(const string& ip, const string& port);
	void is_connecting(bool flag);
	void set_key_iv(const encr::AES::key_iv& key_iv);
private:
	void connnect_finally();
	void process_received_clients_info();

	void make_client(list<RoomClient*>& clients, 
					 int& client_counter,
					 TcpSocket* socket,
					 int listner_port);
	void send_clients_info(list<RoomClient*>& clients,
						   TcpSocket* socket);

	int get_random_port();
	void connect_to_known_clients();

	bool is_port_allowed(vector<int>& ports, int port);
};
};

