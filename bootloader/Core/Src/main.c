#include "main.h"
#include "bootloader.h"

void SystemClock_Config(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();

    /* Run Bootloader Validation & Handoff Sequence */
    bootloader_run();

    while (1) {
        /* Unreachable */
    }
}

void SystemClock_Config(void) {
    /* CubeMX Generated Clock Configuration */
}