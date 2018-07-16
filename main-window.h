#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>
#include <memory>

#include "ui_main-window.h"
#include "measure-car.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void b_connect_handle();
	void mc_speedRead(unsigned int speed);

private:
	Ui_MainWindow ui;
	std::unique_ptr<MeasureCar> m_mc;

};

#endif // MAIN_H
