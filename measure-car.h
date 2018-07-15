#ifndef _MEASURE_CAR_H_
#define _MEASURE_CAR_H_

#include <QByteArray>
#include <QSerialPort>
#include <QTimer>

class MeasureCar : public QObject {
	Q_OBJECT

public:
	explicit MeasureCar(QSerialPort *serialPort, QObject *parent = nullptr);

private slots:
	void handleReadyRead();
	void handleError(QSerialPort::SerialPortError error);

private:
	QSerialPort *m_serialPort = nullptr;
	QByteArray m_readData;
};

#endif
