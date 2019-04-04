#ifndef GCODEINTERFACE_H
#define GCODEINTERFACE_H

#include "gcode/gcodestructures.h"
#include "hseclient.h"

namespace dx200 {

/**
 * @brief The GCode Interface interprets the GCode blocks
 * It's the interface between a HSEClient and a gcode::Reader
 *
 * @details Two things need to be done before you can execute blocks:
 *     1. Set the client using setClient()
 *     2. Set the user transformation using setUserFrame()
 */
class GCodeInterpreter {
    using doubleLimits = std::numeric_limits<double>;

public:
    // 1. set client
    void setClient(HSEClient* client);

    // 2. set frame
    void setUserFrame();

    // 2. execute block
    void execute(gcode::Block block);

private:
    HSEClient* m_client;

    // Keep an internal state on some parameters
    struct ClientState {
        double X = 0, Y = 0, Z = 0, E = 0, F = 0;
    };

    ClientState m_state;

private:
    void sendMove(double x, double y, double z);
};

} // namespace dx200

#endif // GCODEINTERFACE_H
