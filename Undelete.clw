; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CUndeleteDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Undelete.h"

ClassCount=4
Class1=CUndeleteApp
Class2=CUndeleteDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_DLGFIND
Resource4=IDD_UNDELETE_DIALOG
Class4=CFindDlg
Resource5=IDR_MNULSTFILES

[CLS:CUndeleteApp]
Type=0
HeaderFile=Undelete.h
ImplementationFile=Undelete.cpp
Filter=N

[CLS:CUndeleteDlg]
Type=0
HeaderFile=UndeleteDlg.h
ImplementationFile=UndeleteDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_LSTFILES

[CLS:CAboutDlg]
Type=0
HeaderFile=UndeleteDlg.h
ImplementationFile=UndeleteDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_UNDELETE_DIALOG]
Type=1
Class=CUndeleteDlg
ControlCount=9
Control1=IDCANCEL,button,1342242816
Control2=IDC_TREDISKS,SysTreeView32,1350635559
Control3=IDC_LSTFILES,SysListView32,1350664713
Control4=IDC_BTNSCAN,button,1342242816
Control5=IDC_STATIC,static,1342177294
Control6=IDC_LBLTOTFILES,static,1342308352
Control7=IDC_LBLDELFILES,static,1342308352
Control8=IDC_EDTTOTFILES,edit,1342253186
Control9=IDC_EDTDELFILES,edit,1342253186

[MNU:IDR_MNULSTFILES]
Type=1
Class=?
Command1=ID_MNU_FIND_FIRST
Command2=ID_MNU_FIND_NEXT
Command3=ID_MNU_SAVE
CommandCount=3

[DLG:IDD_DLGFIND]
Type=1
Class=CFindDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDTFINDTEXT,edit,1350631552
Control4=IDC_STATIC,static,1342308352

[CLS:CFindDlg]
Type=0
HeaderFile=FindDlg.h
ImplementationFile=FindDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=ID_MNU_FIND_FIRST
VirtualFilter=dWC

