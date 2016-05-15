#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "ODBCConnect.h"
#include <memory>

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

	ODBCConnect *sqlServer = new ODBCConnect();
	sqlServer->setServer("localhost")
		->setPort("1433")
		->setDatabase("dbName")
		->setUsername("username")
		->setPassword("password")
		->connect();

	SQLCHAR *sqlStatement = (SQLCHAR*)"SELECT * FROM tableName";
	sqlServer->query(sqlStatement);

	delete sqlServer;
	return EXIT_SUCCESS;
}