#ifndef INTERFACE_H
#define INTERFACE_H

#include <QWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QTimer>
#include <QLabel>
#include <QPainter>
#include <QStyleFactory>
#include <QMessageBox>
#include <QApplication>
#include <QMediaPlayer>
class interface : public QWidget
{
    Q_OBJECT
public:
    explicit interface(QWidget *parent = 0);
public:
    QTimer* timer;
    bool isok;
    bool classicalisok;

private:
    QLabel* msgLabel;
    QPainter* painter;
private:
    void paintEvent(QPaintEvent*);
public slots:
    void classicalStart();
    void Quit();
};

#endif // INTERFACE_H
