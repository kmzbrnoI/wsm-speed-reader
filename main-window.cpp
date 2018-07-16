#include "main-window.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.b_connect, SIGNAL (clicked()), this, SLOT (on_b_connect_clicked()));
}

MainWindow::~MainWindow() {}

void MainWindow::on_b_connect_clicked() {
}
