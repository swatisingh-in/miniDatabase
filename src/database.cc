#include "database.h"

const string DataBaseInterface::dbName = "mini_database.db";
string FileInterface::createQuery = "CREATE TABLE RECORDS(";
string FileInterface::insertQuery = "INSERT INTO RECORDS VALUES (";
ifstream FileInterface::inFile;
ofstream FileInterface::outFile;
vector<string> FileInterface::columns;
vector<string> FileInterface::columnType;
map <string,string> FileInterface::columnMap;

/**
    FileInterface default constructor
    @param: NULL
    @return: NULL
*/
FileInterface::FileInterface()
{

}

/**
    FileInterface parameterized constructor
    @param: input file and output file
    @return: NULL
*/

FileInterface::FileInterface(const char *input, const char *output)
{
    inFile.open(input);
    outFile.open(output, ios::out | ios::app);
    getColumnNames();
    getColumnType();
    formQueryString();
}

/**
    FileInterface destructor
    @param: NULL
    @return: NULL
*/

FileInterface::~FileInterface()
{
}

/**
    Get column names from first line of the file
    @param: NULL
    @return: NULL
*/

void FileInterface::getColumnNames()
{
	string line;
	if (inFile.is_open())
  	{
    	if (getline(inFile,line)) {
      		boost::split(columns, line, boost::is_any_of(" "));
    	}
      	else
      		throw runtime_error("File read error");
    }
    else 
    	throw runtime_error("File open error");
}

/**
    Get datatype for each column from second line of the file
    @param: NULL
    @return: NULL
*/

void FileInterface::getColumnType()
{
	string line;
	if (inFile.is_open())
  	{
    	if (getline (inFile,line)) {
      		boost::split(columnType, line, boost::is_any_of(" "));
    	}
      	else
      		throw runtime_error("File read error");
    }
    else
    	throw runtime_error("File open error");
}

/**
	convert datatype obtained from file to sqlite3 compatible datatype
    @param: string representing datatype
    @return: corresponding sqlite3 datatype
*/

string FileInterface::typeConversion(string &type)
{
	string str;

	boost::algorithm::to_lower(type);
	if (!type.compare("integer"))
		str = "INTEGER";
	else if (!type.compare("string"))
		str = "TEXT";
	else {
		string errMsg = "Invalid datatype";
   		throw runtime_error(errMsg);
	}
	return str;
}

/**
	create map of column name and column type
    @param: column name and column type
    @return: NULL
*/

void FileInterface::createMap(string &column, string &type)
{
	columnMap.insert(pair<string,string>(column, type));
}

/**
	Form query string for create and insert query
    @param: NULL
    @return: NULL
*/

void FileInterface::formQueryString()
{
	string create = "";
	string insert = "";

 	for(int i = 0; i < columns.size(); i++){
 		columnType[i] = typeConversion(columnType[i]);
 		createMap(columns[i], columnType[i]);
 		create = create + columns[i] + " " + columnType[i] + " " + "NOT NULL" + ","; 
 		insert = insert + "?" + to_string(i+1) + ",";
 	}

 	create = create.substr (0,create.length()-1);
 	insert = insert.substr (0,insert.length()-1);

 	createQuery = createQuery + create + ");";
 	insertQuery = insertQuery + insert + ")";

}

/**
	DataBaseInterface default constructor
    @param: NULL
    @return: NULL
*/

DataBaseInterface::DataBaseInterface()
{
	deleteDataBaseIfExisting();
	createTable();
} 

/**
	DataBaseInterface destructor
    @param: NULL
    @return: NULL
*/ 

DataBaseInterface::~DataBaseInterface()
{
	sqlite3_close(db);
}

/**
    Deletes the database if existing
    @param: NULL
    @return: NULL
*/

void DataBaseInterface::deleteDataBaseIfExisting()
{
	string command = "rm -rf " + dbName;
	system(command.c_str());
}

/**
    Create table in the database
    @param: NULL
    @return: NULL
*/

void DataBaseInterface::createTable()
{  
	FileInterface obj;

   	if( sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) 
   	{
   		string errMsg = sqlite3_errmsg(db);
   		throw runtime_error(errMsg);
   	}

   	string sqlStr = (obj.createQuery).c_str();

  	if( sqlite3_exec(db, sqlStr.c_str(), NULL, NULL, NULL) != SQLITE_OK ){
	   	string errMsg = sqlite3_errmsg(db);
   		throw runtime_error(errMsg);
	}

	if (sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, NULL) != SQLITE_OK) {
		string errMsg = sqlite3_errmsg(db);
   		throw runtime_error(errMsg);
	}

	if (sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", NULL, NULL, NULL) != SQLITE_OK) {
		string errMsg = sqlite3_errmsg(db);
   		throw runtime_error(errMsg);
	}


	cout << "***** TABLE IS CREATED SUCCESSFULLY *****" << endl;
}

/**
    Insert rows in the database
    @param: NULL
    @return: NULL
*/

void DataBaseInterface::insertRows()
{
	FileInterface obj;
	string line;
	vector<string> data;
	sqlite3_stmt* stmt;
	
	if (sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
	{
		string errMsg = sqlite3_errmsg(db);
   		throw runtime_error(errMsg);
	}
 	
	if (sqlite3_prepare_v2(db, (obj.insertQuery).c_str(), strlen((obj.insertQuery).c_str()), &stmt, NULL) != SQLITE_OK)
	{
		string errMsg = sqlite3_errmsg(db);
   		throw runtime_error(errMsg);
	}

	if ((obj.inFile).is_open())
  {
    while(getline (obj.inFile,line)) {
      boost::split(data, line, boost::is_any_of(" "));

      for(int i = 0; i < (obj.columnType).size(); i++){
        if (obj.columnType[i] == "INTEGER")
          sqlite3_bind_int(stmt, i+1, atoi(data[i].c_str()));
    		else 
    			sqlite3_bind_text(stmt, i+1, data[i].c_str(), data[i].size(), SQLITE_STATIC) ;
    	}

    	if (sqlite3_step(stmt) != SQLITE_DONE)
	    {
        string errMsg = sqlite3_errmsg(db);
   			throw runtime_error(errMsg);
	    }

	    sqlite3_reset(stmt);
	    	
		  }
		  if (sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) != SQLITE_OK)
		  {
        string errMsg = sqlite3_errmsg(db);
   			throw runtime_error(errMsg);
		  }
		  sqlite3_finalize(stmt);
		  cout << "***** RECORDS ARE INSERTED SUCCESSFULLY *****" << endl;
		  obj.inFile.close();
	 }	
}

/**
    Check is aggregation_attribute is integer or not
    @param: aggregation_attribute
    @return: true if integer, else false
*/

bool DataBaseInterface::validateQuery(string &aggregation_attr)
{
	FileInterface obj;
	map<string,string>::iterator it;
	if((it = obj.columnMap.find(aggregation_attr)) != obj.columnMap.end()) {
		if ((it->second).compare("INTEGER")){
			cout << "#ERROR: aggregation_attribute should be integer" << endl;
			return false;
		}
	}
	else{
		cout << "#ERROR: Column does not exist" << endl;
		return false;
	}
	return true;
}

/**
    Perform query on the database
    @param: Query string, aggregation_attribute, grouping_attribute
    @return: NULL
*/

void DataBaseInterface::queryDatabase(string &query, string &aggregation_attr, string &grouping_attr)
{
	FileInterface obj;

	string sqlStr = "SELECT " + grouping_attr + "," + query + "(" + aggregation_attr + ") " + "FROM RECORDS GROUP BY " + grouping_attr + ";";

	if (obj.outFile.is_open()){
    	string line = "\n" + query + " " + aggregation_attr + " GROUPED BY " + grouping_attr + "\n";
    	obj.outFile << line;
    }
    else {
    	string errMsg = "#ERROR: Output file is not open";
   		throw runtime_error(errMsg);
    }

	int rc = sqlite3_exec(db, sqlStr.c_str(), callback, &(obj.outFile), NULL);
    
    if (rc != SQLITE_OK ) {        
        string errMsg = sqlite3_errmsg(db);
   		cout << "#ERROR: " << errMsg << endl;
   		return;
    }

}

/**
    callback function
*/

int callback(void *fptr, int argc, char **argv, char **azColName)
{
	ofstream *ofile = (ofstream *)fptr;

	if (ofile->is_open()) {
    	for (int i = 0; i < argc; i++)
    		(*ofile) << argv[i] << "\t\t";
    	(*ofile) << "\n";
    }
    else {
    	string errMsg = "#ERROR: Output file is not open";
   		throw runtime_error(errMsg);
    }
        
    return 0;
}