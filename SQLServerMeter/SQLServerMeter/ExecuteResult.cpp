#include "stdafx.h"
#include "ExecuteResult.h"

CExecuteResult::CExecuteResult():
	m_counts(CExecuteResult::kCountTypeMax, 0), m_times(CExecuteResult::kTimeTypeMax, 0) {
}
CExecuteResult::~CExecuteResult() {
}

void CExecuteResult::add(CExecuteResult::CountType type, unsigned int count/*= 1*/) {
	if(type >= m_counts.size()) {
		return;
	}
	m_counts[type] += count;
}
void CExecuteResult::addTime(CExecuteResult::TimeType type, DWORD addTime) {
	if(type >= m_times.size()) {
		return;
	}
	m_times[type] += addTime;
}

std::tstring CExecuteResult::description() const {
	std::tostringstream oss;
	return oss.str();
}