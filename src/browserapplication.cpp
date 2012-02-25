/*
    Copyright 2008-2009 Benjamin C. Meyer <ben@meyerhome.net>
    Copyright 2011 Jocelyn Turcotte <turcotte.j@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA
*/

/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "browserapplication.h"

#include <qlocalsocket.h>
#include <QFileOpenEvent>

#ifdef Q_OS_WIN
#pragma comment(lib, "User32.lib")
#include <windows.h>
#endif

BrowserApplication::BrowserApplication(int &argc, char **argv)
    : SingleApplication(argc, argv)
{
    connect(this, SIGNAL(messageReceived(QLocalSocket *)), this, SLOT(messageReceived(QLocalSocket *)));

    QString args = QStringList(QCoreApplication::arguments().mid(1)).join(" ");
    if (!args.isEmpty()) {
        sendMessage((QLatin1String("message://openurl/") + args).toUtf8());
    }

    // If we could connect to another instance then exit.
    QString message = QString(QLatin1String("message://getwinid"));
    if (sendMessage(message.toUtf8(), 500))
        return;

    if (!startSingleServer()) {
        qFatal("Could not start the SingleApplication server. Fix it.");
        return;
    }

    bool restoredTabs = m_window.restoreTabs();
    QUrl paramUrl = QUrl::fromUserInput(args);
    if (paramUrl.isValid())
        m_window.addNewTab(paramUrl);
    else if (!restoredTabs)
        m_window.addNewTab(QUrl("http://google.com"));
    m_window.show();
}

BrowserApplication::~BrowserApplication()
{
}

bool BrowserApplication::event(QEvent* e)
{
    switch (e->type()) {
    case QEvent::FileOpen:
        m_window.addNewTab(static_cast<QFileOpenEvent*>(e)->url());
        return true;
    default:
        return SingleApplication::event(e);
    }
}

void BrowserApplication::messageReceived(QLocalSocket *socket)
{
    QString message;
    QTextStream stream(socket);
    stream >> message;
    if (message.isEmpty())
        return;

    if (message.startsWith(QLatin1String("message://openurl/"))) {
        m_window.addNewTab(QUrl::fromUserInput(message.mid(18)));
        return;
    }

    if (message.startsWith(QLatin1String("message://getwinid"))) {
#ifdef Q_OS_WIN
        QString winid = QString(QLatin1String("%1")).arg((qlonglong)m_window.winId());
#else
        m_window.show();
        m_window.setFocus();
        m_window.raise();
        m_window.activateWindow();
        alert(&m_window);
        QString winid;
#endif
        QString message = QLatin1String("message://winid/") + winid;
        socket->write(message.toUtf8());
        socket->waitForBytesWritten();
        return;
    }

    if (message.startsWith(QLatin1String("message://winid"))) {
        QString winid = message.mid(16);
#ifdef Q_OS_WIN
        WId wid = (WId)winid.toLongLong();
        SetForegroundWindow(wid);
#endif
        return;
    }
}


