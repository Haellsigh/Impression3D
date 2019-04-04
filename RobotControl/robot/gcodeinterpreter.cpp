#include "gcodeinterpreter.h"

namespace dx200 {

void GCodeInterpreter::setClient(HSEClient* client)
{
    m_client = client;
}

void GCodeInterpreter::setUserFrame()
{
}

void GCodeInterpreter::execute(gcode::Block block)
{
    using namespace gcode;

    double commandData = block.getCommandData();

    switch (block.getCommandType()) {
    // Gnnn
    case gcode::Standard: {
        if (block.hasParameter(X)) {
            m_state.X = block.getParameter(X);
        }

        // G0: Rapid move
        if (commandData == 0) {
        }
        // G1: Linear move
        else if (commandData == 1) {

        }

        // G28: Move to origin
        else if (commandData == 28) {

        }
        // G92: Linear move
        else if (commandData == 1) {

        }
        // Handle unknown command
        else {
            qWarning().noquote().nospace() << "GCode Interpreter: unknown command G" << commandData;
        }
    } break;

    // Mnnn
    case gcode::RepRap:
        // M18 or M84: Disable steppers
        if (commandData == 18 || commandData == 84) {

        }
        // M104: Set hotend temperature
        else if (commandData == 104) {
            // Expects at least a Snnn

        }
        // M106: Set fan speed
        else if (commandData == 106) {

        }
        // M107: Fan off
        else if (commandData == 107) {

        }
        // M109: Wait for hotend temperature
        else if (commandData == 109) {

        }
        // M140: Set bed temperature
        else if (commandData == 140) {

        }
        // M190: Wait for bed temperature
        else if (commandData == 190) {
        }
        // Handle unknown command
        else {
            qWarning().noquote().nospace() << "GCode Interpreter: unknown command M" << commandData;
        }
        break;
    }
}

void GCodeInterpreter::sendMove(double x, double y, double z)
{
    // TODO: Execute the user transform before sending those coordinates
}

} // namespace dx200
