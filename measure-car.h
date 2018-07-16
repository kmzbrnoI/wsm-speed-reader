#ifndef _MEASURE_CAR_H_
#define _MEASURE_CAR_H_

#include <QByteArray>
#include <QSerialPort>
#include <QTimer>

#include "q-str-exception.h"

class EOpenError : public QStrException {
public:
	EOpenError(const QString str) : QStrException(str) {}
};

class MeasureCar : public QObject {
	Q_OBJECT

public:
	explicit MeasureCar(QString portname, QObject *parent = nullptr);

private slots:
	void handleReadyRead();
	void handleError(QSerialPort::SerialPortError error);

private:
	QSerialPort m_serialPort;
	QByteArray m_readData;
};

#endif
