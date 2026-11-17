/*
 * Name: Karisa Kanchan
 * Student ID: 816043952
 * Lab 2 - Question 4
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
#include "task.h"

#include "q2.h"

static const int LED_PIN = 2;

extern "C" void board_led_write(int on)
{
    digitalWrite(LED_PIN, on ? HIGH : LOW);
}

extern "C" uint32_t board_time_us(void)
{
    return micros();
}

/* Convert a task state into text for the Q4 trace report. */
static const char *task_state_name(eTaskState state)
{
    switch (state)
    {
        case eRunning:   return "Running";
        case eReady:     return "Ready";
        case eBlocked:   return "Blocked";
        case eSuspended: return "Suspended";
        case eDeleted:   return "Deleted";
        default:         return "Unknown";
    }
}

extern "C" void board_status_print(void)
{
    /* Include the three application tasks and FreeRTOS system tasks. */
    static TaskStatus_t task_status[8];
    uint32_t total_run_time = 0U;

    UBaseType_t task_count =
        uxTaskGetSystemState(task_status, 8U, &total_run_time);

    if (task_count == 0U)
    {
        Serial1.println("Q4 trace: task snapshot unavailable");
        SerialUSB.println("Q4 trace: task snapshot unavailable");
        return;
    }

    char line[128];

    snprintf(line, sizeof(line),
             "Q4 trace at %lu ms | total run time=%lu us",
             (unsigned long)millis(),
             (unsigned long)total_run_time);

    Serial1.println(line);
    SerialUSB.println(line);

    for (UBaseType_t i = 0U; i < task_count; i++)
    {
        snprintf(line, sizeof(line),
                 "  %s | priority=%lu | state=%s | run time=%lu us",
                 task_status[i].pcTaskName,
                 (unsigned long)task_status[i].uxCurrentPriority,
                 task_state_name(task_status[i].eCurrentState),
                 (unsigned long)task_status[i].ulRunTimeCounter);

        Serial1.println(line);
        SerialUSB.println(line);
    }
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