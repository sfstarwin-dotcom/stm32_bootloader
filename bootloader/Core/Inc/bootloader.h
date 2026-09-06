#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint.h>
#include "main.h"

/* Memory Map Base Addresses (STM32F446RE Flash Partitioning) */
#define BOOTLOADER_SIZE       (0x8000U)      /* 32 KB (Sectors 0 & 1) */
#define SLOT_A_BASE_ADDR      (0x08008000U)  /* Active Slot (Sectors 2-5, 224 KB) */
#define SLOT_B_BASE_ADDR      (0x08040000U)  /* Backup Slot (Sectors 6-7, 256 KB) */

#define HEADER_MAGIC_0        (0xFF01FF02U)  /* Magic Pattern 0 */
#define HEADER_MAGIC_1        (0xFF03FF04U)  /* Magic Pattern 1 */

/* 32-Byte Application Metadata Structure embedded at Vector Table Offset */
typedef struct {
    uint32_t initial_msp;     /* Stack Pointer at 0x00 */
    uint32_t reset_handler;   /* Address of Reset_Handler at 0x04 */
    uint32_t magic_0;         /* HEADER_MAGIC_0 at 0x08 */
    uint32_t magic_1;         /* HEADER_MAGIC_1 at 0x0C */
    uint32_t image_size;      /* Binary image size in bytes at 0x10 */
    uint32_t crc32;           /* CRC-32 Checksum calculated post-build at 0x14 */
    uint32_t git_hash;        /* Git SHA-1 commit tag at 0x18 */
    uint32_t reserved;        /* Alignment padding at 0x1C */
} FirmwareHeader_t;

/* Function Prototypes */
void bootloader_run(void);
uint32_t compute_crc32(const uint8_t *p_data, uint32_t length);
uint8_t verify_slot_integrity(uint32_t slot_address);
void copy_slot_b_to_slot_a(void);
void jump_to_application(uint32_t app_address);

#endif /* BOOTLOADER_H */