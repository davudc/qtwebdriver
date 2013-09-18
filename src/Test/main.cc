#include <QtCore/QObject>
#include <QtCore/QTextCodec>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <QtConcurrent/QtConcurrentRun>
    #if (WD_TEST_ENABLE_WEB_VIEW == 1)
        #include <QtWebKitWidgets/QWebView>
    #endif
    #include <QtWidgets/QApplication>
#else
    #include <QtGui/QApplication>
    #if (WD_TEST_ENABLE_WEB_VIEW == 1)
        #include <QtWebKit/QtWebKit>
    #endif
#endif

#include <iostream>

#include "TestVariables.h"
std::string tests::testDataFolder;


#include "WindowTest.h"
#include "ClickTest.h"
#include "ElementAttributeTest.h"
#include "FindingTest.h"
#include "CoordinatesTest.h"
#include "ClickScrollingTest.h"
#include "ElementSelectingTest.h"
#include "TypingTest.h"
#include "BasicKeyboardInterfaceTest.h"
#include "TextHandlingTest.h"
#include "FormHandlingTest.h"
#include "XPathElementFindingTest.h"
#include "StaleElementReferenceTest.h"
#include "VisibilityTest.h"
#include "BasicMouseInterfaceTest.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include "WindowWithDeclarativeViewTest.h"
#endif

// Commented VideoTest due to error https://bugreports.qt-project.org/browse/QTBUG-32949
#ifndef OS_IOS
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include "VideoTest.h"
#endif
#endif//OS_IOS

#include "base/at_exit.h"
#include "webdriver_server.h"
#include "webdriver_view_transitions.h"
#include "versioninfo.h"
#include "webdriver_route_table.h"
#include "shutdown_command.h"

#ifndef OS_IOS
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    // headers for Quick2 extension
    #include "extension_qt/quick2_view_creator.h"
    #include "extension_qt/quick2_view_executor.h"
    #include "extension_qt/quick2_view_enumerator.h"
#else
    #include <QtDeclarative/QDeclarativeView>
    // headers for Quick1 extension
    #include "extension_qt/qml_view_creator.h"
    #include "extension_qt/qml_view_executor.h"
    #include "extension_qt/qml_view_enumerator.h"
        #if (WD_TEST_ENABLE_WEB_VIEW == 1)
            #include "extension_qt/qdeclarativewebview.h"
            #include "extension_qt/qml_web_view_enumerator.h"
            #include "extension_qt/qml_web_view_executor.h"
        #endif
#endif
#endif //OS_IOS

#if (WD_TEST_ENABLE_WEB_VIEW == 1)
#include "extension_qt/web_view_creator.h"
#include "extension_qt/web_view_executor.h"
#include "extension_qt/web_view_enumerator.h"
#include "extension_qt/qwebviewext.h"
#include "WindowWithEmbeddedViewTest.h"
#include "WidgetAndWebViewTest.h"
#endif

#include "extension_qt/q_view_runner.h"
#include "extension_qt/q_session_lifecycle_actions.h"
#include "extension_qt/widget_view_creator.h"
#include "extension_qt/widget_view_enumerator.h"
#include "extension_qt/widget_view_executor.h"
#include "extension_qt/wd_event_dispatcher.h"
#include "extension_qt/vnc_event_dispatcher.h"
#include "extension_qt/uinput_event_dispatcher.h"

#include "extension_qt/vncclient.h"
#include "extension_qt/uinput_manager.h"
#include "webdriver_switches.h"

void setQtSettings();
void PrintVersion();
void PrintHelp();
void InitVNCClient();
void InitUInputClient();

int main(int argc, char *argv[])
{
    base::AtExitManager exit;
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
#endif

    webdriver::ViewRunner::RegisterCustomRunner<webdriver::QViewRunner>();

    webdriver::SessionLifeCycleActions::RegisterCustomLifeCycleActions<webdriver::QSessionLifeCycleActions>();

    webdriver::ViewTransitionManager::SetURLTransitionAction(new webdriver::URLTransitionAction_CloseOldView());

    webdriver::ViewCreator* widgetCreator = new webdriver::QWidgetViewCreator();
    widgetCreator->RegisterViewClass<QWidget>("QWidget");
    widgetCreator->RegisterViewClass<WindowTestWidget>("WindowTestWidget");
    widgetCreator->RegisterViewClass<ClickTestWidget>("ClickTestWidget");
    widgetCreator->RegisterViewClass<ElementAttributeTestWidget>("ElementAttributeTestWidget");
    widgetCreator->RegisterViewClass<FindingTestWidget>("FindingTestWidget");
    widgetCreator->RegisterViewClass<CoordinatesTestWidget>("CoordinatesTestWidget");
    widgetCreator->RegisterViewClass<ClickScrollingTestWidget>("ClickScrollingTestWidget");
    widgetCreator->RegisterViewClass<ElementSelectingTestWidget>("ElementSelectingTestWidget");
    widgetCreator->RegisterViewClass<TypingTestWidget>("TypingTestWidget");
    widgetCreator->RegisterViewClass<BasicKeyboardInterfaceTestWidget>("BasicKeyboardInterfaceTestWidget");
    widgetCreator->RegisterViewClass<TextHandlingTestWidget>("TextHandlingTestWidget");
    widgetCreator->RegisterViewClass<FormHandlingTestWidget>("FormHandlingTestWidget");
    widgetCreator->RegisterViewClass<XPathElementFindingTestWidget>("XPathElementFindingTestWidget");
    widgetCreator->RegisterViewClass<StaleElementReferenceTestWidget>("StaleElementReferenceTestWidget");
    widgetCreator->RegisterViewClass<VisibilityTestWidget>("VisibilityTestWidget");
    widgetCreator->RegisterViewClass<BasicMouseInterfaceTestWidget>("BasicMouseInterfaceTestWidget");
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    widgetCreator->RegisterViewClass<WindowWithDeclarativeViewTestWidget>("WindowWithDeclarativeViewTestWidget");
#endif

#ifndef OS_IOS
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    widgetCreator->RegisterViewClass<VideoTestWidget>("VideoTestWidget");
#endif
#endif //OS_IOS

#if (WD_TEST_ENABLE_WEB_VIEW == 1)
    webdriver::ViewCreator* webCreator = new webdriver::QWebViewCreator();
    webCreator->RegisterViewClass<QWebViewExt>("QWebViewExt");
    webdriver::ViewFactory::GetInstance()->AddViewCreator(webCreator);

    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::WebViewEnumeratorImpl());

    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QWebViewCmdExecutorCreator());
    widgetCreator->RegisterViewClass<WindowWithEmbeddedViewTestWidget>("WindowWithEmbeddedViewTestWidget");
    widgetCreator->RegisterViewClass<WidgetAndWebViewTestWindows>("WidgetAndWebViewTestWindows");
#endif

#ifndef OS_IOS 
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    // Quick2 extension
    webdriver::ViewCreator* qmlCreator = new webdriver::Quick2ViewCreator();
    qmlCreator->RegisterViewClass<QQuickView>("QQuickView");
    webdriver::ViewFactory::GetInstance()->AddViewCreator(qmlCreator);

    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::Quick2ViewEnumeratorImpl());

    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::Quick2ViewCmdExecutorCreator());
#else
    // Quick1 extension
    webdriver::ViewCreator* qmlCreator = new webdriver::QQmlViewCreator();
    qmlCreator->RegisterViewClass<QDeclarativeView>("QDeclarativeView");
    webdriver::ViewFactory::GetInstance()->AddViewCreator(qmlCreator);

    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::QmlViewEnumeratorImpl());

    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QQmlViewCmdExecutorCreator());
    
    #if (WD_TEST_ENABLE_WEB_VIEW == 1)
    qmlRegisterType<QDeclarativeWebSettings>();
    qmlRegisterType<QDeclarativeWebView>("CiscoQtWebKit", 1, 0, "CiscoWebView");
    qmlRegisterType<QDeclarativeWebView>("CiscoQtWebKit", 1, 1, "CiscoWebView");
    qmlRegisterRevision<QDeclarativeWebView, 0>("CiscoQtWebKit", 1, 0);
    qmlRegisterRevision<QDeclarativeWebView, 1>("CiscoQtWebKit", 1, 1);
    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::QmlWebViewEnumeratorImpl());
    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QmlWebViewCmdExecutorCreator());
    #endif

#endif    
#endif //OS_IOS
    
    webdriver::ViewFactory::GetInstance()->AddViewCreator(widgetCreator);

    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::WidgetViewEnumeratorImpl());

    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::QWidgetViewCmdExecutorCreator());
	
	CommandLine cmd_line(CommandLine::NO_PROGRAM);
#if defined(OS_WIN)
    cmd_line.ParseFromString(::GetCommandLineW());
#elif defined(OS_POSIX)
    cmd_line.InitFromArgv(argc, argv);
#endif

	// check if --help CL argument is present
    if (cmd_line.HasSwitch("help")) {
        PrintHelp();
        return 0;
    }

    // check if --version CL argument is present
    if (cmd_line.HasSwitch("version")) {
      PrintVersion();
      return 0;
    }

    // check if --test_data_folder CL argument is present
    std::string testDataFolderSwitch = "test_data_folder";
    if (cmd_line.HasSwitch(testDataFolderSwitch)) {
        std::cout << "HasSwitch"<<std::endl;
      tests::testDataFolder = cmd_line.GetSwitchValueASCII(testDataFolderSwitch);
      std::cout << "HasSwitch "<< tests::testDataFolder << std::endl;
    } else {
        std::cout << "Hasn'tSwitch"<<std::endl;
        tests::testDataFolder = "./";
        std::cout << "Hasn'tSwitch"<<std::endl;
    }

#if defined(OS_WIN)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    system("qtenv2.bat vsvars");
#else  //QT_VERSION
    system("qtvars.bat vsvars");
#endif //QT_VERSION
#endif //OS_WIN

    webdriver::Server* wd_server = webdriver::Server::GetInstance();
    if (0 != wd_server->Configure(cmd_line)) {
        return 1;
    }

    webdriver::RouteTable *routeTableWithShutdownCommand = new webdriver::RouteTable(wd_server->GetRouteTable());
    const char shutdownCommandRoute[] = "/-CISCO-shutdown";
    routeTableWithShutdownCommand->Add<webdriver::ShutdownCommand>(shutdownCommandRoute);
    wd_server->SetRouteTable(routeTableWithShutdownCommand);

    InitVNCClient();
    InitUInputClient();

    int startError = wd_server->Start();
    if (startError)
        return startError;

    setQtSettings();
    return app.exec();
}

void setQtSettings() {
#if (WD_TEST_ENABLE_WEB_VIEW == 1)    
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->enablePersistentStorage("./web/html5");
    QWebSettings::globalSettings()->setLocalStoragePath("./web/html5");
    QWebSettings::globalSettings()->setOfflineStoragePath("./web/html5");
    QWebSettings::globalSettings()->setOfflineWebApplicationCachePath("./web/html5");
#endif    
}

void InitVNCClient() {
    // start VNC module
    CommandLine cmdLine = webdriver::Server::GetInstance()->GetCommandLine();
    if (cmdLine.HasSwitch(webdriver::Switches::kVNCLogin))
    {
        QString address = "127.0.0.1";
        QString login = "anonymous";
        QString *password = new QString();
        QString port = "5900";

        QString loginInfo = cmdLine.GetSwitchValueASCII(webdriver::Switches::kVNCLogin).c_str();
        VNCClient::SplitVncLoginParameters(loginInfo, &login, password, &address, &port);

        VNCClient *client = VNCClient::getInstance();
        if (!client->isReady())
        {
            if (password->isEmpty())
                client->Init(address, port.toInt());
            else
                client->Init(address, port.toInt(), password);
        }

        WDEventDispatcher::getInstance()->add(new VNCEventDispatcher(client));
    }
}

void InitUInputClient() {
    // start user input device
#ifdef OS_LINUX
    CommandLine cmdLine = webdriver::Server::GetInstance()->GetCommandLine();
    if (cmdLine.HasSwitch(webdriver::Switches::kUserInputDevice))
    {
        UInputManager *manager = UInputManager::getInstance();
        if (!manager->isReady())
        {
            manager->registerUinputDevice();
        }

        WDEventDispatcher::getInstance()->add(new UInputEventDispatcher(manager));
    }
#endif // OS_LINUX
}

void PrintVersion() {
    std::cout <<webdriver::VersionInfo::CreateVersionString()<< std::endl;
}

void PrintHelp() {
    std::cout << "Usage: WebDriver [--OPTION=VALUE]..."                                             << std::endl

                << "Starts QtWebDriver server"                                                        << std::endl
                << ""                                                                                 << std::endl
                << "OPTION         DEFAULT VALUE      DESCRIPTION"                                    << std::endl
                << "http-threads   4                  The number of threads to use for handling"      << std::endl
                << "                                  HTTP requests"                                  << std::endl
                << "log-path       ./webdriver.log    The path to use for the QtWebDriver server"     << std::endl
                << "                                  log"                                            << std::endl
                << "root           ./web              The path of location to serve files from"       << std::endl
                << "port           9517               The port that QtWebDriver listens on"           << std::endl
                << "silence        false              If true, QtWebDriver will not log anything"     << std::endl
                << "                                  to stdout/stderr"                               << std::endl
                << "verbose        false              If true, QtWebDriver will log lots of stuff"    << std::endl
                << "                                  to stdout/stderr"                               << std::endl
                << "url-base                          The URL path prefix to use for all incoming"    << std::endl
                << "                                  WebDriver REST requests. A prefix and postfix"  << std::endl
                << "                                  '/' will automatically be appended if not"      << std::endl
                << "                                  present"                                        << std::endl
                << "config                            The path to config file (e.g. config.json) in"  << std::endl
                << "                                  JSON format with specified WD parameters as"    << std::endl
                << "                                  described above (port, root, etc.)"             << std::endl
                << "wi-server      false              If true, web inspector will be enabled"         << std::endl
                << "wi-port        9222               Web inspector listening port"                   << std::endl
                << "version                           Print version information to stdout and exit"   << std::endl
                << "vnc-login      127.0.0.1:5900     VNC server login parameters"                    << std::endl
                << "                                  Fomat: login:password@ip:port"                  << std::endl;
}



