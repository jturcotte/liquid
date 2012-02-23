var _controlShiftShortcuts = new Object
var _metaShiftShortcuts = new Object
var _controlShortcuts = new Object
var _metaShortcuts = new Object
var _altShortcuts = new Object
var _shiftShortcuts = new Object
var _bareShortcuts = new Object

// Mac specific tweaks for Ctrl and Cmd.
var _controlForTabSwitching = backend.isMac ? _metaShortcuts : _controlShortcuts
var _controlShiftForTabSwitching = backend.isMac ? _metaShiftShortcuts : _controlShiftShortcuts
var _altForHistoryNav = backend.isMac ? _metaShortcuts : _altShortcuts

_controlShiftShortcuts[Qt.Key_BracketLeft] = "ShowPreviousTab"
_controlShiftShortcuts[Qt.Key_BracketRight] = "ShowNextTab"
_controlShiftShortcuts[Qt.Key_R] = "ReloadFull"
_controlShiftShortcuts[Qt.Key_G] = "FindPreviousInPage"
_controlShiftForTabSwitching[Qt.Key_Tab] = "ShowPreviousTab"

_controlShortcuts[Qt.Key_T] = "OpenNewTab"
_controlShortcuts[Qt.Key_K] = "FocusLocationBar_SearchMode"
_controlShortcuts[Qt.Key_L] = "FocusLocationBar_HistoryMode"
_controlShortcuts[Qt.Key_F] = "FocusSearchInPage"
_controlShortcuts[Qt.Key_G] = "FindNextInPage"
_controlShortcuts[Qt.Key_W] = "CloseTab"
_controlShortcuts[Qt.Key_BracketLeft] = "GoBack"
_controlShortcuts[Qt.Key_BracketRight] = "GoForward"
if (!backend.isMac) {
    // Disable for now since we shouldn't go back with those keys unless the webpage isn't handling them
    // in a text field. Have to figure out how to do this.
    _controlShortcuts[Qt.Key_Left] = "GoBack"
    _controlShortcuts[Qt.Key_Right] = "GoForward"
}
_controlShortcuts[Qt.Key_R] = "Reload"
_controlShortcuts[Qt.Key_F5] = "ReloadFull"
// Doesn't work on OSX yet (same for Ctrl-Shift-Tab) because of https://bugreports.qt-project.org/browse/QTBUG-12232
_controlForTabSwitching[Qt.Key_Tab] = "ShowNextTab"
_controlForTabSwitching[Qt.Key_PageUp] = "ShowPreviousTab"
_controlForTabSwitching[Qt.Key_PageDown] = "ShowNextTab"

if (!backend.isMac) {
    // See comment above.
    _altForHistoryNav[Qt.Key_Left] = "GoBack"
    _altForHistoryNav[Qt.Key_Right] = "GoForward"
}

_shiftShortcuts[Qt.Key_F3] = "FindPreviousInPage"

_bareShortcuts[Qt.Key_Esc] = "Stop"
_bareShortcuts[Qt.Key_F3] = "FindNextInPage"
_bareShortcuts[Qt.Key_F5] = "Reload"

function actionForEvent(key, modifiers) {
    if (modifiers & Qt.ControlModifier && modifiers & Qt.ShiftModifier)
        return _controlShiftShortcuts[key]
    else if (modifiers & Qt.MetaModifier && modifiers & Qt.ShiftModifier)
        return _metaShiftShortcuts[key]
    else if (modifiers & Qt.ControlModifier)
        return _controlShortcuts[key]
    else if (modifiers & Qt.MetaModifier)
        return _metaShortcuts[key]
    else if (modifiers & Qt.AltModifier)
        return _altShortcuts[key]
    else if (modifiers & Qt.ShiftModifier)
        return _shiftShortcuts[key]
    else
        return _bareShortcuts[key]
}

var _handlers = new Object
function setHandler(actionName, handler) {
    _handlers[actionName] = handler
}

function handleShortcut(key, modifiers) {
    var actionName = actionForEvent(key, modifiers)
    var handler = _handlers[actionName]
    if (handler)
        handler()
    else if (actionName)
        console.log("ShortcutHandler: No registered handler for action [" + actionName + "]")
    else
        return false
    return true
}
