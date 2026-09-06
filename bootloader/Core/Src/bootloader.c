#include "bootloader.h"
#include <string.h>

/* Software CRC-32 (IEEE 802.3 standard polynomial: 0xEDB88320) */
uint32_t compute_crc32(const uint8_t *p_data, uint32_t length) {
    uint32_t crc = 0xFFFFFFFFU;
    for (uint32_t i = 0; i < length; i++) {
        crc ^= p_data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 1U) {
                crc = (crc >> 1U) ^ 0xEDB88320U;
            } else {
                crc >>= 1U;
            }
        }
    }
    return ~crc;
}

uint8_t verify_slot_integrity(uint32_t slot_address) {
    const FirmwareHeader_t *header = (const FirmwareHeader_t *)slot_address;

    /* Validate Header Magic Constants */
    if (header->magic_0 != HEADER_MAGIC_0 || header->magic_1 != HEADER_MAGIC_1) {
        return 0; /* Invalid Header Format */
    }

    /* Compute CRC32 across binary payload (skipping CRC field at offset 0x14) */
    const uint8_t *binary_data = (const uint8_t *)slot_address;
    uint32_t computed_crc = compute_crc32(binary_data + sizeof(FirmwareHeader_t), 
                                          header->image_size - sizeof(FirmwareHeader_t));

    return (computed_crc == header->crc32) ? 1 : 0;
}

void copy_slot_b_to_slot_a(void) {
    HAL_FLASH_Unlock();

    /* Erase Slot A Sectors (Sectors 2, 3, 4, 5) */
    FLASH_EraseInitTypeDef erase_init = {
        .TypeErase = FLASH_TYPEERASE_SECTORS,
        .Sector = FLASH_SECTOR_2,
        .NbSectors = 4,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3
    };
    uint32_t sector_error = 0;
    HAL_FLASHEx_Erase(&erase_init, &sector_error);

    /* Copy raw bytes from Slot B to Slot A */
    const FirmwareHeader_t *header_b = (const FirmwareHeader_t *)SLOT_B_BASE_ADDR;
    uint32_t copy_size = header_b->image_size;

    for (uint32_t i = 0; i < copy_size; i += 4) {
        uint32_t data_word = *(__IO uint32_t *)(SLOT_B_BASE_ADDR + i);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, SLOT_A_BASE_ADDR + i, data_word);
    }

    HAL_FLASH_Lock();
}

void jump_to_application(uint32_t app_address) {
    __disable_irq();

    /* De-initialize HAL and Peripheral Clocks */
    HAL_DeInit();

    /* Extract initial Stack Pointer and Reset Vector */
    uint32_t app_msp = *(__IO uint32_t *)app_address;
    uint32_t app_reset_handler = *(__IO uint32_t *)(app_address + 4);

    void (*app_jump)(void) = (void (*)(void))app_reset_handler;

    /* Relocate Vector Table Offset Register (VTOR) */
    SCB->VTOR = app_address;

    /* Set MSP and branch execution */
    __set_MSP(app_msp);
    app_jump();
}

void bootloader_run(void) {
    /* Step 1: Verify Active Slot A */
    if (verify_slot_integrity(SLOT_A_BASE_ADDR)) {
        jump_to_application(SLOT_A_BASE_ADDR);
    } 
    /* Step 2: Fallback to Backup Slot B if Slot A is corrupted */
    else if (verify_slot_integrity(SLOT_B_BASE_ADDR)) {
        copy_slot_b_to_slot_a();
        jump_to_application(SLOT_A_BASE_ADDR);
    } 
    /* Step 3: Hard Error (Unrecoverable system corruption) */
    else {
        while (1) {
            /* Trap core & toggle error LED */
        }
    }
}