#include "DataBase.h"
using namespace SQLite3DataBaseTools;

DataBase::DataBase(const string& path, const string& key,bool first_time)
{
	ok = sqlite3_open(path.c_str(), &db);
	const void* _key = key.c_str();

	if (first_time)
		sqlite3_rekey(db, _key, sizeof(_key));
	if (!first_time)
		if (sqlite3_key(db, _key, sizeof(_key)) != SQLITE_OK)
		{
			correct_password = false;
		}
		else correct_password = true;
	if (!ok) error_message = sqlite3_errmsg(db);
}
DataBase::~DataBase()
{
	sqlite3_close(db);
}

bool DataBase::is_ok()
{
	return ok  == SQLITE_OK;
}
string DataBase::get_error_message()
{
	return error_message;
}
bool DataBase::run_set_request(const string& request)
{
	//run create/insert request

	char* error_message = nullptr;
	int result = sqlite3_exec(db, request.c_str(), NULL, NULL, &error_message);
	
	if (error_message != nullptr)
		this->error_message = error_message;
	
	return result == SQLITE_OK;
}

vector<chunk> DataBase::run_get_request(const string& request)
{
	vector<chunk>* data = new vector<chunk>();
	char* error_message = nullptr;
	int ok = sqlite3_exec(db, request.c_str(), DataBase::get_request_callback, (void*)data, &error_message);

	if (error_message != nullptr)
		this->error_message = error_message;

	return *data;
}

int DataBase::get_request_callback(void* data, int argc, char** argv, char** azColName)
{
	map<string, SQLtype*> extracted_data;

	for (int i = 0; i < argc; ++i)
	{
		function<char(char)> to_lower = tolower;
		string name = Functools::map(string(azColName[i]), to_lower);
		
		if (name == "id")
		{
			int id_val = argv[i] ? atoi(argv[i]) : 0;
			extracted_data[string(azColName[i])] = new PrimaryKey(id_val);
		}
		else
		{
			string val = argv[i] ? argv[i] : "NULL";
			if (val != "NULL")
			{
				SQL_TYPES data_type = get_string_value_real_type(val);
				switch (data_type)
				{
				case SQL_TYPES::INTEGER:
					extracted_data[azColName[i]] = new Integer(atoi(argv[i]));
				break;
				case SQL_TYPES::REAL:
					extracted_data[azColName[i]] = new Real(atof(argv[i]));
				break;
				case SQL_TYPES::BOOLEAN:
				{
					bool val = string(argv[i]) == "true" ? true : false;
					extracted_data[azColName[i]] = new Boolean(val);
				}
				break;
				case SQL_TYPES::TEXT:
					extracted_data[azColName[i]] = new Text(string(argv[i]));
				break;
				}
			}
			else
			{
				//if there is nothing just skip it
			}
		}
		
	}
	static_cast<vector<chunk>*>(data)->push_back(extracted_data);
	
	return 0;
}
vector<chunk> DataBase::run_get_request(const string& request, function<bool(string, SQLtype*)> predicat)
{
	/*
		this function gets request string and predicat.
		predicat takes name of field and it's value.
		so request processes every data matching to 
		predicat.
	*/
	
	vector<chunk>* data = new vector<chunk>();
	char* error_message = nullptr;
	int ok = sqlite3_exec(db, request.c_str(), DataBase::get_request_callback, (void*)data, &error_message);

	if (error_message != nullptr)
		this->error_message = error_message;

	vector<chunk>* result = new vector<chunk>();


	for (auto _chunk : *data)
	{
		chunk curr;
		for (auto pair : _chunk)
		{
			bool pred_result = predicat(pair.first, pair.second);
			if (pred_result)
			{
				curr[pair.first] = pair.second;
			}
		}
		result->push_back(curr);
	}

	return *result;
}


string SQLite3DataBaseTools::make_create_request(const map<string, SQLtype*>& data, const string& table_name)
{
	/*
		generates request to create table with name passed as second argument,
		first element of hash table is field name and second one is is value,
		anyway it's omitted, but it's used to set the correct data of field.
	*/

	string request = "CREATE TABLE "+table_name+"( ";

	size_t counter = 0;
	for (auto row : data)
	{
		switch (row.second->type)
		{
		case SQL_TYPES::PRIMARY_KEY: request += row.first + " INT PRIMARY KEY NOT NULL";
		break;
		case SQL_TYPES::INTEGER: request += row.first + " INT NOT NULL"; break;
		case SQL_TYPES::REAL:    request += row.first + " REAL";    break;
		case SQL_TYPES::TEXT:    request += row.first + " TEXT";    break;
		case SQL_TYPES::BOOLEAN: request += row.first + " BOOLEAN"; break;
		}

		if (counter + 1 != data.size())request += ", ";
		counter++;
	}
	request += ")";
	return request;
}
string SQLite3DataBaseTools::make_insert_request(const map<string, SQLtype*>& data, const string& table_name)
{
	/*
		generates request for inserting new values of table, passed as second argument.
		First element of hash table is name of field, and second one is new value.
	*/

	string request = "INSERT INTO " + table_name + " (";

	size_t counter = 0;
	for (auto row : data)
	{
		request += row.first;
		if(counter + 1 != data.size()) request += ",";
		++counter;
	}
	request += ") VALUES (";

	counter = 0;
	for (auto row : data)
	{
		SQLtype* type_ptr = row.second;
		
		switch (row.second->type)
		{
		case SQL_TYPES::PRIMARY_KEY: request += to_string(static_cast<PrimaryKey*>(type_ptr)->key);
		break;
		case SQL_TYPES::INTEGER: request += to_string(static_cast<Integer*>(type_ptr)->value); break;
		case SQL_TYPES::REAL:    request += to_string(static_cast<Real*>(type_ptr)->value);    break;
		case SQL_TYPES::TEXT:    request += "\""+static_cast<Text*>(type_ptr)->value+ "\"";               break;
		case SQL_TYPES::BOOLEAN: request += to_string(static_cast<Boolean*>(type_ptr)->value); break;
		}

		if (counter + 1 != data.size()) request += ",";
		counter++;
	}
	request += ")";

	return request;
}
string SQLite3DataBaseTools::make_select_request(const string& table_name)
{
	/*
		generate request, selecting exactly everything from table
	*/

	return "SELECT * FROM " + table_name;
}
string SQLite3DataBaseTools::make_select_request(const string& table_name,
												 const vector<string>& fields)
{
	/*
		generates request to select all fields, passed as second argument
		from table passed as first argument.
	*/

	string request = "SELECT ";

	auto counter = 0;
	for (auto& f : fields)
	{
		request += f+" ";
		if (counter + 1 != fields.size()) request += ",";
		++counter;
	}
	request += "FROM " + table_name;
	return request;
}
string SQLite3DataBaseTools::make_update_request(const map<string, SQLtype*>& fields_to_update,
												 const string& table_name)
{
	/*
		This function creates request to update table with passed name.
		First argument represents hash table of field name and its new value.
		It's only creates a request, if you need to change special field with
		special value run DataBase method to `run_get_request` with specific predicat.
		Yes, you won't get nothing, so check errors with `get_error_message` method.
	*/

	string begin_request = "UPDATE "+table_name +" ";
	vector<string> requests;
	for (auto field : fields_to_update)
	{
		string request = begin_request + " SET " + field.first+" = " + type_to_string(field.second) + ";";
		requests.push_back(request);
	}

	function<string(string a,string b)> add =
		[&](string a, string b)
	{
		return a + b;
	};
	return Functools::reduce(requests, add);
}
string SQLite3DataBaseTools::make_delete_request(const string& table_name,
												 const map<string, SQLtype*>& fields_to_update)
{
	/*
		deletes all `fields_to_update` from `table_name`
		first element of fields map is name of field,
		and the second one is value that should be erased.
	*/

	string common = "DELETE from " + table_name + " WHERE ";
	string request;
	for (auto pair : fields_to_update)
	{
		request += common + pair.first + " = " + type_to_string(pair.second)+";";
	}

	return request;
}