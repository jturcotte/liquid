function actionForEvent(key, modifiers) {
    if (modifiers & Qt.ControlModifier && modifiers & Qt.ShiftModifier) {
        switch (key) {
        case Qt.Key_BracketLeft:
            return "ShowPreviousTab";
        case Qt.Key_BracketRight:
            return "ShowNextTab";
        case Qt.Key_R:
            return "ReloadFull";
        case Qt.Key_G:
            return "FindPreviousInPage";
        }
    } else if (modifiers & Qt.MetaModifier && modifiers & Qt.ShiftModifier) {
        switch (key) {
        case Qt.Key_Tab:
            return "ShowPreviousTab";
        }
    } else if (modifiers & Qt.ControlModifier) {
        switch (key) {
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
    } else if (modifiers & Qt.MetaModifier) {
        switch (key) {
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
    } else if (modifiers & Qt.ShiftModifier) {
        switch (key) {
        case Qt.Key_F3:
            return "FindPreviousInPage";
        }
    } else {
        switch (key) {
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

function handleShortcut(key, modifiers) {
    var actionName = actionForEvent(key, modifiers);
    var handler = _handlers[actionName];
    if (handler)
        handler();
    else if (actionName)
        console.log("ShortcutHandler: No registered handler for action [" + actionName + "]");
    else
        return false;
    return true;
}
