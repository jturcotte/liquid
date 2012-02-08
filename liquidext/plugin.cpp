/*
    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2011 Jocelyn Turcotte <turcotte.j@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this program; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "qdeclarativewebview_p.h"

#include "backend.h"
#include "historylocator.h"
#include "location.h"
#include "mousewheelarea.h"
#include "searchlocator.h"
#include "tab.h"
#include "tabmanager.h"
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativeextensionplugin.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>

QT_BEGIN_NAMESPACE

class LiquidExtensionPlugin : public QDeclarativeExtensionPlugin {
    Q_OBJECT
public:
    virtual void registerTypes(const char* uri)
    {
        qmlRegisterType<Tab>();
        qmlRegisterType<QObjectListModel>();
        qmlRegisterType<TabManager>();
        qmlRegisterType<Location>(uri, 1, 0, "Location");
        qmlRegisterType<HistoryLocator>();
        qmlRegisterType<SearchLocator>();
        qmlRegisterType<Backend>();
#ifndef WK2_BUILD
        qmlRegisterType<QDeclarativeWebSettings>();
        qmlRegisterType<QDeclarativeWebView>(uri, 1, 0, "WebView");
#else
        qmlRegisterType<QDeclarativeWKSettings>();
        qmlRegisterType<QDeclarativeWKView>(uri, 1, 0, "WebView");
#endif
        qmlRegisterType<MouseWheelArea>(uri, 1, 0, "MouseWheelArea");
    }
    virtual void initializeEngine(QDeclarativeEngine *engine, const char *)
    {
        Backend *backend = new Backend(engine);
        backend->initializeEngine(engine);
    }
};

QT_END_NAMESPACE

#include "plugin.moc"

Q_EXPORT_PLUGIN2(liquidextensionplugin, QT_PREPEND_NAMESPACE(LiquidExtensionPlugin));

