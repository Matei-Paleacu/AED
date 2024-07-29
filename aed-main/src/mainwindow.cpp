#include "shork/cs/mainwindow.h"
#include "ui_mainwindow.h"

namespace shork::cs
{
    MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
    {
        qRegisterMetaType<model::AedEvent>("model::AedEvent");
        this->device = new AedDevice();
        ui->setupUi(this);
        aed_thread = new QThread;
        device->moveToThread(aed_thread);
        connect(aed_thread, &QThread::started, device, &AedDevice::started);
        connect(aed_thread, &QThread::finished, device, &AedDevice::stopped);
        connect(ui->start, &QPushButton::pressed, this, &MainWindow::start_sim);
        connect(ui->end, &QPushButton::pressed, this, &MainWindow::end_sim);

        connect(device, &AedDevice::aed_event, this, &MainWindow::aed_event);

        connect(ui->shockButton, &QPushButton::pressed, this, &MainWindow::give_shock);
        QPixmap shockPic = QPixmap(":/img/img/Heart.png");
        ui->shockButton->setIcon(shockPic);
        ui->shockButton->setIconSize(shockPic.rect().size());
        ui->shockButton->setEnabled(false);

        connect(ui->checkboxPlacedPads, &QCheckBox::stateChanged, this, &MainWindow::handlePlacePads);

        ui->checkboxPlacedPads->setEnabled(false);

        connect(ui->sliderBattery, &QSlider::valueChanged, this, &MainWindow::handleBatterySliderValueChange);
        connect(ui->comboBoxPatientState, &QComboBox::currentTextChanged, this, &MainWindow::handlePatientStateChanged);
        connect(ui->btnPreformCpr, SIGNAL(released()), this, SLOT(handleCPRDelivered()));
        ui->comboBoxPatientState->setEnabled(false);
    }

    MainWindow::~MainWindow()
    {
        aed_thread->quit();
        aed_thread->wait();
        delete aed_thread;
        delete ui;
    }

    void MainWindow::handlePadStateChange(QString newValue)
    {
        if (newValue == "Connected")
        {
            this->device->set_pads_connected(true);
            return;
        }
        this->device->set_pads_connected(false);
    }

    void MainWindow::handleCPRDelivered()
    {

        bool isCompressionWeak = ui->checkBoxWeakeCompression->isChecked();
        bool isCompressionSlow = ui->checkboxSlowCompression->isChecked();
        device->deliver_cpr_intervention(shork::cs::intervention::Compression { !isCompressionSlow, !isCompressionWeak });
    }

    void MainWindow::handlePatientStateChanged(QString newValue)
    {
        if (newValue == "Unshockable")
        {
            QPixmap ecg_pic = QPixmap(":/img/img/SR.png"); // ecg to SR
            ui->ecg->setPixmap(ecg_pic);
            this->device->set_rhythm(shork::cs::sensor::UNSHOCKABLE);
            return;
        }
        if (newValue == "V Fib")
        {
            QPixmap ecg_pic = QPixmap(":/img/img/VF.png"); // ecg to VF
            ui->ecg->setPixmap(ecg_pic);
            this->device->set_rhythm(shork::cs::sensor::V_FIB);
            return;
        }
        if (newValue == "V Tach")
        {
            QPixmap ecg_pic = QPixmap(":/img/img/VT.png"); // ecg to VT
            ui->ecg->setPixmap(ecg_pic);
            this->device->set_rhythm(shork::cs::sensor::V_TACH);
            return;
        }
    }

    void MainWindow::handlePlacePads(int state)
    {
        if (!is_started)
        {
            return;
        }
        bool is_checked = state == Qt::Checked ? true : false;
        this->device->set_pads_connected(is_checked);
    }

    void MainWindow::aed_event(model::AedEvent event)
    {
        switch (event)
        {
        case model::AedEvent::UNIT_OK:
            logToSystem("Unit ok.");
            break;
        case model::AedEvent::UNIT_FAILED:
            logToSystem("Unit failed.");
            break;
        case model::AedEvent::CHANGE_BATTERIES:
            logToSystem("Change batteries.");
            this->end_sim();
            break;
        case model::AedEvent::CHECK_LOC:
            logToSystem("Check responsiveness.");
            break;
        case model::AedEvent::CALL_EMS:
            logToSystem("Call for help.");
            break;
        case model::AedEvent::STAY_CALM:
            logToSystem("Stay calm.");
            break;
        case model::AedEvent::ATTACH_PADS:
            logToSystem("Attach pads to bare chest of patient.");
            break;
        case model::AedEvent::CHECK_PADS:
            logToSystem("Check pads.");
            break;
        case model::AedEvent::ANALYZING:
            logToSystem("Stand clear of patient. Analyzing.");
            break;
        case model::AedEvent::ANALYSIS_HALTED:
            logToSystem("Analysis halted. Keep patient still.");
            break;
        case model::AedEvent::SHOCK_ADVISED:
            logToSystem("Shock advised. Press the button to deliver shock.");
            this->ui->shockButton->setEnabled(true);
            break;
        case model::AedEvent::SHOCK_NOT_ADVISED:
            logToSystem("No shock advised.");
            break;
        case model::AedEvent::DELIVERING_SHOCK:
            logToSystem("Delivering shock.");
            break;
        case model::AedEvent::SHOCK_DELIVERED:
            ui->shocks->setText(QStringLiteral("Shocks: %1").arg(this->device->get_num_shocks()));
            logToSystem("Shock delivered.");
            break;
        case model::AedEvent::NO_SHOCK_DELIVERED:
            logToSystem("No shock delivered.");
            break;
        case model::AedEvent::START_CPR:
            logToSystem("Start CPR.");
            break;
        case model::AedEvent::STOP_CPR:
            logToSystem("Stop CPR.");
            break;
        case model::AedEvent::PUSH_FASTER:
            logToSystem("Push faster.");
            break;
        case model::AedEvent::PUSH_HARDER:
            logToSystem("Push harder.");
            break;
        case model::AedEvent::GOOD_COMPRESSIONS:
            logToSystem("Good compressions.");
            break;
        default:
            break;
        }
    }

    void MainWindow::start_sim()
    {
        int battery = ui->sliderBattery->value();
        this->dd.battery_percent = battery;
        logToSystem("Powering on.");
        time = new QTime(0, 0);
        timer = new QTimer();
        timer->start(1000);
        connect(timer, &QTimer::timeout, [this]()
                {
            *time=time->addSecs(1);
            ui->timer->setText(time->toString("mm:ss")); });

        device->init(this->dd, this->ecg);

        ui->checkboxPlacedPads->setEnabled(true);

        ui->comboBoxPatientState->setEnabled(true);

        aed_thread->start();
        ui->shockButton->setEnabled(false);

        QPixmap ecg_pic = QPixmap(":/img/img/AS.png"); // ecg default to no shock
        ui->ecg->setPixmap(ecg_pic);
        handlePatientStateChanged(ui->comboBoxPatientState->currentText());
        is_started = true;
    }

    void MainWindow::end_sim()
    {
        // reset everything
        device->state_transition(model::DeviceState::OFF);
        QPixmap ecg_pic = QPixmap();
        ui->ecg->setPixmap(ecg_pic);
        ui->LED1->setStyleSheet("border-radius: 10px; height: 20px; width: 20px; background-color: black;");
        ui->LED2->setStyleSheet("border-radius: 10px; height: 20px; width: 20px; background-color: black;");
        ui->LED3->setStyleSheet("border-radius: 10px; height: 20px; width: 20px; background-color: black;");
        ui->LED4->setStyleSheet("border-radius: 10px; height: 20px; width: 20px; background-color: black;");
        ui->LED5->setStyleSheet("border-radius: 10px; height: 20px; width: 20px; background-color: black;");

        ui->shockButton->setEnabled(false);

        ui->checkboxPlacedPads->setEnabled(false);

        ui->comboBoxPatientState->setEnabled(false);

        timer->stop();
        ui->shocks->setText("Shocks: 0");
        ui->timer->setText("00:00");
        ui->msg->setText("");
        logToSystem("Powering off.");
    }

    void MainWindow::give_shock()
    {
        this->device->request_shock();
        ui->shockButton->setEnabled(false);
    }

    void MainWindow::handleBatterySliderValueChange(int state)
    {
        this->device->set_battery_value(state);
    }

    void MainWindow::logToSystem(QString q)
    {
        QColor color = QColor(220, 20, 60);
        ui->logs->setTextColor(color);
        ui->logs->append(q);
    }
}
