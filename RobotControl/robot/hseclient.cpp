#include "hseclient.h"

#include <QNetworkDatagram>

#include <QMessageBox>

namespace dx200 {

HSEClient::HSEClient()
{
    m_ip   = QHostAddress("192.168.255.1");
    m_port = 10040;

    m_socket.bind(m_ip, m_port);
    connect(&m_socket, &QUdpSocket::readyRead, this, &HSEClient::readPendingDatagrams);
}

void HSEClient::sendCommand(Command command_id, int16_t instance, uint8_t attribut, uint8_t service, QByteArray data)
{
    uint16_t dataLength = data.length();
    QByteArray framedData;
    framedData.resize(32 + dataLength);

    int i = 0;

    // Identifier
    framedData[i++] = 'Y';
    framedData[i++] = 'E';
    framedData[i++] = 'R';
    framedData[i++] = 'C';

    // Header length
    framedData[i++] = 0x20;
    framedData[i++] = 0x00;

    // framedData length
    framedData[i++] = getByte<0>(dataLength);
    framedData[i++] = getByte<1>(dataLength);

    // Reserve1
    framedData[i++] = 3;

    // Processing division
    framedData[i++] = 0x01;

    // ACK
    framedData[i++] = 0x00;

    // Save the current request
    m_requests[m_requestCount] = STATUS_INFORMATION_R;
    // Request ID
    framedData[i++] = m_requestCount++; // Increment by one for each request

    // Block number
    framedData[i++] = 0x00;
    framedData[i++] = 0x00;
    framedData[i++] = 0x00;
    framedData[i++] = 0x00;

    // Reserve
    framedData[i++] = '9';
    framedData[i++] = '9';
    framedData[i++] = '9';
    framedData[i++] = '9';
    framedData[i++] = '9';
    framedData[i++] = '9';
    framedData[i++] = '9';
    framedData[i++] = '9';

    // Command ID
    framedData[i++] = getByte<0>(command_id);
    framedData[i++] = getByte<1>(command_id);

    // Instance
    framedData[i++] = getByte<0>(instance);
    framedData[i++] = getByte<1>(instance);

    // Attribute
    framedData[i++] = attribut;

    // Service
    framedData[i++] = service;

    // Padding
    framedData[i++] = 0x00;
    framedData[i++] = 0x00;

    // Append the data after the 32bytes header
    framedData.append(data);

    // Send data to socket
    emit requestSent();
    m_socket.writeDatagram(framedData, m_ip, m_port);
}

void HSEClient::statusInformationRead()
{
    sendCommand(STATUS_INFORMATION_R, 0x01, 0x00, GET_ALL_ATTRIBUTES);
}

void HSEClient::resetAlarms()
{
    QByteArray data(4, 0);
    data[0] = 1;
    sendCommand(ALARM_RESET, 0x01, 0x01, 0x10, data);
}

void HSEClient::cancelErrors()
{
    QByteArray data(4, 0);
    data[0] = 1;
    sendCommand(ALARM_RESET, 0x02, 0x01, 0x10, data);
}

void HSEClient::hlock(bool lock)
{
    // 1: HLock on, 2: HLock off
    uint8_t toLock = lock ? 1 : 2;

    QByteArray data(4, 0);
    data[0] = toLock;

    sendCommand(HOLD_OR_SERVO_ONOFF, 0x03, 0x01, 0x10, data);
}

void HSEClient::hold(bool hold)
{
    // 1: Hold on, 2: Hold off
    uint8_t toHold = hold ? 1 : 2;

    QByteArray data(4, 0);
    data[0] = toHold;

    sendCommand(HOLD_OR_SERVO_ONOFF, 0x01, 0x01, 0x10, data);
}

void HSEClient::moveCartesian(Movement::Type type, Movement::Cartesian movement)
{

    QByteArray data(26, 0);
    int i     = 0;
    data[i++] = movement.RobotNo;
    data[i++] = movement.StationNo;
    data[i++] = movement.Classification;
    data[i++] = movement.Speed;
    data[i++] = movement.Coordinate;
    data[i++] = movement.X;
    data[i++] = movement.Y;
    data[i++] = movement.Z;
    data[i++] = movement.Tx;
    data[i++] = movement.Ty;
    data[i++] = movement.Tz;

    data[i++] = 0x00;

    data[i++] = movement.Type;
    data[i++] = movement.ExpandedType;
}

void HSEClient::movePulse(Movement::Type type, Movement::Pulse movement)
{
}

void HSEClient::readPendingDatagrams()
{
    while (m_socket.hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_socket.receiveDatagram();
        QByteArray data           = datagram.data();

        // Lire l'id de la requète
        uint8_t request_id = data.at(11);

        /// Lire le status de la requete
        RequestStatus status;
        status.Status          = data.at(25);
        status.AddedStatusSize = data.at(26);

        // Voir page 21 debut manuel ethernet dx200
        // Taille >= 2 bytes ==> Au moins Status 1 existe
        if (status.AddedStatusSize >= 1) // 2 bytes
        {
            status.Status1 = toUInt16(data, 28);
        }
        // Au moins Status 1 et 2 existent
        if (status.AddedStatusSize >= 2) // 4 bytes
        {
            status.Status2 = toUInt16(data, 30);
        }

        emit requestStatus(status);

        /// S'arreter si il y a une erreur
        if (status.Status != 0x00) {
            qWarning().noquote() << "HSEClient" << tr("error") << "!";
            return;
        }

        /// Traiter les données s'il y en a
        uint16_t dataLength = data.length() - 32;
        if (dataLength > 0) {
            QByteArray dataReceived = data.mid(32);
            emit requestData(dataReceived);
            processReceivedData(request_id, dataReceived);
        }
    }
}

void HSEClient::processReceivedData(const uint8_t request_id, const QByteArray data)
{
    Command cmd = m_requests[request_id];

    switch (cmd) {
    case ALARM_DATA_R:
        break;
    case ALARM_HISTORY_R:
        break;
    case STATUS_INFORMATION_R: {
        StatusInformation info;
        info.Data1 = data[0];
        info.Data2 = data[4];
        emit getStatusInformationRead(info);
    } break;
    case JOB_INFORMATION_R:
        break;
    case AXIS_CONFIGURATION_INFORMATION_R:
        break;
    case PLURAL_B_VARIABLE_RW:
        break;
    case BYTE_VARIABLE:
        break;
    case ALARM_RESET:
        break;
    case HOLD_OR_SERVO_ONOFF:
        break;
    case MOVE_CARTESIAN:
        break;
    case MOVE_PULSE:
        break;
    }

    m_requests.remove(cmd);
}

}
