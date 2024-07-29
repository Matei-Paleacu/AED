#include "shork/cs/aed_device.h"

namespace shork::cs
{
    AedDevice::AedDevice()
    {
        this->device_data.state = model::OFF;
    }

    AedDevice::~AedDevice() {}

    void AedDevice::init(model::DeviceData device_data, sensor::Ecg ecg_sensor)
    {
        this->device_data = device_data;
        this->ecg_sensor = ecg_sensor;

        this->state_transition(model::DeviceState::POST);
        bool is_healthy = power_on_test();
        if (!is_healthy)
        {
            return;
        }

        emit this->aed_event(model::AedEvent::UNIT_OK);
        this->state_transition(model::DeviceState::READING_PADS);
    }

    void AedDevice::started()
    {
        state_timer = new QTimer(this);
        state_timer->setInterval(STATE_INTERVAL);

        connect(state_timer, &QTimer::timeout, this, &AedDevice::update_state);
        state_timer->start();
    }

    void AedDevice::stopped()
    {
        this->is_preroutine_finished = false;
        this->is_performing_cpr = false;
        this->current_start_task = 0;
        state_timer->disconnect();
        state_timer->stop();
        delete state_timer;
    }

    void AedDevice::set_battery_value(int state)
    {
        this->device_data.battery_percent = state;
    }

    unsigned int AedDevice::get_battery_value() const
    {
        return this->device_data.battery_percent;
    }

    bool AedDevice::state_transition(model::DeviceState to_state)
    {
        if (this->device_data.state == to_state)
        {
            return true;
        }

        if (this->device_data.state == model::DeviceState::RESUS)
        {
            this->is_performing_cpr = false;
        }

        if (ALWAYS_VALID_TRANSITIONS.find(to_state) != ALWAYS_VALID_TRANSITIONS.end())
        {
            this->device_data.state = to_state;
            return true;
        }

        auto mapping = VALID_TRANSITION_MAPPING.find(this->device_data.state);
        if (mapping != VALID_TRANSITION_MAPPING.end() && mapping->second.find(to_state) != mapping->second.end())
        {
            this->device_data.state = to_state;
            return true;
        }

        return false;
    }

    void AedDevice::update_state()
    {
        if (!this->interval_test())
        {
            if (this->device_data.state == model::DeviceState::SHOCK)
            {
                emit this->aed_event(model::AedEvent::NO_SHOCK_DELIVERED);
                this->state_transition(model::DeviceState::RESUS);
            }
            return;
        }

        switch (this->device_data.state)
        {
        case model::DeviceState::READING_PADS:
            if (this->device_data.is_pads_connected)
            {
                this->state_transition(model::DeviceState::ANALYZE);
                emit this->aed_event(model::AedEvent::ANALYZING);
                this->is_preroutine_finished = true;
                return;
            }

            if (this->is_preroutine_finished)
            {
                emit this->aed_event(model::AedEvent::CHECK_PADS);
                return;
            }

            emit this->aed_event(START_LOOP[this->current_start_task]);
            this->current_start_task = (this->current_start_task + 1) % START_LOOP_SIZE;

            break;
        case model::DeviceState::ANALYZE:
            if (!this->device_data.is_device_stable)
            {
                emit this->aed_event(model::AedEvent::ANALYSIS_HALTED);
                return;
            }

            if (this->is_shock_advised())
            {
                this->state_transition(model::DeviceState::PREPARE_SHOCK);
                emit this->aed_event(model::AedEvent::SHOCK_ADVISED);
            }
            else
            {

                this->state_transition(model::DeviceState::RESUS);
                emit this->aed_event(model::AedEvent::SHOCK_NOT_ADVISED);
            }
            break;
        case model::DeviceState::SHOCK:
            this->device_data.num_shocks += 1;
            emit this->aed_event(model::AedEvent::SHOCK_DELIVERED);
            this->state_transition(model::DeviceState::RESUS);
            break;
        case model::DeviceState::RESUS:
            if (!this->is_performing_cpr)
            {
                emit this->aed_event(model::AedEvent::START_CPR);
                this->is_performing_cpr = true;
            }
        default:
            break;
        }
    }

    bool AedDevice::is_shock_advised()
    {
        if (!this->interval_test())
        {
            return false;
        }
        bool isShockAdvised = (this->ecg_sensor.reading == sensor::V_FIB) || (this->ecg_sensor.reading == sensor::V_TACH);
        return isShockAdvised;
    }

    void AedDevice::set_inop()
    {
        this->state_transition(model::INOP);
    }

    // updates the readings received from the ecg sensor
    void AedDevice::set_rhythm(sensor::EcgReading wave)
    {
        this->ecg_sensor.reading = wave;
    }

    void AedDevice::set_pads_connected(bool is_pads_connected)
    {
        this->device_data.is_pads_connected = is_pads_connected;
        if (is_pads_connected)
        {
            this->state_transition(model::DeviceState::ANALYZE);
            emit this->aed_event(model::AedEvent::ANALYZING);
            this->is_preroutine_finished = true;
        }
    }

    void AedDevice::set_device_stable(bool is_device_stable)
    {
        this->device_data.is_device_stable = is_device_stable;
    }

    // the user delivered the CPR intervention, and the device will return suggestions
    void AedDevice::deliver_cpr_intervention(intervention::Compression cpr)
    {
        if (this->device_data.state != model::DeviceState::RESUS)
        {
            return;
        }

        if (!cpr.is_force_sufficient || !cpr.is_rate_sufficient)
        {
            if (!cpr.is_rate_sufficient)
            {
                emit this->aed_event(model::AedEvent::PUSH_FASTER);
            }
            if (!cpr.is_force_sufficient)
            {
                emit this->aed_event(model::AedEvent::PUSH_HARDER);
            }
        }
        else
        {
            emit this->aed_event(model::AedEvent::GOOD_COMPRESSIONS);
            this->state_transition(model::DeviceState::READING_PADS);
        }
    }

    unsigned int AedDevice::get_num_shocks() const
    {
        return this->device_data.num_shocks;
    }

    // the user pressed the shock button when it was illuminated
    // after this, the machine may notify the user:
    // * begin CPR
    void AedDevice::request_shock()
    {
        if (this->device_data.state == model::DeviceState::PREPARE_SHOCK)
        {
            emit this->aed_event(model::AedEvent::DELIVERING_SHOCK);
            this->state_transition(model::DeviceState::SHOCK);
        }
    }

    bool AedDevice::common_test_suite()
    {
        bool is_battery_sufficient = this->device_data.battery_percent > MIN_BATTERY;
        if (!is_battery_sufficient)
        {
            emit this->aed_event(model::AedEvent::CHANGE_BATTERIES);
            this->state_transition(model::OFF);
            return false;
        }

        if (!this->ecg_sensor.is_sensor_healthy) {
            emit this->aed_event(model::AedEvent::UNIT_FAILED);
            this->device_data.state = model::INOP;
            return false;
        }

        if (this->device_data.state == model::INOP)
        {
            emit this->aed_event(model::AedEvent::UNIT_FAILED);
            return false;
        }

        return true;
    }

    bool AedDevice::power_on_test()
    {
        return common_test_suite();
    }

    bool AedDevice::interval_test()
    {
        if (this->device_data.state == model::DeviceState::OFF)
        {
            return false;
        }
        return common_test_suite();
    }

    bool AedDevice::diagnostic_test()
    {
        if (this->device_data.state == model::DeviceState::OFF)
        {
            return false;
        }
        return common_test_suite();
    }
}
