#ifndef GREEN_GA_H
#define GREEN_GA_H

#include <gdk.h>

#include <QJsonArray>
#include <QJsonObject>

namespace GA {

int reconnect_hint(GA_session* session, const QJsonObject& data);
int connect(GA_session* session, const QJsonObject& data);
QJsonObject auth_handler_get_result(GA_auth_handler* call);
QJsonArray get_subaccounts(GA_session* session);
QJsonObject convert_amount(GA_session* session, const QJsonObject& input);
QJsonObject process_auth2(GA_auth_handler* call);

template <typename F>
QJsonObject process_auth(F f)
{
    GA_auth_handler* call{nullptr};
    f(&call);
    auto res = process_auth2(call);
    GA_destroy_auth_handler(call);
    return res;
}

} // namespace GA

#endif // GREEN_GA_H
