#ifndef HSESTRUCTURES_H
#define HSESTRUCTURES_H

#include <array>
#include <stdint.h>
#include <string>

#include <QByteArray>
#include <QString>

namespace HSE {

template <uint8_t byte, class T>
uint8_t getByte(T value)
{
    return (value & (0xFF << (8 * byte))) >> (8 * byte);
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
    HOLDOR_OR_SERVO_ONOFF            = 0x83,
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
    uint32_t Code    = 0;
    uint32_t Data    = 0;
    std::string Name = {};
    std::string Time = {};
    uint32_t Type    = 0;
};

//TODO: Verifier l'initialisation des std::array
struct MoveDataCartesian {
    std::array<int32_t, 3> BaseAxisPosition    = {};
    int32_t Classification                     = 0;
    int32_t Coordinate                         = 0;
    uint32_t ExpandedType                      = 0;
    int32_t RobotNo                            = 0;
    int32_t Speed                              = 0;
    std::array<int32_t, 6> StationAxisPosition = {};
    int32_t StationNo                          = 0;
    int32_t ToolNo                             = 0;
    int32_t Tx                                 = 0;
    int32_t Ty                                 = 0;
    uint32_t Type                              = 0;
    int32_t Tz                                 = 0;
    int32_t UserCoordinateNo                   = 0;
    int32_t X                                  = 0;
    int32_t Y                                  = 0;
    int32_t Z                                  = 0;
};

struct MoveDataPulse {
    std::array<int32_t, 3> BaseAxisPosition    = {};
    int32_t Classification                     = 0;
    std::array<int32_t, 8> RobotAxisPulseValue = {};
    int32_t RobotNo                            = 0;
    int32_t Speed                              = 0;
    std::array<int32_t, 6> StationAxisPosition = {};
    int32_t StationNo                          = 0;
    int32_t ToolNo                             = 0;
};

struct RequestStatus {
    uint8_t AddedStatusSize = 0;
    uint8_t Status          = 0;
    uint16_t Status1        = 0;
    uint16_t Status2        = 0;
};

/**
 * @brief Station status information
 */
struct StatusInformation {
    bool NetworkError = false;
    uint8_t Data1     = 0;
    uint8_t Data2     = 0;

    /**
     * @brief Step
     * @return
     */
    bool Step()
    {
        return (Data1 & 0b00000001);
    }

    bool Cycle1()
    {
        return (Data1 & 0b00000010);
    }

    bool AutomaticAndContinuous()
    {
        return (Data1 & 0b00000100);
    }

    bool Running()
    {
        return (Data1 & 0b00001000);
    }

    bool InGuardSafeOperation()
    {
        return (Data1 & 0b00010000);
    }

    bool Teach()
    {
        return (Data1 & 0b00100000);
    }

    bool Play()
    {
        return (Data1 & 0b01000000);
    }

    bool CommandRemote()
    {
        return (Data1 & 0b10000000);
    }

    /**
     * @brief In hold status (by programming pendant)
     * @return
     */
    bool InHoldByPP()
    {
        return (Data2 & 0b00000010);
    }

    /**
     * @brief In hold status (externally)
     * @return
     */
    bool InHoldByExt()
    {
        return (Data2 & 0b00000100);
    }

    /**
     * @brief In hold status (by command)
     * @return
     */
    bool InHoldByCmd()
    {
        return (Data2 & 0b00001000);
    }

    bool Alarming()
    {
        return (Data2 & 0b00010000);
    }

    bool ErrorOcurring()
    {
        return (Data2 & 0b00100000);
    }

    bool Servo()
    {
        return (Data2 & 0b01000000);
    }
};

} // namespace HSE

#endif // HSESTRUCTURES_H
