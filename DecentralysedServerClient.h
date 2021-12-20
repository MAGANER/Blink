#pragma once
#include"Server.h"
#include"ConnectionChecker.hpp"
#include<random>
#include<set>
#include"SFML/System/Time.hpp"
#include"SFML/System/Clock.hpp"
namespace Blink
{

	struct LinkData
	{
		LinkData(int port, const string& password, const string& room_name)
		{
			this->port = port;
			this->password = password;
			this->room_name = room_name;
		}
		~LinkData() {}
		int port;
		string password, room_name;
	};

class DecentralysedServerClient:public Server
{
protected:
	bool _cant_connect = false;
	string conn_port, conn_ip;
	bool connecting = false;
	bool connect_to_saved_clients = false;
	bool starting_room = false;

	TcpSocket* socket = nullptr;

	list<RoomClient*> clients;

	typedef pair<IpAddress, int> ip_port;
	vector<ip_port> offline_clients;

	vector<IpAddress> allowed;

	int client_counter = 0;

	Clock timer;
	TcpSocket* entering_socket = new TcpSocket;
public:
	DecentralysedServerClient(command_hash& commands,
							  const string& password,
							  const NetBaseData& data,
							  bool connecting_with_conflink_command,
							  bool starting_room,
							  bool inherited = false);
	
	//special constructor used in graphical mode
	DecentralysedServerClient(command_hash& commands,
							  const string& password,
						      const NetBaseData& data,
							  const string& link_creator_data,
							  bool starting_room,
							  const string& recepient_name,
							  bool save_link,
							  bool inherited = false);
	virtual ~DecentralysedServerClient();

	void prepare();
	bool run_in_console();
	bool cant_connect() { return _cant_connect; };

	void set_ip_and_port_to_connect(const string& ip, const string& port);
	void is_connecting(bool flag);
	void set_key_iv(const encr::AES::key_iv& key_iv);
protected:
	void connnect_finally();
	void process_received_clients_info();

	void make_client(list<RoomClient*>& clients, 
					 int& client_counter,
					 TcpSocket* socket,
					 int listner_port);
	void send_clients_info(list<RoomClient*>& clients,
						   TcpSocket* socket);

	void connect_to_known_clients();

	void check_offline_clients();
private:
	void load_allowed_long_known_clients();
};
};

