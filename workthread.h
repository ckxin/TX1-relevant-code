#ifndef WORKTHREAD_H
#define WORKTHREAD_H
#include <QThread>
class  WorkThread : public QThread
{
    Q_OBJECT
public:
     WorkThread();

signals:
    void show(const QString &name);


protected:
     void run();
};

#endif // WORKTHREAD_H
