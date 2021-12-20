#ifndef GRAPHICAL_DECENTRALYSED_SERVER_CLIENT_H
#define GRAPHICAL_DECENTRALYSED_SERVER_CLIENT_H
#include"DecentralysedServerClient.h"
//adopted version for graphical mode
namespace Blink
{
class GraphicalDecentralysedServerClient:public DecentralysedServerClient
{
private:
	bool should_send = false;
	string text_to_send;

	list<message_store*> received_messages;
public:

	GraphicalDecentralysedServerClient(command_hash& commands,
		const string& password,
		const NetBaseData& data,
		bool connecting_with_conflink_command,
		bool starting_room,
		bool inherited=false):
		DecentralysedServerClient(commands, 
								  password,
								  data,
			connecting_with_conflink_command,
			starting_room,
			inherited)
	{

	}
	GraphicalDecentralysedServerClient(command_hash& commands,
		const string& password,
		const NetBaseData& data,
		const string& link_creator_data,
		bool starting_room,
		bool save_link,
		const string& recepient_name,
		bool inherited=false):
		DecentralysedServerClient(commands, 
								  password,
								  data,
			link_creator_data,
			starting_room,
			recepient_name,
			save_link,
			inherited)
	{

	}
	~GraphicalDecentralysedServerClient()
	{

	}

	void run_in_window();
	void set_text_to_send(const string& text)
	{
		text_to_send = text;
		should_send = true;
	}

	bool has_message_to_show();
	message_store* get_message_to_show();
	void exit() { disconnect = true; }

	LinkData* get_link_data()
	{
		LinkData* data = new LinkData(PORT, password, room_name);
		return data;
	}

	void save_offline_clients();

	string get_room_name() { return room_name; }
private:
	void send_text_to_all(const string& text);
};
};
#endif
