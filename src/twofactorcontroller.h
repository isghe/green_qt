#ifndef GREEN_TWOFACTORCONTROLLER_H
#define GREEN_TWOFACTORCONTROLLER_H

#include "controllers/controller.h"

#include <QByteArray>

class TwoFactorController : public Controller
{
    Q_OBJECT
    Q_PROPERTY(QByteArray method READ method WRITE setMethod NOTIFY methodChanged)

public:
    explicit TwoFactorController(QObject *parent = nullptr);

    QByteArray method() const;
    void setMethod(const QByteArray& method);

    Q_INVOKABLE void enable(const QByteArray& number);
    Q_INVOKABLE void disable();

    Q_INVOKABLE void changeLimit(bool is_fiat, const QString &limit);

    bool update(const QJsonObject& result) override;

signals:
    void methodChanged(const QByteArray& method);

private:
    QByteArray m_method;
};

#endif // GREEN_TWOFACTORCONTROLLER_H
