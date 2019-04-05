#include "gcodeinterpreter.h"

namespace dx200 {

void GCodeInterpreter::setClient(HSEClient* client)
{
    m_client = client;

    connect(m_client, &HSEClient::getStatusInformationRead,
            this, &GCodeInterpreter::handleStatusInformationRead);
}

void GCodeInterpreter::setUserFrame(const double& x, const double& y, const double& z)
{
    m_userBase[0] = x;
    m_userBase[1] = y;
    m_userBase[2] = z;
}

void GCodeInterpreter::execute(gcode::Block block)
{
    using namespace gcode;

    double commandData = block.getCommandData();

    m_state.update(block);

    switch (block.getCommandType()) {
    // Gnnn
    case gcode::Standard: {
        // G0: Rapid move
        if (commandData == 0) {
            sendMove(m_state.X, m_state.Y, m_state.Y, m_state.F);
        }
        // G1: Linear move
        else if (commandData == 1) {
            sendMove(m_state.X, m_state.Y, m_state.Y, m_state.F);
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

void GCodeInterpreter::handleStatusInformationRead(StatusInformation info)
{
}

void GCodeInterpreter::sendMove(const double x, const double y, const double z, const double speed)
{
    // Apply the user base transform
    // mm
    double X = x - m_userBase[0];
    double Y = y - m_userBase[1];
    double Z = z - m_userBase[2];

    // degrees
    double Tx = 180;
    double Ty = 0;
    double Tz = 0;

    Movement::Cartesian mvtData;
    mvtData.robotNo        = 1;
    mvtData.stationNo      = 0; // TODO: Should be 1
    mvtData.classification = Movement::SpeedClassification::CARTESIAN_TRANSLATION;
    mvtData.speed          = speed / 6.; // mm/min to 0.1 mm/s conversion
    mvtData.coordinate     = 17;         // Robot coordinates

    mvtData.x  = X * 1000; // mm to µm conversion
    mvtData.y  = Y * 1000;
    mvtData.z  = Z * 1000;
    mvtData.tx = Tx * 10000; // 0.0001 degree to degree
    mvtData.ty = Ty * 10000;
    mvtData.tz = Tz * 10000;
    // The rest of the members are zero-initialized

    // Send command to robot
    m_client->moveCartesian(Movement::ABSOLUTE_CARTESIAN, mvtData);
}

void GCodeInterpreter::ClientState::update(gcode::Block block)
{
    if (block.hasParameter(gcode::X)) {
        X = block.getParameter(gcode::X);
    }
    if (block.hasParameter(gcode::Y)) {
        Y = block.getParameter(gcode::Y);
    }
    if (block.hasParameter(gcode::Z)) {
        Z = block.getParameter(gcode::Z);
    }
    if (block.hasParameter(gcode::ExtrudeLength)) {
        E = block.getParameter(gcode::ExtrudeLength);
    }
    if (block.hasParameter(gcode::Feedrate)) {
        F = block.getParameter(gcode::Feedrate);
    }
    if (block.hasParameter(gcode::Parameter)) {
        P = block.getParameter(gcode::Parameter);
    }
}

} // namespace dx200
