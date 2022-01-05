#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "QScrollArea"
#include "QVBoxLayout"
#include "QFormLayout"
#include "QCheckBox"
#include <QSerialPort>
#include <QModbusRtuSerialSlave>
#include "QtDebug"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settingForm = new SettingDialog(this);
    settingForm->show();
    connect(settingForm, &SettingDialog::userSettingIsReady, this, &MainWindow::prepareFormAddress) ;

}

//***********************************************************
MainWindow::~MainWindow()
{
    if (modbusDevice)
        modbusDevice->disconnectDevice();
    delete modbusDevice;
    delete ui;
}

//***********************************************************

void MainWindow::addCoils()
{

    coilAddressFrom = settingForm->settings().coilAddressFrom;
    coilAddressTo = settingForm->settings().coilAddressTo;

    QVBoxLayout* lay = new QVBoxLayout();
    for (int i = coilAddressFrom ; i < coilAddressTo; ++i) {
        QCheckBox *dynamic = new QCheckBox(QString::number(i));
        dynamic->setObjectName("coils_" + QString::number(i));
        lay->addWidget(dynamic);
    }
    ui->scrollAreaWidgetContents->setLayout(lay);
}

//***********************************************************
void MainWindow::AddDiscreteInput()
{
    coilDiscreteInputAddressFrom = settingForm->settings().coilDiscreteInputAddressFrom;
    coilDiscreteInputAddressTo = settingForm->settings().coilDiscreteInputAddressTo;
    QVBoxLayout* lay = new QVBoxLayout();
    for (int i = coilDiscreteInputAddressFrom ; i < coilDiscreteInputAddressTo; ++i) {
        QCheckBox *dynamic = new QCheckBox(QString::number(i));
        dynamic->setObjectName("disc_" + QString::number(i));
        lay->addWidget(dynamic);
    }
    ui->scrollAreaWidgetContents_2->setLayout(lay);
}

//***********************************************************
void MainWindow::AddRegister()
{
    RegisterAddressFrom = settingForm->settings().RegisterAddressFrom;
    RegisterAddressTo = settingForm->settings().RegisterAddressTo;
    QGridLayout * lay = new QGridLayout ();
    for (int i = RegisterAddressFrom ; i < RegisterAddressTo; ++i) {
        QLabel *label = new QLabel(QString::number(i));
        QLineEdit *dynamic = new QLineEdit();
        dynamic->setObjectName("inReg_" + QString::number(i));
        lay->addWidget(label,i , 0);
        lay->addWidget(dynamic, i, 1);
    }
    ui->scrollAreaWidgetContents_3->setLayout(lay);
}

//***********************************************************
void MainWindow::AddHoldingReg()
{
    HoldingRegAddressFrom = settingForm->settings().HoldingRegAddressFrom;
    HoldingRegAddressTo = settingForm->settings().HoldingRegAddressTo;
    QGridLayout * lay = new QGridLayout ();
    for (int i = HoldingRegAddressFrom ; i < HoldingRegAddressTo; ++i) {
        QLabel *label = new QLabel(QString::number(i));
        QLineEdit *dynamic = new QLineEdit();
        dynamic->setObjectName("holdReg_" + QString::number(i));
        lay->addWidget(label,i , 0);
        lay->addWidget(dynamic, i, 1);
    }
    ui->scrollAreaWidgetContents_4->setLayout(lay);
}

//***********************************************************
void MainWindow::init()
{
    /*  if (modbusDevice) {
        modbusDevice->disconnect();
        delete modbusDevice;
        modbusDevice = nullptr;
    }*/
#if QT_CONFIG(modbus_serialport)
    modbusDevice = new QModbusRtuSerialSlave(this);
#endif

    if (!modbusDevice) {
        ui->connectButton->setDisabled(true);
        statusBar()->showMessage(tr("Could not create Modbus slave."), 5000);
    } else {
        QModbusDataUnitMap reg;

        hasCoils = settingForm->settings().hasCoils;
        hasDiscreteInputs = settingForm->settings().hasDiscreteInputs;
        hasInputRegisters = settingForm->settings().hasInputRegisters;
        hasHoldingRegisters = settingForm->settings().hasHoldingRegisters;

        if(hasCoils)
        {
            reg.insert(QModbusDataUnit::Coils, { QModbusDataUnit::Coils, coilAddressFrom, static_cast<quint16>(coilAddressTo - coilAddressFrom)});
        }
        if(hasDiscreteInputs)
        {
            reg.insert(QModbusDataUnit::DiscreteInputs, { QModbusDataUnit::DiscreteInputs, coilDiscreteInputAddressFrom, static_cast<quint16>(coilDiscreteInputAddressTo - coilDiscreteInputAddressFrom) });
        }
        if(hasInputRegisters)
        {
            reg.insert(QModbusDataUnit::InputRegisters, { QModbusDataUnit::InputRegisters, RegisterAddressFrom, static_cast<quint16>(RegisterAddressTo - RegisterAddressFrom) });
        }
        if(hasHoldingRegisters)
        {
            qDebug() <<HoldingRegAddressFrom;
            reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, HoldingRegAddressFrom, static_cast<quint16>(HoldingRegAddressTo - HoldingRegAddressFrom) });
        }

        modbusDevice->setMap(reg);
        connect(modbusDevice, &QModbusServer::dataWritten,
                this, &MainWindow::updateWidgets);
        connect(modbusDevice, &QModbusServer::stateChanged,
                this, &MainWindow::onStateChanged);
        connect(modbusDevice, &QModbusServer::errorOccurred,
                this, &MainWindow::handleDeviceError);
    }

}

//***********************************************************
void MainWindow::setupWidgetContainers()
{
    coilButtons.setExclusive(false);
    discreteButtons.setExclusive(false);

    QRegularExpression regexp(QStringLiteral("coils_(?<ID>\\d+)"));
    const QList<QCheckBox *> coils = ui->scrollArea->findChildren<QCheckBox *>(regexp);
    for (QCheckBox *cbx : coils)
        coilButtons.addButton(cbx, regexp.match(cbx->objectName()).captured("ID").toInt());
    connect(&coilButtons, SIGNAL(buttonClicked(int)), this, SLOT(coilChanged(int)));

    regexp.setPattern(QStringLiteral("disc_(?<ID>\\d+)"));
    const QList<QCheckBox *> discs = findChildren<QCheckBox *>(regexp);
    for (QCheckBox *cbx : discs)
        discreteButtons.addButton(cbx, regexp.match(cbx->objectName()).captured("ID").toInt());
    connect(&discreteButtons, SIGNAL(buttonClicked(int)), this, SLOT(discreteInputChanged(int)));

    regexp.setPattern(QLatin1String("(in|hold)Reg_(?<ID>\\d+)"));
    const QList<QLineEdit *> qle = findChildren<QLineEdit *>(regexp);
    for (QLineEdit *lineEdit : qle) {
        registers.insert(lineEdit->objectName(), lineEdit);
        lineEdit->setProperty("ID", regexp.match(lineEdit->objectName()).captured("ID").toInt());
        lineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral("[0-9a-f]{0,4}"),
                                                                                  QRegularExpression::CaseInsensitiveOption), this));
        connect(lineEdit, &QLineEdit::textChanged, this, &MainWindow::setRegister);
    }
}

//***********************************************************
void MainWindow::updateWidgets(QModbusDataUnit::RegisterType table, int address, int size)
{
    for (int i = 0; i < size; ++i) {
        quint16 value;
        QString text;
        switch (table) {
        case QModbusDataUnit::Coils:
            modbusDevice->data(QModbusDataUnit::Coils, address + i, &value);
            coilButtons.button(address + i)->setChecked(value);
            break;
        case QModbusDataUnit::HoldingRegisters:
            modbusDevice->data(QModbusDataUnit::HoldingRegisters, address + i, &value);
            registers.value(QStringLiteral("holdReg_%1").arg(address + i))->setText(text
                                                                                    .setNum(value, 16));
            break;
        default:
            break;
        }
    }
}

//***********************************************************
void MainWindow::onStateChanged(int state)
{

    if (state == QModbusDevice::UnconnectedState)
        ui->connectButton->setText(tr("Connect"));
    else if (state == QModbusDevice::ConnectedState)
        ui->connectButton->setText(tr("Disconnect"));
}

//***********************************************************
void MainWindow::handleDeviceError(QModbusDevice::Error newError)
{
    if (newError == QModbusDevice::NoError || !modbusDevice)
        return;

    statusBar()->showMessage(modbusDevice->errorString(), 5000);
}

//***********************************************************
void MainWindow::prepareFormAddress()
{
    addCoils();
    AddDiscreteInput();
    AddRegister();
    AddHoldingReg();
}

//***********************************************************
void MainWindow::setupDeviceData()
{
    if (!modbusDevice)
        return;

    for (int i = 0; i < coilButtons.buttons().count(); ++i)
        modbusDevice->setData(QModbusDataUnit::Coils, i, coilButtons.button(i)->isChecked());

    for (int i = 0; i < discreteButtons.buttons().count(); ++i) {
        modbusDevice->setData(QModbusDataUnit::DiscreteInputs, i,
                              discreteButtons.button(i)->isChecked());
    }

    bool ok;
    for (QLineEdit *widget : qAsConst(registers)) {
        if (widget->objectName().startsWith(QStringLiteral("inReg"))) {
            modbusDevice->setData(QModbusDataUnit::InputRegisters, widget->property("ID").toInt(),
                                  widget->text().toInt(&ok, 16));
        } else if (widget->objectName().startsWith(QStringLiteral("holdReg"))) {
            modbusDevice->setData(QModbusDataUnit::HoldingRegisters, widget->property("ID").toInt(),
                                  widget->text().toInt(&ok, 16));
        }
    }
}


//***********************************************************
void MainWindow::on_connectButton_clicked()
{
    init();
    setupWidgetContainers();

    bool intendToConnect = (modbusDevice->state() == QModbusDevice::UnconnectedState);

    statusBar()->clearMessage();
    if (intendToConnect)
    {
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
                                             ui->portEdit->text());
#if QT_CONFIG(modbus_serialport)

        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,
                                             QSerialPort::EvenParity);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                                             QSerialPort::Baud19200);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
                                             QSerialPort::Data8);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
                                             QSerialPort::OneStop);
#endif

        modbusDevice->setServerAddress(ui->serverEdit->text().toInt());
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }
        setupDeviceData();
    }
    else {
        modbusDevice->disconnectDevice();
    }
}
//***********************************************************

void MainWindow::coilChanged(int id)
{
    QAbstractButton *button = coilButtons.button(id);
    bitChanged(id, QModbusDataUnit::Coils, button->isChecked());
}

//***********************************************************
void MainWindow::discreteInputChanged(int id)
{
    QAbstractButton *button = discreteButtons.button(id);
    bitChanged(id, QModbusDataUnit::DiscreteInputs, button->isChecked());
}

//***********************************************************
void MainWindow::bitChanged(int id, QModbusDataUnit::RegisterType table, bool value)
{
    if (!modbusDevice)
        return;

    if (!modbusDevice->setData(table, id, value))
        statusBar()->showMessage(tr("Could not set data: ") + modbusDevice->errorString(), 5000);
}

//***********************************************************
void MainWindow::setRegister(const QString &value)
{
    if (!modbusDevice)
        return;

    const QString objectName = QObject::sender()->objectName();
    if (registers.contains(objectName)) {
        bool ok = true;
        const int id = QObject::sender()->property("ID").toInt();
        if (objectName.startsWith(QStringLiteral("inReg")))
            ok = modbusDevice->setData(QModbusDataUnit::InputRegisters, id, value.toInt(&ok, 16));
        else if (objectName.startsWith(QStringLiteral("holdReg")))
            ok = modbusDevice->setData(QModbusDataUnit::HoldingRegisters, id, value.toInt(&ok, 16));

        if (!ok)
            statusBar()->showMessage(tr("Could not set register: ") + modbusDevice->errorString(),
                                     5000);
    }
}

void MainWindow::on_actionSetting_triggered()
{
    // if (modbusDevice)
    //  modbusDevice->disconnectDevice();
    settingForm->resetSetting();

    qDeleteAll(ui->scrollAreaWidgetContents->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));
    qDeleteAll(ui->scrollAreaWidgetContents_2->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));
    qDeleteAll(ui->scrollAreaWidgetContents_3->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));
    qDeleteAll(ui->scrollAreaWidgetContents_4->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly));
    delete  ui->scrollAreaWidgetContents->layout();
    delete  ui->scrollAreaWidgetContents_2->layout();
    delete  ui->scrollAreaWidgetContents_3->layout();
    delete  ui->scrollAreaWidgetContents_4->layout();
    settingForm->show();
}
