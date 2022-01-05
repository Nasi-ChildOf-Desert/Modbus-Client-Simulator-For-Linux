#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QtSerialBus/qtserialbusglobal.h>
#if QT_CONFIG(modbus_serialport)
#include <QSerialPort>
#endif

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    struct Settings {
#if QT_CONFIG(modbus_serialport)
        int parity = QSerialPort::EvenParity;
        int baud = QSerialPort::Baud19200;
        int dataBits = QSerialPort::Data8;
        int stopBits = QSerialPort::OneStop;

        int coilAddressFrom = 0 ;
        int coilAddressTo = 0;
        int coilDiscreteInputAddressFrom = 0 ;
        int coilDiscreteInputAddressTo = 0 ;
        int RegisterAddressFrom = 0 ;
        int RegisterAddressTo = 0 ;
        int HoldingRegAddressFrom = 0 ;
        int HoldingRegAddressTo = 0 ;

        bool hasCoils = false;
        bool hasDiscreteInputs = false;
        bool hasInputRegisters = false;
        bool hasHoldingRegisters = false;
#endif
    };

    explicit SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();

    Settings settings() const;
    void resetSetting();
    void disableAddressText();
public:
signals :
    void userSettingIsReady();

private slots:

    void on_hasCoil_stateChanged(int arg1);

    void on_hasRegister_stateChanged(int arg1);

    void on_hasDiscreteInput_stateChanged(int arg1);

    void on_hasHoldingReg_stateChanged(int arg1);

private:
    Settings m_settings;
    Ui::SettingDialog *ui;

};

#endif // SETTINGDIALOG_H
