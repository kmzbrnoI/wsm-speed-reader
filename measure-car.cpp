#include <QCoreApplication>

#include "measure-car.h"

MeasureCar::MeasureCar(QSerialPort *serialPort, QObject *parent) :
	QObject(parent),
	m_serialPort(serialPort) {
	connect(m_serialPort, &QSerialPort::readyRead, this, &MeasureCar::handleReadyRead);
	connect(m_serialPort, &QSerialPort::errorOccurred, this, &MeasureCar::handleError);
}

void MeasureCar::handleReadyRead() {
	m_readData.append(m_serialPort->readAll());
}

void MeasureCar::handleError(QSerialPort::SerialPortError serialPortError) {
	// TODO
	(void)serialPortError;
}
