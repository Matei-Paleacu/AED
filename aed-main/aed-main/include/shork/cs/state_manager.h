#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H
#include "shork/cs/model/device_data.h"
#include <QObject>
#include "ui_mainwindow.h"

namespace shork::cs
{
class StateManager: public QObject
{
public:
    StateManager(Ui::MainWindow *);
    bool isPowered = false;
    bool hasCheckedPatient = false;
    bool removedClothing = false;
    bool placedPads = false;
    bool calledForHelp = false;
    bool hasFinishedCpr = false;
    bool shockAdvised = false;
    bool isCharged = false;
    bool performingCpr = false;

    shork::cs::model::DeviceState computeState();
private:
    Ui::MainWindow * ui;
};
}

#endif
