#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <switch.h>

#define COL_RESET   "\x1b[0m"
#define COL_ACCENT  "\x1b[38;5;39m"
#define COL_ACCENT2 "\x1b[38;5;45m"
#define COL_WHITE   "\x1b[97m"
#define COL_GRAY    "\x1b[38;5;244m"
#define COL_DIM     "\x1b[38;5;238m"
#define COL_GREEN   "\x1b[38;5;82m"
#define COL_YELLOW  "\x1b[38;5;220m"
#define COL_RED     "\x1b[38;5;196m"
#define COL_PURPLE  "\x1b[38;5;141m"
#define COL_BOLD    "\x1b[1m"
#define COL_CYAN    "\x1b[38;5;51m"

#define SCREEN_W 78

static void print_line(const char *ch) {
    printf(COL_DIM);
    for (int i = 0; i < SCREEN_W; i++) printf("%s", ch);
    printf(COL_RESET "\n");
}

static void print_header(void) {
    consoleClear();
    printf("\n");
    printf(COL_ACCENT COL_BOLD);
    printf("  ███████╗██╗    ██╗███████╗██╗███╗    ██╗███████╗ ██████╗ \n");
    printf("  ██╔════╝╚██╗ ██╔╝██╔════╝██║████╗  ██║██╔════╝██╔═══██╗\n");
    printf("  ███████╗ ╚████╔╝ ███████╗██║██╔██╗ ██║█████╗  ██║   ██║\n");
    printf("  ╚════██║  ╚██╔╝  ╚════██║██║██║╚██╗██║██╔══╝  ██║   ██║\n");
    printf("  ███████║   ██║   ███████║██║██║ ╚████║██║     ╚██████╔╝\n");
    printf("  ╚══════╝   ╚═╝   ╚══════╝╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝ \n");
    printf(COL_RESET);
    printf(COL_GRAY "               an amazing. system information viewer for your switch.\n" COL_RESET);
    printf("\n");
    print_line("=");
}

static void print_section(const char *title) {
    printf("\n");
    printf(COL_ACCENT2 COL_BOLD "  > %s\n" COL_RESET, title);
    print_line("-");
}

static void print_kv(const char *key, const char *value, const char *color) {
    printf(COL_GRAY "  %-28s" COL_RESET, key);
    printf("%s%s" COL_RESET "\n", color ? color : COL_WHITE, value);
}


static void fetch_firmware_info(void) {
    print_section("Firmware & System");

    SetSysFirmwareVersion fw = {0};
    if (R_SUCCEEDED(setsysGetFirmwareVersion(&fw))) {
        char fw_str[128];
        snprintf(fw_str, sizeof(fw_str), "%d.%d.%d (%s)",
            fw.major, fw.minor, fw.micro, fw.display_version);
        print_kv("Firmware Version", fw_str, COL_GREEN);
        print_kv("Display Title", fw.display_title, COL_GRAY);
        print_kv("Platform", fw.platform, COL_GRAY);
    } else {
        print_kv("Firmware Version", "Unavailable", COL_RED);
    }

    SetRegion region = 0;
    if (R_SUCCEEDED(setGetRegionCode(&region))) {
        const char *names[] = {"JPN","USA","EUR","AUS","HKG","KOR","CHN"};
        print_kv("Region", (region >= 0 && region < 7) ? names[region] : "Unknown", COL_WHITE);
    }

    u64 lang_code = 0;
    if (R_SUCCEEDED(setGetLanguageCode(&lang_code))) {
        char lang[16] = {0};
        memcpy(lang, &lang_code, 8);
        print_kv("System Language", lang, COL_WHITE);
    }

    SetSysProductModel model = SetSysProductModel_Invalid;
    if (R_SUCCEEDED(setsysGetProductModel(&model))) {
        const char *models[] = {"Invalid","Nx","Copper","Iowa","Hoag","Calcio","Aula"};
        int idx = (int)model;
        print_kv("Hardware Model", (idx >= 0 && idx <= 6) ? models[idx] : "Unknown", COL_YELLOW);
    }
}

static void fetch_ams_info(void) {
    print_section("Custom Firmware (Atmosphere)");

    u64 ams_ver = 0;
    if (R_SUCCEEDED(splGetConfig((SplConfigItem)65000, &ams_ver))) {
        u32 major = (ams_ver >> 56) & 0xFF;
        u32 minor = (ams_ver >> 48) & 0xFF;
        u32 micro = (ams_ver >> 40) & 0xFF;
        u32 rev   = (ams_ver >> 32) & 0xFF;
        char ver[64];
        snprintf(ver, sizeof(ver), "%u.%u.%u-%u", major, minor, micro, rev);
        print_kv("Atmosphere Version", ver, COL_GREEN);
    } else {
        print_kv("Atmosphere Version", "Not detected / stock", COL_RED);
    }

    u64 target_fw = 0;
    if (R_SUCCEEDED(splGetConfig((SplConfigItem)65007, &target_fw))) {
        char tf[32];
        snprintf(tf, sizeof(tf), "%llu", (unsigned long long)target_fw);
        print_kv("Target Firmware", tf, COL_WHITE);
    }

    u64 emummc_type = 0;
    if (R_SUCCEEDED(splGetConfig((SplConfigItem)65015, &emummc_type))) {
        const char *emu_str =
            emummc_type == 0 ? "Disabled (SysMMC)" :
            emummc_type == 1 ? "Partition-based" : "File-based";
        const char *emu_col =
            emummc_type == 0 ? COL_YELLOW : COL_GREEN;
        print_kv("EmuMMC", emu_str, emu_col);
    }

    u64 needs_reboot = 0;
    if (R_SUCCEEDED(splGetConfig((SplConfigItem)65001, &needs_reboot))) {
        print_kv("Needs Reboot", needs_reboot ? "Yes" : "No",
                 needs_reboot ? COL_YELLOW : COL_GREEN);
    }
}


static void fetch_hardware_info(void) {
    print_section("Hardware");

    SetSysSerialNumber serial = {0};
    if (R_SUCCEEDED(setsysGetSerialNumber(&serial))) {
        print_kv("Serial Number", serial.number, COL_WHITE);
    }

    u64 hw_type = 0;
    if (R_SUCCEEDED(splGetConfig(SplConfigItem_HardwareType, &hw_type))) {
        const char *hw_names[] = {"Icosa","Copper","Hoag","Iowa","Calcio","Aula"};
        print_kv("SoC Type", (hw_type < 6) ? hw_names[hw_type] : "Unknown", COL_YELLOW);
    }

    u64 dram_id = 0;
    if (R_SUCCEEDED(splGetConfig(SplConfigItem_DramId, &dram_id))) {
        char dram_str[32];
        snprintf(dram_str, sizeof(dram_str), "ID %llu", (unsigned long long)dram_id);
        print_kv("DRAM ID", dram_str, COL_WHITE);
    }

    AppletOperationMode opmode = appletGetOperationMode();
    print_kv("Operation Mode",
        opmode == AppletOperationMode_Handheld ? "Handheld" : "Docked",
        COL_CYAN);
}


static void fetch_battery_info(void) {
    print_section("Battery");

    u32 charge = 0;
    if (R_SUCCEEDED(psmGetBatteryChargePercentage(&charge))) {
        char pct[16];
        snprintf(pct, sizeof(pct), "%u%%", charge);
        const char *col = charge > 50 ? COL_GREEN : charge > 20 ? COL_YELLOW : COL_RED;
        print_kv("Charge", pct, col);
    }

    PsmChargerType charger = PsmChargerType_Unconnected;
    if (R_SUCCEEDED(psmGetChargerType(&charger))) {
        const char *charger_str =
            charger == PsmChargerType_Unconnected ? "Not charging" :
            charger == PsmChargerType_EnoughPower ? COL_GREEN "Full Power" :
            charger == PsmChargerType_LowPower    ? COL_YELLOW "Low Power (USB-PD)" :
            charger == PsmChargerType_NotSupported ? COL_RED "Not Supported" : "Unknown";
            
        printf(COL_GRAY "  %-28s" COL_RESET "%s\n", "Charger", charger_str);
    }
}


static void fetch_network_info(void) {
    print_section("Network");

    u32 ip = 0;
    if (R_SUCCEEDED(nifmGetCurrentIpAddress(&ip))) {
        char ip_str[32];
        snprintf(ip_str, sizeof(ip_str), "%u.%u.%u.%u",
            ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
        print_kv("Local IP", ip_str, COL_WHITE);
    } else {
        print_kv("Local IP", "Not connected", COL_RED);
    }
}


static void fetch_storage_info(void) {
    print_section("Storage");

    NcmStorageId ids[] = {
        NcmStorageId_BuiltInSystem,
        NcmStorageId_BuiltInUser,
        NcmStorageId_SdCard,
    };
    const char *names[] = { "NAND System", "NAND User", "SD Card" };

    for (int i = 0; i < 3; i++) {
        s64 free_sz = 0, total_sz = 0;
        bool got_free  = R_SUCCEEDED(nsGetFreeSpaceSize(ids[i], &free_sz));
        bool got_total = R_SUCCEEDED(nsGetTotalSpaceSize(ids[i], &total_sz));
        if (got_free && got_total) {
            double fg = (double)free_sz  / (1024.0*1024.0*1024.0);
            double tg = (double)total_sz / (1024.0*1024.0*1024.0);
            char sz[64];
            snprintf(sz, sizeof(sz), "%.2f GB free / %.2f GB total", fg, tg);
            const char *col = (fg < 1.0) ? COL_RED : (fg < 5.0) ? COL_YELLOW : COL_GREEN;
            print_kv(names[i], sz, col);
        } else {
            print_kv(names[i], "Unavailable", COL_GRAY);
        }
    }
}


static void fetch_perf_info(void) {
    print_section("Performance & Clocks");

    u32 cpu_hz = 0, gpu_hz = 0, emc_hz = 0;

    if (R_SUCCEEDED(pcvGetClockRate(PcvModule_CpuBus, &cpu_hz))) {
        char s[32]; snprintf(s, sizeof(s), "%.0f MHz", cpu_hz / 1000000.0f);
        print_kv("CPU Clock", s, COL_ACCENT);
    }
    if (R_SUCCEEDED(pcvGetClockRate(PcvModule_GPU, &gpu_hz))) {
        char s[32]; snprintf(s, sizeof(s), "%.0f MHz", gpu_hz / 1000000.0f);
        print_kv("GPU Clock", s, COL_PURPLE);
    }
    if (R_SUCCEEDED(pcvGetClockRate(PcvModule_EMC, &emc_hz))) {
        char s[32]; snprintf(s, sizeof(s), "%.0f MHz", emc_hz / 1000000.0f);
        print_kv("Memory Clock", s, COL_WHITE);
    }

    s32 cpu_t = 0, gpu_t = 0;
    if (R_SUCCEEDED(tsGetTemperature(TsLocation_Internal, &cpu_t))) {
        float t = cpu_t / 1000.0f;
        char s[16]; snprintf(s, sizeof(s), "%.1fC", t);
        print_kv("CPU Temp", s, t > 70 ? COL_RED : t > 55 ? COL_YELLOW : COL_GREEN);
    }
    if (R_SUCCEEDED(tsGetTemperature(TsLocation_External, &gpu_t))) {
        float t = gpu_t / 1000.0f;
        char s[16]; snprintf(s, sizeof(s), "%.1fC", t);
        print_kv("GPU Temp", s, t > 70 ? COL_RED : t > 55 ? COL_YELLOW : COL_GREEN);
    }
}


static void fetch_identity_info(void) {
    print_section("Device Identity");

    SetSysDeviceNickName nickname = {0};
    if (R_SUCCEEDED(setsysGetDeviceNickname(&nickname))) {
        print_kv("Device Nickname", nickname.nickname, COL_ACCENT);
    }

    AccountUid uids[8] = {0};
    s32 account_count = 0;
    if (R_SUCCEEDED(accountListAllUsers(uids, 8, &account_count))) {
        char acc[8]; snprintf(acc, sizeof(acc), "%d", account_count);
        print_kv("User Accounts", acc, COL_WHITE);
    }

    bool wlan = false;
    if (R_SUCCEEDED(setsysGetWirelessLanEnableFlag(&wlan)))
        print_kv("WiFi Enabled", wlan ? "Yes" : "No", wlan ? COL_GREEN : COL_RED);

    bool bt = false;
    if (R_SUCCEEDED(setsysGetBluetoothEnableFlag(&bt)))
        print_kv("Bluetooth Enabled", bt ? "Yes" : "No", bt ? COL_GREEN : COL_RED);

    bool nfc = false;
    if (R_SUCCEEDED(setsysGetNfcEnableFlag(&nfc)))
        print_kv("NFC Enabled", nfc ? "Yes" : "No", nfc ? COL_GREEN : COL_RED);

    ColorSetId color_set = ColorSetId_Light;
    if (R_SUCCEEDED(setsysGetColorSetId(&color_set)))
        print_kv("System Theme", color_set == ColorSetId_Dark ? "Dark" : "Light", COL_WHITE);
}

static void print_footer(void) {
    printf("\n");
    print_line("=");
    printf(COL_DIM "  SysInfo  |  (+) Exit  |  (A) Refresh" COL_RESET "\n");
    print_line("=");
}

static void services_init(void) {
    setsysInitialize();
    setInitialize();
    splInitialize();
    appletInitialize();
    psmInitialize();
    nifmInitialize(NifmServiceType_User);
    nsInitialize();
    pcvInitialize();
    tsInitialize();
    accountInitialize(AccountServiceType_Application);
}

static void services_exit(void) {
    accountExit();
    tsExit();
    pcvExit();
    nsExit();
    nifmExit();
    psmExit();
    appletExit();
    splExit();
    setExit();
    setsysExit();
}

int main(int argc, char **argv) {
    consoleInit(NULL);
    services_init();

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    bool dirty = true;

    while (appletMainLoop()) {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus) break;
        if (kDown & HidNpadButton_A) dirty = true;

        if (dirty) {
            dirty = false;
            print_header();
            fetch_identity_info();
            fetch_firmware_info();
            fetch_ams_info();
            fetch_hardware_info();
            fetch_battery_info();
            fetch_network_info();
            fetch_storage_info();
            fetch_perf_info();
            print_footer();
            consoleUpdate(NULL);
        }
    }

    services_exit();
    consoleExit(NULL);
    return 0;
}
