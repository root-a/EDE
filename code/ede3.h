#ifndef EDE3_H
#define EDE3_H

#include <QtWidgets/QMainWindow>
#include <QList>
#include <QString>
#include <QColorDialog>
#include "ui_ede3.h"

struct List
{
	int monsterCount;
	QList<int> monsters;
	int itemIds[256];
	float itemProbability[256];
	QString name;
	int unknown;
	float probability_not;
	//float probability;
	float probabilities[7];
	//uchar empty[24];
};


class EDE3 : public QMainWindow
{
	Q_OBJECT

public:
	EDE3(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~EDE3();

public slots:
	void loadFile();
	void displayList();
	void sDisplayList();
	void updateList();
	void addMobs();
	void addGroup();
	void removeGroup();
	void removeMobs();
	void saveToFile();
	void createNew();
	void clearFunc();
	void clearItems();
	void removeItems();
	void clearItemRates();
	void clearMobs();
	void findMobIDs();
	void findItemIDs();
	void insertGroup();
	List makeGroup();
	void fixMaxNameLength(QString editedText);
	void on_button();

private:
	float round(float value, int decimals);
	void blockEditSignals(bool block);
	Ui::EDE3Class ui;
	QList<List> lists;
	uchar* memory;
	int header;
	int current;
};

#endif // EDE3_H
