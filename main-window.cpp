#include <QMessageBox>
#include <QSettings>

#include "main-window.h"

const QString config_fn = "config.ini";

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent) {
	ui.setupUi(this);
	this->setFixedSize(this->size());

	QSettings s(config_fn, QSettings::IniFormat);
	ui.le_portname->setText(s.value("port", "/dev/ttyUSB0").toString());
	ui.sb_scale->setValue(s.value("scale", 120).toInt());
	ui.dsb_diameter->setValue(s.value("diameter", 8).toDouble());

	QObject::connect(ui.b_connect, SIGNAL(released()), this, SLOT(b_connect_handle()));
	QObject::connect(ui.le_portname, SIGNAL(returnPressed()), this, SLOT(b_connect_handle()));

	QObject::connect(ui.b_scale_update, SIGNAL(released()), this, SLOT(b_scale_update_handle()));
	QObject::connect(ui.b_calc_diam, SIGNAL(released()), this, SLOT(b_calculate_handle()));

	ui.sb_main->showMessage("Battery: ?.?? V [3.5 – 4.2 V] (?, ?)");
}

MainWindow::~MainWindow() {}

void MainWindow::connect() {
	QSettings s(config_fn, QSettings::IniFormat);
	s.setValue("port", ui.le_portname->text());

	try {
		m_mc = std::make_unique<MeasureCar>(ui.le_portname->text(), ui.sb_scale->value(), ui.dsb_diameter->value());
		QObject::connect(m_mc.get(), SIGNAL(speedRead(unsigned int, uint16_t)), this, SLOT(mc_speedRead(unsigned int, uint16_t)));
		QObject::connect(m_mc.get(), SIGNAL(onError(QString)), this, SLOT(mc_onError(QString)));
		QObject::connect(m_mc.get(), SIGNAL(batteryRead(double, uint16_t)), this, SLOT(mc_batteryRead(double, uint16_t)));
		QObject::connect(m_mc.get(), SIGNAL(batteryCritical()), this, SLOT(mc_batteryCritical()));
		ui.b_connect->setText("Disconnect");
		ui.le_portname->setEnabled(false);
	} catch (const EOpenError& e) {
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
	m_mc = nullptr;
	ui.b_connect->setText("Connect");
	ui.le_portname->setEnabled(true);
	ui.l_speed->setText("??");
	ui.l_speed_raw->setText("??");
	ui.sb_main->showMessage("Battery: ?.?? V [3.5 – 4.2 V] (?, ?)");
}

void MainWindow::b_connect_handle() {
	if (nullptr != m_mc)
		disconnect();
	else
		connect();
}

void MainWindow::mc_speedRead(unsigned int speed, uint16_t speed_raw) {
	ui.l_speed->setText(QString::number(speed));
	ui.l_speed_raw->setText("(" + QString::number(speed_raw) + ")");
}

void MainWindow::mc_onError(QString error) {
	QMessageBox m(
		QMessageBox::Icon::Warning,
		"Error!",
		"Serial port error:\n" + error,
		QMessageBox::Ok
	);
	m.exec();
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

	if (nullptr != m_mc) {
		m_mc->scale = ui.sb_scale->value();
		m_mc->wheelDiameter = ui.dsb_diameter->value();
	}
}

void MainWindow::b_calculate_handle() {
	const unsigned int REAL_WHEEL_DIAM = 1000;
	ui.dsb_diameter->setValue((double)REAL_WHEEL_DIAM / ui.sb_scale->value());
}

void MainWindow::mc_batteryRead(double voltage, uint16_t voltage_raw) {
	QString text;
	text.sprintf(
		"Battery: %4.2f V [3.5 – 4.2 V] (%d, %s)",
		voltage,
		voltage_raw,
		QTime::currentTime().toString().toLatin1().data()
	);
	ui.sb_main->showMessage(text);
}

void MainWindow::mc_batteryCritical() {
	QMessageBox m(
		QMessageBox::Icon::Warning,
		"Warning",
		"Battery level critical, device is shutting down!",
		QMessageBox::Ok
	);
	m.exec();
}
