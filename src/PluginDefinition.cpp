//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"

#include "DockingFeature/SelectEncryptedLogDirDlg.h"

const TCHAR sectionName[] = TEXT("CONFIG");
const TCHAR encrypedLogDir[] = TEXT("EncryptedLogDir");
const TCHAR isPannelOpen[] = TEXT("IsPannelOpen");
const TCHAR configFileName[] = TEXT("eGatewayDecryptLogPlugin.ini");
const TCHAR defEncryptedLogDir[] = TEXT("C:\\Program Data\\Mindray\\eGateway\\eGatewayLogs");
SelectEncryptedLogDirDlg _selectEncryptLogDirDlg;
//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

TCHAR iniFilePath[MAX_PATH];
bool isOpenTag = false;
#define DOCKABLE_DECRYPT_LOG_INDEX 0
//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
	// Initialize dockable demo dialog
    _selectEncryptLogDirDlg.init((HINSTANCE)hModule, NULL);
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
    // last open state
	::WritePrivateProfileString(sectionName, isPannelOpen, isOpenTag?TEXT("1"):TEXT("0"), iniFilePath);
    
    // last folder path
    TCHAR  folderPath[MAX_PATH + 1] = {0};
    GetCurrentDirectory(_countof(folderPath),folderPath);
    ::WritePrivateProfileString(sectionName, encrypedLogDir, folderPath, iniFilePath);
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{
	// get path of plugin configuration
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)iniFilePath);

	// if config path doesn't exist, we create it
	if (PathFileExists(iniFilePath) == FALSE)
	{
		::CreateDirectory(iniFilePath, NULL);
	}

	// make your plugin config file full file path name
	PathAppend(iniFilePath, configFileName);

	// get the parameter value from plugin config
	isOpenTag = (::GetPrivateProfileInt(sectionName, isPannelOpen, 0, iniFilePath) != 0);

    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );

    // Shortcut :
    // Following code makes the first command
    // bind to the shortcut Alt-Q
    ShortcutKey *shKey = new ShortcutKey;
    shKey->_isAlt = true;
    shKey->_isCtrl = false;
    shKey->_isShift = false;
    shKey->_key = 0x33; //VK_3
    setCommand(0, TEXT("Select encrypted log directory"), SelectEncryptedLogDirectory, shKey, false);
    setCommand(1, TEXT("---"), NULL, NULL, false);
    setCommand(2, TEXT("About"), AboutMsgBox, NULL, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
    // Don't forget to deallocate your shortcut here
    delete funcItem[0]._pShKey;
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//

void SelectEncryptedLogDirectory()
{
    _selectEncryptLogDirDlg.setParent(nppData._nppHandle);
    tTbData	data = {0};

    if (!_selectEncryptLogDirDlg.isCreated())
    {
        _selectEncryptLogDirDlg.create(&data);

        // define the default docking behaviour
        data.uMask = DWS_DF_CONT_RIGHT;

        data.pszModuleName = _selectEncryptLogDirDlg.getPluginFileName();

        // the dlgDlg should be the index of funcItem where the current function pointer is
        // in this case is DOCKABLE_DEMO_INDEX
        data.dlgID = DOCKABLE_DECRYPT_LOG_INDEX;
        ::SendMessage(nppData._nppHandle, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&data);
    }
    
    _selectEncryptLogDirDlg.display();

    TCHAR  folderPath[MAX_PATH + 1] = {0};
    ::GetPrivateProfileString(sectionName, encrypedLogDir, defEncryptedLogDir, folderPath, _countof(folderPath), iniFilePath);
    _selectEncryptLogDirDlg.setLastLogDir(folderPath);
}

void AboutMsgBox()
{
    WCHAR text[1024] = {0};
    swprintf_s(text, 
        L"Version:   v1.0 \n\n "
        L"Developer: Terry/Ding Yankun, Oct 8th,2017\n\n "
        L"Usage:     Select the encrypted log directory, and double clicked the file name in the listbox.");
    ::MessageBox(nppData._nppHandle, text, NPP_PLUGIN_NAME, MB_OK);
}
