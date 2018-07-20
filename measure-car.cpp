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
	// check timeout
	if (m_receiveTimeout < QDateTime::currentDateTime() && m_readData.size() > 0) {
		// clear input buffer when data not received for a long time
		m_readData.clear();
	}

	m_readData.append(m_serialPort.readAll());
	m_receiveTimeout = QDateTime::currentDateTime().addMSecs(_BUF_IN_TIMEOUT_MS);

	while (m_readData.size() > 0 && m_readData.size() >= (m_readData[0] & 0x0F)+2) {
		unsigned int length = (m_readData[0] & 0x0F)+2; // including header byte & xor
		uint8_t x = 0;
		for (uint i = 0; i < length; i++)
			x ^= m_readData[i];

		if (x != 0) {
			// XOR error
			m_readData.remove(0, length);
			continue;
		}

		QByteArray message;
		message.reserve(length);
		memcpy(message.data(), m_readData.constData(), length);
		parseMessage(message);
		m_readData.remove(0, length);
	}
}

void MeasureCar::handleError(QSerialPort::SerialPortError serialPortError) {
	(void)serialPortError;
	onError(m_serialPort.errorString());
}

void MeasureCar::parseMessage(QByteArray message) {
	if ((message[0] >> 4) == 0) {
		// Speed measured
		if (0x01 == message[1]) {
			// Speed measured via interval measuring.
			uint16_t interval = (message[2] << 8) + message[3];
			// TODO: calculate speed
			unsigned int speed = interval;
			speedRead(speed);
		}
	}
}
