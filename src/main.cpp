/*
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

#include <QApplication>
#include "browserwindow.h"
#include "browserapplication.h"

#include "backend.h"
#include "historylocator.h"
#include "location.h"
#include "mousewheelarea.h"
#include "searchlocator.h"
#include "tab.h"
#include "tabmanager.h"

int main(int argc, char** argv)
{
    qmlRegisterType<Tab>();
    qmlRegisterType<QObjectListModel>();
    qmlRegisterType<TabHistoryListModel>();
    qmlRegisterType<TabManager>();
    qmlRegisterType<HistoryLocator>();
    qmlRegisterType<SearchLocator>();
    qmlRegisterType<Backend>();
    qmlRegisterType<QDeclarativeWebSettings>();
    qmlRegisterType<QDeclarativeWebView>("liquid", 1, 0, "WebView");
    qmlRegisterType<Location>("liquid", 1, 0, "Location");
    qmlRegisterType<MouseWheelArea>("liquid", 1, 0, "MouseWheelArea");

    BrowserApplication app(argc, argv);
    if (!app.isRunning())
        return 0;

    return app.exec();
}
