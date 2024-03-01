#include "ldapconnector.h"

#include <iostream>
#include <QDebug>

LdapConnector::LdapConnector()
{

}

int LdapConnector::init(const QString url)
{
    std::string ip = url.toStdString();
    PWSTR pwstr;
    size_t size = ip.length();
    wchar_t* buffer = new wchar_t[size + 1];
    MultiByteToWideChar(CP_ACP, 0, ip.c_str(), size, buffer, size * sizeof(wchar_t));
    buffer[size] = 0;
    pwstr = buffer;

    int version = LDAP_VERSION3;

    int res = LDAP_SUCCESS;

    ld = ldap_init(pwstr, LDAP_PORT);

    res = ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &version);

    if(res != LDAP_SUCCESS) {
        qCritical() << "FAIL set option!";
        return res;
    }

    const int reff = 0;
    res = ldap_set_option(ld, LDAP_OPT_REFERRALS, &reff);

    if(res != LDAP_SUCCESS) {
        qCritical() << "FAIL set option ref!";
        return res;
    }

    res = ldap_connect(ld, NULL);

    if(res != LDAP_SUCCESS) {
        qCritical() << "FAIL CONNECT";
    }

//    delete[] buffer;

    qInfo() << "CONNECTED";
    return res;
}

int LdapConnector::bind(QString dn, QString passW)
{
    int res = 0;
    PWSTR userPW, passPW;
    std::string user = dn.toStdString();
    std::basic_string pass = passW.toStdString();

    size_t userSize = user.length();
    wchar_t* userBuf = new wchar_t[userSize + 1];
    MultiByteToWideChar(CP_ACP, 0, user.c_str(), userSize, userBuf, userSize * sizeof(wchar_t));
    userBuf[userSize] = 0;
    userPW = userBuf;

    size_t pasSize = pass.length();
    wchar_t* pasBuf = new wchar_t[pasSize + 1];
    MultiByteToWideChar(CP_ACP, 0, pass.c_str(), pasSize, pasBuf, pasSize * sizeof(wchar_t));
    pasBuf[pasSize] = 0;
    passPW = pasBuf;

    int val = 0;

    val = ldap_bind(ld, userPW, passPW, LDAP_AUTH_SIMPLE);

    if (val == -1) {
        qCritical() << "FAIL bind: " << val;
    }

    delete[] userBuf;
    delete[] pasBuf;

    qInfo() << "BINDED";
    return res;
}

int LdapConnector::searchUser(QString userName, int &resCount)
{
    int res = LDAP_SUCCESS;

//(sAMAccountName=%1)
    QString qFilter = QString("(mail=%1)").arg(userName);
    std::string filter = qFilter.toStdString();
    PWSTR filterPW;
    size_t filterSize = filter.length();
    wchar_t* filterBuf = new wchar_t[filterSize + 1];
    MultiByteToWideChar(CP_ACP, 0, filter.c_str(), filterSize, filterBuf, filterSize * sizeof(wchar_t));
    filterBuf[filterSize] = 0;
    filterPW = filterBuf;

    LDAPMessage* searchResMsg;
    res = ldap_search_ext_s(ld, NULL, LDAP_SCOPE_SUBTREE, filterPW, NULL, 0, NULL, NULL, LDAP_NO_LIMIT, LDAP_NO_LIMIT, &searchResMsg);

    if (res != LDAP_SUCCESS) {
        qCritical() << "FAIL SEARCH!" << res;
        qCritical() << "Ldap_search_error: " << ldap_err2string(res);
        return res;
    }

    int result = 0;
    result = ldap_count_entries(ld, searchResMsg);
//    qInfo() << "Search result: " << result;
    resCount = result;

    if(searchResMsg->lm_msgtype == LDAP_RES_SEARCH_ENTRY) {
        PWCHAR a;
        BerElement* ber;

        a = ldap_first_attribute(ld, searchResMsg, &ber);

        struct berval** values;
        struct berval value;

        while (a != NULL)
        {
            if ((values = ldap_get_values_len(ld, searchResMsg, a)) != NULL) {
                for (int i = 0; values[i] != NULL; i++) {
                    value = *values[i];

                    PCHAR buf = value.bv_val;
//                    qInfo() << a << ": " << buf;
                }
            }
            a = ldap_next_attribute(ld, searchResMsg, ber);
        }
        ldap_value_free_len(values);
        ldap_memfree( a );
    }

    ldap_msgfree(searchResMsg);
\
//    delete[] baseBuf;
    delete[] filterBuf;

    return res;
}

int LdapConnector::close()
{
    int res = ldap_unbind(ld);

    if(res != LDAP_SUCCESS) {
        qCritical() << "FAIL close!";
    }

    qInfo() << "CLOSED";

    return res;
}
