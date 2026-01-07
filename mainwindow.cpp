#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->btnRead,&QPushButton::clicked,this,&MainWindow::onBtnReadClicked);
    connect(ui->BtnCopy,&QPushButton::clicked,this,&MainWindow::onBtnCopyClicked);
    connect(ui->BtnGetPremission,&QPushButton::clicked,this,&MainWindow::onBtnGetPremissionClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readHealth()
{
#ifdef Q_OS_ANDROID
    QJniObject context = QNativeInterface::QAndroidApplication::context();
    if (!context.isValid()) {
        ui->txtData->append("Context is invalid!");
        return;
    }

    // init(context) → returns void → signature = (Landroid/content/Context;)V
    QJniObject::callStaticMethod<void>(
        "org/verya/HealthConnectTest/HealthBridge",
        "init",
        "(Landroid/content/Context;)V",
        context.object()
        );

    ui->txtData->append("init OK");


    QJniObject result = QJniObject::callStaticObjectMethod(
        "org/verya/HealthConnectTest/HealthBridge",
        "isInitialized",
        "()Ljava/lang/String;"
        );

    QString status = result.toString();
    ui->txtData->append("Init status = " + status);

    result = QJniObject::callStaticObjectMethod(
        "org/verya/HealthConnectTest/HealthBridge",
        "getAvailability",
        "()Ljava/lang/String;"
        );

    status = result.toString();
    ui->txtData->append("HC availability = " + status);

    // debugInfo
    result = QJniObject::callStaticObjectMethod(
        "org/verya/HealthConnectTest/HealthBridge",
        "debugInfo",
        "()Ljava/lang/String;"
        );
    ui->txtData->append("Debug info = " + result.toString());

    // checkPermissions
    result = QJniObject::callStaticObjectMethod(
        "org/verya/HealthConnectTest/HealthBridge",
        "checkPermissions",
        "()Ljava/lang/String;"
        );
    ui->txtData->append("Permissions = " + result.toString());

    //Height
    result = QJniObject::callStaticObjectMethod(
        "org/verya/HealthConnectTest/HealthBridge",
        "readHeight",
        "()Ljava/lang/String;"
        );

    status = result.toString();
    ui->txtData->append(status);

    //Weight
    result = QJniObject::callStaticObjectMethod(
        "org/verya/HealthConnectTest/HealthBridge",
        "readWeight",
        "()Ljava/lang/String;"
        );

    status = result.toString();
    ui->txtData->append(status);

    QJniObject dump = QJniObject::callStaticObjectMethod(
        "org/verya/HealthConnectTest/HealthBridge",
        "dumpAllHCClasses",
        "()Ljava/lang/String;"
        );
    ui->txtData->append(dump.toString());
#else
    qDebug() << "Not Android";
#endif
}

void MainWindow::callTest()
{
#ifdef Q_OS_ANDROID
    // گرفتن context اندروید
    QJniObject context = QNativeInterface::QAndroidApplication::context();
    if (!context.isValid()) {
        //qDebug() << "Context is invalid!";
        ui->txtData->append(QString("Context is invalid!"));
        return;
    }


    QJniObject result = QJniObject::callStaticObjectMethod(
        "org/verya/HealthConnectTest/HealthBridge",
        "testCall",
        "()Ljava/lang/String;"
        );

    if (!result.isValid()) {
        //qDebug() << "JNI error: result is invalid";
        ui->txtData->append(QString("JNI error: result is invalid"));
        return;
    }

    QString json = result.toString();
    //qDebug() << "HC JSON =" << json;
    ui->txtData->append(QString("HC JSON = %1").arg(json));
#else
    qDebug() << "Not Android";
#endif
}

void MainWindow::onBtnReadClicked()
{
    //QMessageBox::about(this,QString("Tittle"),QString("hello"));
    ui->txtData->append(QString("test from BTN"));
    callTest();
    readHealth();
}

void MainWindow::onBtnCopyClicked()
{
    QClipboard *clipBoard = QApplication::clipboard();
    clipBoard->setText(ui->txtData->toPlainText());
}

void MainWindow::onBtnGetPremissionClicked()
{
#ifdef Q_OS_ANDROID
    // ✅ دریافت Activity (نه Application Context)
    QJniObject activity = QNativeInterface::QAndroidApplication::context();

    if (!activity.isValid()) {
        ui->txtData->append("❌ Activity is invalid!");
        return;
    }

    // Init
    QJniObject::callStaticMethod<void>(
        "org/verya/HealthConnectTest/HealthBridge",
        "init",
        "(Landroid/content/Context;)V",
        activity.object()
        );

    ui->txtData->append("=== 🔐 Health Connect Test ===\n");

    // Debug info
    QJniObject result = QJniObject::callStaticObjectMethod(
        "org/verya/HealthConnectTest/HealthBridge",
        "debugInfo",
        "()Ljava/lang/String;"
        );
    ui->txtData->append("📋 Debug: " + result.toString());

    // Check permissions
    result = QJniObject::callStaticObjectMethod(
        "org/verya/HealthConnectTest/HealthBridge",
        "checkPermissions",
        "()Ljava/lang/String;"
        );
    ui->txtData->append("🔑 Current: " + result.toString());

    // ✅ Request permissions با پاس دادن Activity
    ui->txtData->append("\n🚀 Requesting permissions...");
    result = QJniObject::callStaticObjectMethod(
        "org/verya/HealthConnectTest/HealthBridge",
        "requestPermissions",
        "(Landroid/app/Activity;)Ljava/lang/String;",
        activity.object()
        );

    ui->txtData->append("✅ Result: " + result.toString());
    ui->txtData->append("\n💡 If dialog appeared, grant permissions then press Read.");

#else
    qDebug() << "Not Android";
#endif
}
