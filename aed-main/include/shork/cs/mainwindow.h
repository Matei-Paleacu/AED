#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QThread>
#include <QTime>
#include "shork/cs/aed_device.h"
#include "shork/cs/state_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

namespace shork::cs
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
    private slots:
        void end_sim();
        void start_sim();
        void give_shock();
        void handlePlacePads(int state);
        void handleBatterySliderValueChange(int state);
        void handlePadStateChange(QString state);
        void handlePatientStateChanged(QString state);
        void handleCPRDelivered();
        void aed_event(model::AedEvent event);
    private:
        Ui::MainWindow *ui;
        QTime *time;
        QTimer *timer;
        StateManager *stateManager;
        AedDevice *device;
        model::DeviceData dd = {100, 0, false, true, model::OFF};
        sensor::Ecg ecg{true, sensor::UNSHOCKABLE};
        // UI and AED run on different threads
        QThread *aed_thread;
        bool is_started;
        int shocks;
        void logToSystem(QString q);
        void handle_shock();
    };
}
#endif // MAINWINDOW_H
