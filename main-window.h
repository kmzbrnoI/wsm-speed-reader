#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>
#include <memory>

#include "measure-car.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;
	std::unique_ptr<MeasureCar> m_mc;

};

#endif // MAIN_H
