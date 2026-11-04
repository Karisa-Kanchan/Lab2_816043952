/*
 * Name: Karisa Kanchan
 * Student ID: 816043952
 * Lab 2 - Question 2
 * File: main.cpp
 *
 * Purpose:
 * Initialise the Arduino hardware and start the FreeRTOS application.
 * Provide C-compatible functions for LED control, time measurement
 * and serial output.
 *
 * Hardware:
 * External LED connected to digital pin 2 through a series resistor.
 * Status messages transmitted through Serial1 and mirrored to SerialUSB.
 */

#include <Arduino.h>
#include <FreeRTOS_SAMD21.h>
#include <stdio.h>

#include "q2.h"
/* Counters defined in the C application. */
extern "C"
{
    extern volatile uint32_t task1_count;
    extern volatile uint32_t task2_count;
}

static const int LED_PIN = 2;

extern "C" void board_led_write(int on)
{
    digitalWrite(LED_PIN, on ? HIGH : LOW);
}

extern "C" uint32_t board_time_us(void)
{
    return micros();
}

extern "C" void board_status_print(void)
{
    /* Snapshot the counters before formatting the message. */
    const uint32_t count1 = task1_count;
    const uint32_t count2 = task2_count;
    const unsigned long time_ms = millis();

    char message[128];

    snprintf(message, sizeof(message),
             "Time=%lu ms | Task1=%lu | Task2=%lu | Task3 running",
             time_ms,
             (unsigned long)count1,
             (unsigned long)count2);

    /* Send the same evidence through hardware UART and USB. */
    Serial1.println(message);
    SerialUSB.println(message);
}

void setup(void)
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial1.begin(115200);
    SerialUSB.begin(115200);

    /* Allow time for the USB connection without waiting forever. */
    unsigned long start = millis();

    while (!SerialUSB && (millis() - start < 5000UL))
    {
    }

    vSetErrorSerial(&SerialUSB);

    SerialUSB.println("Starting Q2 FreeRTOS application");

    if (!q2_start())
    {
        SerialUSB.println("ERROR: task creation or scheduler startup failed");

        while (true)
        {
        }
    }
}

void loop(void)
{
    /* No application work is needed here. */
}