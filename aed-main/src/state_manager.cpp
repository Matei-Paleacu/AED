#include "shork/cs/state_manager.h"

namespace shork::cs
{
    StateManager::StateManager(Ui::MainWindow *ui)
    {
        this->ui = ui;
    }
    shork::cs::model::DeviceState StateManager::computeState()
    {
        if (!isPowered)
        {
            return shork::cs::model::DeviceState::OFF;
        }
        if (!this->hasCheckedPatient)
        {
            ui->logs->append("Check Patient ... ");
            ui->LED1->setStyleSheet("border-radius: 10px; height: 20px; width: 20px; background-color: green;");
            return shork::cs::model::DeviceState::POST;
        }
        if (!this->removedClothing)
        {
            ui->logs->append("Remove Clothing... ");
            return shork::cs::model::DeviceState::POST;
        }
        if (!this->calledForHelp)
        {
            ui->LED1->setStyleSheet("border-radius: 10px; height: 20px; width: 20px; background-color: black;");
            ui->logs->append("Call for help...");
            ui->LED2->setStyleSheet("border-radius: 10px; height: 20px; width: 20px; background-color: green;");
            return shork::cs::model::DeviceState::POST;
        }
        if (!this->placedPads)
        {
            ui->LED2->setStyleSheet("border-radius: 10px; height: 20px; width: 20px; background-color: black;");
            ui->logs->append("Please place pads...");
            ui->LED3->setStyleSheet("border-radius: 10px; height: 20px; width: 20px; background-color: green;");
            return shork::cs::model::DeviceState::POST;
        }
        if (this->shockAdvised)
        {
            ui->logs->append("Shock advised. Charging...");
            this->shockAdvised = false;
            this->isCharged = true;
            return shork::cs::model::DeviceState::PREPARE_SHOCK;
        }
        if (this->isCharged)
        {
            ui->shockButton->setEnabled(true);
            ui->logs->append("Stand clear of patient. Press the button to deliver the shock.");
            return shork::cs::model::DeviceState::PREPARE_SHOCK;
        }
        if (this->performingCpr)
        {
            ui->LED4->setStyleSheet("border-radius: 10px; height: 20px; width: 20px; background-color: black;");
            return shork::cs::model::DeviceState::RESUS;
        }
    }
}
