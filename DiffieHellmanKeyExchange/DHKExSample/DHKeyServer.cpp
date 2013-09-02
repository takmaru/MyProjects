#include "stdafx.h"
#include "DHKeyServer.h"

CDHKeyServer::CDHKeyServer() : m_dhkeyMaker(), m_sessionKey() {
}
CDHKeyServer::~CDHKeyServer() {
}

void CDHKeyServer::createPublickey() {
	m_dhkeyMaker.createPublicKey();
}

void CDHKeyServer::setClientKey(const DHKEx::DHBlob& clientKey) {
	m_sessionKey.attach(m_dhkeyMaker.createSecretKey(clientKey));
	m_sessionKey.setAlgorithm(CALG_RC4);
}

DHKEx::DHBlob CDHKeyServer::encrypto(const DHKEx::DHBlob& data) {
	return m_sessionKey.encrypto(data);
}
DHKEx::DHBlob CDHKeyServer::decrypto(const DHKEx::DHBlob& encryptoData) {
	return m_sessionKey.decrypto(encryptoData);
}
