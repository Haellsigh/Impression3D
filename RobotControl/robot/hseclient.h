#ifndef HSECLIENT_H
#define HSECLIENT_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>

#include "structures.h"

namespace dx200 {

/**
 * @brief High Speed Ethernet Client class, interface to the DX200 controller
 * @warning Everything is asynchronous, meaning if you request some data it
 * will arrive in the form of a signal later. Connect to the signal.
 */
class HSEClient : public QObject {
    Q_OBJECT

public:
    HSEClient();

    void sendCommand(Command command_id, int16_t instance, uint8_t attribut, uint8_t service, QByteArray data = QByteArray());

    ///////////////////////////////////////////////////////////////////////////
    // List of possible requests
public slots:
    /**
     * @brief Reads the status information data
     */
    void statusInformationRead();

    /**
     * @brief Reset Alarms on the robot
     */
    void resetAlarms();

    /**
     * @brief Cancel (reset) error on the robot
     */
    void cancelErrors();

    /**
     * @brief Activate or deactivate HLock on the robot
     * @param lock Set to false to deactivate HLock on the robot
     */
    void hlock(bool lock = true);

    /**
     * @brief Hold or unhold the Servo
     * @param hold Set to false to unhold the Servo
     */
    void hold(bool hold = true);

    /**
     * @brief Send a move instruction to the robot in cartesian coordinates
     * @param type The type of movement
     * @param movement Movement data
     */
    void moveCartesian(Movement::Type type, Movement::Cartesian movement);

    /**
     * @brief Send a move instruction to the robot in encoder pulses.
     * @param type The type of movement
     * @param movement Movement data
     */
    void movePulse(Movement::Type type, Movement::Pulse movement);

    /**
     * @brief Read the names of all axis in the selected ControlGroup
     * @param controlGroup
     * @param axisNames
     */
    //void readAxisConfigurationInformation(uint8_t controlGroup);

    ///////////////////////////////////////////////////////////////////////////
    // List of answers to requests
signals:
    void getStatusInformationRead(StatusInformation info);

    ///////////////////////////////////////////////////////////////////////////
signals:
    void requestSent();
    void requestStatus(RequestStatus status);
    void requestData(QByteArray data);

private slots:
    void readPendingDatagrams();
    /**
     * @brief processReceivedData
     * @note Nous avons besoin de request_id car c'est la seule chose qui
     * différencie une réponse d'une autre.
     * @param request_id L'identifiant de la requète
     * @param data Les données à traiter
     */
    void processReceivedData(const uint8_t request_id, const QByteArray data = QByteArray());

private:
    QUdpSocket m_socket;
    QHostAddress m_ip = {};
    quint16 m_port    = 0;

    /**
     * @brief Correspondance entre un identifiant (request_id) et une commande.
     *
     * Cela permettra de savoir lorsqu'on recoit une réponse ou elle doit aller.
     */
    QHash<uint8_t, Command> m_requests;
    uint8_t m_requestCount = 0;

    QTimer m_timeoutStatus;
    void receivedStatusInformation(const StatusInformation& info);
};
}

#endif // HSECLIENT_H
