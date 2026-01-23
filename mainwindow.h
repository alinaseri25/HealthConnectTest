#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QClipboard>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QChart>
#include <QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLineSeries *series1,*series2;

    void readHealth(void);
    void callTest(void);

private slots:
    void onBtnReadClicked(void);
    void onBtnCopyClicked(void);
    void onBtnGetPremissionClicked(void);
    void onBtnExitClicked(void);
};
#endif // MAINWINDOW_H
