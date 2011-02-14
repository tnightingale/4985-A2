#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QObject>
#include "socket.h"

class UDPSocket : public Socket
{
    Q_OBJECT
public:
    UDPSocket(HWND hWnd);
    ~UDPSocket() {}

    /**
     *
     * @param pMsg
     *
     * @author Tom Nightingale
     */
    void receive(PMSG pMsg);

    /**
     *
     * @param pMsg
     *
     * @author Tom Nightingale
     */
    void send(PMSG pMsg);

public slots:
    /**
     *
     * @param pMsg
     *
     * @return
     *
     * @author Tom Nightingale.
     */
    bool slotProcessWSAEvent(PMSG pMsg);
};

#endif // UDPSOCKET_H
