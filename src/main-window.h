#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QColor>
#include <QDateTime>
#include <QMainWindow>
#include <QTimer>
#include <memory>

#include "lib/wsm/wsm.h"
#include "ui_main-window.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

	void retranslate();

private slots:
	void b_connect_handle();
	void b_scale_update_handle();
	void mc_speedRead(double speed, uint16_t speed_raw);
	void mc_onError(QString error);
	void mc_batteryRead(double voltage, uint16_t voltage_raw);
	void mc_batteryCritical();
	void mc_distanceRead(double distance, uint32_t distance_raw);
	void b_dist_reset_handle();
	void chb_log_change();
	void t_disconnect_tick();
	void cb_language_changed(int index);

private:
	Ui_MainWindow ui;
	Wsm::Wsm m_wsm;
	QDateTime m_canBlink;
	QTimer t_disconnect;
	QDateTime m_origin;

	void connect();
	void disconnect();
	void status_set_color(const QColor &color);
	void status_blink();

	void translate_app_cz();
	void translate_app_en();
};

#endif // MAIN_WINDOW_H
