#ifndef DEVICE_H
#define DEVICE_H

#include <QMetaType>

namespace shork::cs::model {
    enum DeviceState {
        // the INOP state is to simulate an arbitrary user-defined hardware error
        POST, READING_PADS, ANALYZE, PREPARE_SHOCK, SHOCK, RESUS, OFF, INOP
    };

    enum AedEvent
    {
        UNIT_OK,
        UNIT_FAILED,
        CHANGE_BATTERIES,
        STAY_CALM,
        CHECK_LOC,
        CALL_EMS,
        ATTACH_PADS,
        CHECK_PADS,
        ANALYZING,
        SHOCK_ADVISED,
        SHOCK_NOT_ADVISED,
        ANALYSIS_HALTED,
        DELIVERING_SHOCK,
        SHOCK_DELIVERED,
        NO_SHOCK_DELIVERED,
        START_CPR,
        PUSH_FASTER,
        PUSH_HARDER,
        GOOD_COMPRESSIONS,
        STOP_CPR,
        // if the operation is not a legal operation
        NO_OP,
    };

    struct DeviceData {
        unsigned int battery_percent = 100;
        unsigned int num_shocks = 0;
        // whether the pads are connected
        bool is_pads_connected = false;
        // whether the signal is too noisy to get a good analysis
        bool is_device_stable = true;
        DeviceState state = DeviceState::OFF;
    };
}

Q_DECLARE_METATYPE(shork::cs::model::AedEvent)

#endif // DEVICE_H
