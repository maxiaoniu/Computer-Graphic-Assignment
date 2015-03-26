#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

namespace Ui {
class MainWindow;
}
class ObjMesh;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


signals:
    void newFile(const QString &filename);

private:
    Ui::MainWindow *ui;
    ObjMesh* m_objmesh;
};

#endif // MAINWINDOW_H
