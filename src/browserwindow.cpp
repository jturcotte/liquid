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

#include "browserwindow.h"

#include <QCoreApplication>
#include <QDeclarativeEngine>
#include <QGraphicsObject>
#include <QKeyEvent>
#include "backend.h"
#include "dwmhelper.h"

BrowserWindow::BrowserWindow()
: m_backend(new Backend(this))
{
    m_backend->initializeEngine(engine());
#ifdef Q_OS_MAC
    // Crappy hardcoded path to use until the qml is bundled as resources.
    m_rootDirPath = QCoreApplication::applicationDirPath() + "/../../..";
#else
    m_rootDirPath = QCoreApplication::applicationDirPath();
#endif
    engine()->addImportPath(m_rootDirPath);
    setResizeMode(QDeclarativeView::SizeRootObjectToView);

    if (DwmHelper::isCompositionEnabled()) {
        // Hard-code the limits just enough to extend the blurred frame under the tab bar.
        DwmHelper::extendFrameIntoClientArea(this, 1, 1, 30, 1);
        setContentsMargins(0, 0, 0, 0);
        QPalette p = palette();
        p.setBrush(backgroundRole(), Qt::NoBrush);
        setPalette(p);
    } else {
        QPalette p = palette();
        p.setColor(backgroundRole(), QColor(170, 170, 170));
        setPalette(p);
    }
    loadQml();
}

void BrowserWindow::addNewTab(const QUrl& url)
{
    rootObject()->metaObject()->invokeMethod(rootObject(), "addNewTab", Qt::DirectConnection, Q_ARG(QVariant, url));
}

void BrowserWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_F12)
        loadQml();
    else
        QDeclarativeView::keyPressEvent(event);
}

void BrowserWindow::loadQml()
{
    delete rootObject();
    engine()->clearComponentCache();
    setSource(QUrl::fromLocalFile(m_rootDirPath + "/qml/main.qml"));
}
