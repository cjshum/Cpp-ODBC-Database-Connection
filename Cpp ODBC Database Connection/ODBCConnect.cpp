#include "ODBCConnect.h"

ODBCConnect::ODBCConnect() {
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnviromentHandle);
	SQLSetEnvAttr(sqlEnviromentHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3_80, 0);
	SQLAllocHandle(SQL_HANDLE_DBC, sqlEnviromentHandle, &sqlConnectionHandle);
	connectionString = "DRIVER={SQL Server};";
}

ODBCConnect::~ODBCConnect() {
	SQLDisconnect(sqlConnectionHandle);
	SQLFreeHandle(SQL_HANDLE_DBC, sqlConnectionHandle);
	SQLFreeHandle(SQL_HANDLE_ENV, sqlEnviromentHandle);
}

void ODBCConnect::connect() {
	SQLCHAR returnMessage[1024];
	connectionString += serverName + "," + port + database + username + password;
	SQLDriverConnect(sqlConnectionHandle, NULL, (SQLCHAR*)connectionString.c_str(),
		SQL_NTS, returnMessage, 1024, NULL, SQL_DRIVER_COMPLETE);
}

ODBCConnect* ODBCConnect::setServer(string serverName) {
	this->serverName = "SERVER=" + serverName + ";";
	return this;
}

ODBCConnect* ODBCConnect::setPort(string port) {
	this->port = "PORT=" + port + ";";
	return this;
}

ODBCConnect* ODBCConnect::setDatabase(string database) {
	this->database = "DATABASE=" + database + ";";
	return this;
}

ODBCConnect* ODBCConnect::setUsername(string username) {
	this->username = "UID=" + username + ";";
	return this;
}

ODBCConnect* ODBCConnect::setPassword(string password) {
	this->password = "PWD=" + password + ";";
	return this;
}

void ODBCConnect::showDiagnostics(SQLSMALLINT handleType, SQLHANDLE handle) {
	SQLCHAR sqlState[32];
	SQLCHAR message[512];
	if (SQL_SUCCESS == SQLGetDiagRec(handleType, handle, 1, sqlState, 0, message, 1024, NULL))
		cout << "MESSAGE: " << message << " SQLSTATE: " << sqlState << endl;
}

void ODBCConnect::query(SQLCHAR *statementString) {
	SQLStatement sqlStatement(sqlConnectionHandle);
	sqlStatement.query(statementString);
	sqlStatement.printResults();
}

ODBCConnect::SQLStatement::SQLStatement(SQLHANDLE sqlConnectionHandle) {
	SQLAllocHandle(SQL_HANDLE_STMT, sqlConnectionHandle, &sqlStatementHandle);
	// Allows the cursor to be repositioned and re-fetch the results
	SQLSetStmtAttr(sqlStatementHandle, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_DYNAMIC, 0);
}

ODBCConnect::SQLStatement::~SQLStatement() {
	SQLFreeHandle(SQL_HANDLE_STMT, sqlStatementHandle);
}

void ODBCConnect::SQLStatement::query(SQLCHAR *statementString) {
	SQLPrepare(sqlStatementHandle, statementString, SQL_NTS);
	SQLExecute(sqlStatementHandle);
}

void ODBCConnect::SQLStatement::printResults() {
	SQLSMALLINT numColumns = 0;
	SQLNumResultCols(sqlStatementHandle, &numColumns);
	SQLCHAR **results = new SQLCHAR*[numColumns];
	SQLUINTEGER *columnSizes = new SQLUINTEGER[numColumns];
	SQLCHAR **columnNames = new SQLCHAR*[numColumns];

	// initialize the array for results and get the column names
	for (int i = 0; i<numColumns; i++) {
		const int colNameSize = 10;
		columnNames[i] = new SQLCHAR[colNameSize];
		SQLSMALLINT datatype = 0;

		SQLDescribeCol(sqlStatementHandle, i + 1, columnNames[i], colNameSize,
			nullptr, &datatype, &columnSizes[i], nullptr, nullptr);
		results[i] = new SQLCHAR[columnSizes[i]];
	}

	// fetch the results and print
	SQLFetchScroll(sqlStatementHandle, SQL_FETCH_ABSOLUTE, 0);
	while (SQLFetch(sqlStatementHandle) == SQL_SUCCESS) {
		for (int i = 0; i<numColumns; i++) {
			SQLGetData(sqlStatementHandle, i + 1, SQL_C_CHAR, results[i], (SQLINTEGER)columnSizes[i], NULL);
			cout << columnNames[i] << ":" << results[i] << " ";
		}
		cout << endl;
	}

	// free memory for column names and results
	for (int i=0; i<numColumns; i++) {
		delete results[i];
		delete columnNames[i];
	}
	delete results;
	delete columnSizes;
	delete columnNames;
}
