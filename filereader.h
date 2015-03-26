#ifndef FILEREADER_H
#define FILEREADER_H

#include <QWidget>
#include <QPushButton>

class FileReader : public QWidget
{
    Q_OBJECT
public:
    explicit FileReader(QWidget *parent = 0);
    ~FileReader();
private:
    QPushButton *m_openFileButton;

signals:
    void fileChanged(const QString &name);
public slots:

private slots:
    void on_open_button_clicked();

};

#endif // FILEREADER_H
