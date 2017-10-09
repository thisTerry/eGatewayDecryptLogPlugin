#ifndef SELECT_ENCRYPTLOGDIR_DLG_H
#define SELECT_ENCRYPTLOGDIR_DLG_H

#include "DockingDlgInterface.h"
#include "resource.h"
#include <list>

struct LogPathPair
{
    WCHAR* encryptedFilePath;
    WCHAR* decryptedFilePath;
    

    LogPathPair()
        : encryptedFilePath(NULL)
        , decryptedFilePath(NULL)
    {
    }
    LogPathPair(const LogPathPair& rhs)
    {
        int lengthEncrypted = lstrlen(rhs.encryptedFilePath);
        int lengthDecrypted = lstrlen(rhs.decryptedFilePath);
        encryptedFilePath = new WCHAR[lengthEncrypted + 1];
        decryptedFilePath = new WCHAR[lengthDecrypted + 1];
        wmemset(encryptedFilePath, 0, lengthEncrypted+1);
        wmemset(decryptedFilePath, 0, lengthDecrypted+1);
        lstrcpy(encryptedFilePath, rhs.encryptedFilePath);
        lstrcpy(decryptedFilePath, rhs.decryptedFilePath);
    }
    ~LogPathPair()
    {
        if (NULL != encryptedFilePath)
        {
            delete[] encryptedFilePath;
        }
        if (NULL != decryptedFilePath)
        {
            delete[] decryptedFilePath;
        }
    }
    void SetPathPair(const WCHAR* encrypted, const WCHAR* decrypted )
    {
        int lengthEncrypted = lstrlen(encrypted);
        int lengthDecrypted = lstrlen(decrypted);
        encryptedFilePath = new WCHAR[lengthEncrypted + 1];
        decryptedFilePath = new WCHAR[lengthDecrypted + 1];
        wmemset(encryptedFilePath, 0, lengthEncrypted+1);
        wmemset(decryptedFilePath, 0, lengthDecrypted+1);
        lstrcpy(encryptedFilePath, encrypted);
        lstrcpy(decryptedFilePath, decrypted);
    }
};
/*! 
You have to create your dialog by inherented DockingDlgInterface class 
in order to make your dialog dockable
*/
class SelectEncryptedLogDirDlg : public DockingDlgInterface
{
public :
    SelectEncryptedLogDirDlg() : DockingDlgInterface(IDD_PLUGIN_SELECT_DIRECTORY){};

    virtual void display(bool toShow = true) const {
        DockingDlgInterface::display(toShow);
        if (toShow)
            ::SetFocus(::GetDlgItem(_hSelf, ID_EDIT_ENCRYPTED_LOG_DIR ));
    };

    void setLastLogDir(const WCHAR* logDir);

    void setParent(HWND parent2set){
        _hParent = parent2set;
    };

protected :
    virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
    void onInitDialg(WPARAM wParam, LPARAM lParam);
    void onNPPActiveDocChanged(WPARAM wParam, LPARAM lParam);
    void onOKButtonPushed(WPARAM wParam, LPARAM lParam);
    void onListboxLButtonDoubleClicked(WPARAM wParam, LPARAM lParam);
    void onRedecryptFileButtonPushed(WPARAM wParam, LPARAM lParam);

private :

    bool selectLogDir(WCHAR* folderPath, int length);
    bool fillFilesInTree(const WCHAR* folderPath);
    bool decryptLog(const WCHAR* encryptedFileName, const WCHAR* decryptedFileName);
    bool isDecryptLogFileName(const WCHAR* filePath);
    bool processSameNameFile(const WCHAR* encryptedFilePath, const WCHAR* decryptedFilePath);
    bool generateDecryptLogName(const WCHAR* filePath, WCHAR* decryptedFileName, int length);
    bool displayFileContent(const WCHAR* filePath);
    bool getEncryptedLogFileName(const WCHAR* decryptedFilePath, WCHAR* encryptedFilePath, int length) const;

private:
    std::list<LogPathPair> _decryptedLogFilePathList;
};

#endif //SELECT_ENCRYPTLOGDIR_DLG_H
