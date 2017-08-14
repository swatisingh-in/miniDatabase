#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <sqlite3.h>
#include <map>
#include <boost/algorithm/string.hpp>
#include <ctime>

using namespace std;

int callback(void *fptr, int argc, char **argv, char **azColName);

class FileInterface
{
public:
	static ifstream inFile;
	static ofstream outFile;
	static vector<string> columns;
	static vector<string> columnType;
	static string createQuery;
	static string insertQuery;
	static map <string,string> columnMap;
public:
	FileInterface();
	FileInterface(const char *input, const char *output);
	void getColumnNames();
    void getColumnType();
    void formQueryString();
    string typeConversion(string &type);
    ~FileInterface();
    
private:
	void createMap(string &column, string &type);
	
};

class DataBaseInterface
{
	static const string dbName;
	sqlite3 *db;
public:
	DataBaseInterface();
	void createTable();
	void insertRows();
	bool validateQuery(string &aggregation_attr);
	void queryDatabase(string &query, string &aggregation_attr, string &grouping_attr);
	~DataBaseInterface();
private:
	void deleteDataBaseIfExisting();
};

#endif

