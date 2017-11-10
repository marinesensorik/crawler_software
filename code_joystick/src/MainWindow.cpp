#include "MainWindow.h"

#include <stdio.h>

#define SERVER_PORT 8888

#define AXIS_THRESHOLD 2000

#define STARTBYTE 240

#define FORWARD 1

#define BACKWARD 2

#define LEFT 8

#define RIGHT 4

#define STOP 16

MainWindow::MainWindow(QWidget* _parent) :
    QWidget(_parent)
{
  ui.setupUi(this);
  
  port = new QSerialPort();
  
  socket = new QTcpSocket(this);
  
  connect(ui.stopButton,SIGNAL(clicked(bool)),this,SLOT(handleStopMovement()));
  connect(ui.forwardButton,SIGNAL(clicked(bool)),this,SLOT(handleForwardMovement()));
  connect(ui.backwardButton,SIGNAL(clicked(bool)),this,SLOT(handleBackwardMovement()));
  connect(ui.leftButton,SIGNAL(clicked(bool)),this,SLOT(handleLeftMovement()));
  connect(ui.rightButton,SIGNAL(clicked(bool)),this,SLOT(handleRightMovement()));
  connect(ui.connectButton,SIGNAL(clicked(bool)),this,SLOT(connectToJoystick()));


  connect(ui.connectToRoverButton,SIGNAL(clicked()),this,SLOT(handleRoverConnection()));
  
  joyTimer = new QTimer();
  joyTimer->setInterval(50);

  joy = new QJoystick;

  payload[0] = STARTBYTE;
  payload[1] = STOP;
  payload[2] = 0;
  payload[3] = STARTBYTE xor STOP;

  connectedToRover = false;

  initJoystick();

  if(joysavail > 0){
     connect(joyTimer,SIGNAL(timeout()),this,SLOT(updateData()));    
  }
}

MainWindow::~MainWindow(){
	delete port;
	
	delete socket;
}

void MainWindow::updateData(){
    joy->getdata();
    pollJoystick();
/*
    for(unsigned int i=0;i<joystick.at(current_joystick)->number_axes;i++)
    {
        printf("axis: %i value: %i\n",i,joystick.at(current_joystick)->axis[i]);
    }
*/
    int16_t forwardBackwardValue = joystick.at(current_joystick)->axis[1];
    int16_t leftRightValue = joystick.at(current_joystick)->axis[0];

    float sendValueFB = (255.0f/32768.0f)*abs(forwardBackwardValue);
    float sendValueLR = (255.0f/32768.0f)*abs(leftRightValue);

    if(!(joystick.at(current_joystick)->button[0])){

      if(forwardBackwardValue < 0){
        if(abs(forwardBackwardValue) > AXIS_THRESHOLD){
          printf("forward: %i\n",joystick.at(current_joystick)->axis[1]);
          if(connectedToRover){
            payload[0] = STARTBYTE;
            payload[1] = FORWARD;
            payload[2] = (int)sendValueFB;
            payload[3] = STARTBYTE xor FORWARD xor (int)sendValueFB;
            socket->write(payload);
          }
        }
      } else {
        if(abs(forwardBackwardValue) > AXIS_THRESHOLD){
          printf("backward: %i \n",joystick.at(current_joystick)->axis[1]);
          if(connectedToRover){
            payload[0] = STARTBYTE;
            payload[1] = BACKWARD;
            payload[2] = (int)sendValueFB;
            payload[3] = STARTBYTE xor FORWARD xor (int)sendValueFB;
            socket->write(payload);
          }        
        }
      }

      if(leftRightValue > 0){
        if(abs(leftRightValue) > AXIS_THRESHOLD){
          printf("right: %i\n",joystick.at(current_joystick)->axis[0]);
         if(connectedToRover){
            payload[0] = STARTBYTE;
            payload[1] = RIGHT;
            payload[2] = (int)sendValueLR;
            payload[3] = STARTBYTE xor BACKWARD xor (int)sendValueLR;
            socket->write(payload);
          }  
        }
      } else {
        if(abs(leftRightValue) > AXIS_THRESHOLD){
          printf("left: %i\n",joystick.at(current_joystick)->axis[0]);
          if(connectedToRover){
            payload[0] = STARTBYTE;
            payload[1] = LEFT;
            payload[2] = (int)sendValueLR;
            payload[3] = STARTBYTE xor BACKWARD xor (int)sendValueLR;
            socket->write(payload);
          }  
        }
      }

     /* if((leftRightValue < AXIS_THRESHOLD) || (forwardBackwardValue < AXIS_THRESHOLD)){
        printf("--- Emergency Stop! ---- \n");
        if(connectedToRover){
          payload[0] = STARTBYTE;
          payload[1] = STOP;
          payload[2] = 0;
          payload[3] = STARTBYTE xor STOP;
          socket->write(payload);
        }         
      }*/


    } else {
      printf("--- Emergency Stop! ---- \n");
        if(connectedToRover){
          payload[0] = STARTBYTE;
          payload[1] = STOP;
          payload[2] = 0;
          payload[3] = STARTBYTE xor STOP;
          socket->write(payload);
        }        
    }

}

void MainWindow::pollJoystick()
{
    joy->getdata();
    for(unsigned int i=0;i<joystick.at(current_joystick)->number_axes;i++)
    {
        joystick.at(current_joystick)->axis[i]=joy->axis[i];
    }

    for(unsigned int i=0;i<joystick.at(current_joystick)->number_btn;i++)
    {
        joystick.at(current_joystick)->button[i] = joy->buttons[i];
    }

}

void MainWindow::connectToJoystick(){
  joy->setJoystick(ui.avail_box->currentIndex());
  joyTimer->start();
}

void MainWindow::initJoystick(){
    ui.avail_box->clear();

    // Find number of joysticks present
    joysavail=joy->availableJoysticks();

    // Create joysticks list
    for(int i=0;i<joysavail;i++)
    {
        joydata *tempjoy;
        tempjoy = new joydata;
        joystick.append(tempjoy);
    }

    // Populate labels depending on the number of joysticks found
    switch (joysavail)
    {
    case 0:
        ui.avail_label->setText(QString("No joysticks found"));
        ui.avail_box->setDisabled(true);
        break;
    default:
        if(joysavail==1)
            ui.avail_label->setText(QString("%1 joystick found").arg(joysavail));
        else
        ui.avail_label->setText(QString("%1 joysticks found").arg(joysavail));

        // Populate data structure for all joysticks
        for(int i=0; i<joysavail;i++)
        {
            joy->setJoystick(i);

            // Populate ComboBox
            ui.avail_box->addItem(joy->joystickName(i));

            // Axes
            joystick.at(i)->number_axes = joy->joystickNumAxes(i);

            for(unsigned int j=0; j<joystick.at(i)->number_axes;j++)
            {
                joystick.at(i)->axis.append(0);
            }

            // Buttons
            joystick.at(i)->number_btn  = joy->joystickNumButtons(i);

            for(unsigned int j=0; j<joystick.at(i)->number_btn;j++)
            {
                joystick.at(i)->button.append(false);
            }
        }

        current_joystick = 0;
        //num_axbtnlabel->setText(QString("%1 axes - %2 buttons").arg(joystick.at(0)->number_axes).arg(joystick.at(0)->number_btn));

        joy->setJoystick(ui.avail_box->currentIndex());

        break;
    } 
}

void MainWindow::handleRoverConnection(){
  printf("Trying to connect to Rover ... ");
  socket->connectToHost("192.168.1.2",SERVER_PORT);
  if(socket->waitForConnected()){
    payload[0] = STARTBYTE;
    payload[1] = STOP;
    payload[2] = 0;
    payload[3] = STARTBYTE xor STOP;
    socket->write(payload);

    connectedToRover = true;
    ui.crawlerConnectedLabel->setText("connected");
    printf("success!\n");
  } else {
    connectedToRover = false;
    ui.crawlerConnectedLabel->setText("disconnected");
	  printf("failed!\n");
  } 
}

void MainWindow::handleStopMovement(){
	
	printf("Stop Button triggered\n");
}

void MainWindow::handleForwardMovement(){
	
	printf("Forward Button triggered\n");
}

void MainWindow::handleBackwardMovement(){
	
	printf("Backward Button triggered\n");
}

void MainWindow::handleLeftMovement(){
	
	printf("Left Button triggered\n");
}

void MainWindow::handleRightMovement(){
	
	printf("Right Button triggered\n");
}