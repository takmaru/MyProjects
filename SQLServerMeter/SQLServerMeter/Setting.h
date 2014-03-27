#pragma once

#include <MyLib/tstring/tstring.h>

class CSetting {
public:
	CSetting();
	~CSetting();

public:
	void load();
	void load(const std::tstring& iniFilePath);

	std::tstring server() const {
		return m_server;
	}
	std::tstring instance() const {
		return m_instance;
	}
	std::tstring database() const {
		return m_database;
	}
	int threadCount() const {
		return m_threadCount;
	}
	int execCount() const {
		return m_execCount;
	}

private:
	std::tstring m_server;
	std::tstring m_instance;
	std::tstring m_database;

	int m_threadCount;
	int m_execCount;
};
