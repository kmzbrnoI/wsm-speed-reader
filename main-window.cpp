#include <QMessageBox>

#include "main-window.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent) {
	ui.setupUi(this);
	this->setFixedSize(this->size());

	QObject::connect(ui.b_connect, SIGNAL(released()), this, SLOT(b_connect_handle()));
	QObject::connect(ui.le_portname, SIGNAL(returnPressed()), this, SLOT(b_connect_handle()));

	QObject::connect(ui.b_scale_update, SIGNAL(released()), this, SLOT(b_scale_update_handle()));
	QObject::connect(ui.b_calc_diam, SIGNAL(released()), this, SLOT(b_calculate_handle()));
}

MainWindow::~MainWindow() {}

void MainWindow::connect() {
	try {
		m_mc = std::make_unique<MeasureCar>(ui.le_portname->text(), ui.sb_scale->value(), ui.dsb_diameter->value());
		QObject::connect(m_mc.get(), SIGNAL(speedRead(unsigned int)), this, SLOT(mc_speedRead(unsigned int)));
		QObject::connect(m_mc.get(), SIGNAL(onError(QString)), this, SLOT(mc_onError(QString)));
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
}

void MainWindow::b_connect_handle() {
	if (nullptr != m_mc)
		disconnect();
	else
		connect();
}

void MainWindow::mc_speedRead(unsigned int speed) {
	ui.l_speed->setText(QString::number(speed));
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

	if (nullptr != m_mc) {
		m_mc->scale = ui.sb_scale->value();
		m_mc->wheelDiameter = ui.dsb_diameter->value();
	}
}

void MainWindow::b_calculate_handle() {
	const unsigned int REAL_WHEEL_DIAM = 1000;
	ui.dsb_diameter->setValue((double)REAL_WHEEL_DIAM / ui.sb_scale->value());
}
