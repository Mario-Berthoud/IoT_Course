#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void wake_stub_example(void);
void wake_stub_bath(void);

typedef struct {
    uint64_t timestamp;
    uint64_t sensor_id;
} Event;

// Store declarations
extern RTC_DATA_ATTR Event event_buffer[];    //Events stores in an aarray
extern RTC_DATA_ATTR uint32_t s_count;        //Event counter
extern RTC_DATA_ATTR Event lastsent_event;    //Last event
extern RTC_DATA_ATTR uint32_t wakeup_count;     //Wake Ups Counter
extern RTC_DATA_ATTR uint32_t wakeup_cause;     //Wake Ups Cause



// // Structure to store PIR event data
// typedef struct {
//     uint64_t timestamp;
//     int event_count;
// } pir_event_t;

// Array in RTC slow memory
// extern RTC_DATA_ATTR static pir_event_t pir_events[MAX_PIR_EVENTS];
// extern RTC_DATA_ATTR static int pir_event_index = 0;  // Tracks the number of events stored

#ifdef __cplusplus
}
#endif
