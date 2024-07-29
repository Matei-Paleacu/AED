#ifndef ECG_H
#define ECG_H

namespace shork::cs::sensor {
    enum EcgReading { V_FIB, V_TACH, UNSHOCKABLE };

    struct Ecg {
        bool is_sensor_healthy = true;
        EcgReading reading;
    };
}

#endif // ECG_H
