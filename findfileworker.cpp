#include "findfileworker.h"

using namespace Workers;

FindFileWorker::FindFileWorker() : m_recurs(false)
{
}

FindFileWorker::~FindFileWorker()
{
}

void FindFileWorker::setData(const QVariant &data)
{
	WorkerOptions options = data.value<WorkerOptions>();
	assert(options.count() >= 2);

	if (options.contains(QLatin1String("recurs")))
		m_recurs = options[QLatin1String("recurs")].toBool();

	m_path = options[QLatin1String("path")].toString();
	m_filename = options[QLatin1String("filename")].toString();
}

void FindFileWorker::startWork()
{
	assert(!m_path.isEmpty() || !m_filename.isEmpty());

	const QDir dir(m_path);
	assert(dir.exists());

	findInDir(dir);
	if (!m_shouldBeInterrupted)
		emit workDone();
}

void FindFileWorker::findInDir(const QDir &dir)
{
	if (m_shouldBeInterrupted)
	{
		return;
	}

	QList<QFileInfo> foundFiles = dir.entryInfoList(
				QStringList() << QLatin1String("*") + m_filename + QLatin1String("*")
				,QDir::Files | QDir::NoDotAndDotDot);

	if (foundFiles.isEmpty() && !m_recurs)
	{
		emit message(QString::fromLatin1("Nothing have been found"), Normal);
	}

	for(QFileInfo& info: foundFiles)
		emit message(info.absoluteFilePath(), Normal);

	emit progressIncreased(1);

	if (!m_recurs || m_shouldBeInterrupted)
		return;

	auto foundSubDirs = dir.entryInfoList(QStringList() << QLatin1String("*"), QDir::Dirs | QDir::NoDotAndDotDot);
	for (QFileInfo& info: foundSubDirs)
	{
		findInDir(QDir(info.absoluteFilePath()));
	}
}
