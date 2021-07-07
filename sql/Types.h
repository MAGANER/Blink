//Data types can be stored in Sqlite3 data base
#pragma once
#include<string>
#include"Functools.hpp"
namespace SQLite3DataBaseTools
{
	using namespace std;
	enum class SQL_TYPES
	{
		INTEGER,
		PRIMARY_KEY,
		REAL,
		BOOLEAN,
		TEXT
	};
	struct SQLtype
	{
		SQL_TYPES type;
	};
	struct PrimaryKey:SQLtype 
	{ 
		int key; 
		PrimaryKey(int val)
		{
			type = SQL_TYPES::PRIMARY_KEY;
			key = val;
		}
	};
	struct Integer:SQLtype 
	{ 
		int value;
		Integer(int val)
		{
			type = SQL_TYPES::INTEGER;
			value = val;
		}
	};
	struct Real:SQLtype 
	{ 
		float value;
		Real(float val)
		{
			type = SQL_TYPES::REAL;
			value = val;
		}
	};
	struct Boolean:SQLtype 
	{ 
		bool value;
		Boolean(bool val)
		{
			type = SQL_TYPES::BOOLEAN;
			value = val;
		}
	};
	struct Text:SQLtype 
	{ 
		string value;
		Text(const string& val)
		{
			type = SQL_TYPES::TEXT;
			value = val;
		}
	};


	static bool is_integer(const string& value)
	{
		function<bool(char)> is_digit = [&](char c) { return isdigit(c); };
		auto check = Functools::filter(value, is_digit);
		return check.size() == value.size();
	}
	static bool is_float(const string& value)
	{
		auto point_pos = value.find('.');
		if(point_pos != string::npos)
			if (point_pos > 0 && point_pos < value.size())
			{
				auto left  = Functools::slice(value, 0, point_pos - 1);
				auto right = Functools::slice(value, point_pos + 1, value.size() - 1);
				return is_integer(left) && is_integer(right);
			}

		return false;
	}
	static bool is_boolean(const string& value)
	{
		return value == "true" || value == "false";
	}

	static SQL_TYPES get_string_value_real_type(const string& value)
	{
		if (is_integer(value))      return SQL_TYPES::INTEGER;
		else if (is_float(value))   return SQL_TYPES::REAL;
		else if (is_boolean(value)) return SQL_TYPES::BOOLEAN;
		else                        return SQL_TYPES::TEXT;
	}

	static string type_to_string(const SQLtype* type)
	{
		auto* ptr_copy = (SQLtype *)type;
		switch (ptr_copy->type)
		{
		case SQL_TYPES::PRIMARY_KEY:
		{
			auto val = static_cast<PrimaryKey*>(ptr_copy)->key;
			return to_string(val);
		}
		break;
		case SQL_TYPES::INTEGER:
		{
			auto val = static_cast<Integer*>(ptr_copy)->value;
			return to_string(val);
		}
		break;
		case SQL_TYPES::REAL:
		{
			auto val = static_cast<Real*>(ptr_copy)->value;
			return to_string(val);
		}
		break;
		case SQL_TYPES::BOOLEAN:
		{
			auto val = static_cast<Boolean*>(ptr_copy)->value;
			return to_string(val);
		}
		break;
		case SQL_TYPES::TEXT:
			return static_cast<Text*>(ptr_copy)->value;
		break;
		}
	}
};