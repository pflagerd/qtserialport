/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <scapig@yandex.ru>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QtSerialPort/serialportinfo.h>
#include <QIntValidator>
#include <QLineEdit>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    intValidator = new QIntValidator(0, 4000000, this);

    ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(ui->applyButton, SIGNAL(clicked()),
            this, SLOT(apply()));
    connect(ui->portsBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(showPortInfo(int)));
    connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomBaudRatePolicy(int)));

    fillPortsParameters();
    fillPortsInfo();

    updateSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

SettingsDialog::Settings SettingsDialog::settings() const
{
    return currentSettings;
}

void SettingsDialog::showPortInfo(int idx)
{
    if (idx != -1) {
        QStringList list = ui->portsBox->itemData(idx).toStringList();
        ui->descriptionLabel->setText(tr("Description: %1").arg(list.at(1)));
        ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.at(2)));
        ui->locationLabel->setText(tr("Location: %1").arg(list.at(3)));
        ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.at(4)));
        ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.at(5)));
    }
}

void SettingsDialog::apply()
{
    updateSettings();
    hide();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    ui->baudRateBox->setEditable(idx == 4);
    if (idx == 4) {
        ui->baudRateBox->clearEditText();
        QLineEdit *edit = ui->baudRateBox->lineEdit();
        edit->setValidator(intValidator);
    }
}

void SettingsDialog::fillPortsParameters()
{
    // fill baud rate (is not the entire list of available values,
    // desired values??, add your independently)
    ui->baudRateBox->addItem(QLatin1String("9600"), SerialPort::Baud9600);
    ui->baudRateBox->addItem(QLatin1String("19200"), SerialPort::Baud19200);
    ui->baudRateBox->addItem(QLatin1String("38400"), SerialPort::Baud38400);
    ui->baudRateBox->addItem(QLatin1String("115200"), SerialPort::Baud115200);
    ui->baudRateBox->addItem(QLatin1String("Custom"));

    // fill data bits
    ui->dataBitsBox->addItem(QLatin1String("5"), SerialPort::Data5);
    ui->dataBitsBox->addItem(QLatin1String("6"), SerialPort::Data6);
    ui->dataBitsBox->addItem(QLatin1String("7"), SerialPort::Data7);
    ui->dataBitsBox->addItem(QLatin1String("8"), SerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    // fill parity
    ui->parityBox->addItem(QLatin1String("None"), SerialPort::NoParity);
    ui->parityBox->addItem(QLatin1String("Even"), SerialPort::EvenParity);
    ui->parityBox->addItem(QLatin1String("Odd"), SerialPort::OddParity);
    ui->parityBox->addItem(QLatin1String("Mark"), SerialPort::MarkParity);
    ui->parityBox->addItem(QLatin1String("Space"), SerialPort::SpaceParity);

    // fill stop bits
    ui->stopBitsBox->addItem(QLatin1String("1"), SerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(QLatin1String("1.5"), SerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QLatin1String("2"), SerialPort::TwoStop);

    // fill flow control
    ui->flowControlBox->addItem(QLatin1String("None"), SerialPort::NoFlowControl);
    ui->flowControlBox->addItem(QLatin1String("RTS/CTS"), SerialPort::HardwareControl);
    ui->flowControlBox->addItem(QLatin1String("XON/XOFF"), SerialPort::SoftwareControl);
}

void SettingsDialog::fillPortsInfo()
{
    ui->portsBox->clear();
    foreach (const SerialPortInfo &info, SerialPortInfo::availablePorts()) {
        QStringList list;
        list << info.portName() << info.description()
             << info.manufacturer() << info.systemLocation()
             << info.vendorIdentifier() << info.productIdentifier();

        ui->portsBox->addItem(list.first(), list);
    }
}

void SettingsDialog::updateSettings()
{
    currentSettings.name = ui->portsBox->currentText();

    // Baud Rate
    if (ui->baudRateBox->currentIndex() == 4) {
        // custom baud rate
        currentSettings.baudRate = ui->baudRateBox->currentText().toInt();
    } else {
        // standard baud rate
        currentSettings.baudRate = static_cast<SerialPort::BaudRate>(
                    ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    }
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);

    // Data bits
    currentSettings.dataBits = static_cast<SerialPort::DataBits>(
                ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    currentSettings.stringDataBits = ui->dataBitsBox->currentText();

    // Parity
    currentSettings.parity = static_cast<SerialPort::Parity>(
                ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    currentSettings.stringParity = ui->parityBox->currentText();

    // Stop bits
    currentSettings.stopBits = static_cast<SerialPort::StopBits>(
                ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    currentSettings.stringStopBits = ui->stopBitsBox->currentText();

    // Flow control
    currentSettings.flowControl = static_cast<SerialPort::FlowControl>(
                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    currentSettings.stringFlowControl = ui->flowControlBox->currentText();
}
