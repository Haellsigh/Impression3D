#ifndef GCODEINTERFACE_H
#define GCODEINTERFACE_H

#include <QObject>
#include <QQueue>

#include "gcode/gcodereader.h"
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
class GCodeInterpreter : public QObject {
    Q_OBJECT

    using doubleLimits = std::numeric_limits<double>;

public:
    // 1. set client
    void setClient(HSEClient* client);

    // 2. set frame
    void setUserFrame(const double& x, const double& y, const double& z);

    // 3. execute gcode
    int executeFile(QString filepath);

    // execute a block
    void execute(gcode::Block block);
    void executeNext();

signals:
    void finished();
    void finishedLine();

private slots:
    void handleStatusInformationRead(StatusInformation info);
    void handleRobotNotRunning();

private:
    HSEClient* m_client;
    gcode::Reader m_reader;

    QQueue<gcode::Block> m_blocks;

    // Keep an internal state on some parameters
    struct ClientState {
        void update(gcode::Block block);

        // Positions in mm
        double X = 0, Y = 0, Z = 0;
        // Length to extrude during the next move command
        double E = 0;
        // Speed in mm/min
        double F = 0;

        double dX = 0, dY = 0, dZ = 0;

        bool running = false;
    };

    ClientState m_state;

private:
    /**
     * @brief Send move command to robot
     * @param x position in mm
     * @param y position in mm
     * @param z position in mm
     * @param speed in mm/min
     *
     * @return The minimum time to wait for the command to finish in s
     */
    double sendMove(const double x, const double y, const double z, const double speed);

    /**
     * @brief Send extrude speed command to robot
     * @param speed in mm/min
     */
    void sendExtrusionSpeed(const double speed);

    std::array<double, 3> m_userBase;

    QTimer m_lastCommandTimer;

    ///////////////////////////////////////////////////////////////////////////
    /// Robot signals
    void robotNotRunning();
};

} // namespace dx200

#endif // GCODEINTERFACE_H
