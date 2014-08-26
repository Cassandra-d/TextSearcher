#include "workersthreadscontroller.h"

using namespace Workers;

WorkersThreadsController::WorkersThreadsController(QObject *parent) :
  QObject(parent), m_workerThread(nullptr), m_worker(nullptr)
{
}

void WorkersThreadsController::bind(Worker *worker)
{
  assert(worker);

  if (m_workerThread.get() == nullptr)
	m_workerThread.reset(new QThread());
  
  m_worker = worker;
  m_worker->moveToThread(m_workerThread.get());
  m_workerThread->start();
}

void WorkersThreadsController::terminateWork()
{
	deinitialize();
}

void WorkersThreadsController::deinitialize()
{
  if (m_workerThread.get() && m_workerThread->isRunning())
  {
	// sometimes emitting signal from worker makes it to be alive even after deleting
	// so, QPointer does not figure out that object is already dead
	// actually object is dead
	// guess it's obscure behaviour of QT's slots/signals + qpointer
	if (!m_worker.isNull())
		m_worker->interruptionRequested();

	m_workerThread->quit();
	m_workerThread->wait();
  }
}
