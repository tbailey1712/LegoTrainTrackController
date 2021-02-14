#include <Ultrasonic.h>

#ifndef DistanceSensors_h
#define DistanceSensors_h

class DistanceSensors 
{
    public:
        DistanceSensors();
        String readDXSensors();
        boolean isTripped(uint8_t sensor);
        
    private:
        Ultrasonic sensorDX1;
        Ultrasonic sensorDX2; 
        Ultrasonic sensorDX3;	
        long dx1TrippedAt = 0, dx2TrippedAt = 0, dx3TrippedAt = 0;
        boolean dx1Tripped = false, dx2Tripped = false, dx3Tripped = false;
        boolean dx1TripWait = false, dx2TripWait = false, dx3TripWait = false;
        int dx1=0, dx2=0, dx3=0;
};

#endif