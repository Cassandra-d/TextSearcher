#ifndef TEXTSEARCHWORKER_H
#define TEXTSEARCHWORKER_H

#include "worker.h"

#include <QString>
#include <QMap>
#include <QFileInfo>

namespace Workers
{

class TextSearchWorker : public Worker
{
    Q_OBJECT
public:
    TextSearchWorker();
    virtual ~TextSearchWorker();

    virtual void setData(const QVariant& data);

protected:
    virtual void startWork();

signals:
    void progressIncreased(int value);

private:
    void findInFiles(const QList<QFileInfo>& files);
    void findInFilesRecurs(const QDir &dir);

	quint32 sizeTreshhold() {return 300 * 1024 * 1024;}

private:
    bool m_case;
    bool m_recurs;

    QString m_path;
    QString m_text;

};

}

#endif // TEXTSEARCHWORKER_H
