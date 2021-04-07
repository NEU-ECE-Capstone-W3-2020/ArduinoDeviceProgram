#ifndef CAPSTONE_BLE
#define CAPSTONE_BLE

#include <SoftwareSerial.h> 
#include <bluefruit.h>

namespace Capstone_BLE {
    // See https://docs.google.com/spreadsheets/d/1hY7SFB_dBNCCz2EGmpBoGptssS9gRMkLEKAGmXN6TGU/edit#gid=0
    enum Encoded_Strings {
        DrMasseyOffice   = 0,
        Room203          = 1,
        ElevatorFloor2   = 2,
        CafeStregaClosed = 3,
        CafeStregaOpen   = 4,
        Calc2            = 5,
        IsecExit         = 6,
        IsecWelcome      = 7
    };

    void setupBLE(SoftwareSerial& bleSerial, BLEUart& bleuart);
    String getLastString();
    void startAdv(void);
    void prph_connect_callback(uint16_t conn_handle);
    void prph_disconnect_callback(uint16_t conn_handle, uint8_t reason);
    void prph_bleuart_rx_callback(uint16_t conn_handle);
    void scan_callback(ble_gap_evt_adv_report_t* report);
}

#endif
