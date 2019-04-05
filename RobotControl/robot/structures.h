#ifndef HSESTRUCTURES_H
#define HSESTRUCTURES_H

#include <array>
#include <stdint.h>
#include <string>

#include <QByteArray>
#include <QString>

namespace dx200 {

template <uint8_t byte, class T>
const uint8_t getByte(const T& value)
{
    uint16_t bits = 8 * byte;
    return (value & (0xFF << bits)) >> bits;
}

uint16_t toUInt16(const QByteArray& data, const int index);

/**
 * @brief Liste des commandes
 * @note Signification des suffixes _R, _W, _RW:
 * R : Reading
 * W : Writing
 * RW : Reading/Writing
 */
enum Command : uint16_t {
    ALARM_DATA_R                     = 0x70,
    ALARM_HISTORY_R                  = 0x71,
    STATUS_INFORMATION_R             = 0x72,
    JOB_INFORMATION_R                = 0x73,
    AXIS_CONFIGURATION_INFORMATION_R = 0x74,
    PLURAL_B_VARIABLE_RW             = 0x302,
    BYTE_VARIABLE                    = 0x7A,
    ALARM_RESET                      = 0x82,
    HOLD_OR_SERVO_ONOFF              = 0x83,
    MOVE_CARTESIAN                   = 0x8A,
    MOVE_PULSE                       = 0x8B
};

enum Attribute : uint8_t {
    DATA1 = 1,
    DATA2 = 2
};

enum Service : uint8_t {
    GET_SINGLE_ATTRIBUTE = 0x0E,
    GET_ALL_ATTRIBUTES   = 0x01
};

struct AlarmData {
    uint32_t code    = 0;
    uint32_t data    = 0;
    std::string name = {};
    std::string time = {};
    uint32_t type    = 0;
};

namespace Movement {

    enum Type : uint8_t {
        ABSOLUTE_JOINT        = 1,
        ABSOLUTE_CARTESIAN    = 2,
        INCREMENTAL_CARTESIAN = 3
    };

    //TODO: Verifier l'initialisation des std::array
    struct Cartesian {
        enum Classification {

        };

        std::array<int32_t, 3> baseAxisPosition    = {};
        int32_t classification                     = 0;
        int32_t coordinate                         = 0;
        uint32_t expandedType                      = 0;
        int32_t robotNo                            = 0;
        int32_t speed                              = 0;
        std::array<int32_t, 6> stationAxisPosition = {};
        int32_t stationNo                          = 0;
        int32_t toolNo                             = 0;
        int32_t tx                                 = 0;
        int32_t ty                                 = 0;
        int32_t tz                                 = 0;
        uint32_t type                              = 0;
        int32_t userCoordinateNo                   = 0;
        int32_t x                                  = 0;
        int32_t y                                  = 0;
        int32_t z                                  = 0;
    };

    struct Pulse {
        std::array<int32_t, 3> baseAxisPosition    = {};
        int32_t classification                     = 0;
        std::array<int32_t, 8> robotAxisPulseValue = {};
        int32_t robotNo                            = 0;
        int32_t speed                              = 0;
        std::array<int32_t, 6> stationAxisPosition = {};
        int32_t stationNo                          = 0;
        int32_t toolNo                             = 0;
    };

} // namespace Movement

struct RequestStatus {
    uint8_t addedStatusSize = 0;
    uint8_t status          = 0;
    uint16_t status1        = 0;
    uint16_t status2        = 0;
};

/**
 * @brief Station status information
 */
struct StatusInformation {
    bool networkError = false;
    uint8_t data1     = 0;
    uint8_t data2     = 0;

    /**
     * @brief Step
     * @return
     */
    bool Step()
    {
        return (data1 & 0b00000001);
    }

    bool Cycle1()
    {
        return (data1 & 0b00000010);
    }

    bool AutomaticAndContinuous()
    {
        return (data1 & 0b00000100);
    }

    bool Running()
    {
        return (data1 & 0b00001000);
    }

    bool InGuardSafeOperation()
    {
        return (data1 & 0b00010000);
    }

    bool Teach()
    {
        return (data1 & 0b00100000);
    }

    bool Play()
    {
        return (data1 & 0b01000000);
    }

    bool CommandRemote()
    {
        return (data1 & 0b10000000);
    }

    /**
     * @brief In hold status (by programming pendant)
     * @return
     */
    bool InHoldByPP()
    {
        return (data2 & 0b00000010);
    }

    /**
     * @brief In hold status (externally)
     * @return
     */
    bool InHoldByExt()
    {
        return (data2 & 0b00000100);
    }

    /**
     * @brief In hold status (by command)
     * @return
     */
    bool InHoldByCmd()
    {
        return (data2 & 0b00001000);
    }

    bool Alarming()
    {
        return (data2 & 0b00010000);
    }

    bool ErrorOcurring()
    {
        return (data2 & 0b00100000);
    }

    bool Servo()
    {
        return (data2 & 0b01000000);
    }
};

} // namespace HSE

#endif // HSESTRUCTURES_H
