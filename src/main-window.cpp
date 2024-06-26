#include <QColor>
#include <QMessageBox>
#include <QSettings>
#include <fstream>
#include <iomanip>

#include "main-window.h"

const QString config_fn = "config.ini";
const unsigned int BLINK_TIMEOUT = 250; // ms

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(), m_origin(QDateTime::currentDateTime()) {
	ui.setupUi(this);
	this->setWindowTitle(QString("Speed Reader v%1.%2").arg(VERSION_MAJOR).arg(VERSION_MINOR));
	this->setFixedSize(this->size());

	QSettings s(config_fn, QSettings::IniFormat);
	ui.le_portname->setText(s.value("port", "/dev/ttyUSB0").toString());
	ui.sb_scale->setValue(s.value("scale", 120).toInt());
	ui.dsb_diameter->setValue(s.value("diameter", 8).toDouble());
	ui.le_log_filename->setText(s.value("log_fn", "speed.csv").toString());

	// WSM init
	m_wsm.scale = ui.sb_scale->value();
	m_wsm.wheelDiameter = ui.dsb_diameter->value();
	m_wsm.ticksPerRevolution =
	    s.value("ticks_per_revolution", static_cast<unsigned>(m_wsm.DEFAULT_TICKS_PER_REVOLUTION))
	        .toInt();
	QObject::connect(&m_wsm, SIGNAL(speedRead(double, uint16_t)), this,
	                 SLOT(mc_speedRead(double, uint16_t)));
	QObject::connect(&m_wsm, SIGNAL(onError(QString)), this, SLOT(mc_onError(QString)));
	QObject::connect(&m_wsm, SIGNAL(batteryRead(double, uint16_t)), this,
	                 SLOT(mc_batteryRead(double, uint16_t)));
	QObject::connect(&m_wsm, SIGNAL(batteryCritical()), this, SLOT(mc_batteryCritical()));
	QObject::connect(&m_wsm, SIGNAL(distanceRead(double, uint32_t)), this,
	                 SLOT(mc_distanceRead(double, uint32_t)));

	// GUI init
	QObject::connect(ui.b_connect, SIGNAL(released()), this, SLOT(b_connect_handle()));
	QObject::connect(ui.le_portname, SIGNAL(returnPressed()), this, SLOT(b_connect_handle()));

	QObject::connect(ui.b_scale_update, SIGNAL(released()), this, SLOT(b_scale_update_handle()));
	QObject::connect(ui.b_calc_diam, SIGNAL(released()), this, SLOT(b_calculate_handle()));

	QObject::connect(ui.b_dist_reset, SIGNAL(released()), this, SLOT(b_dist_reset_handle()));
	QObject::connect(ui.chb_log, SIGNAL(clicked()), this, SLOT(chb_log_change()));

	t_disconnect.setSingleShot(true);
	QObject::connect(&t_disconnect, SIGNAL(timeout()), this, SLOT(t_disconnect_tick()));

	ui.sb_main->showMessage("Battery: ?.?? V [3.5 – 4.2 V] (?, ?)");
}

MainWindow::~MainWindow() {
	QSettings s(config_fn, QSettings::IniFormat);
	s.setValue("log_fn", ui.le_log_filename->text());
	s.setValue("ticks_per_revolution", static_cast<unsigned>(m_wsm.ticksPerRevolution));
}

void MainWindow::connect() {
	QSettings s(config_fn, QSettings::IniFormat);
	s.setValue("port", ui.le_portname->text());

	try {
		m_wsm.connect(ui.le_portname->text());
		ui.b_connect->setText("Disconnect");
		ui.le_portname->setEnabled(false);
	} catch (const Wsm::EOpenError &e) {
		QMessageBox m(
			QMessageBox::Icon::Warning,
			"Error!",
			"Error while opening serial port " + ui.le_portname->text() + ":\n" + e,
			QMessageBox::Ok
		);
		m.exec();
	}
}

void MainWindow::disconnect() {
	m_wsm.disconnect();
	ui.b_connect->setText("Connect");
	ui.le_portname->setEnabled(true);
	ui.l_speed->setText("??.?");
	ui.l_speed_raw->setText("?");
	ui.l_dist->setText("??.??");
	ui.l_dist_raw->setText("?");
	ui.sb_main->showMessage("Battery: ?.?? V [3.5 – 4.2 V] (?, ?)");
	status_set_color(ui.l_speed->palette().color(QPalette::WindowText));
}

void MainWindow::b_connect_handle() {
	if (m_wsm.connected())
		disconnect();
	else
		connect();
}

void MainWindow::mc_speedRead(double speed, uint16_t speed_raw) {
	ui.l_speed->setText(QString::number(speed, 'f', 1));
	ui.l_speed_raw->setText("(" + QString::number(speed_raw) + ")");

	if (m_canBlink < QDateTime::currentDateTime()) {
		status_blink();
		m_canBlink = QDateTime::currentDateTime().addMSecs(BLINK_TIMEOUT);
	}

	if (ui.chb_log->checkState() == Qt::Checked) {
		std::ofstream out(ui.le_log_filename->text().toLatin1().data(), std::ofstream::app);

		out << QTime::currentTime().toString("hh:mm:ss.zzz").toLatin1().data() << ";";
		out << std::fixed << std::setprecision(2)
		    << static_cast<double>(m_origin.msecsTo(QDateTime::currentDateTime())) / 1000 << ";";
		out << speed << ";" << speed_raw << "\n";
	}
}

void MainWindow::mc_onError(QString error) {
	(void)error;

	if (!t_disconnect.isActive() && m_wsm.connected())
		t_disconnect.start(100);
}

void MainWindow::b_scale_update_handle() {
	QMessageBox m(
		QMessageBox::Icon::Information,
		"Info",
		"Speed will be updated after next measurement.",
		QMessageBox::Ok
	);
	m.exec();

	QSettings s(config_fn, QSettings::IniFormat);
	s.setValue("scale", ui.sb_scale->value());
	s.setValue("diameter", ui.dsb_diameter->value());

	if (m_wsm.connected()) {
		m_wsm.scale = ui.sb_scale->value();
		m_wsm.wheelDiameter = ui.dsb_diameter->value();
	}
}

void MainWindow::b_calculate_handle() {
	const unsigned int REAL_WHEEL_DIAM = 1000;
	ui.dsb_diameter->setValue(static_cast<double>(REAL_WHEEL_DIAM) / ui.sb_scale->value());
}

void MainWindow::mc_batteryRead(double voltage, uint16_t voltage_raw) {
	ui.sb_main->showMessage(
		QString("Battery: %1 V [3.5 – 4.2 V] (%2, %3)").\
			arg(voltage, 0, 'f', 2).\
			arg(voltage_raw).\
			arg(QTime::currentTime().toString().toLatin1().data())
	);
}

void MainWindow::mc_batteryCritical() {
	QMessageBox m(
		QMessageBox::Icon::Warning,
		"Warning",
		"Battery level critical, device is shutting down!",
		QMessageBox::Ok
	);
	m.exec();

	if (!t_disconnect.isActive())
		t_disconnect.start(0);
}

void MainWindow::mc_distanceRead(double distance, uint32_t distance_raw) {
	ui.l_dist->setText(QString::number(distance, 'f', 2));
	ui.l_dist_raw->setText("(" + QString::number(distance_raw) + ")");
}

void MainWindow::status_set_color(const QColor &color) {
	QPalette palette = ui.l_alive->palette();
	palette.setColor(QPalette::WindowText, color);
	ui.l_alive->setPalette(palette);
}

void MainWindow::status_blink() {
	QPalette palette = ui.l_alive->palette();
	const QColor &color = palette.color(QPalette::WindowText);
	if (color == Qt::gray)
		status_set_color(palette.color(QPalette::Window));
	else
		status_set_color(Qt::gray);
}

void MainWindow::b_dist_reset_handle() {
	if (m_wsm.connected()) {
		m_wsm.distanceReset();
		mc_distanceRead(0, 0);
	}
}

void MainWindow::chb_log_change() {
	if (ui.chb_log->checkState() == Qt::Checked) {
		// empty log file
		std::ofstream out(ui.le_log_filename->text().toLatin1().data());
	}
}

void MainWindow::t_disconnect_tick() {
	disconnect();

	QMessageBox m(
		QMessageBox::Icon::Warning,
		"Error!",
		"Serial port error!",
		QMessageBox::Ok
	);
	m.exec();
}
