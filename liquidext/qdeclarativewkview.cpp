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


#include "qdeclarativewkview_p.h"

#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QFile>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/qdeclarative.h>
#include <QtGui/QApplication>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPen>
#include "qwkcontext.h"

QT_BEGIN_NAMESPACE

class QDeclarativeWKViewPrivate {
public:
    QDeclarativeWKViewPrivate(QDeclarativeWKView* qq)
      : q(qq)
      , preferredwidth(0)
      , preferredheight(0)
      , progress(1.0)
      , status(QDeclarativeWKView::Null)
      , pending(PendingNone)
      , newWindowComponent(0)
      , newWindowParent(0)
    {
    }

    QDeclarativeWKView* q;

    QUrl url; // page url might be different if it has not loaded yet
    QGraphicsWKView* view;

    int preferredwidth, preferredheight;
    qreal progress;
    QDeclarativeWKView::Status status;
    QString statusText;
    enum { PendingNone, PendingUrl, PendingHtml, PendingContent } pending;
    QUrl pendingUrl;
    QString pendingString;
    QByteArray pendingData;
    mutable QDeclarativeWKSettings settings;
    QDeclarativeComponent* newWindowComponent;
    QDeclarativeItem* newWindowParent;
    QImage icon;

    // static void windowObjectsAppend(QDeclarativeListProperty<QObject>* prop, QObject* o)
    // {
    //     static_cast<QDeclarativeWKViewPrivate*>(prop->data)->windowObjects.append(o);
    //     static_cast<QDeclarativeWKViewPrivate*>(prop->data)->updateWindowObjects();
    // }

    // void updateWindowObjects();
    QObjectList windowObjects;
};

// GraphicsWebView::GraphicsWebView(QDeclarativeWKView* parent)
//     : QGraphicsWebView(parent)
//     , parent(parent)
// {
// }

/*!
    \qmlclass WebView QDeclarativeWKView
    \ingroup qml-view-elements
    \since 4.7
    \brief The WebView item allows you to add Web content to a canvas.
    \inherits Item

    A WebView renders Web content based on a URL.

    This type is made available by importing the \c QtWebKit module:

    \bold{import QtWebKit 1.0}

    The WebView item includes no scrolling, scaling, toolbars, or other common browser
    components. These must be implemented around WebView. See the \l{QML Web Browser}
    example for a demonstration of this.

    The page to be displayed by the item is specified using the \l url property,
    and this can be changed to fetch and display a new page. While the page loads,
    the \l progress property is updated to indicate how much of the page has been
    loaded.

    \section1 Appearance

    If the width and height of the item is not set, they will dynamically adjust
    to a size appropriate for the content. This width may be large for typical
    online web pages, typically greater than 800 by 600 pixels.

    If the \l{Item::}{width} or \l{Item::}{height} is explictly set, the rendered Web site will be
    clipped, not scaled, to fit into the set dimensions.

    If the preferredWidth property is set, the width will be this amount or larger,
    usually laying out the Web content to fit the preferredWidth.

    The appearance of the content can be controlled to a certain extent by changing
    the settings.standardFontFamily property and other settings related to fonts.

    The page can be zoomed by calling the heuristicZoom() method, which performs a
    series of tests to determine whether zoomed content will be displayed in an
    appropriate way in the space allocated to the item.

    \section1 User Interaction and Navigation

    By default, certain mouse and touch events are delivered to other items in
    preference to the Web content. For example, when a scrolling view is created
    by placing a WebView in a Flickable, move events are delivered to the Flickable
    so that the user can scroll the page. This prevents the user from accidentally
    selecting text in a Web page instead of scrolling.

    When this item has keyboard focus, all keyboard input will be sent directly to
    the Web page within.

    When the navigates by clicking on links, the item records the pages visited
    in its internal history

    Because this item is designed to be used as a component in a browser, it
    exposes \l{Action}{actions} for \l back, \l forward, \l reload and \l stop.
    These can be triggered to change the current page displayed by the item.

    \section1 Example Usage

    \beginfloatright
    \inlineimage webview.png
    \endfloat

    The following example displays a scaled down Web page at a fixed size.

    \snippet doc/src/snippets/declarative/webview/webview.qml document

    \clearfloat

    \sa {declarative/modelviews/webview}{WebView example}, {demos/declarative/webbrowser}{Web Browser demo}
*/

/*!
    \internal
    \class QDeclarativeWKView
    \brief The QDeclarativeWKView class allows you to add web content to a QDeclarativeView.

    A WebView renders web content base on a URL.

    \image webview.png

    The item includes no scrolling, scaling,
    toolbars, etc., those must be implemented around WebView. See the WebBrowser example
    for a demonstration of this.

    A QDeclarativeWKView object can be instantiated in Qml using the tag \l WebView.
*/

QDeclarativeWKView::QDeclarativeWKView(QWKContext *context, QDeclarativeItem *parent) : QDeclarativeItem(parent)
{
    // FIXME.
    Q_ASSERT(context);
    init(context);
}

QDeclarativeWKView::~QDeclarativeWKView()
{
    delete d;
}

void QDeclarativeWKView::init(QWKContext *context)
{
    d = new QDeclarativeWKViewPrivate(this);

    // if (QWebSettings::iconDatabasePath().isNull() &&
    //     QWebSettings::globalSettings()->localStoragePath().isNull() &&
    //     QWebSettings::offlineStoragePath().isNull() &&
    //     QWebSettings::offlineWebApplicationCachePath().isNull())
    //     QWebSettings::enablePersistentStorage();

    setAcceptedMouseButtons(Qt::LeftButton);
    setFlag(QGraphicsItem::ItemHasNoContents, true);
    setClip(true);


    d->view = new QGraphicsWKView(context, QGraphicsWKView::Simple, this);
    setFocusProxy(d->view);
    // QWKPage* wp = new QWKPage(this);
    // setPage(wp);
    setPage(0);
    // connect(d->view, SIGNAL(doubleClick(int, int)), this, SIGNAL(doubleClick(int, int)));
    connect(context->iconDatabase(), SIGNAL(iconLoadedForPageURL(const QUrl&)), SLOT(doPageIconChanged(const QUrl&)));
}

void QDeclarativeWKView::componentComplete()
{
    QDeclarativeItem::componentComplete();
    // page()->setNetworkAccessManager(qmlEngine(this)->networkAccessManager());

    switch (d->pending) {
    case QDeclarativeWKViewPrivate::PendingUrl:
        setUrl(d->pendingUrl);
        break;
    case QDeclarativeWKViewPrivate::PendingHtml:
        // setHtml(d->pendingString, d->pendingUrl);
        break;
    case QDeclarativeWKViewPrivate::PendingContent:
        // setContent(d->pendingData, d->pendingString, d->pendingUrl);
        break;
    default:
        break;
    }
    d->pending = QDeclarativeWKViewPrivate::PendingNone;
    // d->updateWindowObjects();
}

QDeclarativeWKView::Status QDeclarativeWKView::status() const
{
    return d->status;
}


/*!
    \qmlproperty real WebView::progress
    This property holds the progress of loading the current URL, from 0 to 1.

    If you just want to know when progress gets to 1, use
    WebView::onLoadFinished() or WebView::onLoadFailed() instead.
*/
qreal QDeclarativeWKView::progress() const
{
    return d->progress;
}

void QDeclarativeWKView::doLoadStarted()
{
    if (!d->url.isEmpty()) {
        d->status = Loading;
        emit statusChanged(d->status);
    }
    emit loadStarted();
}

void QDeclarativeWKView::doLoadProgress(int p)
{
    if (d->progress == p / 100.0)
        return;
    d->progress = p / 100.0;
    emit progressChanged();
}

void QDeclarativeWKView::pageUrlChanged()
{
    if ((d->url.isEmpty() && page()->url() != QUrl(QLatin1String("about:blank")))
        || (d->url != page()->url() && !page()->url().isEmpty()))
    {
        if (d->url.host() != page()->url().host())
            emit iconChanged();

        d->url = page()->url();
        if (d->url == QUrl(QLatin1String("about:blank")))
            d->url = QUrl();
        emit urlChanged();
    }
}

void QDeclarativeWKView::doLoadFinished(bool ok)
{
    if (ok) {
        d->status = d->url.isEmpty() ? Null : Ready;
        emit loadFinished();
    } else {
        d->status = Error;
        emit loadFailed();
    }
    emit statusChanged(d->status);
}

/*!
    \qmlproperty url WebView::url
    This property holds the URL to the page displayed in this item. It can be set,
    but also can change spontaneously (eg. because of network redirection).

    If the url is empty, the page is blank.

    The url is always absolute (QML will resolve relative URL strings in the context
    of the containing QML document).
*/
QUrl QDeclarativeWKView::url() const
{
    return d->url;
}

void QDeclarativeWKView::setUrl(const QUrl& url)
{
    if (isComponentComplete()) {
        d->url = url;
        QUrl seturl = url;
        if (seturl.isEmpty())
            seturl = QUrl(QLatin1String("about:blank"));

        Q_ASSERT(!seturl.isRelative());

        page()->load(seturl);

        emit urlChanged();
    } else {
        d->pending = d->PendingUrl;
        d->pendingUrl = url;
    }
}

/*!
    \qmlmethod bool WebView::evaluateJavaScript(string scriptSource)

    Evaluates the \a scriptSource JavaScript inside the context of the
    main web frame, and returns the result of the last executed statement.

    Note that this JavaScript does \e not have any access to QML objects
    except as made available as windowObjects.
*/
// QVariant QDeclarativeWKView::evaluateJavaScript(const QString& scriptSource)
// {
//     return this->page()->mainFrame()->evaluateJavaScript(scriptSource);
// }

void QDeclarativeWKView::initialLayout()
{
    // nothing useful to do at this point
}

void QDeclarativeWKView::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
    d->view->setGeometry(QRectF(0, 0, newGeometry.width(), newGeometry.height()));
}

/*!
    \qmlproperty list<object> WebView::javaScriptWindowObjects

    A list of QML objects to expose to the web page.

    Each object will be added as a property of the web frame's window object.  The
    property name is controlled by the value of \c WebView.windowObjectName
    attached property.

    Exposing QML objects to a web page allows JavaScript executing in the web
    page itself to communicate with QML, by reading and writing properties and
    by calling methods of the exposed QML objects.

    This example shows how to call into a QML method using a window object.

    \qml
    WebView {
        javaScriptWindowObjects: QtObject {
            WebView.windowObjectName: "qml"

            function qmlCall() {
                console.log("This call is in QML!");
            }
        }

        html: "<script>console.log(\"This is in WebKit!\"); window.qml.qmlCall();</script>"
    }
    \endqml

    The output of the example will be:
    \code
    This is in WebKit!
    This call is in QML!
    \endcode

    If Javascript is not enabled for the page, then this property does nothing.
*/
// QDeclarativeListProperty<QObject> QDeclarativeWKView::javaScriptWindowObjects()
// {
//     return QDeclarativeListProperty<QObject>(this, d, &QDeclarativeWKViewPrivate::windowObjectsAppend);
// }

QDeclarativeWKViewAttached* QDeclarativeWKView::qmlAttachedProperties(QObject* o)
{
    return new QDeclarativeWKViewAttached(o);
}

// void QDeclarativeWKViewPrivate::updateWindowObjects()
// {
//     if (!q->isComponentCompletePublic() || !q->page())
//         return;

//     for (int i = 0; i < windowObjects.count(); ++i) {
//         QObject* object = windowObjects.at(i);
//         QDeclarativeWKViewAttached* attached = static_cast<QDeclarativeWKViewAttached *>(qmlAttachedPropertiesObject<QDeclarativeWKView>(object));
//         if (attached && !attached->windowObjectName().isEmpty())
//             q->page()->mainFrame()->addToJavaScriptWindowObject(attached->windowObjectName(), object);
//     }
// }

/*!
    \qmlsignal WebView::onDoubleClick(int clickx, int clicky)

    The WebView does not pass double-click events to the web engine, but rather
    emits this signals.
*/

#ifndef QT_NO_ACTION
/*!
    \qmlproperty action WebView::back
    This property holds the action for causing the previous URL in the history to be displayed.
*/
QAction* QDeclarativeWKView::backAction() const
{
    return page()->action(QWKPage::Back);
}

/*!
    \qmlproperty action WebView::forward
    This property holds the action for causing the next URL in the history to be displayed.
*/
QAction* QDeclarativeWKView::forwardAction() const
{
    return page()->action(QWKPage::Forward);
}

/*!
    \qmlproperty action WebView::reload
    This property holds the action for reloading with the current URL
*/
QAction* QDeclarativeWKView::reloadAction() const
{
    return page()->action(QWKPage::Reload);
}

/*!
    \qmlproperty action WebView::stop
    This property holds the action for stopping loading with the current URL
*/
QAction* QDeclarativeWKView::stopAction() const
{
    return page()->action(QWKPage::Stop);
}
#endif // QT_NO_ACTION

/*!
    \qmlproperty string WebView::title
    This property holds the title of the web page currently viewed

    By default, this property contains an empty string.
*/
QString QDeclarativeWKView::title() const
{
    return page()->title();
}

/*!
    \qmlproperty pixmap WebView::icon
    This property holds the icon associated with the web page currently viewed
*/
QPixmap QDeclarativeWKView::icon() const
{
    return page()->context()->iconDatabase()->iconForUrl(page()->url());
}

void QDeclarativeWKView::doPageIconChanged(const QUrl& url)
{
    if (url == page()->url())
        emit iconChanged();
}

/*!
    \qmlproperty string WebView::statusText

    This property is the current status suggested by the current web page. In a web browser,
    such status is often shown in some kind of status bar.
*/
void QDeclarativeWKView::setStatusText(const QString& text)
{
    d->statusText = text;
    emit statusTextChanged();
}

// void QDeclarativeWKView::windowObjectCleared()
// {
//     d->updateWindowObjects();
// }

QString QDeclarativeWKView::statusText() const
{
    return d->statusText;
}

QWKPage* QDeclarativeWKView::page() const
{
    return d->view->page();
}

// The QObject interface to settings().
/*!
    \qmlproperty string WebView::settings.standardFontFamily
    \qmlproperty string WebView::settings.fixedFontFamily
    \qmlproperty string WebView::settings.serifFontFamily
    \qmlproperty string WebView::settings.sansSerifFontFamily
    \qmlproperty string WebView::settings.cursiveFontFamily
    \qmlproperty string WebView::settings.fantasyFontFamily

    \qmlproperty int WebView::settings.minimumFontSize
    \qmlproperty int WebView::settings.minimumLogicalFontSize
    \qmlproperty int WebView::settings.defaultFontSize
    \qmlproperty int WebView::settings.defaultFixedFontSize

    \qmlproperty bool WebView::settings.autoLoadImages
    \qmlproperty bool WebView::settings.javascriptEnabled
    \qmlproperty bool WebView::settings.javaEnabled
    \qmlproperty bool WebView::settings.pluginsEnabled
    \qmlproperty bool WebView::settings.privateBrowsingEnabled
    \qmlproperty bool WebView::settings.javascriptCanOpenWindows
    \qmlproperty bool WebView::settings.javascriptCanAccessClipboard
    \qmlproperty bool WebView::settings.developerExtrasEnabled
    \qmlproperty bool WebView::settings.linksIncludedInFocusChain
    \qmlproperty bool WebView::settings.zoomTextOnly
    \qmlproperty bool WebView::settings.printElementBackgrounds
    \qmlproperty bool WebView::settings.offlineStorageDatabaseEnabled
    \qmlproperty bool WebView::settings.offlineWebApplicationCacheEnabled
    \qmlproperty bool WebView::settings.localStorageDatabaseEnabled
    \qmlproperty bool WebView::settings.localContentCanAccessRemoteUrls

    These properties give access to the settings controlling the web view.

    See QWebSettings for details of these properties.

    \qml
    WebView {
        settings.pluginsEnabled: true
        settings.standardFontFamily: "Arial"
        // ...
    }
    \endqml
*/
// QDeclarativeWKSettings* QDeclarativeWKView::settingsObject() const
// {
//     d->settings.s = page()->settings();
//     return &d->settings;
// }

void QDeclarativeWKView::setPage(QWKPage* page)
{
    // FIXME: Unsupported, only doing connections for now.
    Q_ASSERT(!page);
    // if (d->view->page() == page)
    //     return;

    // d->view->setPage(page);
    page = d->view->page();
    connect(page, SIGNAL(urlChanged(QUrl)), this, SLOT(pageUrlChanged()));
    connect(page, SIGNAL(titleChanged(QString)), this, SIGNAL(titleChanged(QString)));
    // connect(page, SIGNAL(titleChanged(QString)), this, SIGNAL(iconChanged()));
    // connect(page->mainFrame(), SIGNAL(iconChanged()), this, SIGNAL(iconChanged()));
    connect(page, SIGNAL(initialLayoutCompleted()), this, SLOT(initialLayout()));

    connect(page, SIGNAL(loadStarted()), this, SLOT(doLoadStarted()));

    connect(page, SIGNAL(loadProgress(int)), this, SLOT(doLoadProgress(int)));
    connect(page, SIGNAL(loadFinished(bool)), this, SLOT(doLoadFinished(bool)));
    connect(page, SIGNAL(statusBarMessage(QString)), this, SLOT(setStatusText(QString)));

    // connect(page->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(windowObjectCleared()));

}

/*!
    \qmlsignal WebView::onLoadStarted()

    This handler is called when the web engine begins loading
    a page. Later, WebView::onLoadFinished() or WebView::onLoadFailed()
    will be emitted.
*/

/*!
    \qmlsignal WebView::onLoadFinished()

    This handler is called when the web engine \e successfully
    finishes loading a page, including any component content
    (WebView::onLoadFailed() will be emitted otherwise).

    \sa progress
*/

/*!
    \qmlsignal WebView::onLoadFailed()

    This handler is called when the web engine fails loading
    a page or any component content
    (WebView::onLoadFinished() will be emitted on success).
*/

// void QDeclarativeWKView::load(const QNetworkRequest& request, QNetworkAccessManager::Operation operation, const QByteArray& body)
// {
//     page()->mainFrame()->load(request, operation, body);
// }

// QString QDeclarativeWKView::html() const
// {
//     return page()->mainFrame()->toHtml();
// }

/*!
    \qmlproperty string WebView::html
    This property holds HTML text set directly

    The html property can be set as a string.

    \qml
    WebView {
        html: "<p>This is <b>HTML</b>."
    }
    \endqml
*/
// void QDeclarativeWKView::setHtml(const QString& html, const QUrl& baseUrl)
// {
//     if (isComponentComplete())
//         page()->mainFrame()->setHtml(html, baseUrl);
//     else {
//         d->pending = d->PendingHtml;
//         d->pendingUrl = baseUrl;
//         d->pendingString = html;
//     }
//     emit htmlChanged();
// }

// void QDeclarativeWKView::setContent(const QByteArray& data, const QString& mimeType, const QUrl& baseUrl)
// {
//     if (isComponentComplete())
//         page()->mainFrame()->setContent(data, mimeType, qmlContext(this)->resolvedUrl(baseUrl));
//     else {
//         d->pending = d->PendingContent;
//         d->pendingUrl = baseUrl;
//         d->pendingString = mimeType;
//         d->pendingData = data;
//     }
// }

QWKHistory* QDeclarativeWKView::history() const
{
    return page()->history();
}

// QWebSettings* QDeclarativeWKView::settings() const
// {
//     return page()->settings();
// }

// FIXME: Use setCreateNewPageFunction
// QDeclarativeWKView* QDeclarativeWKView::createWindow(QWebPage::WebWindowType type)
// {
//     switch (type) {
//     case QWebPage::WebBrowserWindow: {
//         if (!d->newWindowComponent && d->newWindowParent)
//             qWarning("WebView::newWindowComponent not set - WebView::newWindowParent ignored");
//         else if (d->newWindowComponent && !d->newWindowParent)
//             qWarning("WebView::newWindowParent not set - WebView::newWindowComponent ignored");
//         else if (d->newWindowComponent && d->newWindowParent) {
//             QDeclarativeWKView* webview = 0;
//             QDeclarativeContext* windowContext = new QDeclarativeContext(qmlContext(this));

//             QObject* newObject = d->newWindowComponent->create(windowContext);
//             if (newObject) {
//                 windowContext->setParent(newObject);
//                 QDeclarativeItem* item = qobject_cast<QDeclarativeItem *>(newObject);
//                 if (!item)
//                     delete newObject;
//                 else {
//                     webview = item->findChild<QDeclarativeWKView*>();
//                     if (!webview)
//                         delete item;
//                     else {
//                         newObject->setParent(d->newWindowParent);
//                         static_cast<QGraphicsObject*>(item)->setParentItem(d->newWindowParent);
//                     }
//                 }
//             } else
//                 delete windowContext;

//             return webview;
//         }
//     }
//     break;
//     case QWebPage::WebModalDialog: {
//         // Not supported
//     }
//     }
//     return 0;
// }

/*!
    \qmlproperty component WebView::newWindowComponent

    This property holds the component to use for new windows.
    The component must have a WebView somewhere in its structure.

    When the web engine requests a new window, it will be an instance of
    this component.

    The parent of the new window is set by newWindowParent. It must be set.
*/
// QDeclarativeComponent* QDeclarativeWKView::newWindowComponent() const
// {
//     return d->newWindowComponent;
// }

// void QDeclarativeWKView::setNewWindowComponent(QDeclarativeComponent* newWindow)
// {
//     if (newWindow == d->newWindowComponent)
//         return;
//     d->newWindowComponent = newWindow;
//     emit newWindowComponentChanged();
// }


/*!
    \qmlproperty item WebView::newWindowParent

    The parent item for new windows.

    \sa newWindowComponent
*/
// QDeclarativeItem* QDeclarativeWKView::newWindowParent() const
// {
//     return d->newWindowParent;
// }

// void QDeclarativeWKView::setNewWindowParent(QDeclarativeItem* parent)
// {
//     if (parent == d->newWindowParent)
//         return;
//     if (d->newWindowParent && parent) {
//         QList<QGraphicsItem *> children = d->newWindowParent->childItems();
//         for (int i = 0; i < children.count(); ++i)
//             children.at(i)->setParentItem(parent);
//     }
//     d->newWindowParent = parent;
//     emit newWindowParentChanged();
// }

/*!
    Returns the area of the largest element at position (\a x,\a y) that is no larger
    than \a maxWidth by \a maxHeight pixels.

    May return an area larger in the case when no smaller element is at the position.
*/
// QRect QDeclarativeWKView::elementAreaAt(int x, int y, int maxWidth, int maxHeight) const
// {
//     QWebHitTestResult hit = page()->mainFrame()->hitTestContent(QPoint(x, y));
//     QRect hitRect = hit.boundingRect();
//     QWebElement element = hit.enclosingBlockElement();
//     if (maxWidth <= 0)
//         maxWidth = INT_MAX;
//     if (maxHeight <= 0)
//         maxHeight = INT_MAX;
//     while (!element.parent().isNull() && element.geometry().width() <= maxWidth && element.geometry().height() <= maxHeight) {
//         hitRect = element.geometry();
//         element = element.parent();
//     }
//     return hitRect;
// }

// /*!
//     \internal
//     \class QDeclarativeWKPage
//     \brief The QDeclarativeWKPage class is a QWebPage that can create QML plugins.

//     \sa QDeclarativeWKView
// */
// QDeclarativeWKPage::QDeclarativeWKPage(QDeclarativeWKView* parent) :
//     QWebPage(parent)
// {
// }

// QDeclarativeWKPage::~QDeclarativeWKPage()
// {
// }

// QString QDeclarativeWKPage::chooseFile(QWebFrame* originatingFrame, const QString& oldFile)
// {
//     // Not supported (it's modal)
//     Q_UNUSED(originatingFrame)
//     Q_UNUSED(oldFile)
//     return oldFile;
// }

// /*!
//     \qmlsignal WebView::onAlert(string message)

//     The handler is called when the web engine sends a JavaScript alert. The \a message is the text
//     to be displayed in the alert to the user.
// */


// void QDeclarativeWKPage::javaScriptAlert(QWebFrame* originatingFrame, const QString& msg)
// {
//     Q_UNUSED(originatingFrame)
//     emit viewItem()->alert(msg);
// }

// bool QDeclarativeWKPage::javaScriptConfirm(QWebFrame* originatingFrame, const QString& msg)
// {
//     // Not supported (it's modal)
//     Q_UNUSED(originatingFrame)
//     Q_UNUSED(msg)
//     return false;
// }

// bool QDeclarativeWKPage::javaScriptPrompt(QWebFrame* originatingFrame, const QString& msg, const QString& defaultValue, QString* result)
// {
//     // Not supported (it's modal)
//     Q_UNUSED(originatingFrame)
//     Q_UNUSED(msg)
//     Q_UNUSED(defaultValue)
//     Q_UNUSED(result)
//     return false;
// }


// QDeclarativeWKView* QDeclarativeWKPage::viewItem()
// {
//     return static_cast<QDeclarativeWKView*>(parent());
// }

// QWebPage* QDeclarativeWKPage::createWindow(WebWindowType type)
// {
//     QDeclarativeWKView* newView = viewItem()->createWindow(type);
//     if (newView)
//         return newView->page();
//     return 0;
// }

QT_END_NAMESPACE

