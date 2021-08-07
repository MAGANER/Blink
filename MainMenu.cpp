#include "MainMenu.h"
using namespace Blink;


MainMenu::MainMenu(const string& encr_key):DataBaseProcessor(encr_key)
{
	commands["exit"]    = function<void()>([&]() { _exit = true; });
	commands["create"]  = function<void()>([&]() { create();     });
	commands["enter"]   = function<void()>([&]() { enter();      });
	commands["connect"] = function<void()>([&]() { connect();   });
	commands["conflink"] = function<void()>([&]() { connect_with_filelink();   });
	//commands["conflink"] = function<void()>([&]() { connect_with_link();   });
}
void MainMenu::create()
{
	string name, password, port;
	cout << "enter room name:";      cin >> name;
	cout << "enter room password:"; cin >> password;
	cout << "enter room port:";      cin >> port;
	if (!sql::is_integer(port))
	{
		cout << "can not create room! incorrect port >>" << port << endl;
		return;
	}
	if (!does_room_exists(name))create_new_room(name, password, port);
	else cout << "room" << " `" + name + "`" << " already exists!" << endl;
}
void MainMenu::enter()
{
	string name, password;
	cout << "enter room name:";      cin >> name;
	cout << "enter room password:"; 
	password = get_secret_data();

	if (!is_password_correct(name,password)) cout << "can not enter to " << "`" + name + "`" << " room";
	else
	{
		data.room     = name;
		data.password = password;
		data.port = to_string(get_room_port(name, get_encr_key()));
		can_enter_room = true;
		system("cls");
		cout << "welcome to " << name << " room!" << endl;
	}
}
bool MainMenu::can_connect()
{
	//connect to server
	TcpSocket socket;
	if (socket.connect(IpAddress(data.ip), atoi(data.port.c_str())) != TcpSocket::Done)
	{
		cout << "can not connect to " << data.ip << "!" << endl;
		return false;
	}
	
	//send data to get right to be connected
	string message = convert_message_to_json(data.password, 
											 data.room,
											 MessageType::ComeInRequest);
	Packet p; p << message;
	if (socket.send(p) != TcpSocket::Done)
	{
		cout << "can not send!" << endl;
		return false;
	}
	p.clear();
	
	//get the answer
	socket.receive(p);
	string received_result;
	p >> received_result;
	bool ability = false;
	if (received_result == "1") ability =  true;
	
	if (!ability)cout << "can not come in!" << endl;
	return ability;
}
void MainMenu::connect()
{
	cout << "enter ip:";
	cin >> data.ip;
	cout << "enter port:";
	cin >> data.port;
	cout << "enter room name:";
	cin >> data.room;
	cout << "enter password:";
	data.password = get_secret_data();

	if (can_connect())
	{
		cout << "connecting to " << data.room << endl;
		can_connect_to_room = true;
		con_regime = CONNECTION_REGIME::Manually;
	}
	else data.clear();
}
void MainMenu::connect_with_filelink()
{
	string path;
	ifstream link_file;
	while (true)
	{
		cout << "enter path to link:";
		cin >> path;
		link_file.open(path, ios::binary);
		if (!link_file)
		{
			cout << "can not open " << path << "!" << endl;
			path.clear();
		}
		else break;
	}

	
	string link;
	istreambuf_iterator<char> inputIt(link_file), emptyInputIt;
	back_insert_iterator<string> stringInsert(link);

	copy(inputIt, emptyInputIt, stringInsert);


	link = decrypt_invite_link(link);

	json jlink = json::parse(link);
	data.ip   = jlink["ip"];
	data.port = jlink["port"];
	data.room = jlink["room"];
	data.password = jlink["password"];
	
	encr_data = new EncryptionData(jlink["key"],jlink["iv"]);
	
	auto curr_time = parse_time(get_curr_time() + " d");
	int curr_seconds = get_seconds(curr_time[3]);
	
	string _time = jlink["time"];
	string time = Functools::slice(_time, 0, _time.size() - 1);
	auto creation_time = parse_time(time + " d");
	int creation_seconds = get_seconds(creation_time[3]);
	
	int eq_counter = 0;
	for (size_t i = 0; i < curr_time.size(); i++)
	{
		if (i != 3)
		{
			if (curr_time[i] == creation_time[i])eq_counter++;
		}
	}
	bool date_is_correct = eq_counter == 4;
	bool time_diff_is_correct = abs(curr_seconds - creation_seconds) < (5 * 60);
	if (date_is_correct && time_diff_is_correct)
	{
		if (can_connect())
		{
			can_connect_to_room = true;
			con_regime = CONNECTION_REGIME::WithLink;
		}else cout << "link is denyied!" << endl;
	}
	else
	{
		cout << "link is denyied!" << endl;
	}
	
}
vector<string> MainMenu::split(const string& s, char delim)
{
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) 
	{
		elems.push_back(item);
	}

	function<bool(string)> should_copy = [&](const string& arg)
	{
		if (arg.size() == 0)return false;
		return true;
	};
	function<bool(char)> filter_elem_pd=  [&](char ch) { return isprint(ch); };
	function<string(string)> filter_str = [&](const string& str)
	{
		return Functools::filter(str, filter_elem_pd);
	};
	return Functools::map(Functools::filter(elems, should_copy),filter_str);
}
string MainMenu::convert_time_string_to_seconds(const string& str)
{
	int h, m, s = 0;
	if (sscanf(str.c_str(), "%d:%d:%d", &h, &m, &s) >= 2)
	{
		int secs = h * 3600 + m * 60 + s;
		return to_string(secs);
	}
	return "";
}
string MainMenu::convert_secs_to_regular_time_view(int seconds)
{
	auto min = (int)seconds / 60;
	auto hour = (int)min / 60;
	auto secs = (int)seconds % 60;
	return to_string(hour) + ":" + to_string(min) + ":" + to_string(secs);
}
vector<string> MainMenu::parse_time(const string& time)
{
	auto curr_time = split(time, ' ');
	curr_time = Functools::slice(curr_time, 0, 5);
	return curr_time;
}
int MainMenu::get_seconds(const string& time)
{
	string seconds_str = convert_time_string_to_seconds(time);
	if (seconds_str.empty())
	{
		cout << "can not read link!";
		exit();
	}
	return atoi(seconds_str.c_str());
}