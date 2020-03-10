#include "processtable.h"
#include <qdebug.h>

ProcessTable::ProcessTable(QWidget *parent) :
    QWidget(parent)
{
    hlayout = new QHBoxLayout;
    button = new QPushButton("Kill process");
    button->setToolTip("To kill the process, select it and press the  \"Kill\"");
    hlayout->addStretch();
    hlayout->addWidget(button);
    layout = new QVBoxLayout;
    table = new QTableWidget;
    combo_box = new QComboBox;
    combo_box->addItem("sort by ID");
    combo_box->addItem("sort by memory usage");
    combo_box->addItem("sort by name");
    update();
    layout->addWidget(table);
    layout->addWidget(combo_box);
    layout->addLayout(hlayout);
    this->setLayout(layout);
    QTimer *timer = new QTimer;

    connect(button,SIGNAL(clicked()),this,SLOT(kill()));
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    connect(combo_box, SIGNAL(currentIndexChanged(int )), SLOT(sort_by_id(int )));
    timer->start(4000);
}

void ProcessTable::update()
{
    table->setColumnCount(3);
    table->setRowCount(0);
    QStringList list;
    list << "Name" << "PID"<<"Memory usage";
    table->setHorizontalHeaderLabels(list);
    table->setColumnWidth(2, 150);
    QDir * dir = new QDir("/proc");
    list = dir->entryList(QStringList("*"),QDir::AllDirs);
    foreach(QString str, list) {
        if(str.toInt()) {
            ifstream stream;

            stream.open("/proc/" + str.toAscii() + "/comm");
            string s; getline(stream,s);
            int lastRow = table->rowCount();

            table->insertRow(lastRow);
            table->setColumnWidth(0,150);

            table->setItem(lastRow,0,new QTableWidgetItem(QString::fromStdString(s)));
            table->setItem(lastRow,1,new QTableWidgetItem(str));
        } else {
            continue;
        }
    }
}


void ProcessTable::kill()
{
    QList<QTableWidgetItem*> list = table->selectedItems();
    QTableWidgetItem* item = list.value(0);
    QString str = item->text();
    QProcess::execute("kill", QStringList() << str);
    update();
}

void ProcessTable::sort_by_id(int id )
{
    QString name = this->combo_box->currentText();
    if(name == "sort by ID")
    {
        qDebug()<<"choose sort by ID";
    }
}
