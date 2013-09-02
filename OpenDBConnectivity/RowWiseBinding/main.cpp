#include <iostream>
#include <vector>

#include <EnvironmentHandle.h>
#include <ConnectionHandle.h>
#include <StatementHandle.h>
#include <ErrorInfo.h>

#include <sql.h>

struct InsertValues {
	int IntValue;
	InsertValues() : IntValue(0) {
	};
};
struct InsertValues2 {
	int IntValue;
	wchar_t CharValue[256 + 1];

	InsertValues2() : IntValue(0) {
		memset(CharValue, 0, sizeof(CharValue));
	};
};
struct InsertValues3 {
	int IntValue;
	wchar_t CharValue[256 + 1];
	SQLINTEGER IntValueInd;
	SQLINTEGER CharValueLenOrInd;

	InsertValues3() : IntValue(0), IntValueInd(0), CharValueLenOrInd(0) {
		memset(CharValue, 0, sizeof(CharValue));
	};
};


int wmain(int argc, wchar_t* argv[], wchar_t* arge[]) {
	std::locale::global(std::locale("", std::locale::ctype));

	// データの作成
	int dataCount = 1000;
	if(argc >= 2) {
		dataCount = _wtoi(argv[1]);
	}
	std::wcout << L"データ件数=" << dataCount << std::endl;

	std::vector<InsertValues> insertValues(dataCount);
	for(int i = 0; i < dataCount; ++i) {
		insertValues[i].IntValue = i + 1;
	}
/*
	std::vector<InsertValues2> insertValues2(dataCount);
	for(int i = 0; i < dataCount; ++i) {
		insertValues2[i].IntValue = i + 1;
		_itow_s(i + 1, insertValues2[i].CharValue, 10);
	}
*/
/*
	std::vector<InsertValues3> insertValues3(dataCount);
	for(int i = 0; i < dataCount; ++i) {
		insertValues3[i].IntValue = i + 1;
		_itow_s(i + 1, insertValues3[i].CharValue, 10);
	}
*/
	ODBCLib::CEnvironmentHandle envHandle;
	ODBCLib::CConnectionHandle connectionHandle(envHandle);
//	if(connectionHandle.Connect(L"DRIVER={SQL Server Native Client 10.0};SERVER=(local);Trusted_Connection=yes;Database=TestDB")) {
	if(connectionHandle.Connect(L"DRIVER={SQL Server Native Client 10.0};SERVER=localhost\\SKYSEAMASTERSVR;Trusted_Connection=yes;Database=scvdb")) {
		ODBCLib::CStatementHandle statementHandle(connectionHandle);

		std::vector<SQLUSMALLINT> statusArray(dataCount);
		SQLUINTEGER processCount = 0;

		statementHandle.SetRowWiseBinding(sizeof(InsertValues), dataCount, &(statusArray[0]), &processCount);
		statementHandle.Prepare(L"INSERT INTO dbo.TestTable2(IntValue) VALUES(?)");
		statementHandle.BindParameter(1, &(insertValues[0].IntValue), NULL);
/*
		statementHandle.SetRowWiseBinding(sizeof(InsertValues2), dataCount, &(statusArray[0]), &processCount);
		statementHandle.Prepare(L"INSERT INTO dbo.TestTable2(IntValue, CharValue) VALUES(?, ?)");
		statementHandle.BindParameter(1, &(insertValues2[0].IntValue), NULL);
		statementHandle.BindParameter(2, insertValues2[0].CharValue, sizeof(insertValues2[0].CharValue), NULL);
*/
/*
		statementHandle.SetRowWiseBinding(sizeof(InsertValues3), dataCount, &(statusArray[0]), &processCount);
		statementHandle.Prepare(L"INSERT INTO dbo.TestTable2(IntValue, CharValue) VALUES(?, ?)");
		statementHandle.BindParameter(1, &(insertValues3[0].IntValue), &(insertValues3[0].IntValueInd));
		statementHandle.BindParameter(2, insertValues3[0].CharValue, sizeof(insertValues3[0].CharValue), &(insertValues3[0].CharValueLenOrInd));
*/
		DWORD startTime = ::GetTickCount();
		connectionHandle.StartTransaction();
		bool isExecuteSuccess = SQL_SUCCEEDED(statementHandle.Execute());
		if(isExecuteSuccess) {
			connectionHandle.Commit();
		} else {
			ODBCLib::CErrorInfo errInfo(SQL_HANDLE_STMT, (SQLHSTMT)statementHandle);
			for(unsigned int i = 0; i < errInfo.GetCount(); ++i) {
				std::wcout << L"Error[" << i <<
					L"] state=" << errInfo.GetState(i) <<
					L" code=" << errInfo.GetErrorCode(i) <<
					L" message=" << errInfo.GetErrorMessage(i) << std::endl;
			}
			connectionHandle.Rollback();
		}
		std::wcout << L"Execute " << (isExecuteSuccess ? L"Success" : L"Error") << L" Time=" << ::GetTickCount() - startTime << L"(ms)" << std::endl;
	}
	

	return 0;
}