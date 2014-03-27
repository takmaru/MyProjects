// SQLServerMeter.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <process.h>

#include <time.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

#include <ODBCLibInclude.h>

#include <MyLib/tstring/tstring.h>

#include "Setting.h"
#include "ExecuteThread.h"

#define SERVER_NAME	_T("localhost\\SKYSEAMASTERSVR")
#define DB_NAME		_T("scvdb")

struct _ExecuteParam {
	_ExecuteParam():
		count(0), trids(), agentid(0), connectErrorCount(0), executeCount(0),
		count_ResultSet(0), count_RowCount(0), count_Info(0), count_Error(0), count_None(0) {
	}

	int count;
	std::vector<int> trids;
	int agentid;

	int connectErrorCount;
	int executeCount;
	int count_ResultSet;
	int count_RowCount;
	int count_Info;
	int count_Error;
	int count_None;
} typedef ExecuteParam;

std::wstring StatementFromSQLFile(const wchar_t* filename);
void ExecuteStatement(std::shared_ptr<ODBCLib::CODBCStatement> statement);
void ExecuteStatement(std::shared_ptr<ODBCLib::CODBCStatement> statement, ExecuteParam* param);
void ExecuteStatement(std::shared_ptr<ODBCLib::CODBCStatement> statement, const std::wstring& sql, ExecuteParam* param);
std::wstring ExecSettingGet3Statement(int trid, int agentid);
std::wstring CallSettingGet3Statement(int trid, int agentid);

void CallStoredProcedure(std::shared_ptr<ODBCLib::CODBCStatement> statement);

unsigned __stdcall ThreadMain(void* pParam);

void OutputExecuteParam(ExecuteParam& param);

int _tmain(int argc, _TCHAR* argv[]) {
	std::locale::global(std::locale("", std::locale::ctype));

	// 設定ファイル ロード
	CSetting setting;
	setting.load();
	if(setting.server().size() == 0) {
		std::tcout << _T("サーバーを指定して下さい。") << std::endl;
		return 1;
	}

	std::tcout <<
		_T("サーバー:") << setting.server() << _T("\\") << setting.instance() << _T(", ") <<
		_T("ＤＢ:") << setting.database() << _T(", ") <<
		_T("スレッド数:") << setting.threadCount() << _T(", ") <<
		_T("実行回数:") << setting.execCount() << std::endl;
/*
	int threadCount = THREAD_COUNT_DEF;
	int executeCount = EXEC_COUNT_DEF;
	if(argc >= 3) {
		threadCount = _wtoi(argv[1]);
		if(	(threadCount < THREAD_COUNT_MIN) ||
			(threadCount > THREAD_COUNT_MAX)	) {
			threadCount = THREAD_COUNT_DEF;
		}
		executeCount = _wtoi(argv[2]);
		if(	(executeCount < EXEC_COUNT_MIN) ||
			(executeCount > EXEC_COUNT_MAX)	) {
			executeCount = EXEC_COUNT_DEF;
		}
	}
	std::wcout << L"スレッド数: " << threadCount << L", 実行回数: " << executeCount << std::endl;
*/

	std::vector<ExecuteParam> params(setting.threadCount());
	int trid = 1000;
	for(int i = 0; i < setting.threadCount(); i++) {
		params[i].count = setting.execCount();
		for(int j = 0; j < params[i].count; j++) {
			params[i].trids.push_back(trid);
			trid++;
		}
		params[i].agentid = i + 1;
	}

//	CExecuteThread
	std::vector<HANDLE> handles;
	for(int i = 0; i < setting.threadCount(); i++) {
		handles.push_back((HANDLE)::_beginthreadex(NULL, 0, ThreadMain, &params[i], 0, NULL));
	}
	::WaitForMultipleObjects(handles.size(), &handles[0], TRUE, INFINITE);

	for(std::vector<ExecuteParam>::iterator it = params.begin(); it != params.end(); it++) {
		OutputExecuteParam((*it));
	}

	std::tcout << L"完了" << std::endl;

	return 0;
}


std::wstring StatementFromSQLFile(const wchar_t* filename) {

	ATL::CAtlFile sqlFile;
	// SQLファイル Open
	HRESULT ret = sqlFile.Create(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING);
	if(ret != S_OK) {
		std::wcerr << _T("err : CAtlFile::Create err=") << ret << _T(" file=") << filename << std::endl;
		return std::wstring();
	}
	ULONGLONG filesize = 0;
	sqlFile.GetSize(filesize);
	std::vector<unsigned char> buffer((unsigned int)filesize + sizeof(wchar_t), 0);
	sqlFile.Read(&buffer[0], buffer.size());
	if(ret != S_OK) {
		std::wcerr << _T("err : CAtlFile::Read err=") << ret << _T(" file=") << filename << std::endl;
		return std::wstring();
	}

	return std::wstring((wchar_t*)&buffer[0]);
}

int g_storedParamRet = -1;
int g_storedParam1 = -1;
int g_storedParam2 = -1;
void ExecuteStatement(std::shared_ptr<ODBCLib::CODBCStatement> statement) {
	std::shared_ptr<ODBCLib::CStatementResult> result = statement->execute();
std::wcout << _T("ExecuteStatement after execute ret:") << g_storedParamRet << _T(", p1:") << g_storedParam1 << _T(", p2:") << g_storedParam2 << std::endl;
	while(result->resultType() != ODBCLib::CStatementResult::RT_None) {
		switch(result->resultType()) {
		case ODBCLib::CStatementResult::RT_ResultSet:
			std::wcout << L"結果セット" << std::endl;
			break;
		case ODBCLib::CStatementResult::RT_ResultRowCount:
			std::wcout << L"行数" << std::endl;
			break;
		case ODBCLib::CStatementResult::RT_Info:
			std::wcout << L"情報" << std::endl <<
				result->diagInfo()->description() << std::endl;
			break;
		case ODBCLib::CStatementResult::RT_Error:
			std::wcout << L"エラー " << std::endl <<
				result->diagInfo()->description() << std::endl;
			break;
		default:
			std::wcout << L"その他" << std::endl;
			break;
		}
		result = statement->nextResult();
std::wcout << _T("ExecuteStatement after next ret:") << g_storedParamRet << _T(", p1:") << g_storedParam1 << _T(", p2:") << g_storedParam2 << std::endl;
	}
}

void ExecuteStatement(std::shared_ptr<ODBCLib::CODBCStatement> statement, ExecuteParam* param) {
	std::shared_ptr<ODBCLib::CStatementResult> result = statement->execute();
	param->executeCount++;
	while(	(result->resultType() != ODBCLib::CStatementResult::RT_None) &&
			(result->resultType() != ODBCLib::CStatementResult::RT_Error)	) {
		switch(result->resultType()) {
		case ODBCLib::CStatementResult::RT_ResultSet:
			param->count_ResultSet++;
			break;
		case ODBCLib::CStatementResult::RT_ResultRowCount:
			param->count_RowCount++;
			break;
		case ODBCLib::CStatementResult::RT_Info:
			param->count_Info++;
			break;
		}
		result = statement->nextResult();
	}
	if(result->resultType() == ODBCLib::CStatementResult::RT_None) {
		param->count_None++;
	} else if(result->resultType() == ODBCLib::CStatementResult::RT_Error) {
		param->count_Error++;
	}
}

void ExecuteStatement(std::shared_ptr<ODBCLib::CODBCStatement> statement, const std::wstring& sql, ExecuteParam* param) {
	std::shared_ptr<ODBCLib::CStatementResult> result = statement->execute(sql.c_str());
	param->executeCount++;
	while(	(result->resultType() != ODBCLib::CStatementResult::RT_None) &&
			(result->resultType() != ODBCLib::CStatementResult::RT_Error)	) {
		switch(result->resultType()) {
		case ODBCLib::CStatementResult::RT_ResultSet:
			param->count_ResultSet++;
			break;
		case ODBCLib::CStatementResult::RT_ResultRowCount:
			param->count_RowCount++;
			break;
		case ODBCLib::CStatementResult::RT_Info:
			param->count_Info++;
			break;
		}
		result = statement->nextResult();
	}
	if(result->resultType() == ODBCLib::CStatementResult::RT_None) {
		param->count_None++;
	} else if(result->resultType() == ODBCLib::CStatementResult::RT_Error) {
//std::wcout << result->diagInfo()->description() << std::endl;
		param->count_Error++;
	}
}

unsigned __stdcall ThreadMain(void* pParam) {

	ExecuteParam* result = reinterpret_cast<ExecuteParam*>(pParam);

	for(int i = 0; i < result->count; i++) {

		ODBCLib::CODBCSession session;
		session.setServerName(SERVER_NAME);
		session.setDatabaseName(DB_NAME);
		if(session.startSession()) {

//			session.createStatement()->execute(StatementFromSQLFile(_T("CreateLocalTempTable.sql")).c_str());

			std::shared_ptr<ODBCLib::CTransaction> tran = session.beginTransaction();
			std::shared_ptr<ODBCLib::CODBCStatement> statement = session.createStatement();
			statement->setCursorScrollable(SQL_SCROLLABLE);
			statement->setCursorSensitivity(SQL_INSENSITIVE);
			statement->prepare(_T("{? = call SampleStoredProcedure(?, ?)}"));
			CallStoredProcedure(statement);
///			CallStoredProcedure(session.createStatement(_T("{? = call SampleStoredProcedure(?, ?)}")));
//			ExecuteStatement(StatementFromSQLFile(session, _T("Main.sql")), result);
/*
			ExecuteStatement(
				session.createStatement(
					ExecSettingGet3Statement(result->trids[result->executeCount], result->agentid).c_str()), result);
*/
/*
			ExecuteStatement(
				session.createStatement(
					CallSettingGet3Statement(result->trids[result->executeCount], result->agentid).c_str()), result);
*/
			tran->commit();

			session.endSession();
		} else {
			result->connectErrorCount++;
		}
	}

	return 0;
}

void OutputExecuteParam(ExecuteParam& param) {
	std::wcout <<
		L"実行回数:" << param.executeCount <<
			L" (RS:" << param.count_ResultSet <<
			L", RC:" << param.count_RowCount <<
			L", IF:" << param.count_Info <<
			L", ER:" << param.count_Error <<
			L", NO:" << param.count_None << L")" <<
		L" 接続エラー:" << param.connectErrorCount << std::endl;
}

std::wstring ExecSettingGet3Statement(int trid, int agentid) {
	std::wostringstream oss;
	oss <<
		L"DECLARE @nReqInfo_Out int" << std::endl <<
		L"DECLARE @nReserve_Out int" << std::endl <<
		L"DECLARE @strOption_Out nvarchar(1024)" << std::endl <<
		L"DECLARE @nMode_Out int" << std::endl <<
		L"DECLARE @n64ThisTime_Out bigint" << std::endl <<
		L"DECLARE @n64AgentDivisionId_Out bigint" << std::endl <<
		L"DECLARE @n64UserId_Out bigint" << std::endl <<
		L"DECLARE @n64UserDivisionId_Out bigint" << std::endl <<
		L"DECLARE @nResult_Out int" << std::endl <<
		L"DECLARE @nErrorNo_Out int" << std::endl <<
		std::endl <<
		L"EXECUTE GENERAL_REQ_SETTING_GET3" << std::endl <<
		L"\t" << trid << L", 0, 8000, N'', 0, " << agentid << L", NULL, NULL," << std::endl <<
		L"\t@nReqInfo_Out, @nReserve_Out, @strOption_Out, @nMode_Out," << std::endl <<
		L"\t@n64ThisTime_Out, @n64AgentDivisionId_Out, @n64UserId_Out," << std::endl <<
		L"\t@n64UserDivisionId_Out, @nResult_Out, @nErrorNo_Out" << std::endl;
	return oss.str();
}

std::wstring CallSettingGet3Statement(int trid, int agentid) {
	std::wostringstream oss;
	oss <<
		L"DECLARE @nReqInfo_Out int" << std::endl <<
		L"DECLARE @nReserve_Out int" << std::endl <<
		L"DECLARE @strOption_Out nvarchar(1024)" << std::endl <<
		L"DECLARE @nMode_Out int" << std::endl <<
		L"DECLARE @n64ThisTime_Out bigint" << std::endl <<
		L"DECLARE @n64AgentDivisionId_Out bigint" << std::endl <<
		L"DECLARE @n64UserId_Out bigint" << std::endl <<
		L"DECLARE @n64UserDivisionId_Out bigint" << std::endl <<
		L"DECLARE @nResult_Out int" << std::endl <<
		L"DECLARE @nErrorNo_Out int" << std::endl <<
		std::endl <<
		L"{call GENERAL_REQ_SETTING_GET3(" << std::endl <<
		L"\t" << trid << L", 0, 8000, N'', 0, " << agentid << L", NULL, NULL," << std::endl <<
		L"\t@nReqInfo_Out, @nReserve_Out, @strOption_Out, @nMode_Out," << std::endl <<
		L"\t@n64ThisTime_Out, @n64AgentDivisionId_Out, @n64UserId_Out," << std::endl <<
		L"\t@n64UserDivisionId_Out, @nResult_Out, @nErrorNo_Out" << std::endl <<
		L")};" << std::endl;
	return oss.str();
}

void CallStoredProcedure(std::shared_ptr<ODBCLib::CODBCStatement> statement) {
//	int storedParamRet = -1;
//	int storedParam1 = -1;
//	int storedParam2 = -1;

	statement->bindOutputParameter(1, &g_storedParamRet, NULL);
	statement->bindParameter(2, &g_storedParam1, NULL);
	statement->bindOutputParameter(3, &g_storedParam2, NULL);
std::wcout << _T("CallStoredProcedure before execute ret:") << g_storedParamRet << _T(", p1:") << g_storedParam1 << _T(", p2:") << g_storedParam2 << std::endl;
	ExecuteStatement(statement);
std::wcout << _T("CallStoredProcedure after execute ret:") << g_storedParamRet << _T(", p1:") << g_storedParam1 << _T(", p2:") << g_storedParam2 << std::endl;
}
