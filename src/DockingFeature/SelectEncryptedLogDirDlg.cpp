
#include "SelectEncryptedLogDirDlg.h"
#include "../PluginDefinition.h"
#include <shlobj.h>
#include "../menuCmdID.h"

#define MAXREAD     8192
#define DIRATTR     (DDL_READWRITE | DDL_READONLY | DDL_DIRECTORY | DDL_ARCHIVE)

#define DTFLAGS     (DT_WORDBREAK | DT_EXPANDTABS | DT_NOCLIP | DT_NOPREFIX)
extern NppData nppData;

static const WCHAR* DECRYPTED_LOG_APPENDIX = L".decrypted";

INT_PTR CALLBACK SelectEncryptedLogDirDlg::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) 
    {
    case WM_INITDIALOG:
        {
            onInitDialg(wParam, lParam);
            return 0;
        }
        break;
    case WM_COMMAND : 
        {
            //////////////////////////////////////////////////////////////////////////            
            if (IDOK == wParam)
            {
                onOKButtonPushed(wParam, lParam);
                return 0;
            }

            //////////////////////////////////////////////////////////////////////////
            if (LOWORD (wParam) == IDC_LIST_LOG_FILES)
            {
                int d = HIWORD (wParam);
                WCHAR text[128] = {0};
                swprintf_s(text, _countof(text), L"########MSG TYPE: %d\n", d);
                OutputDebugStringW(text);
            }
            if (LOWORD (wParam) == IDC_LIST_LOG_FILES && HIWORD (wParam) == LBN_DBLCLK)
            {
                onListboxLButtonDoubleClicked(wParam, lParam);
                return 0 ;
            }
            if (LOWORD (wParam) == IDC_LIST_LOG_FILES && HIWORD (wParam) == LBN_SELCHANGE)
            {
                ::EnableWindow(GetDlgItem(_hSelf, ID_REDECRYPT_FILE), FALSE);
                return 0;
            }
                
            //////////////////////////////////////////////////////////////////////////
            if (ID_REDECRYPT_FILE == wParam)
            {
                onRedecryptFileButtonPushed(wParam, lParam);
                return 0;
            }
        }
        break;

    default :
        return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
    }
}

void SelectEncryptedLogDirDlg::onInitDialg(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    HWND hwndList = ::GetDlgItem(_hSelf, IDC_LIST_LOG_FILES);
    if (NULL == hwndList)
    {
        int error = GetLastError();
        WCHAR text[128] = {0};
        swprintf_s(text, _countof(text), L"GetDlgItem IDC_LIST_LOG_FILES handle failed, error:%d", error);
        ::MessageBox(_hSelf, text, L"ERROR", MB_OK);
    }

    //////////////////////////////////////////////////////////////////////////
    ::EnableWindow(GetDlgItem(_hSelf, ID_REDECRYPT_FILE), FALSE);
}

void SelectEncryptedLogDirDlg::onOKButtonPushed(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    WCHAR folderPath[512] = {0};

    if (selectLogDir(folderPath, _countof(folderPath)))
    {
        ::SetDlgItemText(_hSelf, ID_EDIT_ENCRYPTED_LOG_DIR, folderPath);
        fillFilesInTree(folderPath);
    }
    else
    {
        lstrcpy(folderPath, L"please select a valid folder");
        ::SetDlgItemText(_hSelf, ID_EDIT_ENCRYPTED_LOG_DIR, folderPath);
    }
}

void SelectEncryptedLogDirDlg::onListboxLButtonDoubleClicked(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    static BOOL     bValidFile ;
    static TCHAR    decryptedFileName[MAX_PATH + 1] = {0} ;

    TCHAR           szBuffer[MAX_PATH + 1] = {0};
    TCHAR           folderPath[MAX_PATH + 1] = {0};
    TCHAR           filePath[MAX_PATH + 1] = {0};
    int             i=0;
    HANDLE          hFile = NULL;
    HWND            hwndList = NULL; 

    hwndList = ::GetDlgItem(_hSelf, IDC_LIST_LOG_FILES);
    if (LB_ERR == (i = SendMessage (hwndList, LB_GETCURSEL, 0, 0)))
    {
        int error = GetLastError();
        WCHAR text[128] = {0};
        swprintf_s(text, _countof(text), L"onListboxLButtonDoubleClicked LB_GETCURSEL failed, error:%d", error);
        ::MessageBox(_hSelf, text, L"ERROR", MB_OK);
        return ;
    }

    // current select item text
    SendMessage (hwndList, LB_GETTEXT, i, (LPARAM) szBuffer) ;

    // current folder path
    GetCurrentDirectory(_countof(folderPath),folderPath);

    // file path test
    lstrcpy (filePath, folderPath) ;
    if (filePath[lstrlen(filePath)-1] != L'\\')
    {
        lstrcat(filePath, L"\\");
    }
    lstrcat (filePath, szBuffer) ;

    if (INVALID_HANDLE_VALUE != (hFile = CreateFile (filePath, 
        GENERIC_READ, FILE_SHARE_READ, NULL, 
        OPEN_EXISTING, 0, NULL)))

    {
        CloseHandle (hFile) ;
        bValidFile = TRUE ;
        ::EnableWindow(GetDlgItem(_hSelf, ID_REDECRYPT_FILE), TRUE);

        if (isDecryptLog(decryptedFileName))
        {
            ::DeleteFile(decryptedFileName);
        }
        if (generateDecryptLogName(filePath, decryptedFileName, _countof(decryptedFileName)))
        {
            decryptLog(filePath, decryptedFileName, _countof(decryptedFileName));
            displayFileContent(decryptedFileName);
        }
        else
        {
            WCHAR text[128] = {0};
            swprintf_s(text, _countof(text), L"generateDecryptLogName failed");
            ::MessageBox(_hSelf, text, L"ERROR", MB_OK);
        }
    }
    else
    {
        bValidFile = FALSE ;
        ::EnableWindow(GetDlgItem(_hSelf, ID_REDECRYPT_FILE), FALSE);

        szBuffer [lstrlen (szBuffer) - 1] = '\\' ;
        szBuffer [0] = L'\\';

        lstrcat(folderPath, szBuffer);
        ::SetDlgItemText(_hSelf, ID_EDIT_ENCRYPTED_LOG_DIR, folderPath);
        SetCurrentDirectory(folderPath);

        lstrcat(folderPath, L"*");
        SendMessage (hwndList, LB_RESETCONTENT, 0, 0) ;
        SendMessage (hwndList, LB_DIR, DIRATTR, (LPARAM)folderPath) ;
    }
}

void SelectEncryptedLogDirDlg::onRedecryptFileButtonPushed(WPARAM wParam, LPARAM lParam)
{
    ::SendMessage(nppData._nppHandle, WM_COMMAND, IDM_FILE_CLOSE, 0);
    onListboxLButtonDoubleClicked(wParam,lParam);
}


bool SelectEncryptedLogDirDlg::isDecryptLog(const WCHAR* filePath)
{
    bool ret = false;
    int count = 0;
    int length = lstrlen(filePath);
    for (int i=0; i<length; i++)
    {
        for (int j=0; j<length-i; j++)
        {
            if (filePath[i+j] != DECRYPTED_LOG_APPENDIX[j])
            {
                count = 0;
                break;
            }
            else
            {
                count++;
            }
        }
        if (count == lstrlen(DECRYPTED_LOG_APPENDIX))
        {
            ret = true;
            break;
        }
    }
    return ret;
}

bool SelectEncryptedLogDirDlg::generateDecryptLogName(const WCHAR* fileNamePath, WCHAR* decryptedFileName, int /*length*/)
{
    TCHAR drive[MAX_PATH] = L"";
    TCHAR dir[MAX_PATH] = L"";
    TCHAR filePath[MAX_PATH] = L"";
    TCHAR ext[MAX_PATH] = L"";
    _wsplitpath(fileNamePath, drive, dir, filePath, ext);

    TCHAR strPath[MAX_PATH] = {0};  
    GetTempPath(MAX_PATH, strPath);  

    lstrcpy(decryptedFileName, strPath);
    lstrcat(decryptedFileName, L"\\");
    lstrcat(decryptedFileName, filePath);
    lstrcat(decryptedFileName, ext);
    lstrcat(decryptedFileName, DECRYPTED_LOG_APPENDIX);

    return true;
}

bool SelectEncryptedLogDirDlg::decryptLog(const WCHAR* encryptedFileName, WCHAR* decryptedFileName, int length)
{
    return true;
}


bool SelectEncryptedLogDirDlg::displayFileContent(const WCHAR* filePath)
{
    HANDLE hFile = NULL;
    if (INVALID_HANDLE_VALUE == (hFile = CreateFile (filePath, 
        GENERIC_READ, FILE_SHARE_READ, NULL, 
        OPEN_ALWAYS, 0, NULL)))
    {
        int error = GetLastError();
        WCHAR text[128] = {0};
        swprintf_s(text, _countof(text), L"displayFileContent failed, error: %d", error);
        ::MessageBox(_hSelf, text, L"ERROR", MB_OK);
    }
    LRESULT ret = ::SendMessage(nppData._nppHandle, NPPM_DOOPEN , 0, (LPARAM)filePath);
    if (0 != ret)
    {
        // SUCCESS;
    }
    else
    {
        //WCHAR text[128] = {0};
        //swprintf_s(text, _countof(text), L"displayFileContent failed");
        //::MessageBox(_hSelf, text, L"ERROR", MB_OK);
    }
    return (0 != ret);
}

bool SelectEncryptedLogDirDlg::selectLogDir(WCHAR* folderPath, int /*length*/)
{
    TCHAR szBuffer[MAX_PATH] = {0};   
    BROWSEINFO bi;   
    ZeroMemory(&bi,sizeof(BROWSEINFO));   
    bi.hwndOwner = _hSelf;   
    bi.pszDisplayName = szBuffer;   
    bi.lpszTitle = _T("select encryted log directory:");   
    bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_EDITBOX | BIF_VALIDATE;  

    LPITEMIDLIST idl = SHBrowseForFolder(&bi);   
    if (NULL == idl)   
    {   
        return false;   
    }   

    SHGetPathFromIDList(idl,szBuffer);
    lstrcpy(folderPath, szBuffer);
    return true;
}

bool SelectEncryptedLogDirDlg::fillFilesInTree(const WCHAR* folderPath)
{
    SetCurrentDirectory(folderPath);


    HWND hwndList = ::GetDlgItem(_hSelf, IDC_LIST_LOG_FILES);
    WCHAR text[MAX_PATH] = {0};
    lstrcpy(text, folderPath);
    if (text[lstrlen(text)-1] == L'\\')
    {
        lstrcat(text, L"*");
    }
    else
    {
        lstrcat(text, L"\\*");
    }   
    ::SendMessage (hwndList, LB_RESETCONTENT, 0, 0) ;
    ::SendMessage (hwndList, LB_DIR, DIRATTR, (LPARAM)text) ;
    return true;
}

void SelectEncryptedLogDirDlg::setLastLogDir(const WCHAR* logDir)
{
    if (lstrlen(logDir) > 0)
    {
        ::SetDlgItemText(_hSelf, ID_EDIT_ENCRYPTED_LOG_DIR, logDir);
        fillFilesInTree(logDir);
    }
}