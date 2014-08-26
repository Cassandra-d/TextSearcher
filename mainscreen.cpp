#include "mainscreen.h"
#include "ui_mainscreen.h"

#include "textsearchworker.h"
#include "findfileworker.h"

#include <QFileDialog>
#include <QMouseEvent>
#include <QTextStream>
#include <QDir>
#include <QStringList>

#include <assert.h>

MainScreen::MainScreen(QWidget *parent) :
    QWidget(parent),
	ui(new Ui::MainScreen),
	m_progress(0),
	m_currentWorker(nullptr)
{
    ui->setupUi(this);

    qRegisterMetaType<MessageLevel>("MessageLevel");

	connect(ui->searchText, SIGNAL(textChanged(QString)), SLOT(setSearchText(QString)));
    connect(ui->caseCheckBox, SIGNAL(toggled(bool)), SLOT(setCaseSensitive(bool)));
    connect(ui->recursiveCheckBox, SIGNAL(toggled(bool)), SLOT(setRecursiveSearch(bool)));
    connect(ui->searchButton, SIGNAL(clicked()), SLOT(searchButtonClicked()));
    connect(ui->findButton, SIGNAL(clicked()), SLOT(findButtonClicked()));

	connect(ui->cancelTask, &QPushButton::clicked, &m_workersThreadsController, &WorkersThreadsController::terminateWork);
	connect(ui->cancelTask, &QPushButton::clicked, [&]() {
		ui->resultView->append(QLatin1String("Interrupted"));
		hideProgress();
	});

    ui->pathLabel->installEventFilter(this);
	hideProgress();
	m_workersThreadsController.setParent(this);
}

MainScreen::~MainScreen()
{
    delete ui;
}

bool MainScreen::eventFilter(QObject* o, QEvent* e)
{
    if (o == ui->pathLabel)
    {
		if (e->type() == QEvent::MouseButtonRelease)
		{
            QMouseEvent* mE = dynamic_cast<QMouseEvent*>(e);
			if (mE && mE->button() == Qt::LeftButton)
			{
				pathLabelClicked();
            }
        }
    }
    return QWidget::eventFilter(o, e);
}

void MainScreen::writeError(const QString &text) const
{
    ui->resultView->append(QString::fromLatin1("ERROR: ").append(text));
}

void MainScreen::writeWarning(const QString &text) const
{
    ui->resultView->append(QString::fromLatin1("WARNING: ").append(text));
}

void MainScreen::writeMessage(const QString &text) const
{
    ui->resultView->append(text);
}

void MainScreen::setCaseSensitive(bool value)
{
    m_caseSensitive = value;
}

void MainScreen::setRecursiveSearch(bool value)
{
    m_recursively = value;
}

void MainScreen::setSearchText(QString value)
{
    m_searchText = value.trimmed();
}

void MainScreen::setSearchPath(QString value)
{
    m_searchPath = value;
}

void MainScreen::increaseCounter(int num)
{
	m_progress += num;
	QString str = QString("Scanned: %1").arg(QString::number(m_progress));
	ui->progress->setText(str);
}

void MainScreen::handleMessage(const QString &mess, MessageLevel level)
{
    switch (level) {
    case Normal:
        writeMessage(mess);
        break;
    case Warning:
        writeWarning(mess);
        break;
    case Error:
        writeError(mess);
        break;
    }
}

void MainScreen::workEnded()
{
    writeMessage(QLatin1String("Done"));
	hideProgress();
}

bool MainScreen::validFields()
{
    QDir dir(m_searchPath);

    if (m_searchPath.isEmpty() || !dir.exists())
    {
        writeError(QString::fromLatin1("Directory doesn't exist"));
        return false;
    }

    if (m_searchText.isEmpty())
    {
		writeError(QString::fromLatin1("Empty search text"));
        return false;
    }

    return true;
}

void MainScreen::setCurrentWorker(Workers::Worker *worker)
{
	m_currentWorker.reset(worker);
}

void MainScreen::hideProgress()
{
	ui->cancelTask->hide();
	ui->progress->hide();
}

void MainScreen::showProgress()
{
	ui->cancelTask->show();
	ui->progress->show();
}

void MainScreen::searchButtonClicked()
{
	m_workersThreadsController.terminateWork();

    ui->resultView->clear();

	m_progress = 0;

    if (!validFields())
    {
        return;
    }

    if (!m_caseSensitive)
    {
        m_searchText = m_searchText.toLower();
    }

    Workers::WorkerOptions options;
    options[QLatin1String("case")] = m_caseSensitive;
    options[QLatin1String("searchPath")] = m_searchPath;
    options[QLatin1String("searchText")] = m_searchText;
    options[QLatin1String("recurs")] = m_recursively;

    setCurrentWorker(new Workers::TextSearchWorker);

    connect(currentWorker(), &Workers::Worker::message, this, &MainScreen::handleMessage);
    connect(currentWorker<Workers::TextSearchWorker>(), &Workers::TextSearchWorker::progressIncreased, this, &MainScreen::increaseCounter);
    connect(currentWorker(), &Workers::Worker::workDone, this, &MainScreen::workEnded);
	connect(this, &MainScreen::beginWork, currentWorker(), &Workers::Worker::begin, Qt::QueuedConnection);

	currentWorker()->setData(QVariant::fromValue(options));

	m_workersThreadsController.bind(currentWorker());

	showProgress();

    emit beginWork();
}

void MainScreen::findButtonClicked()
{
	m_workersThreadsController.terminateWork();

    ui->resultView->clear();

	m_progress = 0;

    if (!validFields())
    {
        return;
    }

    if (m_caseSensitive)
    {
        m_searchText = m_searchText.toLower();
    }

    Workers::WorkerOptions options;
    options[QLatin1String("filename")] = m_searchText;
    options[QLatin1String("path")] = m_searchPath;
    options[QLatin1String("recurs")] = m_recursively;

    setCurrentWorker(new Workers::FindFileWorker);

    connect(currentWorker(), &Workers::Worker::message, this, &MainScreen::handleMessage);
    connect(currentWorker<Workers::FindFileWorker>(), &Workers::FindFileWorker::progressIncreased, this, &MainScreen::increaseCounter);
    connect(currentWorker(), &Workers::Worker::workDone, this, &MainScreen::workEnded);
	connect(this, &MainScreen::beginWork, currentWorker(), &Workers::Worker::begin, Qt::QueuedConnection);

	currentWorker()->setData(options);

	m_workersThreadsController.bind(currentWorker());

	showProgress();

    emit beginWork();
}

void MainScreen::pathLabelClicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    if (dialog.exec())
        m_searchPath = dialog.selectedFiles().at(0);

    if (!m_searchPath.isEmpty())
        ui->pathLabel->setText(m_searchPath);
}
