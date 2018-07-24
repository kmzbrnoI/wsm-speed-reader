#include <QCoreApplication>
#include <QSerialPort>
#include <math.h>

#include "measure-car.h"

MeasureCar::MeasureCar(QString portname, unsigned int scale, double wheelDiameter, QObject *parent)
	: QObject(parent), scale(scale), wheelDiameter(wheelDiameter) {
	m_serialPort.setBaudRate(9600);
	m_serialPort.setFlowControl(QSerialPort::FlowControl::HardwareControl);
	m_serialPort.setPortName(portname);
	m_serialPort.setReadBufferSize(256);

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
		message.setRawData(m_readData.data(), length); // will not copy, we must preserve m_readData!
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
			uint16_t interval = (uint8_t)(message[2] << 8) + (uint8_t)message[3];

			unsigned int speed = ((double)M_PI * wheelDiameter * F_CPU * 3.6 * scale) /
			                     (HOLE_COUNT * PSK * interval);

			speedRead(speed);
		}
	}
}
