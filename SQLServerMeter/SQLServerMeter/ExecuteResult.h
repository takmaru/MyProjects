#pragma once

#include <Windows.h>

#include <vector>

#include <MyLib/tstring/tstring.h>

class CExecuteResult {
public:
	enum CountType {
		kCountType_Execute,
		kCountType_ExecStatement,
		kCountType_ResultSet,
		kCountType_Info,
		kCountType_Error,
		kCountType_StartSessionError,

		kCountTypeMax
	};
	enum TimeType {
		kTimeTypeAll,
		kTimeTypeExecStatement,

		kTimeTypeMax
	};

private:
	typedef std::vector<unsigned int> Counts;
	typedef std::vector<DWORD> Times;

public:
	CExecuteResult();
	~CExecuteResult();

public:
	void add(CountType type, unsigned int count = 1);
	void addTime(TimeType type, DWORD addTime);

public:
	std::tstring description() const;

private:
	Counts m_counts;
	Times m_times;
};
