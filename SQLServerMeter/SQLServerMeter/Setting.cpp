#include "stdafx.h"
#include "Setting.h"

#include <Windows.h>
#include <tchar.h>

#include <MyLib/Application/ApplicationInfo.h>
#include <MyLib/Path/Path.h>
#include <MyLib/File/IniFile.h>

namespace {
	static const TCHAR kServer_Default[] = _T("localhost");
	static const int kThreadCount_Default = 1;
	static const int kThreadCount_Min = 1;
	static const int kThreadCount_Max = 128;
	static const int kExecCount_Default = 1;
	static const int kExecCount_Min = 1;
	static const int kExecCount_Max = 1024;

	static const TCHAR kSection_General[] = _T("General");
	static const TCHAR kKeyGenral_Server[] = _T("Server");
	static const TCHAR kKeyGenral_Instance[] = _T("Instance");
	static const TCHAR kKeyGenral_Database[] = _T("db");
	static const TCHAR kKeyGenral_ThreadCount[] = _T("ThreadCount");
	static const TCHAR kKeyGenral_ExecCount[] = _T("ExecCount");

}

CSetting::CSetting():
	m_server(), m_instance(), m_database(),
	m_threadCount(kThreadCount_Default), m_execCount(kExecCount_Default) {
}
CSetting::~CSetting() {
}

void CSetting::load() {
	this->load(MyLib::Path::renameExtention(MyLib::App::modulefilename(), _T("ini")));
}
void CSetting::load(const std::tstring& iniFilePath) {
	m_server = MyLib::File::getIniFileString(iniFilePath, kSection_General, kKeyGenral_Server, kServer_Default);
	m_instance = MyLib::File::getIniFileString(iniFilePath, kSection_General, kKeyGenral_Instance, _T(""));
	m_database = MyLib::File::getIniFileString(iniFilePath, kSection_General, kKeyGenral_Database, _T(""));
	m_threadCount = MyLib::File::getIniFileInteger(iniFilePath, kSection_General, kKeyGenral_ThreadCount, kThreadCount_Default);
	m_execCount = MyLib::File::getIniFileInteger(iniFilePath, kSection_General, kKeyGenral_ExecCount, kExecCount_Default);
}
