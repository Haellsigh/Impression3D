#include "hsestructures.h"

namespace HSE {

uint16_t toUInt16(const QByteArray& data, const int index)
{
    //TODO: Tester cette méthode!
    return ((uint8_t)data.at(index + 1) << 8) + (uint8_t)data.at(index);
}

}
