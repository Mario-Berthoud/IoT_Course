#pragma once

//CAPS Seminar IP
// #define MQTT_BROKER                "131.159.85.238" 
// #define EXAMPLE_ESP_WIFI_SSID      "CAPS-Seminar-Room"
// #define EXAMPLE_ESP_WIFI_PASS      "caps-schulz-seminar-room-wifi"
// #define SNTP_SERVER_NAME           "ntp1.in.tum.de"

//House IP
#define MQTT_BROKER                "192.168.1.132" 
#define EXAMPLE_ESP_WIFI_SSID      "SSN_AP_Tr9ANH"
#define EXAMPLE_ESP_WIFI_PASS      "ykKTzYzHnt9F"
#define SNTP_SERVER_NAME           "pool.ntp.org"

//BATH old code
// #define DEVICE_ID                  "3"
// #define DEVICE_TOPIC               "1/3/data"
// #define DEVICE_KEY                 "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE3MzA5MjYzMTksImlzcyI6ImlvdHBsYXRmb3JtIiwic3ViIjoiMS8zIn0.mb1M-Xum890gHZGEooyuY9B6wZ3qhBotjKN6Z8LHdBynTQVhr9hbC0fuNskbVNBULycYwOR3OnwFLr2vsefAvPLAdCsXvbGzlxx-SFGu9fbn7U949wcFwD87XM9KOCK4aH_F6pieHFwJ8wkC4_EdnR5gfYDbAVL5Ox7Dl9jd9TsKWlF43KZkktrAAApqM_tEZN_1ouN7GTZ5RRn7oJYZjaXCgt85EuQ6ayKJIxELp3b5WYj2ofxUXvQpbKUG0mOKJ5X17xUQ-MrKUfWle1qYrOBH9YPVbBmeQWWHLOkbX1lbHG5cuOvk00RXUcJuDPG1GMD0e9V93IXs33-hoeeNrGRnjuJp_xBcPERmA2l_uAVFPTkMG4Tq5oL64ljBzR6o6r50ruy5HhGxxcdhhZq_Zd6vNzp-mDrPygi_9jXmf351E9bNRlNmeKcxPV-iyGLzzaQT-x_t03dBBNIXFhldAJt4UmR7Ea7MW35_TfgGOpiGDvdnRVmkRU1-icxb8ApHu5Ej_AJPQazZrn1ZPQfq4zlNLNzAeGK0p3XczYBvmkr8UDPk8rnX4kXeVb0da_CezLOOQUc98LjWvglEz9i2aNt1FbcAjWIGe3vaWp4zq-HiI_zKPfu8OeBcnCVwC5SUSz7Oicfqycogk-jqiz411sfh4rwYv8Oh8g8VGZnbJBE"

//GPIOs 
#define PIR_PIN     27 
#define DOOR_PIN    33 

//Device Properties
extern char* DEVICE_ID;
extern char* DEVICE_TOPIC;
extern char* DEVICE_KEY;
