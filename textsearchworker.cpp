#include "textsearchworker.h"

#include <assert.h>

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>

using namespace Workers;

TextSearchWorker::TextSearchWorker() : m_case(false), m_recurs(false)
{
}

TextSearchWorker::~TextSearchWorker()
{
}

void TextSearchWorker::startWork()
{
	assert(!m_path.isEmpty() || !m_text.isEmpty());

	const QDir dir(m_path);
	assert(dir.exists());

	findInFilesRecurs(dir);
	emit workDone();
}

void TextSearchWorker::setData(const QVariant &data)
{
	WorkerOptions options = data.value<WorkerOptions>();
	assert(options.count() >= 2);

	if (options.contains(QLatin1String("case")))
		m_case = options[QLatin1String("case")].toBool();

	if (options.contains(QLatin1String("recurs")))
		m_recurs = options[QLatin1String("recurs")].toBool();

	m_path = options[QLatin1String("searchPath")].toString();
	m_text = options[QLatin1String("searchText")].toString();

	if (!m_case)
		m_text = m_text.toLower();
}

void TextSearchWorker::findInFiles(const QList<QFileInfo>& files)
{
	if (m_shouldBeInterrupted)
	{
		return;
	}

	foreach (QFileInfo fInfo, files)
	{
		if (m_shouldBeInterrupted)
			return;

		if (fInfo.isExecutable() || !fInfo.isReadable())
			continue;

		QFile file(fInfo.absoluteFilePath());

		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QString mess = QString::fromLatin1("File ")
					.append(fInfo.absoluteFilePath())
					.append(" cannot be opened");
			emit message(mess, Warning);
			continue;
		}

		QTextStream in(&file);
		int lineNumber = 1;
		QStringList data;

		QString d;
		try
		{
			// sometimes readAll consumes too much memory and leads to crashes
			// and readLine makes no sense if a file have no newlines
			d = in.readAll();
		}
		catch (const std::bad_alloc& ex)
		{
			Q_UNUSED(ex);
			QString mess = QString::fromLatin1("File ")
				.append(QLatin1String("\"") + fInfo.absoluteFilePath() + QLatin1String("\""))
				.append(QLatin1String(" cannot be opened because of insufficient memory."))
				.append(QLatin1String(" File size is "))
				.append(QString::number(fInfo.size() / 1024 / 1024))
				.append(QLatin1String("Mb"));
			emit message(mess, Warning);
			continue;
		}

		// we close the file at this point only for earlier memory deallocation, no memory leaks if we forget to close
		file.close();

		if (!m_case)
			data = d.toLower().split(QLatin1String("\n"));
		else
			data = d.split(QLatin1String("\n"));
		
		for (QString& line: data)
		{
			if (line.contains(m_text))
			{
				QString mess = QString::fromLatin1("Found in ")
						.append(fInfo.absoluteFilePath())
						.append(" at ")
						.append(QString::number(lineNumber));
				emit message(mess, Normal);
			}
			++lineNumber;
		}

		emit progressIncreased(1);
	}
}

void TextSearchWorker::findInFilesRecurs(const QDir& dir)
{
	findInFiles(dir.entryInfoList(QDir::Files | QDir::Readable));

	if (!m_recurs)
		return;

	QFileInfoList subDirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	foreach (QFileInfo directory, subDirs)
	{
		if (m_shouldBeInterrupted)
			return;

		if (!directory.exists())
			continue;

		const QDir subDir(directory.absoluteFilePath());
		findInFilesRecurs(subDir);
	}
}
