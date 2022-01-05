#include "SettingDialog.h"
#include "ui_SettingDialog.h"
#include "QDebug"

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    disableAddressText();
    ui->parityCombo->setCurrentIndex(1);
#if QT_CONFIG(modbus_serialport)
    ui->baudCombo->setCurrentText(QString::number(m_settings.baud));
    ui->dataBitsCombo->setCurrentText(QString::number(m_settings.dataBits));
    ui->stopBitsCombo->setCurrentText(QString::number(m_settings.stopBits));
#endif

    connect(ui->applyButton, &QPushButton::clicked, [this]() {
#if QT_CONFIG(modbus_serialport)
        m_settings.parity = ui->parityCombo->currentIndex();
        if (m_settings.parity > 0)
            m_settings.parity++;
        m_settings.baud = ui->baudCombo->currentText().toInt();
        m_settings.dataBits = ui->dataBitsCombo->currentText().toInt();
        m_settings.stopBits = ui->stopBitsCombo->currentText().toInt();

        if(ui->hasCoil->isChecked())
        {
            m_settings.coilAddressFrom = ui->coilAddressFrom->text().toInt();
            m_settings.coilAddressTo = ui->coilAddressTo->text().toInt();
             qDebug() << "ui->coilAddressFrom->text().toInt()" << ui->coilAddressFrom->text().toInt();
             qDebug() << " m_settings.coilAddressFrom" <<   m_settings.coilAddressFrom;
        }
        if(ui->hasDiscreteInput->isChecked())
        {
            m_settings.coilDiscreteInputAddressFrom = ui->DiscretecoilAddressFrom->text().toInt();
            m_settings.coilDiscreteInputAddressTo = ui->DiscretecoilAddressTo->text().toInt();
        }
        if(ui->hasRegister->isChecked())
        {
            m_settings.RegisterAddressFrom = ui->RegisterAddressFrom->text().toInt();
            m_settings.RegisterAddressTo = ui->RegisterAddressTo->text().toInt();
        }
        if(ui->hasHoldingReg->isChecked())
        {
            m_settings.HoldingRegAddressFrom = ui->holdingRegAddressFrom->text().toInt();
            m_settings.HoldingRegAddressTo = ui->holdingRegAddressTo->text().toInt();
        }
#endif
        emit userSettingIsReady();
        hide();
    });
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

SettingDialog::Settings SettingDialog::settings() const
{
    return m_settings;
}

void SettingDialog::resetSetting()
{
    m_settings.parity = QSerialPort::EvenParity;
    m_settings.baud = QSerialPort::Baud19200;
    m_settings.dataBits = QSerialPort::Data8;
    m_settings.stopBits = QSerialPort::OneStop;

    m_settings.coilAddressFrom = 0 ;
    m_settings.coilAddressTo = 0;
    m_settings.coilDiscreteInputAddressFrom = 0 ;
    m_settings.coilDiscreteInputAddressTo = 0 ;
    m_settings.RegisterAddressFrom = 0 ;
    m_settings.RegisterAddressTo = 0 ;
    m_settings.HoldingRegAddressFrom = 0 ;
    m_settings.HoldingRegAddressTo = 0 ;

     m_settings.hasCoils = false;
     m_settings.hasDiscreteInputs = false;
     m_settings.hasInputRegisters = false;
     m_settings.hasHoldingRegisters = false;

     ui->coilAddressFrom->clear();
     ui->coilAddressTo->clear();
     ui->RegisterAddressFrom->clear();
     ui->RegisterAddressTo->clear();
     ui->DiscretecoilAddressFrom->clear();
     ui->DiscretecoilAddressTo->clear();
     ui->holdingRegAddressFrom->clear();
     ui->holdingRegAddressTo->clear();

     disableAddressText();
}

void SettingDialog::disableAddressText()
{
    ui->coilAddressFrom->setEnabled(false);
    ui->coilAddressTo->setEnabled(false);
    ui->RegisterAddressFrom->setEnabled(false);
    ui->RegisterAddressTo->setEnabled(false);
    ui->DiscretecoilAddressFrom->setEnabled(false);
    ui->DiscretecoilAddressTo->setEnabled(false);
    ui->holdingRegAddressFrom->setEnabled(false);
    ui->holdingRegAddressTo->setEnabled(false);
    ui->hasCoil->setChecked(false);
    ui->hasRegister->setChecked(false);
    ui->hasDiscreteInput->setChecked(false);
    ui->hasHoldingReg->setChecked(false);

}


void SettingDialog::on_hasCoil_stateChanged(int arg1)
{
    if(arg1)
    {
        m_settings.hasCoils = true;
        ui->coilAddressFrom->clear();
        ui->coilAddressTo->clear();
        ui->coilAddressFrom->setEnabled(true);
        ui->coilAddressTo->setEnabled(true);
    }
    else
    {
        m_settings.hasCoils = false;
        ui->coilAddressFrom->clear();
        ui->coilAddressTo->clear();
        ui->coilAddressFrom->setEnabled(false);
        ui->coilAddressTo->setEnabled(false);
    }
}


void SettingDialog::on_hasRegister_stateChanged(int arg1)
{
    if(arg1)
    {
        m_settings.hasInputRegisters = true;
        ui->RegisterAddressFrom->clear();
        ui->RegisterAddressTo->clear();
        ui->RegisterAddressFrom->setEnabled(true);
        ui->RegisterAddressTo->setEnabled(true);
    }
    else
    {
        m_settings.hasInputRegisters = false;
        ui->RegisterAddressFrom->clear();
        ui->RegisterAddressTo->clear();
        ui->RegisterAddressFrom->setEnabled(false);
        ui->RegisterAddressTo->setEnabled(false);
    }
}

void SettingDialog::on_hasDiscreteInput_stateChanged(int arg1)
{
    if(arg1)
    {
        m_settings.hasDiscreteInputs = true;
        ui->DiscretecoilAddressFrom->clear();
        ui->DiscretecoilAddressTo->clear();
        ui->DiscretecoilAddressFrom->setEnabled(true);
        ui->DiscretecoilAddressTo->setEnabled(true);
    }
    else
    {
        m_settings.hasDiscreteInputs = false;
        ui->DiscretecoilAddressFrom->clear();
        ui->DiscretecoilAddressTo->clear();
        ui->DiscretecoilAddressFrom->setEnabled(false);
        ui->DiscretecoilAddressTo->setEnabled(false);
    }
}

void SettingDialog::on_hasHoldingReg_stateChanged(int arg1)
{
    if(arg1)
    {
        m_settings.hasHoldingRegisters = true;
        ui->holdingRegAddressFrom->clear();
        ui->holdingRegAddressTo->clear();
        ui->holdingRegAddressFrom->setEnabled(true);
        ui->holdingRegAddressTo->setEnabled(true);
    }
    else
    {
        m_settings.hasHoldingRegisters = false;
        ui->holdingRegAddressFrom->clear();
        ui->holdingRegAddressTo->clear();
        ui->holdingRegAddressFrom->setEnabled(false);
        ui->holdingRegAddressTo->setEnabled(false);
    }
}
