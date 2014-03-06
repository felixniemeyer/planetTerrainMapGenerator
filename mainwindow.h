#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <noise/noise.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionGenerateNoise_triggered();

private:
    Ui::MainWindow *ui;
    QImage *m_img;
    QLabel *m_centralWidget;
};

#endif // MAINWINDOW_H
