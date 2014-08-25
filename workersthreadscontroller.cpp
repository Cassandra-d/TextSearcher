#include "workersthreadscontroller.h"

using namespace Workers;

WorkersThreadsController::WorkersThreadsController(QObject *parent) :
  QObject(parent), m_initialized(false), m_workerThread(nullptr), m_worker(nullptr)
{
}

void WorkersThreadsController::bind(Worker *worker)
{
  assert(worker);

  if (m_workerThread.get() != nullptr)
	 deinitialize();
  else
	m_workerThread.reset(new QThread());
  
  m_worker = worker;
  m_worker->moveToThread(m_workerThread.get());
  m_workerThread->start();
  m_initialized = true;
}

void WorkersThreadsController::terminateWork()
{
	deinitialize();
}

void WorkersThreadsController::deinitialize()
{
  if (!m_worker.isNull())
	m_worker->interruptionRequested();

  if (m_workerThread.get())
  {
	m_workerThread->quit();
	m_workerThread->wait();
  }
}
