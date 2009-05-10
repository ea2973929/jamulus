/******************************************************************************\
 * Copyright (c) 2004-2009
 *
 * Author(s):
 *  Volker Fischer
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later 
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
\******************************************************************************/

#if !defined ( TESTBENCH_HOIHJH8_3_43445KJIUHF1912__INCLUDED_ )
#define TESTBENCH_HOIHJH8_3_43445KJIUHF1912__INCLUDED_

#include <qobject.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qhostaddress.h>
#include "global.h"
#include "socket.h"
#include "protocol.h"
#include "util.h"


/* Classes ********************************************************************/
class CTestbench : public QObject
{
    Q_OBJECT

public:
    CTestbench()
    {
        // bind socket
        UdpSocket.bind ( QHostAddress ( QHostAddress::Any ), 22222 );

        // connect protocol signal
        QObject::connect ( &Protocol, SIGNAL ( MessReadyForSending ( CVector<uint8_t> ) ),
            this, SLOT ( OnSendProtMessage ( CVector<uint8_t> ) ) );

        // connect and start the timer (testbench heartbeat)
        QObject::connect ( &Timer, SIGNAL ( timeout() ),
            this, SLOT ( OnTimer() ) );
        Timer.start ( 1 ); // 1 ms
    }

protected:
    int GenRandomIntInRange ( const int iStart, const int iEnd ) const
    {
        return static_cast<int> ( iStart +
            ( ( static_cast<double> ( iEnd - iStart + 1 ) * rand() ) / RAND_MAX ) );
    }

    QTimer     Timer;
    CProtocol  Protocol;
    QUdpSocket UdpSocket;

public slots:
    void OnTimer()
    {
        // generate random protocol message
        switch ( GenRandomIntInRange ( 0, 11 ) )
        {
        case 0:
            Protocol.CreateJitBufMes ( GenRandomIntInRange ( 0, 10 ) );
            break;

        case 1:
            Protocol.CreateReqJitBufMes();
            break;

        case 2:
            Protocol.CreateNetwBlSiFactMes ( GenRandomIntInRange ( 0, 10 ) );
            break;

        case 3:
            Protocol.CreateChanGainMes ( GenRandomIntInRange ( 0, 20 ),
                GenRandomIntInRange ( -100, 100 ) );
            break;

        case 4:
            Protocol.CreateServerFullMes();
            break;

        case 5:
            Protocol.CreateReqConnClientsList();
            break;

        case 6:
            Protocol.CreateChanNameMes ( QString ( "test%1" ).arg (
                GenRandomIntInRange ( 0, 1000 ) ) );
            break;

        case 7:
            Protocol.CreateChatTextMes ( QString ( "test%1" ).arg (
                GenRandomIntInRange ( 0, 1000 ) ) );
            break;

        case 8:
            Protocol.CreatePingMes ( GenRandomIntInRange ( 0, 100000 ) );
            break;

        case 9:
            Protocol.CreateReqNetwTranspPropsMes();
            break;

        case 10:
            Protocol.CreateAndSendAcknMess ( GenRandomIntInRange ( -10, 100 ),
                GenRandomIntInRange ( -100, 100 ) );
            break;

        case 11:
            // arbitrary "audio" packet (with random sizes)
            CVector<uint8_t> vecMessage ( GenRandomIntInRange ( 1, 1000 ) );
            OnSendProtMessage ( vecMessage );
            break;
        }
    }

    void OnSendProtMessage ( CVector<uint8_t> vecMessage )
    {
        UdpSocket.writeDatagram (
            (const char*) &( (CVector<uint8_t>) vecMessage )[0],
            vecMessage.Size(), QHostAddress ( "127.0.0.1" ),
            LLCON_DEFAULT_PORT_NUMBER );

        // reset protocol so that we do not have to wait for an acknowledge to
        // send the next message
        Protocol.Reset();
    }
};

#endif /* !defined ( TESTBENCH_HOIHJH8_3_43445KJIUHF1912__INCLUDED_ ) */