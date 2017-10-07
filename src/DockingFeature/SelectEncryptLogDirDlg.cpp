
#include "SelectEncryptLogDirDlg.h"
#include "../PluginDefinition.h"

extern NppData nppData;

INT_PTR CALLBACK SelectEncryptLogDirDlg::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) 
    {
    case WM_COMMAND : 
        {
            switch (wParam)
            {
            case IDOK :
                {
                    int line = getLine();
                    if (line != -1)
                    {
                        // Get the current scintilla
                        int which = -1;
                        ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
                        if (which == -1)
                            return FALSE;
                        HWND curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;

                        ::SendMessage(curScintilla, SCI_ENSUREVISIBLE, line-1, 0);
                        ::SendMessage(curScintilla, SCI_GOTOLINE, line-1, 0);
                    }
                    return TRUE;
                }
            }
            return FALSE;
        }

    default :
        return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
    }
}

