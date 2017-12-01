#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QTcpSocket>
#include "qjoystick.h"
#include <QTimer>
#include <QTime>

#include "ui_mainwindow.h"

class MainWindow : public QWidget {
    Q_OBJECT

public:
    MainWindow(QWidget* parent=0);
    ~MainWindow();

private slots:
    void handleStopMovement();
    void handleLeftMovement();
	void handleRightMovement();
	void handleForwardMovement();
	void handleBackwardMovement();
	void handleRoverConnection();
    void connectToJoystick();
    void updateData();
    void pollJoystick();
    void secTimer_timeout();

private:
  Ui::MainWindow ui;
  QSerialPort *port;
  QTcpSocket *socket;
  QJoystick *joy;

  void initJoystick();
  void resetUI();

  int joysavail;
  int current_joystick;

  struct joydata{
    unsigned int number_axes;
    unsigned int number_btn;
    QList<int> axis;
    QList<bool> button;
  };

  QList<joydata*> joystick;
  QTimer *joyTimer;
  bool connectedToRover;
  char payload[4];
};

#endif
