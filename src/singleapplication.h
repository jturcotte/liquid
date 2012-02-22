/*
    Copyright (C) 2011 Jocelyn Turcotte <turcotte.j@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

    This file incorporates work covered by the following copyright and
    permission notice:

      Copyright (c) 2008-2009, Benjamin C. Meyer

      Redistribution and use in source and binary forms, with or without
      modification, are permitted provided that the following conditions
      are met:
      1. Redistributions of source code must retain the above copyright
         notice, this list of conditions and the following disclaimer.
      2. Redistributions in binary form must reproduce the above copyright
         notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.
      3. Neither the name of the Benjamin Meyer nor the names of its contributors
         may be used to endorse or promote products derived from this software
         without specific prior written permission.

      THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
      ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
      IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
      ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
      FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
      DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
      OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
      HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
      LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
      OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
      SUCH DAMAGE.
*/

#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <qapplication.h>

/*
    QApplication subclass that should be used when you only want one
    instant of the application to exist at a time.
*/
class QLocalServer;
class QLocalSocket;
class SingleApplication : public QApplication
{
    Q_OBJECT

signals:
    void messageReceived(QLocalSocket *socket);

public:
    SingleApplication(int &argc, char **argv);

    bool sendMessage(const QByteArray &message, int waitMsecsForReply = 0);
    bool startSingleServer();
    bool isRunning() const;

private slots:
    void newConnection();

private:
    QString serverName() const;
    QLocalServer *m_localServer;

};

#endif // SINGLEAPPLICATION_H

