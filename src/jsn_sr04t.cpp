#include  "jsn_sr04t.h"

void jsnSr04t::setup( int tp, int ep ) {
  trigger_pin = tp;
  echo_pin = ep;
  pinMode(trigger_pin,OUTPUT);
  pinMode(echo_pin,INPUT);
}

float jsnSr04t::__getRangeMeters( float* rejectRatioPtr ) {

    // general approach
    // 1) Try to take RANGE_SAMPLES number of samples
    // 2) Store only those that are in mechanical range of the device
    // 3) "buketize" into 100us intervals, count how many in each bucket

    float durations[SR04T_RANGE_SAMPLES];
    int   durationsBucketized[SR04T_RANGE_SAMPLES];
    int   bucketTimesSeen[SR04T_RANGE_SAMPLES];
    int   mostTimesBucketSeen = -1;
    float bucketMostSeenValue = -1;

    int storedDurations = 0;

    int samplesOutOfRange = 0;
    int samplesRejected = 0;

    digitalWrite(trigger_pin, LOW);        // Return trigger pin back to LOW again.

    int storedSampleIndex = 0;
    for ( int i=0; i < SR04T_RANGE_SAMPLES; i++ ) {
        delay(10);

        ////////////////////////////
        //make the measurement
        //noInterrupts();
        digitalWrite(trigger_pin, HIGH);       // Set trigger pin HIGH 
        delayMicroseconds(20);              // Hold pin HIGH for 20 uSec
        digitalWrite(trigger_pin, LOW);        // Return trigger pin back to LOW again.
        //interrupts();
        float duration = pulseIn(echo_pin,HIGH);  // Measure time in uSec for echo to come back.

        // store off all masurements that are within hardware range
        // note which bucket it is in
        // make counts of those buckets (biggest bucket will set the center of accepted range)
        if ( duration < SR04T_RANGE_MAX_DURATION && duration > SR04T_RANGE_MIN_DURATION ) {
            durations[storedSampleIndex] = duration;                        //real duration
            durationsBucketized[storedSampleIndex] = (int)(duration/10);    //put into a bucket, store the bucket value
            bucketTimesSeen[storedSampleIndex] = 0;                         //initialize the counts

            for ( int j=0; j <= storedSampleIndex; j++ ) { 
		        if ( durationsBucketized[j] == durationsBucketized[storedSampleIndex] ) {
		            bucketTimesSeen[j]++;
                }
                if ( bucketTimesSeen[j] >  mostTimesBucketSeen ) { 
			        mostTimesBucketSeen = bucketTimesSeen[j]; 
			        bucketMostSeenValue = durationsBucketized[j];
                }
            }
            storedSampleIndex++;
        } else {
            samplesOutOfRange++;
        }
    }
    storedDurations = storedSampleIndex;
    // should exit this with 1) a list of durations, value when bucketized, time that bucket has been seen
    //                       2) the value of the most seen bucket

    // now we will run thought the samples (that were in mech range) but elimiate outliers

    float retainedDurations = 0;
    float retainedDurationsSum = 0;
    for ( int i=0; i < storedDurations; i++ ) {
         //Serial.printf("  S:%d Sd:%d i:%d d:%.2f d:%d bs:%d ", SR04T_RANGE_SAMPLES, storedDurations, i, durations[i], 
          ////                                                      durationsBucketized[i], bucketTimesSeen[i] );
        if ( abs( durationsBucketized[i] - bucketMostSeenValue ) <  ( bucketMostSeenValue * 0.05 ) ) { 
            //Serial.println("RETAINED");
            retainedDurationsSum += durations[i];
            retainedDurations++;
        } else {
            samplesRejected++;
            //Serial.println("REJECTED");
        }
    }
    
    float rangeInMeters = ( ( retainedDurationsSum / 2 ) / retainedDurations ) * SR04T_METERS_PER_US; 

    Serial.printlnf(" Range:%.4f samples:%d outOfRange:%d rejected:%d RejectRate:%.0f%% ", 
        rangeInMeters,
        SR04T_RANGE_SAMPLES,
        samplesOutOfRange,
        samplesRejected,
         ( (samplesOutOfRange +  samplesRejected + 0.0 ) / SR04T_RANGE_SAMPLES ) * 100.0 );

    *rejectRatioPtr = ( 0.0 + SR04T_RANGE_SAMPLES - retainedDurations ) / SR04T_RANGE_SAMPLES;

    if ( retainedDurations < SR04T_RANGE_REQD_GOOD_SAMPLES ) { 
        return -1; 
    } else {
        return rangeInMeters;
    }
}

float jsnSr04t::getRangeMeters( float* rejectRatioPtr ) {
	float range = __getRangeMeters( rejectRatioPtr );
        return range;
}

float jsnSr04t::getRangeMeters( ) {
        float *rejectRatioPtr;  // not used here
	float range = __getRangeMeters( rejectRatioPtr );
        return range;
}

