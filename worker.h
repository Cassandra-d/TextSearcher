#ifndef WORKER_H
#define WORKER_H

#include "common.h"

#include <QObject>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QMutex>
#include <QWaitCondition>

namespace Workers
{

typedef QMap<QString, QVariant> WorkerOptions ;

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker();
    virtual ~Worker();

    virtual void setData(const QVariant& data) = 0;

public slots:
    void begin();
	void interruptionRequested();

signals:
    void workDone();
    void message(const QString& message, MessageLevel level);

protected:
    virtual void startWork() = 0;

	bool m_shouldBeInterrupted;

};

}

#endif // WORKER_H
