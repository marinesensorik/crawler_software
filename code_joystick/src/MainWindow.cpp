#include "MainWindow.h"
#include <stdio.h>

#define SERVER_PORT 8888
#define AXIS_THRESHOLD 5000
#define STARTBYTE 240
#define FORWARD 1
#define BACKWARD 2
#define LEFT 8
#define RIGHT 4
#define STOP 16
#define DEPTHCONVERSION 10.0

QTime mtime(0,0,0);
QFile logfile;
double depthCorrection=0.0;
uint32_t frameCount=0;

MainWindow::MainWindow(QWidget* _parent) :
    QWidget(_parent)
{
  ui.setupUi(this);
  resetUI();

  QPixmap myPixmapForNow;
      if(!myPixmapForNow.load("icbm_logo.png")){
          qWarning("Failed to load");
      }
  ui.label_8->setPixmap(myPixmapForNow);
  this->setWindowIcon(myPixmapForNow);

  if(QDir("Logfiles").exists()==false)
      QDir().mkdir("Logfiles");

  port = new QSerialPort();
  socket = new QTcpSocket(this);
  
  connect(ui.stopButton,SIGNAL(clicked(bool)),this,SLOT(handleStopMovement()));
  connect(ui.forwardButton,SIGNAL(clicked(bool)),this,SLOT(handleForwardMovement()));
  connect(ui.backwardButton,SIGNAL(clicked(bool)),this,SLOT(handleBackwardMovement()));
  connect(ui.leftButton,SIGNAL(clicked(bool)),this,SLOT(handleLeftMovement()));
  connect(ui.rightButton,SIGNAL(clicked(bool)),this,SLOT(handleRightMovement()));
  connect(ui.connectButton,SIGNAL(clicked(bool)),this,SLOT(connectToJoystick()));
  connect(ui.connectToRoverButton,SIGNAL(clicked()),this,SLOT(handleRoverConnection()));
  connect(socket, SIGNAL(readyRead()),this, SLOT(readData()));
  
  ui.stopButton->setAutoRepeat(true);
  ui.forwardButton->setAutoRepeat(true);
  ui.backwardButton->setAutoRepeat(true);
  ui.leftButton->setAutoRepeat(true);
  ui.rightButton->setAutoRepeat(true);

  joyTimer = new QTimer();
  joyTimer->setInterval(50);
  joy = new QJoystick;

  QTimer *secTimer = new QTimer(this);
  connect(secTimer, SIGNAL(timeout()), this, SLOT(secTimer_timeout()));
  secTimer->start(1000);

  payload.resize(4);
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

    int16_t forwardBackwardValue = joystick.at(current_joystick)->axis[1];
    int16_t leftRightValue = joystick.at(current_joystick)->axis[0];

    float sendValueFB = (255.0f/32768.0f)*abs(forwardBackwardValue);
    float sendValueLR = (255.0f/32768.0f)*abs(leftRightValue);

    if(!(joystick.at(current_joystick)->button[0])){

      if(forwardBackwardValue < 0){
        if(abs(forwardBackwardValue) > AXIS_THRESHOLD){
          printf("forward: %i\n",joystick.at(current_joystick)->axis[1]);
          if(connectedToRover){
            TxData(FORWARD,(uint8_t)sendValueFB);
          }
        }
      } else {
        if(abs(forwardBackwardValue) > AXIS_THRESHOLD){
          printf("backward: %i \n",joystick.at(current_joystick)->axis[1]);
          if(connectedToRover){
            TxData(BACKWARD, (uint8_t)sendValueFB);
          }
        }
      }

      if(leftRightValue > 0){
        if(abs(leftRightValue) > AXIS_THRESHOLD){
          printf("right: %i\n",joystick.at(current_joystick)->axis[0]);
         if(connectedToRover){
            TxData(RIGHT, (uint8_t)sendValueLR);
          }
        }
      } else {
        if(abs(leftRightValue) > AXIS_THRESHOLD){
          printf("left: %i\n",joystick.at(current_joystick)->axis[0]);
          if(connectedToRover){
            TxData(LEFT, (uint8_t)sendValueLR);
          }
        }
      }
    } else {
      printf("--- Emergency Stop! ---- \n");
        if(connectedToRover){
          TxData(STOP, 0);
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
  ui.connectButton->setEnabled(false);
  ui.avail_box->setEnabled(false);
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
        ui.avail_box->setDisabled(true);
        ui.connectButton->setDisabled(true);
        break;
    default:
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
  printf("Trying to connect to Rover ...\n");
  socket->connectToHost("192.168.1.2",SERVER_PORT);
  if(socket->waitForConnected()){
    TxData(STOP, 0);

    connectedToRover = true;
    ui.Msg_Box->setText("connected to crawler");
    printf("success!\n");
    createLogfile();
    ui.connectToRoverButton->setDisabled(true);
  } else {
    connectedToRover = false;
    ui.Msg_Box->setText("failed to connect to crawler");
    printf("failed!\n");
  } 
}

void MainWindow::handleStopMovement(){
    TxData(STOP, 0);
	printf("Stop Button triggered\n");
}

void MainWindow::handleForwardMovement(){
    TxData(FORWARD, 255);
	printf("Forward Button triggered\n");
}

void MainWindow::handleBackwardMovement(){
    TxData(BACKWARD, 255);
	printf("Backward Button triggered\n");
}

void MainWindow::handleLeftMovement(){
    TxData(LEFT, 255);
	printf("Left Button triggered\n");
}

void MainWindow::handleRightMovement(){
    TxData(RIGHT, 255);
	printf("Right Button triggered\n");
}

void MainWindow::secTimer_timeout()
{
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm:ss");
    ui.clockBox->display(text);

    if(connectedToRover==true)
    {
        mtime=mtime.addSecs(1);
        text=mtime.toString("hh:mm:ss");
        ui.missionTimeBox->display(text);
    }
}

void MainWindow::resetUI()
{
    ui.voltageBox->display(" ");
    ui.depthBox->display(" ");
    ui.wTempBox->display(" ");
    ui.missionTimeBox->display(" ");
    ui.pressureBox->display(" ");
    ui.currentBox->display(" ");
    ui.bearingBox->display(" ");
    ui.pitchBox->display(" ");
    ui.rollBox->display(" ");
}

void MainWindow::TxData(uint8_t dat1, uint8_t dat2)
{
    if(connectedToRover)
    {
        payload[0] = STARTBYTE;
        payload[1] = dat1;
        payload[2] = dat2;
        payload[3] = payload[0] xor payload[1] xor payload[2];
        socket->write(payload);
        socket->waitForBytesWritten(1000);
        printf("TX: %d %d %d %d\n", (uint8_t)payload[0], (uint8_t)payload[1], (uint8_t)payload[2],(uint8_t) payload[3]);
    }
}

void MainWindow::readData()
{
    double voltage=0;
    double current=0;
    double pressure=0;
    double watertemp=0;
    double bearing=0;
    int pitch=0;
    int roll=0;
    int sensors=0;

    uint16_t rawvoltage=0;
    uint16_t rawcurrent=0;
    uint16_t rawpressure=0;
    int16_t rawwatertemp=0;
    uint16_t rawbearing=0;
    int8_t rawpitch=0;
    int8_t rawroll=0;
    uint8_t rawsensors=0;
    uint8_t rawcrc=0;
    uint8_t crc=0;

    QByteArray rxdata = socket->readAll();
    printf("RX: " + rxdata + '\n');
    writeLog(rxdata);

    if(rxdata[0]==(char)STARTBYTE)
    {
       rawvoltage = (rxdata[1]<<8 | rxdata[2]);
       rawcurrent = (rxdata[3]<<8 | rxdata[4]);
       rawpressure = (rxdata[5]<<8 | rxdata[6]);
       rawwatertemp = (rxdata[7]<<8 | rxdata[8]);
       rawbearing = (rxdata[9]<<8 | rxdata[10]);
       rawpitch = rxdata[11];
       rawroll = rxdata[12];
       rawsensors = rxdata[13];
       rawcrc=rxdata[14];
       crc=rxdata[1] xor rxdata[2] xor rxdata[3] xor rxdata[4] xor rxdata[5] xor rxdata[6] xor rxdata[7] xor rxdata[8] xor rxdata[9] xor rxdata[10] xor rxdata[11] xor rxdata[12] xor rxdata[13];

       if(rawcrc==crc)
       {
           printf("RX Data valid!\r\n");
           voltage=rawvoltage/1000.0;
           current=rawcurrent/1000.0;
           pressure=rawpressure/1000.0;
           watertemp=rawwatertemp*100.0;
           bearing=rawbearing/10.0;
           pitch=rawpitch;
           roll=rawroll;
           sensors=rawsensors;

           if(frameCount == 10)
           {
               depthCorrection=pressure*DEPTHCONVERSION;
               printf("Depth Correction set to: %f", depthCorrection);
           }

           // Refresh UI
           ui.voltageBox->display(voltage);
           ui.currentBox->display(current);
           ui.pressureBox->display(pressure);
           ui.depthBox->display((pressure*DEPTHCONVERSION-depthCorrection));
           ui.wTempBox->display(watertemp);
           ui.bearingBox->display(bearing);
           ui.pitchBox->display(pitch);
           ui.rollBox->display(roll);

           // ToDo: Save to Log

           frameCount++;
       }
       else
       {
           printf("RX Data invalid!\r\n");
       }
    }
}

void MainWindow::createLogfile()
{
    QDate curDate = QDate().currentDate();
    int filecount=0;
    QDir::setCurrent("./Logfiles");
    QString filename="logfile_"+curDate.toString("yyyy_dd_MM_")+QVariant(filecount).toString()+".txt";

    while(QFile().exists(filename) && filecount<10000)
    {
        filecount++;
        filename="logfile_"+curDate.toString("yyyy_dd_MM_")+QVariant(filecount).toString()+".txt";
    }
    printf(filename.toLatin1().constData());
    printf("\n");
    ui.logfileBox->setText(filename);

    logfile.setFileName(filename);
    QString header = "Crawler Control Center Logfile\n"+curDate.toString("yyyy_dd_MM ")+QTime().currentTime().toString("hh:mm:ss")+"\n**********\n";
    QString dataheader = "Sample\tVoltage/V\tCurrent/A\tPressure/bar\tDepth/m\tWaterTemp./°C\tBearing/°\tPitch/°\tRoll/°\tSensors\t";
    header=header+dataheader;
    if (logfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
           QTextStream stream(&logfile);
           stream << header << endl;
       }
    logfile.close();
}

void MainWindow::writeLog(QString entry)
{
    if (logfile.open(QIODevice::Append | QIODevice::Text)) {
           QTextStream stream(&logfile);
           stream << entry << endl;
       }
    logfile.close();
}
