#ifndef FINDFILEWORKER_H
#define FINDFILEWORKER_H

#include "worker.h"

#include <QDir>

namespace Workers
{

class FindFileWorker : public Worker
{
    Q_OBJECT
public:
    explicit FindFileWorker();
    virtual ~FindFileWorker();

    virtual void setData(const QVariant& data);

protected:
    virtual void startWork();

signals:
    void progressIncreased(int value);

private:
    void findInDir(const QDir& dir);

private:
    bool m_recurs;

    QString m_path;
    QString m_filename;

};

}

#endif // FINDFILEWORKER_H
