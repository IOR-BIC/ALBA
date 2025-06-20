!macro AddToPathPage

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
        "Would you like to add '${PRODUCT_INSTALLER_DIR_WIN}\bin' to the system PATH?"
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
    ReadRegStr $1 HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "Path"
    StrCpy $2 "${PRODUCT_INSTALLER_DIR_WIN}\bin"

    ; Check if already present
    Push $1
    Push $2
    Call SubStrFound
    Pop $3
    StrCmp $3 "found" SkipUpdate

    StrCpy $1 "$1;$2"
    WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "Path" "$1"

    System::Call 'user32::SendMessageTimeoutW(i 0xffff, i ${WM_SETTINGCHANGE}, i 0, w "Environment", i 0, i 5000, *i .r0)'

SkipUpdate:
FunctionEnd

; Simple substring search function (minimal)
Function SubStrFound
    Exch $R1 ; needle
    Exch
    Exch $R0 ; haystack

    Push $R2
    Push $R3

    StrLen $R2 $R1
    StrCpy $R3 0

Loop:
    StrCpy $0 $R0 $R2 $R3
    StrCmp $0 $R1 Found
    IntOp $R3 $R3 + 1
    StrLen $R4 $R0
    IntCmp $R3 $R4 0 Loop
    StrCpy $0 ""
    Goto Done

Found:
    StrCpy $0 "found"

Done:
    Pop $R3
    Pop $R2
    Exch $0
FunctionEnd

!macroend
