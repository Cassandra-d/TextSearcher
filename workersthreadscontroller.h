#ifndef WORKERSTHREADSCONTROLLER_H
#define WORKERSTHREADSCONTROLLER_H

#include "worker.h"

#include <memory>

#include <QObject>
#include <QThread>
#include <QPointer>

class WorkersThreadsController : public QObject
{
	Q_OBJECT
  public:
	explicit WorkersThreadsController(QObject *parent = 0);

	void bind(Workers::Worker* worker);
	void terminateWork();

  private:
	void deinitialize();

private:
	bool m_initialized;
	std::unique_ptr<QThread> m_workerThread;
	QPointer<Workers::Worker> m_worker;
};

#endif // WORKERSTHREADSCONTROLLER_H
