#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <windowsx.h>
#include <winsock2.h>

#define MSGSIZE 1024
#define WM_WSASYNC (WM_USER + 1)
#define DATA_BUFSIZE 8192

class Socket : public QObject
{
    Q_OBJECT
protected:
    /**
     *
     */
    SOCKET socket_;

    /**
     *
     */
    HWND hWnd_;

public:
    //Socket() {}
    //virtual ~Socket() {}
    SOCKET getSocket() { return socket_; }

    /**
     *
     * @param addressFamily
     * @param connectionType
     * @param protocol
     *
     * @return
     *
     * @author Tom Nightingale.
     */
    bool open(int addressFamily, int connectionType, int protocol);

    /**
     *
     * @param pSockAddr
     *
     * @return
     *
     * @author Tom Nightingale.
     */
    virtual bool listen(PSOCKADDR_IN pSockAddr);

    /**
     *
     * @param pMsg
     *
     * @author Tom Nightingale.
     */
    void close(PMSG pMsg);

    /**
     *
     * @param socket
     * @param hWnd
     * @param flags
     *
     * @return
     *
     * @author Tom Nightingale.
     */
    static bool init(SOCKET socket, HWND hWnd, int flags) {
        int err = 0;

        if ((err = WSAAsyncSelect(socket, hWnd, WM_WSASYNC,
                                  flags)) == SOCKET_ERROR) {
            throw "Connection::startServer(): Error setting up async select.";
            return false;
        }

        return true;
    }

signals:
    void signalCloseSocket(SOCKET socket);

public slots:
    /**
     *
     * @param pMsg
     *
     * @return
     *
     * @author Tom Nightingale.
     */
    virtual bool slotProcessWSAEvent(PMSG pMsg);

};

#endif // SOCKET_H
