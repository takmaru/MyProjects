#pragma once

#include <vector>

#include "MySockTypedef.h"
#include "AddrInfo.h"
#include "UDPSocket.h"

namespace MySock {

template<class S, class A = MySock::CAddrInfo>
class CSocketAddr {
public:
	CSocketAddr(S& s, A& a) : sock_(s), addr_(a) {
	}
public:
	S& sock() {
		return sock_;
	}
	A& addr() {
		return addr_;
	}
private:
	S sock_;
	A addr_;
};
typedef CSocketAddr<MySock::CUDPSocket> CUDPSocketAddr;
typedef std::vector<MySock::CUDPSocketAddr> UDPSocketAddrList;

}
