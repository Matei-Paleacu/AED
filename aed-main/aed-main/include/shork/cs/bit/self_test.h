#ifndef SELF_TEST_H
#define SELF_TEST_H

namespace shork::cs::bit {
    class SelfTest {
    public:
        // tests to run per interval
        virtual bool interval_test() = 0;
        // tests to run by technicians
        virtual bool diagnostic_test() = 0;
    protected:
        // tests to run on startup
        virtual bool power_on_test() = 0;
    };
}

#endif // SELF_TEST_H
