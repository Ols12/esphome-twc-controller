// Copyright retained from original
#ifndef TWC_PROTOCOL_H
#define TWC_PROTOCOL_H

#include <cstdint>
#include <cstring>
#include <vector>
#include <memory>

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "twc_connector.h"
#include "twc_controller.h"

namespace esphome {
namespace twc_controller {

#define MAX_PACKET_LENGTH 512

class TeslaController {
    public:
        TeslaController(uart::UARTComponent* serial, TeslaControllerIO *io, uint16_t twcid, GPIOPin *flow_control_pin, int passive_mode);

        void Begin();
        void GetPowerStatus(uint16_t secondary_twcid);
        void GetFirmware(uint16_t secondary_twcid);
        void GetSerial(uint16_t secondary_twcid);
        void GetFirmwareVer(uint16_t secondary_twcid);
        void GetVin(uint16_t secondary_twcid);
        void Handle();
        void SendCommand(uint16_t command, uint16_t send_to);
        void SendPresence(bool presence2 = false);
        void SendPresence2();
        void SendHeartbeat(uint16_t secondary_twcid);
        void SendIdle();
        void Startup();
        void SendData(uint8_t *packet, size_t length);
        void DecodePowerState(EXTENDED_RESP_PACKET_T *power_state);
        void DecodePrimaryPresence(RESP_PACKET_T *presence, uint8_t num);
        void DecodePrimaryHeartbeat();

        // convenience
        size_t ChargersConnected();
        TeslaConnector * GetConnector(uint16_t twcid);
        bool IsCharging();

    private:
        uint8_t CalculateChecksum(uint8_t *buffer, size_t length);
        bool VerifyChecksum(uint8_t *buffer, size_t length);
        void DecodeLinkReady();
        void DecodePrimaryHeartbeat(P_HEARTBEAT_T *heartbeat);
        void DecodeSecondaryPresence(RESP_PACKET_T *presence);
        void DecodeSecondaryHeartbeat(S_HEARTBEAT_T *heartbeat);
        void DecodeVin(EXTENDED_RESP_PACKET_T *vin);
        void DecodeExtFirmwareVerison(RESP_PACKET_T *firmware_ver);
        void DecodeSerialNumber(EXTENDED_RESP_PACKET_T *serial);
        void SetCurrent(uint8_t current);
        void SetMaxCurrent(uint8_t maxCurrent);
        void SetMinCurrent(uint8_t mincurrent);
        void SetStopStartDelay(uint16_t stopstart_delay);
        uint8_t ChargersConnected_Internal();
        void UpdateTotalActualCurrent();
        void UpdateTotalPhaseCurrent(uint8_t phase);
        void UpdateTotalConnectedCars();
        void StopCharging(uint16_t twcid);
        void StartCharging(uint16_t twcid);
        static void startupTask_(void *pvParameter);
        void Debug(bool enabled);
        void SendDataFromString(const uint8_t* dataString, size_t length);

    private:
        uart::UARTComponent *serial_;
        GPIOPin *flow_control_pin_{nullptr};
        TeslaControllerIO *controller_io_;
        uint8_t num_connected_chargers_;
        uint16_t twcid_;
        uint8_t sign_;
        uint8_t receive_buffer_[MAX_PACKET_LENGTH];
        uint8_t receive_index_;
        bool message_started_ = false;
        uint8_t available_current_;

        // controller state
        uint8_t max_current_;
        uint8_t min_current_;
        uint16_t stopstart_delay_;
        bool current_changed_;
        bool debug_;
        uint8_t passive_mode_;
        uint8_t total_current_;

        // OWNERSHIP: dynamic container of discovered chargers
        std::vector<std::unique_ptr<TeslaConnector>> chargers_;
};

} // namespace twc_controller
} // namespace esphome

#endif // TWC_PROTOCOL_H
