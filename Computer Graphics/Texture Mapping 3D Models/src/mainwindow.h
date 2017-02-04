#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

/*
 * The Controller Class. Handles all GUI events and redirect them to  right function in
 * View Class (Myglwidget)
 *
*/

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init_widget(char *argv[]);

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_zRotSlider_valueChanged(int value);

    void on_yRotSlider_valueChanged(int value);

    void on_xRotSlider_valueChanged(int value);

    void on_zoom_valueChanged(int value);

    void on_light0_clicked(bool checked);

    void on_light1_clicked(bool checked);

    void on_light2_clicked(bool checked);

    void on_light3_clicked(bool checked);

    void on_boundingBox_clicked(bool checked);

    void on_texture1_clicked(bool checked);

    void on_texture2_clicked(bool checked);

    void on_changeTex1_released();

    void on_changeTex2_released();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
