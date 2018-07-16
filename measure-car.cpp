#include <QCoreApplication>
#include <QSerialPort>

#include "measure-car.h"

MeasureCar::MeasureCar(QString portname, QObject *parent)
	: QObject(parent) {
	m_serialPort.setBaudRate(9600);
	m_serialPort.setFlowControl(QSerialPort::FlowControl::HardwareControl);
	m_serialPort.setPortName(portname);

	connect(&m_serialPort, &QSerialPort::readyRead, this, &MeasureCar::handleReadyRead);
	connect(&m_serialPort, &QSerialPort::errorOccurred, this, &MeasureCar::handleError);

	if (!m_serialPort.open(QIODevice::ReadOnly))
		throw EOpenError(m_serialPort.errorString());
}

void MeasureCar::handleReadyRead() {
	m_readData.append(m_serialPort.readAll());
}

void MeasureCar::handleError(QSerialPort::SerialPortError serialPortError) {
	(void)serialPortError;
	onError(m_serialPort.errorString());
}
