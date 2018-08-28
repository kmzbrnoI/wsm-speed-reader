#include <QCoreApplication>
#include <QSerialPort>
#include <QtMath>

#include "measure-car.h"

MeasureCar::MeasureCar(QString portname, unsigned int scale, double wheelDiameter, QObject *parent)
	: QObject(parent), scale(scale), wheelDiameter(wheelDiameter), m_distStart(0) {
	m_serialPort.setBaudRate(9600);
	m_serialPort.setFlowControl(QSerialPort::FlowControl::HardwareControl);
	m_serialPort.setPortName(portname);
	m_serialPort.setReadBufferSize(256);

	connect(&m_serialPort, &QSerialPort::readyRead, this, &MeasureCar::handleReadyRead);
	connect(&m_serialPort, &QSerialPort::errorOccured, this, &MeasureCar::handleError);

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
			x ^= m_readData[i] & 0x7F;

		if (x != 0) {
			// XOR error
			m_readData.remove(0, static_cast<int>(length));
			continue;
		}

		QByteArray message;
		message.reserve(static_cast<int>(length));
		message.setRawData(m_readData.data(), length); // will not copy, we must preserve m_readData!
		parseMessage(message);
		m_readData.remove(0, static_cast<int>(length));
	}
}

void MeasureCar::handleError(QSerialPort::SerialPortError serialPortError) {
	(void)serialPortError;
	onError(m_serialPort.errorString());
}

void MeasureCar::parseMessage(QByteArray message) {
	const uint8_t MSG_SPEED = 0x1;
	const uint8_t MSG_BATTERY = 0x2;

	uint8_t type = (static_cast<uint8_t>(message[0]) >> 4) & 0x7;
	if (type == MSG_SPEED) {
		// Speed measured

		if (0x81 == static_cast<uint8_t>(message[1])) {
			// Speed measured via interval measuring.
			uint16_t interval = \
					((static_cast<uint8_t>(message[2]) & 0x03) << 14) | \
					((static_cast<uint8_t>(message[3]) & 0x7F) << 7) |  \
					(static_cast<uint8_t>(message[4]) & 0x7F);

			if (interval == 0xFFFF)
				speedRead(0, 0xFFFF);
			else {
				double speed = (static_cast<double>(M_PI) * wheelDiameter * F_CPU * 3.6 * scale / 1000) /
				               (HOLE_COUNT * PSK * interval);

				speedRead(speed, interval);
			}
		} else if (0x82 == static_cast<uint8_t>(message[1])) {
			// distance measured
			m_dist = \
					((static_cast<uint8_t>(message[2]) & 0x0F) << 28) | \
					((static_cast<uint8_t>(message[3]) & 0x7F) << 21) | \
					((static_cast<uint8_t>(message[4]) & 0x7F) << 14) | \
					((static_cast<uint8_t>(message[5]) & 0x7F) << 7) |  \
					(static_cast<uint8_t>(message[6]) & 0x7F);

			uint32_t distDelta = m_dist - m_distStart;
			double distRealDelta = (distDelta * static_cast<double>(M_PI) * wheelDiameter) / (1000 * HOLE_COUNT);
			distanceRead(distRealDelta, distDelta);

		}
	} else if (type == MSG_BATTERY) {
		uint16_t measured = (static_cast<uint8_t>(message[1] & 0x07) << 7) | (static_cast<uint8_t>(message[2]) & 0x7F);
		double voltage = (measured * 4.587 / 1024);
		batteryRead(voltage, measured);

		bool critical = (message[1] >> 6) & 0x1;
		if (critical)
			batteryCritical();
	}
}

void MeasureCar::distanceReset() {
	m_distStart = m_dist;
}
