#include "worker.h"

#include <QThread>

using namespace Workers;

Worker::Worker() : m_shouldBeInterrupted(false)
{
}

Worker::~Worker()
{
}

void Worker::begin()
{
  startWork();
}

void Worker::interruptionRequested()
{
  m_shouldBeInterrupted = true;
}
