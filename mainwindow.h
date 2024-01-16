#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ClauseApp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
public slots:
    void start();
    void setQuestion();
    void setStyleSheet(QString, int);
    void toggleDisabledButtons();
    void onNextPressed();
    void onAnswerPressed(int);
    void keyPressEvent(QKeyEvent*) override;
    void showForm();
    void addClause();
    void closeForm();
    void restart();

private:
    Ui::MainWindow *ui;
    AppEngine engine;
    bool answerPressed;
};
#endif // MAINWINDOW_H
