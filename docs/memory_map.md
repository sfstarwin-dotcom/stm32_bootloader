# STM32F446RE Flash Memory Map

| Region | Sector(s) | Base Address | Size | Function |
| :--- | :--- | :--- | :--- | :--- |
| **Bootloader** | Sectors 0–1 | `0x08000000` | 32 KB | Boot, Verification, & Rollback Logic |
| **Slot A (Active)** | Sectors 2–5 | `0x08008000` | 224 KB | Primary Application Image |
| **Slot B (Backup)** | Sectors 6–7 | `0x08040000` | 256 KB | Backup / Rollback Image |