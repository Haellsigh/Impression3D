#include "gcodeinterpreter.h"

#include <cmath>

#include "log.h"

namespace dx200 {

void GCodeInterpreter::setClient(HSEClient* client)
{
    m_client = client;
    m_lastCommandTimer.setSingleShot(true);
    m_lastCommandTimer.setTimerType(Qt::PreciseTimer);

    // The client periodically emits readStatusInformation
    connect(m_client, &HSEClient::readStatusInformation,
            this, &GCodeInterpreter::handleStatusInformationRead);

    connect(&m_lastCommandTimer, &QTimer::timeout, this, &GCodeInterpreter::executeNext);
}

void GCodeInterpreter::setUserFrame(const double& x, const double& y, const double& z)
{
    m_userBase[0] = x;
    m_userBase[1] = y;
    m_userBase[2] = z;
}

int GCodeInterpreter::executeFile(QString filepath)
{
    m_blocks.clear();
    auto blocks = m_reader.decodeFile(filepath);

    // Add the blocks in order of execution
    foreach (auto block, blocks) {
        m_blocks.append(block);
    }

    // Execute the first block
    m_lastCommandTimer.start(0);

    return m_blocks.count();
}

void GCodeInterpreter::execute(gcode::Block block)
{
    using namespace gcode;

    double commandData = block.getCommandData();
    double time        = 0;

    // Update the internal state from the block's data (x, y, z, speed)
    m_state.update(block);

    switch (block.getCommandType()) {
    // Gnnn
    case gcode::Standard: {
        qInfo() << tr("GCode Interpreter:") << QString(" G%1").arg(commandData);

        // G0: Rapid move
        if (commandData == 0) {
            time = sendMove(m_state.X, m_state.Y, m_state.Z, m_state.F);
        }
        // G1: Linear move
        else if (commandData == 1) {
            // Special case if there are no X, Y or Z coordinates
            // But there is a Feedrate and Extrude length
            if (!block.hasParameter(gcode::X)
                && !block.hasParameter(gcode::Y)
                && !block.hasParameter(gcode::Z)
                && block.hasParameter(gcode::Feedrate)
                && block.hasParameter(gcode::ExtrudeLength)) {
                // Compute time from speed and extrude length
                time = 60. * abs(m_state.E) / m_state.F;
            }
            // Normal case
            else {
                time = sendMove(m_state.X, m_state.Y, m_state.Z, m_state.F);
            }
        }

        // G28: Move to origin
        else if (commandData == 28) {
            // Move to the axis' 0 or to the value
            double x = 0, y = 0, z = 0, speed = 10000;
            if (block.hasParameter(gcode::X))
                x = m_state.X;
            if (block.hasParameter(gcode::Y))
                y = m_state.Y;
            if (block.hasParameter(gcode::Z))
                z = m_state.Z;
            if (block.hasParameter(gcode::Feedrate))
                speed = m_state.F;

            time = sendMove(x, y, z, speed);
        }
        // G92: Set current position to zero
        else if (commandData == 92) {

        }
        // Handle unknown command
        else {
            qWarning() << tr("GCode Interpreter: unknown command ") << "G" << commandData;
        }
    } break;

    // Mnnn
    case gcode::RepRap:
        qInfo() << tr("GCode Interpreter:") << QString(" M%1").arg(commandData);

        // M18 or M84: Disable steppers
        if (commandData == 18 || commandData == 84) {
            m_client->servo(false);
            time = 10. / 1000.; // 10ms
        }
        // M104: Set hotend temperature
        else if (commandData == 104) {
            // Expects at least a Snnn field
        }
        // M106: Set fan speed
        else if (commandData == 106) {
            // Not implemented
        }
        // M107: Fan off
        else if (commandData == 107) {
            // Not implemented
        }
        // M109: Wait for hotend temperature
        else if (commandData == 109) {
            // To be implemented
        }
        // M140: Set bed temperature
        else if (commandData == 140) {
            // Not implemented
        }
        // M190: Wait for bed temperature
        else if (commandData == 190) {
            // Not implemented
        }
        // Handle unknown command
        else {
            qWarning() << tr("GCode Interpreter:") << tr(" unknown command ") << "M" << commandData;
        }
        break;
    }

    // Time to wait before executing the next command
    m_lastCommandTimer.start(time * 1000);
    // Compute extrusion speed from extrusion length
    if (block.hasParameter(gcode::ExtrudeLength) && time != 0) {
        // speed = length / time (in mm/min)
        sendExtrusionSpeed(m_state.E / time / 60.);
    } else {
        sendExtrusionSpeed(0);
    }
}

void GCodeInterpreter::executeNext()
{
    if (!m_blocks.isEmpty()) {
        execute(m_blocks.dequeue());
        emit finishedLine();
    } else {
        emit finished();
    }
}

void GCodeInterpreter::stopExecution()
{
    m_lastCommandTimer.stop();
}

void GCodeInterpreter::handleStatusInformationRead(StatusInformation info)
{
    if (!info.Running()) {
        handleRobotNotRunning();
    }
}

void GCodeInterpreter::handleRobotNotRunning()
{
    // If the last command theoretically finished running
    if (!m_lastCommandTimer.isActive()) {
        executeNext();
    }
}

double GCodeInterpreter::sendMove(const double x, const double y, const double z, const double speed)
{
    // Apply the user base transform
    // Position in mm
    double X = x - m_userBase[0];
    double Y = y - m_userBase[1];
    double Z = z - m_userBase[2];

    // Rotation in degrees
    double Tx = -180;
    double Ty = 0;
    double Tz = 0;

    Movement::Cartesian mvtData;
    mvtData.robotNo        = 1;
    mvtData.stationNo      = 0; // TODO: Should be 1
    mvtData.classification = Movement::SpeedClassification::CARTESIAN_TRANSLATION;
    mvtData.speed          = speed / 6.; // mm/min to 0.1 mm/s conversion
    mvtData.coordinate     = 17;         // 17 = Robot coordinates

    mvtData.x  = X * 1000; // mm to µm conversion
    mvtData.y  = Y * 1000;
    mvtData.z  = Z * 1000;
    mvtData.tx = Tx * 10000; // 0.0001 degree to degree
    mvtData.ty = Ty * 10000;
    mvtData.tz = Tz * 10000;
    // The rest of the members are zero-initialized

    // Send command to robot
    m_client->moveCartesian(Movement::ABSOLUTE_CARTESIAN, mvtData);

    // Length of this action in time: time = DeltaPos/speed
    return 60. * sqrt(m_state.dX * m_state.dX + m_state.dY * m_state.dY + m_state.dZ * m_state.dZ)
           / speed;
}

void GCodeInterpreter::sendExtrusionSpeed(const double speed)
{
    // TODO: Send this value to a IO port of the dx200 and process it with the
    // arduino
}

void GCodeInterpreter::ClientState::update(gcode::Block block)
{
    if (block.hasParameter(gcode::X)) {
        double newX = block.getParameter(gcode::X);
        dX          = newX - X;
        X           = newX;
    } else {
        dX = 0;
    }
    if (block.hasParameter(gcode::Y)) {
        double newY = block.getParameter(gcode::Y);
        dY          = newY - Y;
        Y           = newY;
    } else {
        dY = 0;
    }
    if (block.hasParameter(gcode::Z)) {
        double newZ = block.getParameter(gcode::Z);
        dZ          = newZ - Z;
        Z           = newZ;
    } else {
        dZ = 0;
    }
    if (block.hasParameter(gcode::ExtrudeLength)) {
        E = block.getParameter(gcode::ExtrudeLength);
    }
    if (block.hasParameter(gcode::Feedrate)) {
        F = block.getParameter(gcode::Feedrate);
    }
}

} // namespace dx200
