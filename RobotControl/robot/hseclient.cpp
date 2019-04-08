#include "hseclient.h"

#include <QNetworkDatagram>

#include "log.h"

namespace dx200 {

HSEClient::HSEClient()
{
    m_ip   = QHostAddress("192.168.255.1");
    m_port = 10040;

    m_socket.bind(m_ip, m_port);
    connect(&m_socket, &QUdpSocket::readyRead, this, &HSEClient::readPendingDatagrams);

    // Periodic requests
    connect(&m_timeoutStatus, &QTimer::timeout, this, &HSEClient::statusInformationRead);
    connect(&m_timeoutStatus, &QTimer::timeout, [&]() {
        qWarning() << tr("HSEClient timeout: Robot's station did not answer in time");
    });
    m_timeoutStatus.setInterval(500);
    m_timeoutStatus.start();
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

void HSEClient::hold(bool hold)
{
    // 1: Hold on, 2: Hold off
    uint8_t toHold = hold ? 1 : 2;

    QByteArray data(4, 0);
    data[0] = toHold;

    sendCommand(HOLD_OR_SERVO_ONOFF, 0x01, 0x01, 0x10, data);
}

void HSEClient::servo(bool on)
{
    // 1: Servo on, 2: Servo off
    uint8_t toServo = on ? 1 : 2;

    QByteArray data(4, 0);
    data[0] = toServo;

    sendCommand(HOLD_OR_SERVO_ONOFF, 0x02, 0x01, 0x10, data);
}

void HSEClient::hlock(bool lock)
{
    // 1: HLock on, 2: HLock off
    uint8_t toLock = lock ? 1 : 2;

    QByteArray data(4, 0);
    data[0] = toLock;

    sendCommand(HOLD_OR_SERVO_ONOFF, 0x03, 0x01, 0x10, data);
}

void HSEClient::moveCartesian(Movement::Type type, Movement::Cartesian movement)
{
    const uint32_t n = 26;
    std::array<uint32_t, n> data32bit;
    int i = 0;

    // i = 0
    data32bit[i++] = movement.robotNo;
    data32bit[i++] = movement.stationNo;
    data32bit[i++] = movement.classification;
    data32bit[i++] = movement.speed;
    data32bit[i++] = movement.coordinate;
    data32bit[i++] = movement.x;
    data32bit[i++] = movement.y;
    data32bit[i++] = movement.z;
    data32bit[i++] = movement.tx;
    data32bit[i++] = movement.ty;
    data32bit[i++] = movement.tz;

    // i = 11
    data32bit[i++] = 0x00;
    data32bit[i++] = 0x00;

    // i = 13
    data32bit[i++] = movement.type;
    data32bit[i++] = movement.expandedType;
    data32bit[i++] = movement.toolNo;
    data32bit[i++] = movement.userCoordinateNo;

    // i = 17
    data32bit[i++] = movement.baseAxisPosition.at(0);
    data32bit[i++] = movement.baseAxisPosition.at(1);
    data32bit[i++] = movement.baseAxisPosition.at(2);

    // i = 20
    data32bit[i++] = movement.stationAxisPosition.at(0);
    data32bit[i++] = movement.stationAxisPosition.at(1);
    data32bit[i++] = movement.stationAxisPosition.at(2);
    data32bit[i++] = movement.stationAxisPosition.at(3);
    data32bit[i++] = movement.stationAxisPosition.at(4);
    data32bit[i++] = movement.stationAxisPosition.at(5);

    // Pack the array of 32bit data into an array of 8bit data
    QByteArray data(n * 4, Qt::Initialization::Uninitialized);
    for (uint32_t i = 0; i < n; i++) {
        data[i * 4]     = getByte<0>(data32bit[i]);
        data[i * 4 + 1] = getByte<1>(data32bit[i]);
        data[i * 4 + 2] = getByte<2>(data32bit[i]);
        data[i * 4 + 3] = getByte<3>(data32bit[i]);
    }

    sendCommand(MOVE_CARTESIAN, type, 0x01, 0x02, data);
}

void HSEClient::movePulse(Movement::Type type, Movement::Pulse movement)
{
}

void HSEClient::readPendingDatagrams()
{
    while (m_socket.hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_socket.receiveDatagram();
        QByteArray data           = datagram.data();

        // Read the request id
        uint8_t request_id = data.at(11);

        /// Read the answer's status
        RequestStatus status;
        status.status          = data.at(25);
        status.addedStatusSize = data.at(26);

        // See page 21 of the dx200 ethernet manual
        // Size >= 1 bytes ==> At least status1 exists
        if (status.addedStatusSize >= 1) // 2 bytes
        {
            status.status1 = toUInt16(data, 28);
        }
        // At least status 1 et 2 exists
        if (status.addedStatusSize >= 2) // 4 bytes
        {
            status.status2 = toUInt16(data, 30);
        }

        emit requestStatus(status);

        /// Stop here if there's an error
        if (status.status != 0x00) {
            return;
        }

        /// Process the data if there's any
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
        info.data1 = data[0];
        info.data2 = data[4];
        emit readStatusInformation(info);

        // Reset the timeout
        m_timeoutStatus.start();
        // Request new information as soon as possible
        statusInformationRead();
        // TODO: See if this overloads the connection limits (maybe add a delay)
        // The connection limit is 3000 packets/s
    } break;
    case JOB_INFORMATION_R:
        break;
    case AXIS_CONFIGURATION_INFORMATION_R:
        break;
    case ROBOT_POSITION_DATA_R: {
        int i = 0; // Index of data type
        // Pulse values
        if (toUInt32(data, 4 * (i - 1)) == 0) {
            Movement::Pulse pos;

            i          = 3;
            pos.toolNo = toUInt32(data, 4 * (i - 1));

            for (i = 6; i <= 13; i++) {
                pos.robotAxisPulseValue[i - 6] = toUInt32(data, 4 * (i - 1));
            }

            emit readPositionPulse(pos);
        } else if (toUInt32(data, 0) == 16) {
            Movement::Cartesian pos;

            i                    = 2;
            pos.type             = toUInt32(data, 4 * (i++ - 1));
            pos.toolNo           = toUInt32(data, 4 * (i++ - 1));
            pos.userCoordinateNo = toUInt32(data, 4 * (i++ - 1));
            pos.expandedType     = toUInt32(data, 4 * (i++ - 1));
            pos.x                = toUInt32(data, 4 * (i++ - 1));
            pos.y                = toUInt32(data, 4 * (i++ - 1));
            pos.z                = toUInt32(data, 4 * (i++ - 1));
            pos.tx               = toUInt32(data, 4 * (i++ - 1));
            pos.ty               = toUInt32(data, 4 * (i++ - 1));
            pos.tz               = toUInt32(data, 4 * (i++ - 1));

            //TODO: What to do with 7th and 8th axis

            emit readPositionCartesian(pos);
        }
    } break;
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

    // Remove the processed request
    m_requests.remove(cmd);
}

}
