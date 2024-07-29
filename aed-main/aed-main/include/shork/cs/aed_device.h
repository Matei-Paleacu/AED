#ifndef AED_H
#define AED_H

#include <string>
#include <map>
#include <set>

#include <QObject>
#include <QTimer>

#include "shork/cs/bit/self_test.h"
#include "shork/cs/intervention/compression.h"
#include "shork/cs/model/device_data.h"
#include "shork/cs/sensor/ecg.h"

namespace shork::cs
{
    class AedDevice : public QObject, public bit::SelfTest
    {
        Q_OBJECT
    public:
        AedDevice();
        virtual ~AedDevice();
        void init(model::DeviceData device_data, sensor::Ecg ecg_sensor);
        bool state_transition(model::DeviceState to_state);

        void deliver_cpr_intervention(intervention::Compression cpr);
        // when illuminated shock button is pressed, verifies a shock was actually advised first
        void request_shock();

        unsigned int get_num_shocks() const;
        unsigned int get_battery_value() const;

        void set_inop();
        void set_rhythm(sensor::EcgReading wave);
        void set_pads_connected(bool is_pads_connected);
        void set_device_stable(bool is_device_stable);
        void set_battery_value(int state);

        // tests to run per interval
        bool interval_test() override;
        // tests to run by technicians
        bool diagnostic_test() override;
    signals:
        void aed_event(model::AedEvent event);
    public slots:
        // POST, loop instructions
        void started();
        // shutdown
        void stopped();
    protected:
        // tests to run on startup
        bool power_on_test() override;
    private slots:
        void update_state();
    private:
        bool common_test_suite();
        bool is_shock_advised();

        const unsigned int MIN_BATTERY = 5;
        const unsigned long STATE_INTERVAL = 2000;

        const std::set<model::DeviceState> ALWAYS_VALID_TRANSITIONS = {
            model::OFF, model::INOP
        };

        const std::map<model::DeviceState, std::set<model::DeviceState>> VALID_TRANSITION_MAPPING = {
            { model::OFF, { model::POST } },
            { model::POST, { model::READING_PADS } },
            { model::READING_PADS, { model::ANALYZE } },
            { model::ANALYZE, { model::PREPARE_SHOCK, model::RESUS, model::READING_PADS } },
            { model::PREPARE_SHOCK, { model::SHOCK, model::READING_PADS } },
            { model::SHOCK, { model::RESUS, model::READING_PADS } },
            { model::RESUS, { model::ANALYZE, model::READING_PADS } },
            { model::INOP, {} },
        };

        const static size_t START_LOOP_SIZE = 4;
        const model::AedEvent START_LOOP[START_LOOP_SIZE] =
        {
            model::AedEvent::CHECK_LOC,
            model::AedEvent::CALL_EMS,
            model::AedEvent::STAY_CALM,
            model::AedEvent::ATTACH_PADS,
        };

        size_t current_start_task = 0;
        bool is_preroutine_finished = false;
        bool is_performing_cpr = false;
        QTimer *state_timer;
        model::DeviceData device_data;
        sensor::Ecg ecg_sensor;
    };
}

#endif // AED_H
