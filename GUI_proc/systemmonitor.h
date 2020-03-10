#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QtGui>
#include "info.h"
#include "processtable.h"


class SystemMonitor:public QWidget
{
    Q_OBJECT
public:
    explicit SystemMonitor(QWidget *parent = 0);
};

#endif // SYSTEMMONITOR_H
