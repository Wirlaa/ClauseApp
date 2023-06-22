#include <iostream>
#include <QKeyEvent>
#include <QApplication>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , engine(PROJECT_PATH "ClauseAppFile.txt")
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/clap.png"));
    ui->labelClause->setText("Welcome.");
    ui->labelTranslation->setVisible(false);
    ui->labelResult->setVisible(false);
    ui->button0->setVisible(false);
    ui->button1->setVisible(false);
    ui->button2->setVisible(false);
    ui->button3->setVisible(false);
    ui->lineClause->setVisible(false);
    ui->lineTranslation->setVisible(false);
    ui->actionReturn->setVisible(false);
    connect(ui->actionShow, &QAction::triggered, [this] {this->showForm();});
    connect(ui->actionReturn, &QAction::triggered, [this] {this->closeForm();});
    connect(ui->buttonNext, &QPushButton::clicked, [this] {this->start();});
    ui->buttonNext->setText("Start");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start()
{
    ui->labelTranslation->setVisible(true);
    ui->labelResult->setVisible(true);
    ui->button0->setVisible(true);
    ui->button1->setVisible(true);
    ui->button2->setVisible(true);
    ui->button3->setVisible(true);
    ui->buttonNext->disconnect();
    ui->buttonNext->setText("Next");
    connect(ui->button0, &QPushButton::clicked, [this] {this->onAnswerPressed(0);});
    connect(ui->button1, &QPushButton::clicked, [this] {this->onAnswerPressed(1);});
    connect(ui->button2, &QPushButton::clicked, [this] {this->onAnswerPressed(2);});
    connect(ui->button3, &QPushButton::clicked, [this] {this->onAnswerPressed(3);});
    connect(ui->buttonNext, &QPushButton::clicked, [this] {this->onNextPressed();});
    setQuestion();
}

void MainWindow::setQuestion()
{
    Clause clause = engine.currentClause();
    ui->labelClause->setText(QString::fromStdString(clause.clause));
    ui->labelTranslation->setText(QString::fromStdString(clause.translation));
    ui->labelResult->setText("");
    ui->button0->setText(QString::fromStdString(engine.getAnswer(clause.answersIndexes[0])));
    ui->button1->setText(QString::fromStdString(engine.getAnswer(clause.answersIndexes[1])));
    ui->button2->setText(QString::fromStdString(engine.getAnswer(clause.answersIndexes[2])));
    ui->button3->setText(QString::fromStdString(engine.getAnswer(clause.answersIndexes[3])));
}

void MainWindow::setStyleSheet(QString stylesheet, int index)
{
    switch (index){
        case 0:
            ui->button0->setStyleSheet(stylesheet);
            break;
        case 1:
            ui->button1->setStyleSheet(stylesheet);
            break;
        case 2:
            ui->button2->setStyleSheet(stylesheet);
            break;
        case 3:
            ui->button3->setStyleSheet(stylesheet);
            break;
    }
}

void MainWindow::toggleDisabledButtons()
{
    ui->button0->setDisabled(answerPressed);
    ui->button1->setDisabled(answerPressed);
    ui->button2->setDisabled(answerPressed);
    ui->button3->setDisabled(answerPressed);
}

void MainWindow::onNextPressed()
{
    ui->labelPoints->setText(QString::fromStdString(engine.getPointsString()));
    setStyleSheet("background-color: rgb(0, 85, 127);", 0);
    setStyleSheet("background-color: rgb(0, 85, 127);", 1);
    setStyleSheet("background-color: rgb(0, 85, 127);", 2);
    setStyleSheet("background-color: rgb(0, 85, 127);", 3);
    engine.nextClause();
    answerPressed = false;
    toggleDisabledButtons();
    if (engine.currentClause().clause.empty()) {
        ui->labelClause->setText("No more clauses.");
        ui->labelTranslation->setVisible(false);
        ui->labelResult->setVisible(false);
        ui->button0->setVisible(false);
        ui->button1->setVisible(false);
        ui->button2->setVisible(false);
        ui->button3->setVisible(false);
        connect(ui->buttonNext, &QPushButton::clicked, qApp, &QCoreApplication::quit, Qt::QueuedConnection);
        ui->buttonNext->setText("Exit");
        return;
    }
    setQuestion();
}

void MainWindow::onAnswerPressed(int answer)
{
    answerPressed = true;
    if (engine.isCorrect(answer)) {
        engine.addPoint();
        ui->labelResult->setText("Correct");
        setStyleSheet("background-color: rgb(85, 170, 0); border-style: outset; border-width: 3px; border-color: rgb(200, 200, 200);", engine.currentClause().correctIndex);
    }
    else {
        ui->labelResult->setText("Incorrect");
        setStyleSheet("background-color: rgb(0, 85, 127); border-style: outset; border-width: 3px; border-color: rgb(90, 90, 90);", answer);
    }
    ui->labelPoints->setText(QString::fromStdString(engine.getPointsString()));
    toggleDisabledButtons();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (Qt::Key_Escape == event->key()){
        close();
        qApp->quit();
    } else if (Qt::Key_N == event->key()) {
        ui->buttonNext->click();
    } else if (Qt::Key_1 == event->key()) {
        ui->button0->click();
    } else if (Qt::Key_2 == event->key()) {
        ui->button1->click();
    } else if (Qt::Key_3 == event->key()) {
        ui->button2->click();
    } else if (Qt::Key_4 == event->key()) {
        ui->button3->click();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::showForm()
{
    ui->labelClause->setText("Clause adding form.");
    ui->labelTranslation->setVisible(false);
    ui->labelResult->setVisible(false);
    ui->labelPoints->setVisible(false);
    ui->button0->setVisible(false);
    ui->button1->setVisible(false);
    ui->button2->setVisible(false);
    ui->button3->setVisible(false);
    ui->lineClause->setVisible(true);
    ui->lineTranslation->setVisible(true);
    ui->actionShow->setVisible(false);
    ui->actionReturn->setVisible(true);
    ui->buttonNext->disconnect();
    connect(ui->buttonNext, &QPushButton::clicked, [this] {this->addClause();});
    ui->buttonNext->setText("Add");
}

void MainWindow::addClause()
{
    if (ui->lineClause->text() != "" && ui->lineTranslation->text() != "") {
        engine.addQuestion(ui->lineClause->text().toStdString(), ui->lineTranslation->text().toStdString());
        ui->lineClause->setText("");
        ui->lineTranslation->setText("");
    }
}

void MainWindow::closeForm()
{
    ui->lineClause->setVisible(false);
    ui->lineTranslation->setVisible(false);
    ui->actionShow->setVisible(true);
    ui->actionReturn->setVisible(false);
    ui->labelPoints->setVisible(true);
    start();
}

