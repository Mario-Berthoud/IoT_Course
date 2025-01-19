#include "main.h"

#include <stdio.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gauge.h"
#include "mqtt.h"
#include "nvs_flash.h"
#include "sntp.h"
#include "wifi.h"
#include "esp_sleep.h"

#include "rtc_wake_stub_bath.h"

//MAC Bath     EC:62:60:BC:E8:20
//MAC Corridor EC:62:60:BC:E7:D8
//MAC Bed      94:B9:7E:42:14:C8

// Define GPIOs
#define PIR_PIN 27 
#define DOOR_PIN 33

// Sleep interval in microseconds (x minutes) 
#define SLEEP_TIME (2 * 60 * 1000000)  

//Define the maximum events 
#define EVENT_MAX_COUNT 10

char* DEVICE_ID;
char* DEVICE_TOPIC;
char* DEVICE_KEY;

int count = 0;

void IRAM_ATTR handlePIRevent(void *arg) {
    count++;
    ets_printf("Got PIR event %d\n", count);
    
}

void app_main() {

    //Store MAC 
    uint8_t mac[6];
    char mac_str[18];

    //Read MAC
    esp_err_t result = esp_efuse_mac_get_default(mac);

    if (result == ESP_OK) {
        //Change MAC to a chain in format XX:XX:XX:XX:XX:XX
        snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        
        printf("!!!!Current MAC address: %s\n", mac_str);

        //MAC adress comparation
        if (strcmp(mac_str, "EC:62:60:BC:E7:D8") == 0) {           
            printf("CORRIDOR ESP.\n");
            DEVICE_ID=                  "2";
            DEVICE_TOPIC=               "1/2/data";
            DEVICE_KEY=                 "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE3MzAzMjk3OTcsImlzcyI6ImlvdHBsYXRmb3JtIiwic3ViIjoiMS8yIn0.rxYvyObaG2jm5yUApyQedVc7JvC3zY5BWjyj--5LgPyz-6f_1PloApKW3reVtQ3sgWgOZOqPFZv4-pMvS2LBgj2N_ZjvDaYZftWf4mgRid9FH3jSdvfthZrO_9oJOIQ7ayZm_3Er5EIUlPDDhd5M8m1-CCwjuIp5_M4M4ASkcH9zj0eylrMIXT_5aQZjVcm7rcWpMQKcs7gHpXP8KBUQSFTtkjYo0oJL_dyKIdP2QdUPzGAjKyON3joVygDpG2BADAMRE7nt6O3sDhjrOuGpLCNFem-M38M_7nJ83KVlsp3OH_4gT4zFSJEBzYp1HrQb1-98XGcA41hoagrQPP6zoLlzz3YGBfFS5RSapfg-B4gE3F0ZEoaOTcO27YOqBR35IluDGW_BIoOCt_11WW2WQ5gNtaSesDXZMpY-6sI_cepuukPsdMetVxc9cKuEr6AtZIPkHkxa5prtv1X699bAPJOgY5NBUtRwc41Z-l5I-AWCWYdgw2haAVRsKqs_eIoIBqA73pzqdKbJLglcueKdhaM8-trpSx941-8kuDu95s6aSnlSHdIix0Ejb4BYDs1yUdLnwhsEDwD_WyO8Z1ro1p080WYc5YpZUCWUGhitqQBx0CEKMyeSMkxMfO2SdgitFH5b1VY9XElRnz2_3ARraYeOtcHB2zdcQ4TBNditu3Q";
            
        } 
        else if (strcmp(mac_str, "EC:62:60:BC:E8:20") == 0) {            
            printf("BATH ESP.\n");            
            DEVICE_ID=                  "3";
            DEVICE_TOPIC=               "1/3/data";
            DEVICE_KEY=                 "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE3MzA5MjYzMTksImlzcyI6ImlvdHBsYXRmb3JtIiwic3ViIjoiMS8zIn0.mb1M-Xum890gHZGEooyuY9B6wZ3qhBotjKN6Z8LHdBynTQVhr9hbC0fuNskbVNBULycYwOR3OnwFLr2vsefAvPLAdCsXvbGzlxx-SFGu9fbn7U949wcFwD87XM9KOCK4aH_F6pieHFwJ8wkC4_EdnR5gfYDbAVL5Ox7Dl9jd9TsKWlF43KZkktrAAApqM_tEZN_1ouN7GTZ5RRn7oJYZjaXCgt85EuQ6ayKJIxELp3b5WYj2ofxUXvQpbKUG0mOKJ5X17xUQ-MrKUfWle1qYrOBH9YPVbBmeQWWHLOkbX1lbHG5cuOvk00RXUcJuDPG1GMD0e9V93IXs33-hoeeNrGRnjuJp_xBcPERmA2l_uAVFPTkMG4Tq5oL64ljBzR6o6r50ruy5HhGxxcdhhZq_Zd6vNzp-mDrPygi_9jXmf351E9bNRlNmeKcxPV-iyGLzzaQT-x_t03dBBNIXFhldAJt4UmR7Ea7MW35_TfgGOpiGDvdnRVmkRU1-icxb8ApHu5Ej_AJPQazZrn1ZPQfq4zlNLNzAeGK0p3XczYBvmkr8UDPk8rnX4kXeVb0da_CezLOOQUc98LjWvglEz9i2aNt1FbcAjWIGe3vaWp4zq-HiI_zKPfu8OeBcnCVwC5SUSz7Oicfqycogk-jqiz411sfh4rwYv8Oh8g8VGZnbJBE";

        } 
        else if (strcmp(mac_str, "94:B9:7E:42:14:C8") == 0) {            
            printf("BED ESP.\n");
            DEVICE_ID=                  "4";
            DEVICE_TOPIC=               "1/4/data";
            DEVICE_KEY=                 "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE3MzA5Mjc0MjAsImlzcyI6ImlvdHBsYXRmb3JtIiwic3ViIjoiMS80In0.I_6fWB_lDEytKi9ZP5PsS1miL--isKz9EGzTYzSRl-vTyeKscu5WCE8oHS1JhnDpjPAsTwvAHxaNaS4cjlG4_ZGi1pjfF1YGmpPqw3slJDbKkxrLp8x_643AedCbvpQMVvuuOQQlO4-bJaA5A1yyvTd5r-Bgbf6C4hLzv8GlNa6U7wvEaU5uPoZIYuXW3k7W9V7hY8jQ920ve-xzuoh8Y1AD-6BooRdbyTK1GfDXO0Jlk8w19oVP5cysTGSmWH57hI-_5IitXmfXHrxSiEPOX5FNVBm3DqLmz93N6xRYynCvvl3G9BxqHeSvNjBj2gt-ZKm5mYtVNctim1MJ5jOeVWsF7WEviU1PXe4pWWGujk37leIlIZpBftNmodXu_jbmpD978vwTT6U8kX4hwyafUB9AnnBKkn7KChogwCskUBxYa2IKJe_f3CWThaZCVNapBaacHawWkSUuB3V489pOIEOhHGnHa5asDprJRkMjcViQULUaEFW6l-516AOQAfOyruAAeEl5sKRPme0btZ0XAqDuhFz78_EPoYqXdUjDdQN44qDyMs306x3Kq1eWq-62uYNAaMQ5y7k_vEhSEBWU9fJx4-d0lOQuRbJfjaikzagKXdVt4Jwjp142QBLhzSf9PwJztyfFXqXyLJSsf5KVB9N9kff8bAmHQdKSSrMK8Zk";

        } 
        else {            
            printf("Unknown MAC.\n");

        }      
    } 
    
    else {
        printf("Error al obtener la dirección MAC: %s\n", esp_err_to_name(result));
    }


    if (strcmp(mac_str, "94:B9:7E:42:14:C8") != 0) { 
    getRSOC();
    }

    ESP_LOGI("progress", "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI("progress", "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_ERROR);
    esp_log_level_set("mqtt", ESP_LOG_INFO);
    esp_log_level_set("progress", ESP_LOG_INFO);
    esp_log_level_set("gauge", ESP_LOG_INFO);

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //Initialize Components
    ESP_LOGI("progress", "Starting Wifi");
    start_wifi();

    ESP_LOGI("progress", "Starting Clock");
    start_clock();

    ESP_LOGI("progress", "Starting MQTT");
    start_mqtt();

    // if(strcmp(mac_str, "94:B9:7E:42:14:C8") != 0) {
    // // Send Battery Status to Golden ESP
    // ESP_LOGI("progress", "Wake up count is %d", count);

    if (wakeup_count >= 10) {
        ESP_LOGI("progress", "Sending battery status to MQTT");
        sendBatteryStatusToMQTT();
        wakeup_count = 0;
    } 
    else {
        wakeup_count++;
        ESP_LOGI("progress", "Wake count: %d", wakeup_count);
    }

    // ESP_LOGI("progress", "Sending battery status to MQTT");
    // sendBatteryStatusToMQTT();

    if (wakeup_cause==1){
        ESP_LOGI("progress", "SAVING PIR event to MQTT");
        //sendPIReventToMQTT();
        //sendStoredEventsToMQTT();
    }

    if (s_count >= EVENT_MAX_COUNT) {
        sendStoredEventsToMQTT();
    }

    if (strcmp(mac_str, "EC:62:60:BC:E8:20") == 0 && wakeup_cause==2 && s_count >= EVENT_MAX_COUNT) {

        ESP_LOGI("progress", "Sending Door event to MQTT");
        sendDoorEventToMQTT();
    }

    // Initialize GPIO pins
    ESP_ERROR_CHECK(gpio_set_direction(PIR_PIN, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(rtc_gpio_pulldown_en(PIR_PIN));
    ESP_ERROR_CHECK(gpio_set_direction(DOOR_PIN, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(rtc_gpio_pullup_en(DOOR_PIN));

    // Enable external wake-up sources (PIR and DOOR GPIO pins)
    const uint64_t ext1_wakeup_mask = (1ULL << DOOR_PIN);
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(PIR_PIN, 1));
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(ext1_wakeup_mask, ESP_EXT1_WAKEUP_ANY_HIGH));

    // Enable periodic wake-up every 2 minutes (120,000,000 microseconds)
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(SLEEP_TIME));

    // ESP_LOGI("progress", "Installing wakeup");

    // Main loop for checking PIR sensor and managing sleep/wake cycles
    while (gpio_get_level(PIR_PIN) == 1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Log the process before going to sleep
    ESP_LOGI("progress", "Going to sleep");
    esp_set_deep_sleep_wake_stub(&wake_stub_bath);
    esp_deep_sleep_start();
}