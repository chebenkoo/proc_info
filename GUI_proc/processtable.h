#ifndef PROCESSTABLE_H
#define PROCESSTABLE_H


#include <QtGui>
#include <iostream>
#include <fstream>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>

using namespace std;
class ProcessTable : public QWidget
{
    Q_OBJECT
public:
    explicit ProcessTable(QWidget *parent = 0);
private:
    QTableWidget    * table;
    QHBoxLayout     * hlayout;
    QPushButton     * button;
    QVBoxLayout     * layout;
    QComboBox       * combo_box;
public slots:
    void update();
    void kill();
    void sort_by_id(int id);
};


#endif // PROCESSTABLE_H
