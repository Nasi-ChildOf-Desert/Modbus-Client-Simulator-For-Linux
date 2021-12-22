#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusServer>
#include <QButtonGroup>
#include "QLineEdit"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void coilChanged(int id);
    void discreteInputChanged(int id);
    void bitChanged(int id, QModbusDataUnit::RegisterType table, bool value);
    void setRegister(const QString &value);
    void updateWidgets(QModbusDataUnit::RegisterType table, int address, int size);

    void on_AddCoil_clicked();
    void on_AddDiscreteInput_clicked();
    void on_AddRegister_clicked();
    void on_AddHoldingReg_clicked();

    void on_connectButton_clicked();
    void onStateChanged(int state);
    void handleDeviceError(QModbusDevice::Error newError);

private:
    void  init();
    void setupWidgetContainers();
    void setupDeviceData();

    Ui::MainWindow *ui;
    QModbusServer *modbusDevice;
    QButtonGroup coilButtons;
    QButtonGroup discreteButtons;
    QHash<QString, QLineEdit *> registers;

    int coilAddressFrom;
    int coilAddressTo ;
    int coilDiscreteInputAddressFrom ;
    int coilDiscreteInputAddressTo ;
    int RegisterAddressFrom ;
    int RegisterAddressTo ;
    int HoldingRegAddressFrom ;
    int HoldingRegAddressTo ;

};
#endif // MAINWINDOW_H
