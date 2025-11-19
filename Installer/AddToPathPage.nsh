!macro AddToPathPage

;---------------------------
; Include richiesto
;---------------------------
!include "EnvVarUpdate.nsh"
!include "nsDialogs.nsh"
!include "LogicLib.nsh"
!include "WinMessages.nsh"

;---------------------------
; Pagina personalizzata
;---------------------------
Page custom ShowAddToPathPage LeaveAddToPathPage

Var CHECKBOX

Function ShowAddToPathPage
    !insertmacro MUI_HEADER_TEXT "Add to PATH" "Configure environment variables"

    nsDialogs::Create 1018
    Pop $0
    ${If} $0 == error
        Abort
    ${EndIf}

    ${NSD_CreateLabel} 0 0 100% 30u \
        "Would you like to add '$INSTDIR\bin' to the system PATH?"
    Pop $1

    ${NSD_CreateCheckbox} 0 35u 100% 12u "Add to PATH"
    Pop $CHECKBOX
    ${NSD_SetState} $CHECKBOX ${BST_CHECKED}

    nsDialogs::Show
FunctionEnd

Function LeaveAddToPathPage
    ${NSD_GetState} $CHECKBOX $0
    IntCmp $0 1 DoUpdate SkipUpdate

DoUpdate:
    ; Add to system PATH safely (handles REG_EXPAND_SZ, duplicates, and length)
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\bin"

    ; Notify the system of environment change
    System::Call 'user32::SendMessageTimeoutW(i 0xffff, i ${WM_SETTINGCHANGE}, i 0, w "Environment", i 0, i 5000, *i .r0)'

SkipUpdate:
FunctionEnd

!macroend
