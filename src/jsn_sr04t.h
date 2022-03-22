#include "application.h"

#define SR04T_RANGE_SAMPLES 50 
#define SR04T_RANGE_REQD_GOOD_SAMPLES 5
#define SR04T_RANGE_MIN_DURATION 1400   // round trip should never be less than these us -- 22cm
#define SR04T_RANGE_MAX_DURATION 36000  // round trip should never be more than these us -- 6.8m
#define SR04T_METERS_PER_US 0.000343    // speed of sound at 20C

class jsnSr04t {
    public:
        void setup( int, int);
        float getRangeMeters();
        float getRangeMeters( float* rejectRatio );
    private:
        float __getRangeMeters( float* rejectRatio );
        int trigger_pin;
        int echo_pin;
};
