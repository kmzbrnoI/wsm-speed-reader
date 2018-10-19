#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>
#include <memory>
#include <QColor>
#include <QDateTime>
#include <QTimer>

#include "ui_main-window.h"
#include "lib/wsm/wsm.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void b_connect_handle();
	void b_scale_update_handle();
	void mc_speedRead(double speed, uint16_t speed_raw);
	void mc_onError(QString error);
	void b_calculate_handle();
	void mc_batteryRead(double voltage, uint16_t voltage_raw);
	void mc_batteryCritical();
	void mc_distanceRead(double distance, uint32_t distance_raw);
	void b_dist_reset_handle();
	void chb_log_change();
	void t_disconnect_tick();

private:
	Ui_MainWindow ui;
	Wsm::Wsm m_wsm;
	QDateTime m_canBlink;
	QTimer t_disconnect;
	QDateTime m_origin;

	void connect();
	void disconnect();
	void status_set_color(const QColor& color);
	void status_blink();
};

#endif // MAIN_H
