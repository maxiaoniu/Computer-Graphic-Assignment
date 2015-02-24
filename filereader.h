#ifndef FILEREADER_H
#define FILEREADER_H

#include <QWidget>

class FileReader : public QWidget
{
    Q_OBJECT
public:
    explicit FileReader(QWidget *parent = 0);
    ~FileReader();

signals:

public slots:
};

#endif // FILEREADER_H
