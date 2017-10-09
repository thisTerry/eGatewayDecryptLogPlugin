#ifndef SELECT_ENCRYPTLOGDIR_DLG_H
#define SELECT_ENCRYPTLOGDIR_DLG_H

#include "DockingDlgInterface.h"
#include "resource.h"

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
    static LRESULT CALLBACK ListProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual INT_PTR CALLBACK run_listProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
    void onInitDialg(WPARAM wParam, LPARAM lParam);
    void onOKButtonPushed(WPARAM wParam, LPARAM lParam);
    void onListboxLButtonDoubleClicked(WPARAM wParam, LPARAM lParam);
    void onRedecryptFileButtonPushed(WPARAM wParam, LPARAM lParam);

private :

    bool selectLogDir(WCHAR* folderPath, int length);
    bool fillFilesInTree(const WCHAR* folderPath);
    bool decryptLog(const WCHAR* encryptedFileName, WCHAR* decryptedFileName, int length);
    bool isDecryptLog(const WCHAR* filePath);
    bool generateDecryptLogName(const WCHAR* filePath, WCHAR* decryptedFileName, int length);
    bool displayFileContent(const WCHAR* filePath);

private:
    WNDPROC _OldListProc ;
};

#endif //SELECT_ENCRYPTLOGDIR_DLG_H
