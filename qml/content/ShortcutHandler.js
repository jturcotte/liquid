/*
Options:
- Import JS file with global variable in which items register themselves on load and on change.
- main.qml passes around the shortcut handler and they register themselves.
- a set of qactions is set somewhere and the shortcut handler uses it (qaction isn't reliable for qt5 though).
- a map of action->function is set somewhere and the shortcut handler uses it.
*/
function actionForEvent(event) {
    if (event.modifiers & Qt.ControlModifier && event.modifiers & Qt.ShiftModifier) {
        switch (event.key) {
        case Qt.Key_BracketLeft:
            return "ShowPreviousTab";
        case Qt.Key_BracketRight:
            return "ShowNextTab";
        case Qt.Key_R:
            return "ReloadFull";
        case Qt.Key_G:
            return "FindPreviousInPage";
        }
    } else if (event.modifiers & Qt.MetaModifier && event.modifiers & Qt.ShiftModifier) {
        switch (event.key) {
        case Qt.Key_Tab:
            return "ShowPreviousTab";
        }
    } else if (event.modifiers & Qt.ControlModifier) {
        switch (event.key) {
        case Qt.Key_T:
            return "OpenNewTab"
        case Qt.Key_K:
            return "FocusLocationBar_SearchMode";
        case Qt.Key_L:
            return "FocusLocationBar_HistoryMode";
        case Qt.Key_F:
            return "FocusSearchInPage";
        case Qt.Key_G:
            return "FindNextInPage";
        case Qt.Key_W:
            return "CloseTab";
        case Qt.Key_BracketLeft:
        case Qt.Key_Left:
            return "GoBack";
        case Qt.Key_BracketRight:
        case Qt.Key_Right:
            return "GoForward";
        case Qt.Key_R:
            return "Reload";
        case Qt.Key_F5:
            return "ReloadFull";
        }
    } else if (event.modifiers & Qt.MetaModifier) {
        switch (event.key) {
        case Qt.Key_Tab:
            return "ShowNextTab";
        case Qt.Key_PageUp:
            return "ShowNextTab";
        case Qt.Key_PageDown:
            return "ShowPreviousTab";
        case Qt.Key_Left:
            return "GoBack";
        case Qt.Key_Right:
            return "GoForward";
        }
    } else if (event.modifiers & Qt.ShiftModifier) {
        switch (event.key) {
        case Qt.Key_F3:
            return "FindPreviousInPage";
        }
    } else {
        switch (event.key) {
        case Qt.Key_Esc:
            return "Stop";
        case Qt.Key_F3:
            return "FindNextInPage";
        case Qt.Key_F5:
            return "Reload";
        }
    }
    return null;
}

var _handlers = new Object;
function setHandler(actionName, handler) {
    _handlers[actionName] = handler;
}

function handleEvent(event) {
    var actionName = actionForEvent(event);
    var handler = _handlers[actionName];
    if (handler)
        handler();
    else if (actionName)
        console.log("ShortcutHandler: No registered handler for action [" + actionName + "]");
}
