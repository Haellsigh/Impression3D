#include "hseclient.h"

#include <QNetworkDatagram>

#include <QMessageBox>

namespace HSE {

Client::Client()
{
    m_ip   = QHostAddress("192.168.255.1");
    m_port = 10040;

    m_socket.bind(m_ip, m_port);
    connect(&m_socket, &QUdpSocket::readyRead, this, &Client::readPendingDatagrams);
}

void Client::SendCommand(Command command_id, int16_t instance, uint8_t attribut, uint8_t service, QByteArray data)
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

void Client::StatusInformationRead()
{
    m_requests[m_requestCount] = STATUS_INFORMATION_R;
    SendCommand(STATUS_INFORMATION_R, 0x01, 0x00, GET_ALL_ATTRIBUTES);
}

void Client::readPendingDatagrams()
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
        if (status.Status != 0x00)
            return;

        /// Traiter les données s'il y en a
        uint16_t dataLength = data.length() - 32;
        if (dataLength > 0) {
            QByteArray dataReceived = data.mid(32);
            emit requestData(dataReceived);
            processReceivedData(request_id, dataReceived);
        }
    }
}

void Client::processReceivedData(const uint8_t request_id, const QByteArray data)
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
    case HOLDOR_OR_SERVO_ONOFF:
        break;
    case MOVE_CARTESIAN:
        break;
    case MOVE_PULSE:
        break;
    }

    m_requests.remove(cmd);
}

}
