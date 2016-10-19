#include "ede3.h"
#include <QtWidgets/QApplication>
#include <iostream>
int main(int argc, char *argv[])
{
	//Qt::FramelessWindowHint;
	QApplication a(argc, argv);
	EDE3 w;
	w.show();
	return a.exec();
	
}
