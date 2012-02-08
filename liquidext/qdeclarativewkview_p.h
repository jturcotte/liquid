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


#ifndef qdeclarativewkview_p_h
#define qdeclarativewkview_p_h

#include <QtCore/QBasicTimer>
#include <QtCore/QUrl>
#include <QtDeclarative/QDeclarativeItem>
#include <QtGui/QAction>
#include <QtNetwork/QNetworkAccessManager>
#include <qgraphicswkview.h>
#include <qwkpage.h>


QT_BEGIN_HEADER

class QWebHistory;
class QWebSettings;

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QDeclarativeWKSettings;
class QDeclarativeWKViewPrivate;
class QNetworkRequest;
class QDeclarativeWKView;
class QDeclarativeWKViewPrivate;

// class QDeclarativeWKPage : public QWebPage {
//     Q_OBJECT
// public:
//     explicit QDeclarativeWKPage(QDeclarativeWKView *parent);
//     ~QDeclarativeWKPage();
// protected:
//     QWebPage *createWindow(WebWindowType type);
//     QString chooseFile(QWebFrame *originatingFrame, const QString& oldFile);
//     void javaScriptAlert(QWebFrame *originatingFrame, const QString& msg);
//     bool javaScriptConfirm(QWebFrame *originatingFrame, const QString& msg);
//     bool javaScriptPrompt(QWebFrame *originatingFrame, const QString& msg, const QString& defaultValue, QString* result);

// private:
//     QDeclarativeWKView *viewItem();
// };

// class GraphicsWebView : public QGraphicsWebView {
//     Q_OBJECT
// public:
//     GraphicsWebView(QDeclarativeWKView* parent = 0);
// protected:

// Q_SIGNALS:
//     void doubleClick(int clickX, int clickY);
// private:
//     QDeclarativeWKView *parent;
//     friend class QDeclarativeWKView;
// };

class QDeclarativeWKViewAttached;

// TODO: browser plugins

class QDeclarativeWKView : public QDeclarativeItem {
    Q_OBJECT

    Q_ENUMS(Status SelectionMode)

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QPixmap icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

    // Q_PROPERTY(QString html READ html WRITE setHtml NOTIFY htmlChanged)

    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

#ifndef QT_NO_ACTION
    Q_PROPERTY(QAction* reload READ reloadAction CONSTANT)
    Q_PROPERTY(QAction* back READ backAction CONSTANT)
    Q_PROPERTY(QAction* forward READ forwardAction CONSTANT)
    Q_PROPERTY(QAction* stop READ stopAction CONSTANT)
#endif

    // Q_PROPERTY(QDeclarativeWKSettings* settings READ settingsObject CONSTANT)

    // Q_PROPERTY(QDeclarativeListProperty<QObject> javaScriptWindowObjects READ javaScriptWindowObjects CONSTANT)

    // Q_PROPERTY(QDeclarativeComponent* newWindowComponent READ newWindowComponent WRITE setNewWindowComponent NOTIFY newWindowComponentChanged)
    // Q_PROPERTY(QDeclarativeItem* newWindowParent READ newWindowParent WRITE setNewWindowParent NOTIFY newWindowParentChanged)

public:
    QDeclarativeWKView(QWKContext *context = 0, QDeclarativeItem *parent = 0);
    ~QDeclarativeWKView();

    QUrl url() const;
    void setUrl(const QUrl &);

    QString title() const;

    QPixmap icon() const;

    // QRect elementAreaAt(int x, int y, int minwidth, int minheight) const;

    enum Status { Null, Ready, Loading, Error };
    Status status() const;
    qreal progress() const;
    QString statusText() const;

#ifndef QT_NO_ACTION
    QAction *reloadAction() const;
    QAction *backAction() const;
    QAction *forwardAction() const;
    QAction *stopAction() const;
#endif

    QWKPage *page() const;
    void setPage(QWKPage *page);

    // void load(const QNetworkRequest &request,
    //           QNetworkAccessManager::Operation operation = QNetworkAccessManager::GetOperation,
    //           const QByteArray &body = QByteArray());

    // QString html() const;

    // void setHtml(const QString &html, const QUrl &baseUrl = QUrl());
    // void setContent(const QByteArray &data, const QString &mimeType = QString(), const QUrl &baseUrl = QUrl());

    QWKHistory* history() const;
    // QWebSettings* settings() const;
    // QDeclarativeWKSettings *settingsObject() const;

    // QDeclarativeListProperty<QObject> javaScriptWindowObjects();

    static QDeclarativeWKViewAttached* qmlAttachedProperties(QObject*);

    // QDeclarativeComponent *newWindowComponent() const;
    // void setNewWindowComponent(QDeclarativeComponent *newWindow);
    // QDeclarativeItem* newWindowParent() const;
    // void setNewWindowParent(QDeclarativeItem* newWindow);

    bool isComponentCompletePublic() const { return isComponentComplete(); }

Q_SIGNALS:
    void urlChanged();
    void progressChanged();
    void statusChanged(Status);
    void titleChanged(const QString&);
    void iconChanged();
    void statusTextChanged();
    void htmlChanged();
    void newWindowComponentChanged();
    void newWindowParentChanged();
    void loadStarted();
    void loadFinished();
    void loadFailed();

    void doubleClick(int clickX, int clickY);

    void zoomTo(qreal zoom, int centerX, int centerY);

    void alert(const QString& message);

public Q_SLOTS:
    // QVariant evaluateJavaScript(const QString&);

private Q_SLOTS:
    void doLoadStarted();
    void doLoadProgress(int p);
    void doLoadFinished(bool ok);
    void setStatusText(const QString&);
    void windowObjectCleared();
    void pageUrlChanged();
    void doPageIconChanged(const QUrl&);
    void initialLayout();

    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);
    // QDeclarativeWKView* createWindow(QWebPage::WebWindowType type);

private:
    void init(QWKContext *context);
    virtual void componentComplete();
    Q_DISABLE_COPY(QDeclarativeWKView)
    QDeclarativeWKViewPrivate* d;
    QMouseEvent* sceneMouseEventToMouseEvent(QGraphicsSceneMouseEvent*);
    QMouseEvent* sceneHoverMoveEventToMouseEvent(QGraphicsSceneHoverEvent*);
    friend class QDeclarativeWKPage;
};

class QDeclarativeWKViewAttached : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString windowObjectName READ windowObjectName WRITE setWindowObjectName)
public:
    QDeclarativeWKViewAttached(QObject* parent)
        : QObject(parent)
    {
    }

    QString windowObjectName() const
    {
        return m_windowObjectName;
    }

    void setWindowObjectName(const QString &n)
    {
        m_windowObjectName = n;
    }

private:
    QString m_windowObjectName;
};

class QDeclarativeWKSettings : public QObject {
    Q_OBJECT

    // Q_PROPERTY(QString standardFontFamily READ standardFontFamily WRITE setStandardFontFamily)
    // Q_PROPERTY(QString fixedFontFamily READ fixedFontFamily WRITE setFixedFontFamily)
    // Q_PROPERTY(QString serifFontFamily READ serifFontFamily WRITE setSerifFontFamily)
    // Q_PROPERTY(QString sansSerifFontFamily READ sansSerifFontFamily WRITE setSansSerifFontFamily)
    // Q_PROPERTY(QString cursiveFontFamily READ cursiveFontFamily WRITE setCursiveFontFamily)
    // Q_PROPERTY(QString fantasyFontFamily READ fantasyFontFamily WRITE setFantasyFontFamily)

    // Q_PROPERTY(int minimumFontSize READ minimumFontSize WRITE setMinimumFontSize)
    // Q_PROPERTY(int minimumLogicalFontSize READ minimumLogicalFontSize WRITE setMinimumLogicalFontSize)
    // Q_PROPERTY(int defaultFontSize READ defaultFontSize WRITE setDefaultFontSize)
    // Q_PROPERTY(int defaultFixedFontSize READ defaultFixedFontSize WRITE setDefaultFixedFontSize)

    // Q_PROPERTY(bool autoLoadImages READ autoLoadImages WRITE setAutoLoadImages)
    // Q_PROPERTY(bool javascriptEnabled READ javascriptEnabled WRITE setJavascriptEnabled)
    // Q_PROPERTY(bool javaEnabled READ javaEnabled WRITE setJavaEnabled)
    // Q_PROPERTY(bool pluginsEnabled READ pluginsEnabled WRITE setPluginsEnabled)
    // Q_PROPERTY(bool privateBrowsingEnabled READ privateBrowsingEnabled WRITE setPrivateBrowsingEnabled)
    // Q_PROPERTY(bool javascriptCanOpenWindows READ javascriptCanOpenWindows WRITE setJavascriptCanOpenWindows)
    // Q_PROPERTY(bool javascriptCanAccessClipboard READ javascriptCanAccessClipboard WRITE setJavascriptCanAccessClipboard)
    // Q_PROPERTY(bool developerExtrasEnabled READ developerExtrasEnabled WRITE setDeveloperExtrasEnabled)
    // Q_PROPERTY(bool linksIncludedInFocusChain READ linksIncludedInFocusChain WRITE setLinksIncludedInFocusChain)
    // Q_PROPERTY(bool zoomTextOnly READ zoomTextOnly WRITE setZoomTextOnly)
    // Q_PROPERTY(bool printElementBackgrounds READ printElementBackgrounds WRITE setPrintElementBackgrounds)
    // Q_PROPERTY(bool offlineStorageDatabaseEnabled READ offlineStorageDatabaseEnabled WRITE setOfflineStorageDatabaseEnabled)
    // Q_PROPERTY(bool offlineWebApplicationCacheEnabled READ offlineWebApplicationCacheEnabled WRITE setOfflineWebApplicationCacheEnabled)
    // Q_PROPERTY(bool localStorageDatabaseEnabled READ localStorageDatabaseEnabled WRITE setLocalStorageDatabaseEnabled)
    // Q_PROPERTY(bool localContentCanAccessRemoteUrls READ localContentCanAccessRemoteUrls WRITE setLocalContentCanAccessRemoteUrls)

public:
    QDeclarativeWKSettings() {}

    // QString standardFontFamily() const { return s->fontFamily(QWebSettings::StandardFont); }
    // void setStandardFontFamily(const QString& f) { s->setFontFamily(QWebSettings::StandardFont, f); }
    // QString fixedFontFamily() const { return s->fontFamily(QWebSettings::FixedFont); }
    // void setFixedFontFamily(const QString& f) { s->setFontFamily(QWebSettings::FixedFont, f); }
    // QString serifFontFamily() const { return s->fontFamily(QWebSettings::SerifFont); }
    // void setSerifFontFamily(const QString& f) { s->setFontFamily(QWebSettings::SerifFont, f); }
    // QString sansSerifFontFamily() const { return s->fontFamily(QWebSettings::SansSerifFont); }
    // void setSansSerifFontFamily(const QString& f) { s->setFontFamily(QWebSettings::SansSerifFont, f); }
    // QString cursiveFontFamily() const { return s->fontFamily(QWebSettings::CursiveFont); }
    // void setCursiveFontFamily(const QString& f) { s->setFontFamily(QWebSettings::CursiveFont, f); }
    // QString fantasyFontFamily() const { return s->fontFamily(QWebSettings::FantasyFont); }
    // void setFantasyFontFamily(const QString& f) { s->setFontFamily(QWebSettings::FantasyFont, f); }

    // int minimumFontSize() const { return s->fontSize(QWebSettings::MinimumFontSize); }
    // void setMinimumFontSize(int size) { s->setFontSize(QWebSettings::MinimumFontSize, size); }
    // int minimumLogicalFontSize() const { return s->fontSize(QWebSettings::MinimumLogicalFontSize); }
    // void setMinimumLogicalFontSize(int size) { s->setFontSize(QWebSettings::MinimumLogicalFontSize, size); }
    // int defaultFontSize() const { return s->fontSize(QWebSettings::DefaultFontSize); }
    // void setDefaultFontSize(int size) { s->setFontSize(QWebSettings::DefaultFontSize, size); }
    // int defaultFixedFontSize() const { return s->fontSize(QWebSettings::DefaultFixedFontSize); }
    // void setDefaultFixedFontSize(int size) { s->setFontSize(QWebSettings::DefaultFixedFontSize, size); }

    // bool autoLoadImages() const { return s->testAttribute(QWebSettings::AutoLoadImages); }
    // void setAutoLoadImages(bool on) { s->setAttribute(QWebSettings::AutoLoadImages, on); }
    // bool javascriptEnabled() const { return s->testAttribute(QWebSettings::JavascriptEnabled); }
    // void setJavascriptEnabled(bool on) { s->setAttribute(QWebSettings::JavascriptEnabled, on); }
    // bool javaEnabled() const { return s->testAttribute(QWebSettings::JavaEnabled); }
    // void setJavaEnabled(bool on) { s->setAttribute(QWebSettings::JavaEnabled, on); }
    // bool pluginsEnabled() const { return s->testAttribute(QWebSettings::PluginsEnabled); }
    // void setPluginsEnabled(bool on) { s->setAttribute(QWebSettings::PluginsEnabled, on); }
    // bool privateBrowsingEnabled() const { return s->testAttribute(QWebSettings::PrivateBrowsingEnabled); }
    // void setPrivateBrowsingEnabled(bool on) { s->setAttribute(QWebSettings::PrivateBrowsingEnabled, on); }
    // bool javascriptCanOpenWindows() const { return s->testAttribute(QWebSettings::JavascriptCanOpenWindows); }
    // void setJavascriptCanOpenWindows(bool on) { s->setAttribute(QWebSettings::JavascriptCanOpenWindows, on); }
    // bool javascriptCanAccessClipboard() const { return s->testAttribute(QWebSettings::JavascriptCanAccessClipboard); }
    // void setJavascriptCanAccessClipboard(bool on) { s->setAttribute(QWebSettings::JavascriptCanAccessClipboard, on); }
    // bool developerExtrasEnabled() const { return s->testAttribute(QWebSettings::DeveloperExtrasEnabled); }
    // void setDeveloperExtrasEnabled(bool on) { s->setAttribute(QWebSettings::DeveloperExtrasEnabled, on); }
    // bool linksIncludedInFocusChain() const { return s->testAttribute(QWebSettings::LinksIncludedInFocusChain); }
    // void setLinksIncludedInFocusChain(bool on) { s->setAttribute(QWebSettings::LinksIncludedInFocusChain, on); }
    // bool zoomTextOnly() const { return s->testAttribute(QWebSettings::ZoomTextOnly); }
    // void setZoomTextOnly(bool on) { s->setAttribute(QWebSettings::ZoomTextOnly, on); }
    // bool printElementBackgrounds() const { return s->testAttribute(QWebSettings::PrintElementBackgrounds); }
    // void setPrintElementBackgrounds(bool on) { s->setAttribute(QWebSettings::PrintElementBackgrounds, on); }
    // bool offlineStorageDatabaseEnabled() const { return s->testAttribute(QWebSettings::OfflineStorageDatabaseEnabled); }
    // void setOfflineStorageDatabaseEnabled(bool on) { s->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, on); }
    // bool offlineWebApplicationCacheEnabled() const { return s->testAttribute(QWebSettings::OfflineWebApplicationCacheEnabled); }
    // void setOfflineWebApplicationCacheEnabled(bool on) { s->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, on); }
    // bool localStorageDatabaseEnabled() const { return s->testAttribute(QWebSettings::LocalStorageDatabaseEnabled); }
    // void setLocalStorageDatabaseEnabled(bool on) { s->setAttribute(QWebSettings::LocalStorageDatabaseEnabled, on); }
    // bool localContentCanAccessRemoteUrls() const { return s->testAttribute(QWebSettings::LocalContentCanAccessRemoteUrls); }
    // void setLocalContentCanAccessRemoteUrls(bool on) { s->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, on); }

    // QWebSettings *s;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeWKView)
QML_DECLARE_TYPE(QDeclarativeWKSettings)
QML_DECLARE_TYPEINFO(QDeclarativeWKView, QML_HAS_ATTACHED_PROPERTIES)

QT_END_HEADER

#endif
