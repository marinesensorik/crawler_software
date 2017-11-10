#ifndef JOYPICK_H
#define JOYPICK_H

#include <QWidget>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QTimer>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QList>
#include "qjoystick.h"

class QProgressBar;
class QCheckBox;
class QLabel;

class Joypick : public QWidget {
    Q_OBJECT
public:
    Joypick(QWidget *parent = 0);
    ~Joypick();

    // Structure created for each joystick encountered
    struct joydata{
        unsigned int number_axes;
        unsigned int number_btn;
        QList<int> axis;
        QList<bool> button;
    };

    // Available joystick count. Only set at initialization
    int joysavail;

    // List of joystick data
    QList<joydata*> joystick;

    // current_joystick is dynamic, set by the user selection
    int current_joystick;

    void pollJoystick();
    void exttoggle(bool);

private:
    QJoystick *joy;
    void init_joysticks();
    QTimer *data_timer;
    QList<QProgressBar*> pbarlist;
    QList<QLabel*> pbarlablist;
    QList<QCheckBox*> checklist;
    void setupui();

    // UI components
    QVBoxLayout *mainLayout;
    QLabel *avail_label;
    QHBoxLayout *horizontalLayout;
    QComboBox *avail_box;
    QCheckBox *sample_box;
    QLabel *num_axbtnlabel;
    QHBoxLayout *horizontalLayout2;
    QHBoxLayout *axes_Layout;
    QHBoxLayout *horizontalLayout3;
    QHBoxLayout *buttons_Layout;

private slots:
    void updateSelection(int index);
    void updateData();
    void toggleSampling(bool down);

};

#endif // JOYPICK_H
