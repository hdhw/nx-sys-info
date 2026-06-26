# SysInfo — Nintendo Switch Homebrew

Nyx-inspired system information viewer. Pulls everything: firmware, Atmosphere
version, hardware model, emummc status, battery, network, storage, clocks, temps,
device nickname, accounts, and more.

---

## Requirements

- devkitPro with Switch support (devkitA64)
- libnx

---

## Build (Windows — devkitPro MSYS2 shell)

Open the devkitPro MSYS2 shell (Start → devkitPro → MSYS2).

```bash
# 1. Install deps if you haven't
pacman -S switch-dev

# 2. Clone / navigate to the project folder
cd /path/to/switch-sysinfo

# 3. Build
make

# Output: sysinfo.nro
```

The `.nro` goes in `/switch/sysinfo/sysinfo.nro` on your SD card.

---

## Controls

| Button | Action         |
|--------|---------------|
| A      | Refresh info   |
| +      | Exit           |

---

## What it shows

| Section          | Fields                                                              |
|-----------------|---------------------------------------------------------------------|
| Device Identity  | Nickname, user accounts, WiFi/BT/NFC state, sleep, theme           |
| Firmware         | Version string, display title, platform, region, language           |
| Atmosphere       | AMS version, target FW, EmuMMC type, privileged PID                |
| Hardware         | Serial, Device ID, SoC type (Icosa/Iowa/etc), DRAM config          |
| Battery          | Charge %, charger type                                              |
| Network          | Type (WiFi/Ethernet), local IP, MAC address                        |
| Storage          | NAND System, NAND User, SD Card — free/total GB each               |
| Clocks & Temps   | CPU/GPU/EMC MHz, CPU/GPU °C with color-coded heat warnings         |
