#ifndef SELECT_ENCRYPTLOGDIR_DLG_H
#define SELECT_ENCRYPTLOGDIR_DLG_H

#include "DockingDlgInterface.h"
#include "resource.h"

class SelectEncryptLogDirDlg : public DockingDlgInterface
{
public :
    SelectEncryptLogDirDlg() : DockingDlgInterface(IDD_PLUGIN_SELECT_DIRECTORY){};

    virtual void display(bool toShow = true) const {
        DockingDlgInterface::display(toShow);
        if (toShow)
            ::SetFocus(::GetDlgItem(_hSelf, ID_GOLINE_EDIT));
    };

    void setParent(HWND parent2set){
        _hParent = parent2set;
    };

protected :
    virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);

private :

    int getLine() const {
        BOOL isSuccessful;
        int line = ::GetDlgItemInt(_hSelf, ID_GOLINE_EDIT, &isSuccessful, FALSE);
        return (isSuccessful?line:-1);
    };

};

#endif //SELECT_ENCRYPTLOGDIR_DLG_H
