#include <inttypes.h>
#include "esp_sleep.h"
#include "esp_cpu.h"
#include "esp_rom_sys.h"
#include "esp_wake_stub.h"
#include "sdkconfig.h"

#include "esp_rom_sys.h"
#include "hal/rtc_io_ll.h"
#include "rom/rtc.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "soc/sens_periph.h"
#include "soc/timer_group_reg.h"
#include "soc/uart_reg.h"
#include "esp_private/esp_clk.h"
#include "rtc_wake_stub_bath.h"
#include "main.h"

//Define the maximum events 
#define EVENT_MAX_COUNT 10

//Counter value stored in RTC memory
RTC_IRAM_ATTR uint32_t s_count = 0;
RTC_IRAM_ATTR uint32_t max_count = EVENT_MAX_COUNT;

//Wakeup_cause and count
RTC_IRAM_ATTR uint32_t wakeup_count= 0;
RTC_IRAM_ATTR uint32_t wakeup_cause= 0;   // 0 = undefined, 1 = ext0, 2 = ext1, 3 = timer

//Wakeup_time from CPU start to wake stub
RTC_IRAM_ATTR uint32_t last_timestamp = 0;  // Ensure it's in RTC memory

//Events store array  (max events)
RTC_IRAM_ATTR Event event_buffer[EVENT_MAX_COUNT];
// Variable to store the timestamp of the last event


// my_rtc_time_get_us() returns the rtc clock value in the wakeup stub
RTC_IRAM_ATTR uint64_t my_rtc_time_get_us(void) {

  // Trigger RTC time update
  SET_PERI_REG_MASK(RTC_CNTL_TIME_UPDATE_REG, RTC_CNTL_TIME_UPDATE);
  while (GET_PERI_REG_MASK(RTC_CNTL_TIME_UPDATE_REG, RTC_CNTL_TIME_VALID) == 0) {
    ets_delay_us(1);  // might take 1 RTC slowclk period, don't flood RTC bus
  }
  SET_PERI_REG_MASK(RTC_CNTL_INT_CLR_REG, RTC_CNTL_TIME_VALID_INT_CLR);

  // Read the RTC time registers
  uint64_t t = READ_PERI_REG(RTC_CNTL_TIME0_REG);
  t |= ((uint64_t)READ_PERI_REG(RTC_CNTL_TIME1_REG)) << 32;

  // Convert to microseconds using RTC clock period
  uint32_t period = REG_READ(RTC_SLOW_CLK_CAL_REG);
  uint64_t now1 = ((t * period) >> RTC_CLK_CAL_FRACT);
  return now1;
}

void print_event_chain() {
    ESP_RTC_LOGI("Printing stored events...");

    // Loop through the event buffer and print the details of each event
    for (uint32_t i = 0; i < s_count; i++) {
        ESP_RTC_LOGI("Event %u: Timestamp: %u, Sensor ID: %u", i, event_buffer[i].timestamp, event_buffer[i].sensor_id);
    }
}

// Function to reset the device
RTC_IRAM_ATTR void reboot_device() {
    esp_restart();
}

// Wake-up stub
void wake_stub_bath(void) {
    ets_printf("Wakeup Stub Executed\n");

    // Watchdog timer feed
    REG_WRITE(TIMG_WDTFEED_REG(0), 1);

    // Get wakeup time and cause
    // wakeup_time = esp_cpu_get_cycle_count() / esp_rom_get_cpu_ticks_per_us();
    wakeup_cause = esp_wake_stub_get_wakeup_cause();
    
    ESP_RTC_LOGI("Wakeup Cause: %d", wakeup_cause);
    
    uint32_t timestamp = my_rtc_time_get_us();
    ESP_RTC_LOGI("Current Timestamp: %u, Last Timestamp: %u", timestamp, last_timestamp);

    // Handle wakeup event based on cause
    switch (wakeup_cause) {
        case 1:  // External wakeup (ext0)
        case 2:  // External wakeup (ext1)
            if (timestamp - last_timestamp >= 1000000) {  // 1 second = 1,000,000 microseconds
                if (s_count < EVENT_MAX_COUNT) {
                    event_buffer[s_count].timestamp = timestamp;
                    event_buffer[s_count].sensor_id = 0;  // Default sensor ID
                    ESP_RTC_LOGI("Stored event at position %u", s_count);
                    last_timestamp = timestamp;  // Update the last timestamp
                    s_count++;
                    
                } else {
                    ESP_RTC_LOGI("Event buffer full, waking up main CPU");
                    esp_default_wake_deep_sleep();  // Ensure to process the buffer
                }
            }
            break;

        case 3:  // Timer wakeup
            ESP_RTC_LOGI("Wake stub: timer wakeup detected");
            break;

        default:  // Undefined wakeup cause
            ESP_RTC_LOGI("Wake stub: undefined wakeup cause");
            break;
    }
    // Print the event chain after storing the events
    print_event_chain();
    // Check if buffer is full or needs to wake main CPU
    if (s_count >= max_count) {
        ESP_RTC_LOGI("Event buffer full, waking up main CPU to process events");
        esp_default_wake_deep_sleep();
    } 
    //else {
    //     ESP_RTC_LOGI("Returning to deep sleep for next event");
    //     esp_wake_stub_set_wakeup_time(30 * 1000000);  // Set next wakeup in 30 seconds
    //     esp_wake_stub_sleep(&wake_stub_bath);  // Go back to deep sleep
    // }
}




































// //Deadline for sending the event to MQTT
// #define PIR_EVENT_DEADLINE 60000  //milliseconds





//PIR Counts !!!
// RTC_DATA_ATTR static int pir_event_count = 0; // To track PIR event count
// RTC_DATA_ATTR static uint64_t pir_event_time = 0; // To store the timestamp of when the PIR event occurred



    //uint32_t last_event = 0;  // Store last PIR event time to manage deadline

    // if (wakeup_cause == ESP_SLEEP_WAKEUP_EXT0) {
    //     // If it's the first PIR event after wake-up, record the timestamp
    //     if (pir_event_count == 0) {
    //         pir_event_time = esp_timer_get_time(); // Current timestamp in microseconds
    //         // Print the recorded timestamp
    //         ets_printf("!!!PIR event timestamp: %lld microseconds\n", pir_event_time);
    //     }
    
    //     ets_printf("DESPUES DEL IF\n");
    //     pir_event_count++; // Increment the count

    //     // Current PIR event count
    //     ets_printf("!!!Current PIR event count: %d\n", pir_event_count);
    //     rtc_printf("PIR event count: %d\n", pir_event_count);


    //     // Check if the deadline has passed (if more than PIR_EVENT_DEADLINE ms passed)
    //     uint64_t current_time = esp_timer_get_time(); // Current timestamp
    //     uint64_t time_elapsed = (current_time - pir_event_time) / 1000; // Elapsed time in milliseconds

    //     // Print the current time and elapsed time
    //     ets_printf("!!!Current time: %lld microseconds\n", current_time);
    //     ets_printf("!!!Time elapsed: %lld ms\n", time_elapsed);

    //     if (time_elapsed >= PIR_EVENT_DEADLINE) {
    //         // Send the event to MQTT after the deadline
    //         ets_printf("!!!!PIR event sent to MQTT after deadline of %d ms\n", PIR_EVENT_DEADLINE);
    //         pir_event_count = 0; // Reset the PIR event count after sending
    //     }

    //     // Reboot if count reaches 5
    //     if (pir_event_count >= 5) {
    //         ets_printf("!!!!Rebooting device\n");
    //         reboot_device();
    //     }
     // }
// }





// // // mAXIMUM PIR Events
// #define MAX_PIR_EVENTS 10  // Configurable number of PIR events to store

// // Structure to store PIR event data
// typedef struct {
//     uint64_t timestamp;
//     int event_count;
// } pir_event_t;

// Array in RTC slow memory
// RTC_DATA_ATTR static pir_event_t pir_events[MAX_PIR_EVENTS];
// RTC_DATA_ATTR static int pir_event_index = 0;  // Tracks the number of events stored





    // if (pir_event_index < MAX_PIR_EVENTS) {
    // // Record the PIR event
    //     pir_events[pir_event_index].timestamp = current_time;
    //     pir_events[pir_event_index].event_count = pir_event_index + 1;
    //     pir_event_index++;  // Increment the event count
    //     last_pir_event_time = current_time;
    //     ets_printf("Stored PIR event: %d, Timestamp: %lld\n", pir_event_index, current_time);
    // }
    // else {
    //     // If maximum events are stored, reset the count after sending
    //     ets_printf("PIR event buffer full. Resetting index.\n");
    //     pir_event_index = 0;  // Reset the index to store new events
