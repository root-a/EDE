#include "ede3.h"
#include "QFileDialog.h"
#include <QtGlobal>
#include <math.h>
#include <QMessageBox>
#include <QTextCodec>
#include <QDebug>

EDE3::EDE3(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	QObject::connect(ui.loadFilePushButton,SIGNAL(clicked()),this,SLOT(loadFile()));
	QObject::connect(ui.listWidget,SIGNAL(itemPressed(QListWidgetItem *)),this,SLOT(displayList()));
	QObject::connect(ui.listWidget,SIGNAL(itemSelectionChanged()),this,SLOT(displayList()));
	QObject::connect(ui.listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(sDisplayList()));
	QObject::connect(ui.listNameLineEdit,SIGNAL(editingFinished()),this,SLOT(updateList()));
	QObject::connect(ui.listNameLineEdit,SIGNAL(textChanged(QString)),this,SLOT(fixMaxNameLength(QString)));
	QObject::connect(ui.probability1DoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(updateList()));
	QObject::connect(ui.probability2DoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(updateList()));
	QObject::connect(ui.probability3DoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(updateList()));
	QObject::connect(ui.probability4DoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(updateList()));
	QObject::connect(ui.probability5DoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(updateList()));
	QObject::connect(ui.probability6DoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(updateList()));
	QObject::connect(ui.probability7DoubleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(updateList()));
	QObject::connect(ui.mobTable,SIGNAL(cellChanged(int,int)),this,SLOT(updateList()));
	QObject::connect(ui.itemTable,SIGNAL(cellChanged(int,int)),this,SLOT(updateList()));
	QObject::connect(ui.addGroupPushButton,SIGNAL(clicked()),this,SLOT(addGroup()));
	QObject::connect(ui.insertGroupPushButton,SIGNAL(clicked()),this,SLOT(insertGroup()));
	QObject::connect(ui.removeGroupPushButton,SIGNAL(clicked()),this,SLOT(removeGroup()));
	QObject::connect(ui.saveAsPushButton,SIGNAL(clicked()),this,SLOT(saveToFile()));
	QObject::connect(ui.addMobsPushButton,SIGNAL(clicked()),this,SLOT(addMobs()));
	QObject::connect(ui.removeMobsPushButton,SIGNAL(clicked()),this,SLOT(removeMobs()));
	QObject::connect(ui.newPushButton,SIGNAL(clicked()),this,SLOT(createNew()));
	QObject::connect(ui.clearMobsPushButton,SIGNAL(clicked()),this,SLOT(clearMobs()));
	QObject::connect(ui.deleteItemsPushButton,SIGNAL(clicked()),this,SLOT(removeItems()));
	QObject::connect(ui.clearItemsPushButton,SIGNAL(clicked()),this,SLOT(clearItems()));
	QObject::connect(ui.clearItemRatesPushButton,SIGNAL(clicked()),this,SLOT(clearItemRates()));
	QObject::connect(ui.findMobPushButton,SIGNAL(clicked()),this,SLOT(findMobIDs()));
	QObject::connect(ui.findMobIDLineEdit,SIGNAL(returnPressed()),this,SLOT(findMobIDs()));
	QObject::connect(ui.findItemPushButton,SIGNAL(clicked()),this,SLOT(findItemIDs()));
	QObject::connect(ui.findItemIDLineEdit,SIGNAL(returnPressed()),this,SLOT(findItemIDs()));

	//QObject::connect(ui.ColorPushButton,SIGNAL(clicked()),this,SLOT(on_button()));
	ui.ColorPushButton->setVisible(false);
	QValidator *validator = new QIntValidator(this);
	ui.findMobIDLineEdit->setValidator(validator);
	ui.findItemIDLineEdit->setValidator(validator);
	ui.monterAmountLineEdit->setValidator(validator);
	// Slot for the button
	setWindowFilePath("EDE - New file");
}

EDE3::~EDE3()
{
	
}

void EDE3::loadFile()
{
	QString filename = QFileDialog::getOpenFileName(this,
         tr("Open Extra Drops file"), "",
         tr("Extra Drops file (*.sev);;All Files (*)"));
	QFile file(filename);
	if (filename.isEmpty())
	{
         return;
	}
	else
	{
		QFileInfo fileInfo(file.fileName());
		QString filename(fileInfo.fileName());
		filename = "EDE - " + filename;
		setWindowFilePath(filename);
		file.open(QFile::ReadWrite);
		memory = file.map(0, file.size());
		
		Q_ASSERT(memory);
		
		
		ui.listWidget->clear();
		lists.clear();


		int listCount = 0;

		memcpy(&header, &memory[0], sizeof(int));
		memcpy(&listCount, &memory[4], sizeof(int));

		int currentPos = 8;
		for(int i = 0; i != listCount; i++)
		{
			List newList;
			memcpy(&newList.monsterCount, &memory[currentPos], sizeof(int));
			currentPos+=4;
			for(int ii = 0; ii != newList.monsterCount; ii++)
			{
				int monster = 0;
				memcpy(&monster, &memory[currentPos], sizeof(int));
				newList.monsters.append(monster);
				currentPos+=4;
			}

			for(int ii = 0; ii < 256; ii++)
			{
				memcpy(&newList.itemIds[ii], &memory[currentPos], sizeof(int));
				currentPos+=4;

				memcpy(&newList.itemProbability[ii], &memory[currentPos], sizeof(float));
				currentPos+=4;
			}

			char tempName[128];
			memcpy(tempName, &memory[currentPos], 128);
			QTextCodec *codec = QTextCodec::codecForName("GBK");

			QByteArray EncodedStringWithGBK = QByteArray(reinterpret_cast<char*>(tempName),-1);
			QString DecodedString;
			DecodedString+= codec->toUnicode(EncodedStringWithGBK);

			newList.name = DecodedString;

			currentPos+=128;

			memcpy(&newList.unknown, &memory[currentPos], sizeof(int));
			currentPos+=4;

			memcpy(&newList.probability_not, &memory[currentPos], sizeof(float));
			currentPos+=4;

			for (int ii = 0; ii < 7; ii++)
			{
				memcpy(&newList.probabilities[ii], &memory[currentPos], sizeof(float));
				currentPos += 4;
			}

			ui.listWidget->addItem(newList.name);
			lists.append(newList);
		}

		ui.listWidget->setCurrentRow(0);
		displayList();
		file.close();
	}
}

void EDE3::fixMaxNameLength(QString editedText)
{
	QTextCodec *codec = QTextCodec::codecForName("GBK");
	QByteArray EncodedString = codec->fromUnicode(editedText);

	ui.listNameLineEdit->setMaxLength(editedText.size()+128-EncodedString.size());
}

//double click display list
void EDE3::sDisplayList()
{
	displayList();
	ui.mobTable->selectRow(0);
}

void EDE3::displayList()
{
	current = ui.listWidget->currentRow();
	if(current>-1)
	{
		blockEditSignals(true);
		ui.listNameLineEdit->setText(lists[current].name);
		ui.listNameLineEdit->setCursorPosition(0);
		ui.probability1DoubleSpinBox->setValue(lists[current].probabilities[0] * 100);
		ui.probability2DoubleSpinBox->setValue(lists[current].probabilities[1] * 100);
		ui.probability3DoubleSpinBox->setValue(lists[current].probabilities[2] * 100);
		ui.probability4DoubleSpinBox->setValue(lists[current].probabilities[3] * 100);
		ui.probability5DoubleSpinBox->setValue(lists[current].probabilities[4] * 100);
		ui.probability6DoubleSpinBox->setValue(lists[current].probabilities[5] * 100);
		ui.probability7DoubleSpinBox->setValue(lists[current].probabilities[6] * 100);
		float combinedProbabilities = (lists[current].probabilities[0] + lists[current].probabilities[1] + lists[current].probabilities[2] + lists[current].probabilities[3] + lists[current].probabilities[4] + lists[current].probabilities[5] + lists[current].probabilities[6]);
		ui.combinedDropDoubleSpinBox->setValue(combinedProbabilities * 100);
		lists[current].probability_not = 1 - combinedProbabilities;
		ui.mobTable->setRowCount(0);
		ui.mobTable->setRowCount(lists[current].monsterCount);
		ui.itemTable->setRowCount(0);
		ui.itemTable->setRowCount(256);
		for(int i = 0; i != lists[current].monsterCount; i++)
		{
			int mobid = lists[current].monsters[i];
			QTableWidgetItem* newItem = new QTableWidgetItem(tr("%0").arg(mobid));
			ui.mobTable->setItem(i,0,newItem);

		}

		float tot = 0;
		//QString itemProbString; 
		QTableWidgetItem* newItem;
		for(int i = 0; i != 256; i++)
		{
			int itemid = lists[current].itemIds[i];
			newItem = new QTableWidgetItem(tr("%0").arg(itemid));	
			ui.itemTable->setItem(i,0,newItem);
			
			float itemprob = lists[current].itemProbability[i];
			itemprob = itemprob * 100;
			
			newItem = new QTableWidgetItem(tr("%0").arg(itemprob, 0 , 'f' , 12));
			ui.itemTable->setItem(i,1,newItem);
			


			tot += lists[current].itemProbability[i];
			
		}
		ui.totalDropDoubleSpinBox->setValue(tot * 100);
		ui.probNotDoubleSpinBox->setValue(lists[current].probability_not * 100);
		blockEditSignals(false);
	}
	else
	{
		clearFunc();
	}
}

void EDE3::updateList()
{	
	//qDebug() << "\nupdate";
	if (ui.listWidget->currentRow() >= 0)
	{
		lists[current].name = ui.listNameLineEdit->text();
		ui.listWidget->item(current)->setText(lists[current].name);
		lists[current].probabilities[0] = (float)ui.probability1DoubleSpinBox->value() / 100.f;
		lists[current].probabilities[1] = (float)ui.probability2DoubleSpinBox->value() / 100.f;
		lists[current].probabilities[2] = (float)ui.probability3DoubleSpinBox->value() / 100.f;
		lists[current].probabilities[3] = (float)ui.probability4DoubleSpinBox->value() / 100.f;
		lists[current].probabilities[4] = (float)ui.probability5DoubleSpinBox->value() / 100.f;
		lists[current].probabilities[5] = (float)ui.probability6DoubleSpinBox->value() / 100.f;
		lists[current].probabilities[6] = (float)ui.probability7DoubleSpinBox->value() / 100.f;
		float combinedProbabilities = (lists[current].probabilities[0] + lists[current].probabilities[1] + lists[current].probabilities[2] + lists[current].probabilities[3] + lists[current].probabilities[4] + lists[current].probabilities[5] + lists[current].probabilities[6]);
		ui.combinedDropDoubleSpinBox->setValue(combinedProbabilities * 100);
		lists[current].probability_not = 1 - combinedProbabilities;
		lists[current].monsterCount = ui.mobTable->rowCount();
		//actually update should happen from the table and consider row count in specific table rather than monster count or 256
		for(int i = 0; i != lists[current].monsterCount; i++)
		{
			lists[current].monsters[i] = ui.mobTable->item(i,0)->text().toInt();
		}
		float tot = 0;
		int currentRowCount = ui.itemTable->rowCount();
		for(int i = 0; i != 256; i++)
		{
			lists[current].itemIds[i] = ui.itemTable->item(i,0)->text().toInt();
			lists[current].itemProbability[i] = ui.itemTable->item(i, 1)->text().toFloat() / 100;
			tot += lists[current].itemProbability[i];

		}
		ui.totalDropDoubleSpinBox->setValue(tot * 100);
		ui.probNotDoubleSpinBox->setValue(lists[current].probability_not * 100);
	}
}

void EDE3::blockEditSignals(bool block)
{
	ui.listNameLineEdit->blockSignals(block);
	ui.probability1DoubleSpinBox->blockSignals(block);
	ui.probability2DoubleSpinBox->blockSignals(block);
	ui.probability3DoubleSpinBox->blockSignals(block);
	ui.probability4DoubleSpinBox->blockSignals(block);
	ui.probability5DoubleSpinBox->blockSignals(block);
	ui.probability6DoubleSpinBox->blockSignals(block);
	ui.probability7DoubleSpinBox->blockSignals(block);
	ui.combinedDropDoubleSpinBox->blockSignals(block);
	ui.mobTable->blockSignals(block);
	ui.itemTable->blockSignals(block);
}

void EDE3::addMobs()
{
	if (ui.listWidget->currentRow() >= 0)
	{
		ui.mobTable->blockSignals(true);
		int amountOfMobs = ui.monterAmountLineEdit->text().toInt();
		if (amountOfMobs > 0)
		{
			for (int i = 0; i < amountOfMobs; i++)
			{
				lists[current].monsterCount = lists[current].monsterCount + 1;
				ui.mobTable->setRowCount(lists[current].monsterCount);
				QTableWidgetItem* newItem = new QTableWidgetItem(tr("%0").arg(0));
				ui.mobTable->setItem(lists[current].monsterCount-1,0,newItem);

				lists[current].monsters.append(0);
			}
		}
		
		ui.mobTable->blockSignals(false);
	}

}

void EDE3::removeMobs()
{
	if (ui.listWidget->currentRow() >= 0 && lists[current].monsterCount > 0)
	{
		ui.mobTable->blockSignals(true);
		QList<QTableWidgetItem*> selected_rows = ui.mobTable->selectedItems();
		while( !selected_rows.isEmpty() )
		{
			QTableWidgetItem *mob = selected_rows.at(0);
			lists[current].monsters.removeAt(mob->row());
			lists[current].monsterCount = lists[current].monsterCount - 1;
			ui.mobTable->removeRow(mob->row());
			selected_rows = ui.mobTable->selectedItems();
		}
		ui.mobTable->selectRow(ui.mobTable->currentRow());
		ui.mobTable->blockSignals(false);
	}
}

List EDE3::makeGroup()
{
	List updateList;

	updateList.name = "New List";

	updateList.monsterCount = 0;
	
	for(int i = 0;i<256;i++)
	{
		updateList.itemIds[i] = 0;
	}
	for(int i = 0;i<256;i++)
	{
		updateList.itemProbability[i] = 0.0f;
	}
	
	updateList.unknown = 2;
	updateList.probabilities[0] = 0;
	updateList.probabilities[1] = 0;
	updateList.probabilities[2] = 0;
	updateList.probabilities[3] = 0;
	updateList.probabilities[4] = 0;
	updateList.probabilities[5] = 0;
	updateList.probabilities[6] = 0;
	updateList.probability_not = 1;

	return updateList;
}

void EDE3::addGroup()
{
	List updateList = makeGroup();

	lists.append(updateList);

	ui.listWidget->addItem(updateList.name);
	ui.listWidget->setCurrentRow(ui.listWidget->count()-1);
	displayList();
}

void EDE3::insertGroup()
{
	if (current < 0)
	{
		current = 0;
	}
	List updateList = makeGroup();

	lists.insert(current, updateList);

	ui.listWidget->insertItem(current, updateList.name);
	ui.listWidget->setCurrentRow(current);
	displayList();

	
}

void EDE3::removeGroup()
{
	if (ui.listWidget->currentRow() >= 0)
	{
		ui.listWidget->blockSignals(true);
		delete ui.listWidget->item(current);
		ui.listWidget->blockSignals(false);
		lists.removeAt(current); 
		displayList();
	}	
}

float EDE3::round(float value, int decimals)
{
	int multiplier = 1;
	float result = value;
	for (int i=0;i<decimals;i++)
	{
		multiplier*=10;
	}
	float lastDeci = value*multiplier-(int)(value*multiplier);
	if (lastDeci>=0.5)
	{
		result = value+((1.0f/multiplier)-(lastDeci*(1.0f/multiplier)));
	}
	else
	{
		result = value-(lastDeci*(1.0f/multiplier));
	}
	return result;
}

void EDE3::createNew()
{
	clearFunc();
}

void EDE3::saveToFile()
{
	if(lists.size()>0)
	{
		QString filename = QFileDialog::getSaveFileName(this,
			 tr("Save Extra Drops file"), "",
			 tr("Extra Drops file (*.sev) extra_drops.sev;;All Files (*)"));
		QFile file(filename);
		file.open(QFile::ReadWrite | QIODevice::Truncate);
		QDataStream out(&file);
		out.setByteOrder(QDataStream::LittleEndian);
		out << header;
		out << lists.size();
		for(int i = 0; i<lists.size(); i++)
		{
			out << lists[i].monsterCount;
			for(int ii = 0; ii<lists[i].monsterCount; ii++)
			{
				out << lists[i].monsters[ii];
			}
			for(int ii = 0; ii<256; ii++)
			{
				out << lists[i].itemIds[ii];
				int tempProbability = reinterpret_cast<int&>(lists[i].itemProbability[ii]);
				out << tempProbability;
			}

			QTextCodec *codec = QTextCodec::codecForName("GBK");
			QByteArray EncodedString = codec->fromUnicode(lists[i].name);

			char tempName[128]={0};

			memcpy(tempName, EncodedString, EncodedString.size());
			out.writeRawData(tempName,128);
			out << lists[i].unknown;
			int prob_notConv = reinterpret_cast<int&>(lists[i].probability_not);
			out << prob_notConv;
			for (int ii = 0; ii<7; ii++)
			{
				int tempProbability = reinterpret_cast<int&>(lists[i].probabilities[ii]);
				out << tempProbability;
			}
			
		}
		QFileInfo fileInfo(file.fileName());
		QString fileName(fileInfo.fileName());
		fileName = "EDE - " + fileName;
		setWindowFilePath(fileName);

		file.close();
	}
}

void EDE3::clearFunc()
{

	ui.listWidget->clear();
	ui.mobTable->setRowCount(0);
	ui.itemTable->setRowCount(0); // or clearContents()
	ui.totalDropDoubleSpinBox->setValue(0);
	ui.probability1DoubleSpinBox->setValue(0);
	ui.probability2DoubleSpinBox->setValue(0);
	ui.probability3DoubleSpinBox->setValue(0);
	ui.probability4DoubleSpinBox->setValue(0);
	ui.probability5DoubleSpinBox->setValue(0);
	ui.probability6DoubleSpinBox->setValue(0);
	ui.probability7DoubleSpinBox->setValue(0);
	ui.combinedDropDoubleSpinBox->setValue(0);
	ui.probNotDoubleSpinBox->setValue(0);
	ui.listNameLineEdit->clear();
	lists.clear();
	setWindowFilePath("EDE - New file");
}

void EDE3::clearMobs()
{   
	if (ui.listWidget->currentRow() >= 0 && lists[current].monsterCount > 0)
	{
		lists[current].monsters.clear();
		ui.mobTable->setRowCount(0);
		updateList();
	}
}

void EDE3::removeItems()
{
	if (ui.listWidget->currentRow() >= 0)
	{
		QList<QTableWidgetItem*> selected_rows = ui.itemTable->selectedItems();
		while( !selected_rows.isEmpty() )
		{
			QTableWidgetItem *item = selected_rows.at(0);
			ui.itemTable->removeRow(item->row());
			selected_rows = ui.itemTable->selectedItems();
		}
		int currentRow = ui.itemTable->currentRow();
		
		QTableWidgetItem* newItem;
		int initCurrentRowCount = ui.itemTable->rowCount();
		int amountOfNewRows = 256-initCurrentRowCount;
		blockEditSignals(true);
		ui.itemTable->setRowCount(256);
		for(int i = 0; i != amountOfNewRows; i++)
		{
			newItem = new QTableWidgetItem(tr("%0").arg(0));	
			ui.itemTable->setItem(initCurrentRowCount+i,0,newItem);
			
			newItem = new QTableWidgetItem(tr("%0").arg(0.0, 0 , 'f' , 12));
			ui.itemTable->setItem(initCurrentRowCount+i,1,newItem);
			
		}
		blockEditSignals(false);
		ui.itemTable->selectRow(currentRow);
		updateList();
		
	}
}

void EDE3::clearItems()
{
	if (ui.listWidget->currentRow() >= 0)
	{
		for(int i = 0; i != 256; i++)
		{
			lists[current].itemIds[i] = 0;
		}
		displayList();
	}
}

void EDE3::clearItemRates()
{
	if (ui.listWidget->currentRow() >= 0)
	{
		for(int i = 0; i != 256; i++)
		{
			lists[current].itemProbability[i] = 0;
		}
		displayList();
	}
}

void EDE3::findMobIDs()
{
	

	if (ui.listWidget->currentRow() >= 0 && ui.findMobIDLineEdit->text().length() > 0)
	{
		int selectedMobRow = ui.mobTable->currentRow();
		int nextSearch = ui.listWidget->currentRow();
		QString SearchedID = ui.findMobIDLineEdit->text();  
		QString MobID;
		int n = 0; 
		int listsSize = lists.size(); 
		while(n <= listsSize) 
		{
			if (nextSearch >= listsSize)
			{
				nextSearch = 0;
			}
			for (int i = 0; i < lists[nextSearch].monsterCount; i++)
			{
				MobID = MobID.number(lists[nextSearch].monsters[i]);
				if (MobID == SearchedID)
				{					
					ui.listWidget->setCurrentRow(nextSearch);
					displayList(); 
					QList<QTableWidgetItem *> foundItems = ui.mobTable->findItems(SearchedID, Qt::MatchExactly);
					int numberOfFoundItems = foundItems.count();
					for (int i = 0; i < numberOfFoundItems; i++)
					{
						int rowOfItem = foundItems.at(i)->row();
						int currentMobRow = ui.mobTable->currentRow();
						
						if (foundItems.at(i)->row() > selectedMobRow)
						{
							ui.mobTable->selectRow(rowOfItem);
							foundItems.clear();
							return;
						}
					}
				}
			}
			selectedMobRow = -1;
			nextSearch += 1;
			n+=1;
		}
		QMessageBox msgBox;
		msgBox.setText("Mob ID was not found.");
		msgBox.exec();
	}
}

void EDE3::findItemIDs()
{
	if (ui.listWidget->currentRow() >= 0 && ui.findItemIDLineEdit->text().length() > 0)
	{
		int selectedItemRow = ui.itemTable->currentRow();
		int nextSearch = ui.listWidget->currentRow();
		QString SearchedID = ui.findItemIDLineEdit->text();
		QString ItemID;
		int n = 0;
		int listsSize = lists.size();
		while(n <= listsSize) 
		{
			if (nextSearch >= listsSize)
			{
				nextSearch = 0;
			}
			for(int i = 0; i < 256; i++)
			{
				ItemID = ItemID.number(lists[nextSearch].itemIds[i]);
				if (ItemID == SearchedID)
				{
					ui.listWidget->setCurrentRow(nextSearch);
					displayList();
					QList<QTableWidgetItem *> foundItems = ui.itemTable->findItems(SearchedID, Qt::MatchExactly);
					int numberOfFoundItems = foundItems.count();
					for (int i = 0; i < numberOfFoundItems; i++)
					{
						if (foundItems.at(i)->row() > selectedItemRow)
						{
							ui.itemTable->selectRow(foundItems.at(i)->row());
							foundItems.clear();
							return;
						}
					}					
				}
			}
			selectedItemRow = -1;
			nextSearch += 1;
			n+=1;
			
		}
		QMessageBox msgBox;
		msgBox.setText("Item ID was not found.");
		msgBox.exec();
	}
	
}

void EDE3::on_button()
{
    QColor color = QColorDialog::getColor();
    QString s("background: #"
                          + QString(color.red() < 16? "0" : "") + QString::number(color.red(),16)
                          + QString(color.green() < 16? "0" : "") + QString::number(color.green(),16)
                          + QString(color.blue() < 16? "0" : "") + QString::number(color.blue(),16) + ";");
    ui.ColorPushButton->setStyleSheet(s);
    ui.ColorPushButton->update();
	//ui.centralWidget->setStyleSheet("QPushButton { background-color: yellow }");
	//ui.centralWidget->setStyleSheet(s);
}
		
// implement color picker
