#pragma once

#include <windows.h>
#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

using namespace std;

class ODBCConnect {
	SQLHANDLE sqlEnviromentHandle;
	SQLHANDLE sqlConnectionHandle;
	SQLRETURN returnCode;

	string connectionString;
	string serverName;
	string port;
	string database;
	string username;
	string password;

public:
	class SQLStatement {
		SQLHANDLE sqlStatementHandle;
	public:
		SQLStatement(SQLHANDLE sqlConnectionHandle);
		~SQLStatement();
		void query(SQLCHAR *statementString);
		void printResults();
	};

	ODBCConnect();
	~ODBCConnect();
	void connect();

	ODBCConnect* setServer(string serverName);
	ODBCConnect* setPort(string port);
	ODBCConnect* setDatabase(string database);
	ODBCConnect* setUsername(string username);
	ODBCConnect* setPassword(string password);

	void query(SQLCHAR *statement);
	void showDiagnostics(SQLSMALLINT handleType, SQLHANDLE handle);
};