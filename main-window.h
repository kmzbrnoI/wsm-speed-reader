#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>
#include <memory>
#include <QColor>
#include <QDateTime>

#include "ui_main-window.h"
#include "measure-car.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void b_connect_handle();
	void b_scale_update_handle();
	void mc_speedRead(double speed, uint16_t speed_raw);
	void mc_onError(QString error);
	void b_calculate_handle();
	void mc_batteryRead(double voltage, uint16_t voltage_raw);
	void mc_batteryCritical();

private:
	Ui_MainWindow ui;
	std::unique_ptr<MeasureCar> m_mc;
	QDateTime m_canBlink;

	void connect();
	void disconnect();
	void status_set_color(QColor color);
	void status_blink();

};

#endif // MAIN_H
