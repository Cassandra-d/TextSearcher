#ifndef MAINSCREEN_H
#define MAINSCREEN_H

#include "common.h"
#include "worker.h"
#include "workersthreadscontroller.h"

#include <memory>

#include <QFileInfo>
#include <QWidget>
#include <QThread>


namespace Ui {
class MainScreen;
}

class MainScreen : public QWidget
{
    Q_OBJECT

public:
    explicit MainScreen(QWidget *parent = 0);
    ~MainScreen();

protected:
    virtual bool eventFilter(QObject *o, QEvent *e);

private:
    void writeError(const QString& text) const;
    void writeWarning(const QString& text) const;
    void writeMessage(const QString& text) const;

    bool validFields();

    void setCurrentWorker(Workers::Worker* worker);

	void hideProgress();
	void showProgress();

    Workers::Worker* currentWorker() const
    {
        return currentWorker<Workers::Worker>();
    }

    template<typename T>
    T* currentWorker() const
    {
		T* retVal = qobject_cast<T*>(m_currentWorker.get());

        return retVal;
    }

signals:
    void counterIncreased(int num);
    void beginWork();

private slots:
    void searchButtonClicked();
    void findButtonClicked();
    void pathLabelClicked();

    void setCaseSensitive(bool value);
    void setRecursiveSearch(bool value);
    void setSearchText(QString value);
    void setSearchPath(QString value);

    void increaseCounter(int num);
    void handleMessage(const QString& mess, MessageLevel level);

    void workEnded();

private:
    Ui::MainScreen *ui;

    QString m_searchPath;
    QString m_searchText;

    bool m_caseSensitive;
    bool m_recursively;
	uint m_progress;

	std::unique_ptr<Workers::Worker> m_currentWorker;
	WorkersThreadsController m_workersThreadsController;
};

#endif // MAINSCREEN_H
