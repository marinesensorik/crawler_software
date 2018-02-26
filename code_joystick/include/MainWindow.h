#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QTcpSocket>
#include "qjoystick.h"
#include <QTimer>
#include <QTime>
#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QDate>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtMultimediaWidgets/qvideowidget.h>
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
    void readData();

private:
  Ui::MainWindow ui;
  QSerialPort *port;
  QTcpSocket *socket;
  QJoystick *joy;

  void initJoystick();
  void resetUI();
  void TxData(int dat1, int dat2);
  void createLogfile();
  void writeLog(QString entry);

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
};

#endif
