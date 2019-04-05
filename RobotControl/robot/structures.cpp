#include "structures.h"

namespace dx200 {

uint16_t toUInt16(const QByteArray& data, const int index)
{
    //TODO: Tester cette méthode!
    return ((uint8_t)data.at(index + 1) << 8)
           + ((uint8_t)data.at(index + 0) << 0);
}

uint16_t toUInt32(const QByteArray& data, const int index)
{
    return ((uint8_t)data.at(index + 3) << 24)
           + ((uint8_t)data.at(index + 2) << 16)
           + ((uint8_t)data.at(index + 1) << 8)
           + ((uint8_t)data.at(index + 0) << 0);
}

}
