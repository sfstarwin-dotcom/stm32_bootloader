## 📁 Project Structure

```javascript
├── bootloader/
│   ├── Core/
│   │   ├── Inc/              # Header files (main.h, bootloader.h, etc.)
│   │   └── Src/              # Source files (main.c, bootloader.c, flash_ops.c)
│   ├── Startup/
│   │   └── startup_stm32f446xx.s
│   └── STM32F446RETx_FLASH.ld
├── scripts/
│   └── append_crc.py         # Post-build metadata injection script
├── docs/
│   └── memory_map.md
├── README.md
└── .gitignore
```

---
