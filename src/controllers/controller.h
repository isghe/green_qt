#ifndef GREEN_CONTROLLER_H
#define GREEN_CONTROLLER_H

#include <QDebug>
#include <QObject>

#include "../ga.h"

class Wallet;

class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QJsonObject result READ result NOTIFY resultChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(Wallet* wallet READ wallet WRITE setWallet NOTIFY walletChanged)

public:
    explicit Controller(QObject* parent = nullptr);

    QObject* context() const;
    GA_session* session() const;

    QJsonObject result() const { return m_result; }

    virtual Wallet* wallet() const;
    void setWallet(Wallet* wallet);

    void process();

    bool isBusy() const { return m_busy > 0; }

public slots:
    virtual void reset();
    void cancel();

    void requestCode(const QByteArray& method);
    void resolveCode(const QByteArray& code);

signals:
    void resolveCode();
    void resultChanged(const QJsonObject& result);
    void busyChanged(bool busy);
    void walletChanged(Wallet* wallet);
    void statusChanged(const QString& status);

protected:
    Wallet* m_wallet{nullptr};
    GA_auth_handler* m_auth_handler{nullptr};
    QJsonObject m_result;
    int m_busy{0};

    void setResult(const QJsonObject& result);

    void incrementBusy();
    void decrementBusy();

    virtual bool update(const QJsonObject& result);

    template <typename F>
    void dispatch(F&& f) {
        QMetaObject::invokeMethod(context(), [this, f] {
            Q_ASSERT(m_auth_handler == nullptr);
            f(session(), &m_auth_handler);
            Q_ASSERT(m_auth_handler != nullptr);
            process();
        });
    }
};

#endif // GREEN_CONTROLLER_H
