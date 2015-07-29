#ifndef linuxAndroidQt_H
#define linuxAndroidQt_H

#include <QtCore/QObject>

class linuxAndroidQt : public QObject
{
    Q_OBJECT

public:
    linuxAndroidQt();
    virtual ~linuxAndroidQt();

private slots:
    void output();
};

#endif // linuxAndroidQt_H
