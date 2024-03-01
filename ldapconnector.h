#ifndef LDAPCONNECTOR_H
#define LDAPCONNECTOR_H

#if Q_OS_LINUX
#include <ldap>
#include <lber>
#else
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "Ws2_32.lib")

#include <Windows.h>
#include <Winldap.h>
#include <WinBer.h>
#endif

#include <QString>

class LdapConnector
{
public:
    LdapConnector();

    int init(const QString url);

    int bind(QString dn, QString passW);

    int searchUser(QString userName, int &resCount);

    int close();

private:
    LDAP *ld = NULL;
};

#endif // LDAPCONNECTOR_H
