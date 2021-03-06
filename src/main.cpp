#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QStyleHints>
#include <QTranslator>
#include <QWindow>

#include "account.h"
#include "applicationengine.h"
#include "devices/device.h"
#include "gui.h"
#include "network.h"
#include "transaction.h"
#include "wallet.h"
#include "walletmanager.h"
#include "wally.h"
#include "twofactorcontroller.h"
#include "settingscontroller.h"
#include "controllers/createaccountcontroller.h"
#include "controllers/sendtransactioncontroller.h"
#include "controllers/renameaccountcontroller.h"

#if defined(Q_OS_MAC)
#include "devicemanagermacos.h"

void removeTitlebarFromWindow(QWidget* window);
#endif

#include <QZXing.h>


#include <QtPlugin>
#if defined(QT_QPA_PLATFORM_MINIMAL)
Q_IMPORT_PLUGIN(QMinimalIntegrationPlugin);
#endif
#if defined(QT_QPA_PLATFORM_XCB)
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin);
#elif defined(QT_QPA_PLATFORM_WINDOWS)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#elif defined(QT_QPA_PLATFORM_COCOA)
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin);
#endif

class Translator : public QTranslator
{
public:
    virtual QString translate(const char *context, const char *sourceText, const char *disambiguation, int n) const;
};

QString Translator::translate(const char *context, const char *sourceText, const char *disambiguation, int n) const
{
    Q_UNUSED(context);
    return QTranslator::translate("green", sourceText, disambiguation, n);
}

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Green");
    QCoreApplication::setOrganizationName("Blockstream");
    QCoreApplication::setOrganizationDomain("blockstream.com");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);

    app.styleHints()->setTabFocusBehavior(Qt::TabFocusAllControls);

    const QLocale locale = QLocale::system();
    const QString language = locale.name().split('_').first();

    Translator language_translator;
    language_translator.load(QString(":/i18n/green_%1.qm").arg(language));

    Translator locale_translator;
    locale_translator.load(QString(":/i18n/green_%1.qm").arg(locale.name()));

    app.installTranslator(&language_translator);
    app.installTranslator(&locale_translator);

    QQuickStyle::setStyle("Material");

    ApplicationEngine engine;

    QZXing::registerQMLTypes();
    QZXing::registerQMLImageProvider(engine);

    qmlRegisterUncreatableType<Device>("Blockstream.Green", 0, 1, "Device", "Devices are instantiated automatically");
    qmlRegisterUncreatableType<Network>("Blockstream.Green", 0, 1, "Network", "Networks are created by NetworkManager");
    qmlRegisterType<Wallet>("Blockstream.Green", 0, 1, "Wallet");
    qmlRegisterUncreatableType<Transaction>("Blockstream.Green", 0, 1, "Transaction", "Transactions are created by accounts");

    qmlRegisterType<ReceiveAddress>("Blockstream.Green", 0, 1, "ReceiveAddress");
    qmlRegisterType<TwoFactorController>("Blockstream.Green", 0, 1, "TwoFactorController");
    qmlRegisterType<CreateAccountController>("Blockstream.Green", 0, 1, "CreateAccountController");
    qmlRegisterType<RenameAccountController>("Blockstream.Green", 0, 1, "RenameAccountController");
    qmlRegisterType<WordValidator>("Blockstream.Green", 0, 1, "WordValidator");
    qmlRegisterType<SendTransactionController>("Blockstream.Green", 0, 1, "SendTransactionController");
    qmlRegisterType<SettingsController>("Blockstream.Green", 0, 1, "SettingsController");
    qmlRegisterUncreatableType<Account>("Blockstream.Green", 0, 1, "Account", "Accounts are created by the wallet");
    qmlRegisterUncreatableType<Controller>("Blockstream.Green", 0, 1, "Controller", "Controller is an abstract type");

    qmlRegisterSingletonInstance("Blockstream.Green", 0, 1, "NetworkManager", NetworkManager::instance());
    qmlRegisterSingletonInstance("Blockstream.Green", 0, 1, "WalletManager", new WalletManager);
    qmlRegisterSingletonInstance("Blockstream.Green", 0, 1, "Wally", new Wally);
#if defined(Q_OS_MAC)
    qmlRegisterSingletonInstance("Blockstream.Green", 0, 1, "DeviceManager", new DeviceManagerMacos);
#endif

    qmlRegisterType<Action>("Blockstream.Green.Gui", 0, 1, "Action");
    qmlRegisterType<Menu>("Blockstream.Green.Gui", 0, 1, "Menu");
    qmlRegisterType<MenuBar>("Blockstream.Green.Gui", 0, 1, "MenuBar");
    qmlRegisterType<Separator>("Blockstream.Green.Gui", 0, 1, "Separator");

    QMainWindow main_window;
    engine.rootContext()->setContextProperty("main_window", &main_window);

    engine.load(QUrl(QStringLiteral("loader.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QWindow* window = static_cast<QWindow*>(engine.rootObjects().first());

    main_window.setCentralWidget(QWidget::createWindowContainer(window));
    main_window.setMinimumSize(QSize(1024, 600));
    main_window.show();

#if defined(Q_OS_MAC)
    removeTitlebarFromWindow(&main_window);
#endif

#if defined(Q_OS_MAC) || defined(Q_OS_UNIX)
    // Workaround due to https://bugreports.qt.io/browse/QTBUG-34414
    QObject::connect(&app, &QGuiApplication::focusWindowChanged, [&](QWindow* focusWindow) {
        if (focusWindow == main_window.windowHandle()) {
            main_window.centralWidget()->activateWindow();
            window->requestActivate();
        }
    });
#endif
    window->requestActivate();

    return app.exec();
}
