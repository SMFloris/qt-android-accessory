#include "linuxAndroidQt.h"

#include <QTimer>
#include <iostream>

linuxAndroidQt::linuxAndroidQt()
{
    QTimer* timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), SLOT(output()) );
    timer->start( 1000 );
}

linuxAndroidQt::~linuxAndroidQt()
{}

void linuxAndroidQt::output()
{
    std::cout << "Hello World!" << std::endl;
}


