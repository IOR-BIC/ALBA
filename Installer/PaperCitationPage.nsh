!macro PaperCitationPage

Page Custom PaperCitationPageCreate PaperCitationPageLeave

Function PaperCitationPageCreate
    !insertmacro MUI_HEADER_TEXT "Scientific citation" \
        "How to cite this software in scientific publications"

    nsDialogs::Create 1018
    Pop $0
    ${If} $0 == error
        Abort
    ${EndIf}

    ; Info text
    ${NSD_CreateLabel} 0 0 100% 50u \
    "This software is free to use.$\r$\n$\r$\n\
If you use it in scientific research, citing the related paper \
would be greatly appreciated. This is not mandatory, \
but it helps supporting the project."

    Pop $1

    ; Clickable link
    ${NSD_CreateLink} 0 55u 100% 12u "${PAPER_LINK}"
    Pop $2
    ${NSD_OnClick} $2 OpenPaperLink

    nsDialogs::Show
FunctionEnd


Function OpenPaperLink
    ExecShell "open" "${PAPER_LINK}"
FunctionEnd


Function PaperCitationPageLeave
FunctionEnd

!macroend
