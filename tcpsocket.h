#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include "socket.h"

class TCPSocket : public Socket
{
    Q_OBJECT
public:
    TCPSocket(HWND hWnd);
    ~TCPSocket() {}

    /**
     *
     * @param pMsg
     *
     * @author Tom Nightingale
     */
    void accept(PMSG pMsg);

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

    bool listen(PSOCKADDR_IN pSockAddr);

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

#endif // TCPSOCKET_H
