#pragma once

// Possible messages received via uart
typedef enum {
    MESSAGE_STOP = 0x00,
    MESSAGE_N = 0x01,
    MESSAGE_NE = 0x02,
    MESSAGE_E = 0x03,
    MESSAGE_SE = 0x04,
    MESSAGE_S = 0x05,
    MESSAGE_SW = 0x06,
    MESSAGE_W = 0x07,
    MESSAGE_NW = 0x08,
    MESSAGE_BT_CONNECT = 0x0A
} message_list_t;

// Masks the bits associated with each task 
typedef enum {
    MESSAGE_MOTOR_EVENT_FLAG_MASK  = 0b11111,
    MESSAGE_BT_EVENT_FLAG_MASK    = 0b100000,
    MESSAGE_EXT_LED_EVENT_FLAG_MASK  = 0b1000000    
} message_event_flags;
