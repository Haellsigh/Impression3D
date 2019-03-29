#ifndef HSECLIENT_H
#define HSECLIENT_H

#include <QObject>

#include <QUdpSocket>

#include "hsestructures.h"

namespace HSE {

class Client : public QObject {
    Q_OBJECT

public:
    Client();

    void SendCommand(Command command_id, int16_t instance, uint8_t attribut, uint8_t service, QByteArray data = QByteArray());

    // Liste des requètes possible
public slots:
    void StatusInformationRead();

    // Liste des signaux pour récupérer des infos de requètes
signals:
    void getStatusInformationRead(StatusInformation info);

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
};
}

#endif // HSECLIENT_H
