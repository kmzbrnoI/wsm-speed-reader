#include <QMessageBox>

#include "main-window.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.b_connect, SIGNAL (released()), this, SLOT (b_connect_handle()));
}

MainWindow::~MainWindow() {}

void MainWindow::b_connect_handle() {
	try {
		m_mc = std::make_unique<MeasureCar>(ui.le_portname->text());
		connect(m_mc.get(), SIGNAL (speedRead()), this, SLOT(mc_speedRead()));
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

void MainWindow::mc_speedRead(unsigned int speed) {
	ui.l_speed->setText(QString::number(speed));
}

