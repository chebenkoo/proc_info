#include "info.h"
#include <QApplication>
#include "systemmonitor.h"
#include <QStyle>
#include <qdebug.h>
 #include <QStyleFactory>

int main(int argc, char **argv)
{

     QApplication app(argc,argv);
     app.setStyle(QStyleFactory::create("Cleanlooks"));
     SystemMonitor sys;
     return app.exec();
}
