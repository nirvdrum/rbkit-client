#include "rbkitmainwindow.h"
#include "ui_rbkitmainwindow.h"
#include "askhost.h"
#include "jsbridge.h"

RbkitMainWindow::RbkitMainWindow(QWidget *parent) :
    QMainWindow(parent), connected(false), host(""),
    ui(new Ui::RbkitMainWindow)
{
    this->connected = false;
    ui->setupUi(this);
    qRegisterMetaType<RBKit::ObjectStore>();
    qRegisterMetaType<RBKit::ObjectDetail>();

    QWebSettings *settings = ui->chartingView->settings();
    settings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    settings->setAttribute(QWebSettings::AutoLoadImages, true);
    settings->setMaximumPagesInCache(0);
    settings->setObjectCacheCapacities(0, 0, 0);
    settings->setIconDatabasePath("");

    settings->setAttribute(QWebSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebSettings::PluginsEnabled, false);
    settings->setAttribute(QWebSettings::JavaEnabled, false);
    settings->setAttribute(QWebSettings::PrivateBrowsingEnabled, false);
    settings->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
    settings->setAttribute(QWebSettings::JavascriptCanAccessClipboard, false);

    connect(ui->chartingView, SIGNAL(loadFinished(bool)), this, SLOT(onPageLoad(bool)));
    ui->chartingView->setUrl(QUrl("qrc:/web/index.html"));

    jsBridge = new RBKit::JsBridge();
}

RbkitMainWindow::~RbkitMainWindow()
{
    delete jsBridge;
    delete ui;
}


void RbkitMainWindow::on_action_Connect_triggered()
{
    qDebug() << "main-window" << connected;

    if (!connected) {
        setupSubscriber();
        askForServerInfo();
    } else {
        disconnectFromSocket();
    }
}

void RbkitMainWindow::on_action_Quit_triggered()
{
    disconnectFromSocket();
    close();
}

void RbkitMainWindow::askForServerInfo() {
    if(host.size() == 0) {
        this->askHost = new AskHost(this);
        connect(this->askHost, SIGNAL(userHasSelectedHost(QString, QString)),
                this, SLOT(useSelectedHost(QString, QString)));
        askHost->show();
    }
}

void RbkitMainWindow::useSelectedHost(QString commandsSocket, QString eventsSocket)
{
    askHost->close();
    qDebug() << "Emitting signal with " << eventsSocket;
    emit connectToSocket(commandsSocket, eventsSocket);
}

void RbkitMainWindow::on_action_About_Rbkit_triggered()
{
    QMessageBox msgBox(this);
    msgBox.setText(tr("Rbkit is low overhead profiler for Ruby, brought to you by Codemancers team"));
    msgBox.exec();
}

void RbkitMainWindow::onPageLoad(bool ok)
{
    QWebFrame *frame = ui->chartingView->page()->mainFrame();
    frame->addToJavaScriptWindowObject(QString("jsBridge"), jsBridge);
}

void RbkitMainWindow::objectDumpAvailable(const RBKit::ObjectStore& objectStore)
{
    HeapDumpForm *heapUI = new HeapDumpForm();
    heapUI->setObjectStore(objectStore);
    ui->chartingTab->addTab(heapUI, "Heap Dump #1");
}

void RbkitMainWindow::disconnectFromSocket()
{
    subscriberThread.requestInterruption();
    subscriberThread.exit();
    subscriberThread.wait();
}

void RbkitMainWindow::setupSubscriber()
{
    // move jsbridge to subscriber thread first.
    //jsBridge->moveToThread(&subscriberThread);

    //Create a subscriber and move it to it's own thread
    subscriber = new Subscriber(jsBridge);
    subscriber->moveToThread(&subscriberThread);

    //Events to/from parent/subcriber thread
    connect(&subscriberThread, &QThread::finished, subscriber, &QObject::deleteLater);
    connect(this, SIGNAL(connectToSocket(QString, QString)),
            subscriber, SLOT(startListening(QString, QString)));
    connect(this, SIGNAL(triggerGc()), subscriber, SLOT(triggerGc()));
    connect(this, SIGNAL(takeSnapshot()), subscriber, SLOT(takeSnapshot()));

    connect(this, SIGNAL(disconnectSubscriber()), subscriber, SLOT(stop()));

    connect(subscriber, &Subscriber::errored, this, &RbkitMainWindow::onError);
    connect(subscriber, &Subscriber::connected, this, &RbkitMainWindow::connectedToSocket);
    connect(subscriber, &Subscriber::disconnected, this, &RbkitMainWindow::disconnectedFromSocket);
    connect(subscriber, &Subscriber::objectDumpAvailable, this, &RbkitMainWindow::objectDumpAvailable);

    subscriberThread.start();
}

void RbkitMainWindow::disconnectedFromSocket()
{
    ui->action_Connect->setText(tr("&Connect"));
    this->connected = false;
    emit disconnectSubscriber();
}


void RbkitMainWindow::connectedToSocket()
{
    ui->action_Connect->setText(tr("&Disconnect"));
    this->connected = true;
}

void RbkitMainWindow::onError(const QString &error)
{
    disconnectFromSocket();
    QMessageBox::critical(this, tr("rbkit"), error);
}

void RbkitMainWindow::on_action_Trigger_GC_triggered()
{
    emit triggerGc();
}

void RbkitMainWindow::on_actionHeap_Snapshot_triggered()
{
    emit takeSnapshot();
}