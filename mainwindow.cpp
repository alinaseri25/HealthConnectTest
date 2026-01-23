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
    connect(ui->BtnExit,&QPushButton::clicked,this,&MainWindow::onBtnExitClicked);
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
    QString status;
    QJniObject result;

    series1 = new QLineSeries(this);
    series2 = new QLineSeries(this);

    series1->setName(QString("نمودار قد"));
    series2->setName(QString("نمودار وزن"));


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
    QJsonDocument *document = new QJsonDocument(QJsonDocument::fromJson(status.toUtf8()));
    QJsonArray arr = document->array();
    for(uint32_t i = 0 ; i < arr.size() ; i++)
    {
        QJsonObject obj = arr.at(i).toObject();
        QDateTime dateTime = QDateTime::fromString(obj["time"].toString(), Qt::ISODate);
        ui->txtData->append(QString("قد : %1 و زمان ثبت : %2").arg(obj["height_m"].toDouble()).arg(dateTime.toString(QString("yyyy/mm/dd hh:MM:ss"))));
        series1->append(dateTime.toSecsSinceEpoch(),(obj["height_m"].toDouble() * 100));
    }

    //Weight
    result = QJniObject::callStaticObjectMethod(
        "org/verya/HealthConnectTest/HealthBridge",
        "readWeight",
        "()Ljava/lang/String;"
        );

    status = result.toString();
    document = new QJsonDocument(QJsonDocument::fromJson(status.toUtf8()));
    arr = document->array();
    for(uint32_t i = 0 ; i < arr.size() ; i++)
    {
        QJsonObject obj = arr.at(i).toObject();
        QDateTime dateTime = QDateTime::fromString(obj["time"].toString(), Qt::ISODate);
        ui->txtData->append(QString("وزن : %1 و زمان ثبت : %2").arg(obj["weight_kg"].toDouble()).arg(dateTime.toString(QString("yyyy/mm/dd hh:MM:ss"))));
        series2->append(dateTime.toSecsSinceEpoch(),obj["weight_kg"].toDouble());
    }
    ui->txtData->append(document->toJson());

    // ui->chart->chart()->addSeries(series1);
    // ui->chart->chart()->addSeries(series2);

    QChart *chart = new QChart();

    // محور Y برای سری اول
    QValueAxis *axisY1 = new QValueAxis();
    axisY1->setTitleText("قد");
    axisY1->setRange(170, 190);
    chart->addAxis(axisY1, Qt::AlignLeft);

    // محور Y برای سری اول
    QValueAxis *axisY2 = new QValueAxis();
    axisY2->setTitleText("وزن");
    axisY2->setRange(75, 95);
    chart->addAxis(axisY2, Qt::AlignRight);


    chart->addSeries(series1);
    chart->addSeries(series2);


    // اتصال سری 1 به محور X اصلی و Y اصلی (سمت چپ)
    chart->series().at(0)->attachAxis(axisY1);

    // اتصال سری 2 به محور X اصلی و Y ثانویه (سمت راست)
    chart->series().at(1)->attachAxis(axisY2);

    ui->chart->setChart(chart);
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

    // Check permissions
    QJniObject result = QJniObject::callStaticObjectMethod(
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

void MainWindow::onBtnExitClicked()
{
    QApplication::exit(0);
}
