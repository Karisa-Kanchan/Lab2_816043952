/*
 * Name: Karisa Kanchan
 * Student ID: 816043952
 * Lab 2 - Question 5
 * File: q2.c
 *
 * Purpose:
 * Create three FreeRTOS tasks and use a binary semaphore to
 * coordinate access to a shared LED on the Arduino MKR Zero.
 *
 * Task 1: Turn the LED on, actively wait for 500 ms, then yield.
 * Task 2: Turn the LED off, then block for 1000 ms.
 * Task 3: Print a status message, then block for 1000 ms.
 */

#include <stddef.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "q2.h"

/* Keep the distinct priorities used for the Question 4 comparison. */
static const int TASK1_PRIORITY = 1;
static const int TASK2_PRIORITY = 2;
static const int TASK3_PRIORITY = 3;

static SemaphoreHandle_t led_semaphore;

/* Actively wait without putting the task into the Blocked state. */
static void active_wait_500ms(void)
{
    uint32_t start = board_time_us();

    while ((uint32_t)(board_time_us() - start) < 500000UL)
    {
        /* Busy waiting. */
    }
}

/* Task 1: turn the LED on, actively wait, and then yield. */
static void task1(void *parameters)
{
    (void)parameters;

    for (;;)
    {
        if (xSemaphoreTake(led_semaphore, portMAX_DELAY) == pdTRUE)
        {
            board_led_write(1);
            active_wait_500ms();

            xSemaphoreGive(led_semaphore);
        }

        taskYIELD();
    }
}

/* Task 2: turn the LED off, then block for one second. */
static void task2(void *parameters)
{
    (void)parameters;

    for (;;)
    {
        if (xSemaphoreTake(led_semaphore, portMAX_DELAY) == pdTRUE)
        {
            board_led_write(0);

            xSemaphoreGive(led_semaphore);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* Task 3: print a status report, then block for one second. */
static void task3(void *parameters)
{
    (void)parameters;

    for (;;)
    {
        board_status_print();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int q2_start(void)
{
    led_semaphore = xSemaphoreCreateBinary();

    if (led_semaphore == NULL)
    {
        return 0;
    }

    /* A new binary semaphore is empty; make it available to the tasks. */
    if (xSemaphoreGive(led_semaphore) != pdTRUE)
    {
        return 0;
    }

    if (xTaskCreate(task1, "LED_ON", 256, NULL,
                    TASK1_PRIORITY, NULL) != pdPASS)
    {
        return 0;
    }

    if (xTaskCreate(task2, "LED_OFF", 256, NULL,
                    TASK2_PRIORITY, NULL) != pdPASS)
    {
        return 0;
    }

    if (xTaskCreate(task3, "STATUS", 256, NULL,
                    TASK3_PRIORITY, NULL) != pdPASS)
    {
        return 0;
    }

    vTaskStartScheduler();

    /* Reaching here means the scheduler could not start. */
    return 0;
}