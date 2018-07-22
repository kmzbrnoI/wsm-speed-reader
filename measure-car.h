#ifndef _MEASURE_CAR_H_
#define _MEASURE_CAR_H_

#include <QByteArray>
#include <QSerialPort>
#include <QTimer>
#include <QDateTime>

#include "q-str-exception.h"

const unsigned int _BUF_IN_TIMEOUT_MS = 300;

class EOpenError : public QStrException {
public:
	EOpenError(const QString str) : QStrException(str) {}
};

class MeasureCar : public QObject {
	Q_OBJECT

public:
	explicit MeasureCar(QString portname, unsigned int scale=120,
	                    QObject *parent = nullptr);
	unsigned int scale;

private slots:
	void handleReadyRead();
	void handleError(QSerialPort::SerialPortError error);

signals:
	void speedRead(unsigned int speed);
	void onError(QString error);

private:
	const unsigned int F_CPU = 3686400; // Hz
	const unsigned int PSK = 64;
	const unsigned int AXLE_DIAMETER = 8; // mm
	const unsigned int HOLE_COUNT = 8;

	QSerialPort m_serialPort;
	QByteArray m_readData;
	QDateTime m_receiveTimeout;

	void parseMessage(QByteArray message);
};

#endif
