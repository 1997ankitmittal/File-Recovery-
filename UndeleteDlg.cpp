// UndeleteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Undelete.h"
#include "UndeleteDlg.h"
#include "FindDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PHYSICAL_DRIVE "\\\\.\\PhysicalDrive0"

int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData) 
{
	switch(uMsg) 
	{
	case BFFM_INITIALIZED: 
		SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)pData);
		break;
	default:
		break;
	}
	return 0;
}

BOOL GetDirectory(HWND hWnd, LPTSTR szDir, LPTSTR szDefaultPath) 
{ 
	BOOL			fRet; 
	TCHAR			szPath[MAX_PATH]; 
	LPITEMIDLIST	pidl; 
	LPMALLOC		lpMalloc;  
	BROWSEINFO		bi = {hWnd, NULL, szPath, " Please select a folder to extract ", BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE, NULL, 0L, 0};


	if(szDefaultPath)
	{
		bi.lpfn = BrowseCallbackProc;
		bi.lParam = (LPARAM)szDefaultPath;
	}

	pidl = SHBrowseForFolder(&bi);  
	if (NULL != pidl) 
		fRet = SHGetPathFromIDList(pidl, szDir); 
	else 
		fRet = FALSE; 
	// Get the shell's allocator to free PIDLs 
	if(!SHGetMalloc(&lpMalloc) && (NULL != lpMalloc)) 
	{ 
		if(NULL != pidl) 
		{ 
			lpMalloc->Free(pidl); 
		}  
		lpMalloc->Release(); 
	}  
	return fRet; 
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUndeleteDlg dialog

CUndeleteDlg::CUndeleteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUndeleteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUndeleteDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUndeleteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUndeleteDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUndeleteDlg, CDialog)
	//{{AFX_MSG_MAP(CUndeleteDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTNSCAN, OnBtnScan)
	ON_WM_DESTROY()
	ON_COMMAND(ID_MNU_FIND_FIRST, OnMnuFindFirst)
	ON_COMMAND(ID_MNU_FIND_NEXT, OnMnuFindNext)
	ON_COMMAND(ID_MNU_SAVE, OnMnuSave)
	ON_NOTIFY(NM_RCLICK, IDC_LSTFILES, OnRClickLstFiles)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LSTFILES, OnKeyDownLstFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUndeleteDlg message handlers

BOOL CUndeleteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_hDrive = INVALID_HANDLE_VALUE;
	
	CListCtrl *pList = (CListCtrl*)GetDlgItem(IDC_LSTFILES);
	
	pList->InsertColumn(0,"Filename",LVCFMT_CENTER,190,0);
	pList->InsertColumn(1,"#",LVCFMT_CENTER,60,0);
	pList->InsertColumn(2,"Attribute",LVCFMT_CENTER,80,0);
	pList->InsertColumn(3,"Clusters",LVCFMT_CENTER,70,0);
	pList->InsertColumn(3,"Deleted",LVCFMT_CENTER,50,0);
	
	pList->SetExtendedStyle(LVS_EX_FULLROWSELECT);

	int nArr[] = {1,0,2,3,4};

	pList->SetColumnOrderArray(5,nArr);


	m_hTreeRoot = ((CTreeCtrl*)GetDlgItem(IDC_TREDISKS))->InsertItem("Drives", 0, 0, TVI_ROOT, TVI_LAST);

	void *lpMsgBuf;
	int nRet = ScanLogicalDrives();
	if(nRet)
	{
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,nRet, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL);
		MessageBox((LPTSTR)lpMsgBuf, "GetLastError", MB_OK|MB_ICONERROR);
		LocalFree(lpMsgBuf);
	}

	m_bStopScanFilesThread = true;
	m_hScanFilesThread = NULL;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUndeleteDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CUndeleteDlg::OnDestroy() 
{
	if(m_hDrive != INVALID_HANDLE_VALUE)
		CloseHandle(m_hDrive);

	m_hDrive = INVALID_HANDLE_VALUE;

	CDialog::OnDestroy();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUndeleteDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUndeleteDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CUndeleteDlg::AddDrive(char *szDrvTxt, DRIVEPACKET *pstDrive)
{
	DRIVEPACKET *pDrive = new DRIVEPACKET;

	memcpy(pDrive,pstDrive, sizeof(DRIVEPACKET));

	((CTreeCtrl*)GetDlgItem(IDC_TREDISKS))->InsertItem(TVIF_TEXT|TVIF_PARAM, szDrvTxt, 0, 0, 0, 0, (LPARAM)pDrive, m_hTreeRoot, TVI_LAST);
}

/// This function is from vinoj kumar's article forensic in codeguru
//    btw, it was a messy code  I have modified it left & right 
int CUndeleteDlg::ScanLogicalDrives()
{
	int i,nRet;
	DWORD dwBytes;

	PARTITION *PartitionTbl;
	DRIVEPACKET stDrive;

	BYTE szSector[512];
	WORD wDrive =0;

	char szTmpStr[64];
	char szTxt[255];
	
	DWORD dwMainPrevRelSector =0;
	DWORD dwPrevRelSector =0;

	HANDLE hDrive = CreateFile(PHYSICAL_DRIVE,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
	if(hDrive == INVALID_HANDLE_VALUE)
		return GetLastError();

	nRet = ReadFile(hDrive,szSector,512,&dwBytes,0);
	if(!nRet)
		return GetLastError();

	dwPrevRelSector =0;
	dwMainPrevRelSector =0;

	PartitionTbl = (PARTITION*)(szSector+0x1BE);

	for(i=0; i<4; i++) /// scanning partitions in the physical disk
	{
		stDrive.wCylinder = PartitionTbl->chCylinder;
		stDrive.wHead = PartitionTbl->chHead;
		stDrive.wSector = PartitionTbl->chSector;
		stDrive.dwNumSectors = PartitionTbl->dwNumberSectors;
		stDrive.wType = ((PartitionTbl->chType == PART_EXTENDED) || (PartitionTbl->chType == PART_DOSX13X)) ? EXTENDED_PART:BOOT_RECORD;

		if((PartitionTbl->chType == PART_EXTENDED) || (PartitionTbl->chType == PART_DOSX13X))
		{
			dwMainPrevRelSector			= PartitionTbl->dwRelativeSector;
			stDrive.dwNTRelativeSector	= dwMainPrevRelSector;
		}
		else
		{
			stDrive.dwNTRelativeSector = dwMainPrevRelSector + PartitionTbl->dwRelativeSector;
		}

		if(stDrive.wType == EXTENDED_PART)
			break;

		if(PartitionTbl->chType == 0)
			break;

		switch(PartitionTbl->chType)
		{
		case PART_DOS2_FAT:
			strcpy(szTmpStr, "FAT12");
			break;
		case PART_DOSX13:
		case PART_DOS4_FAT:
		case PART_DOS3_FAT:
			strcpy(szTmpStr, "FAT16");
			break;
		case PART_DOS32X:
		case PART_DOS32:
			strcpy(szTmpStr, "FAT32");			//Normal FAT32
			break;
		case 7:
			strcpy(szTmpStr, "NTFS");	// NTFS , v r interested only on this
			break;
		default:
			strcpy(szTmpStr, "Unknown");
			break;
		}

		wsprintf(szTxt, "%s Drive %d", szTmpStr,wDrive);

		AddDrive(szTxt,&stDrive);
		PartitionTbl++;
		wDrive++;
	}

	if(i==4)
		return ERROR_SUCCESS;

	for(int LogiHard=0; LogiHard<50; LogiHard++) // scanning extended partitions
	{
		if(stDrive.wType == EXTENDED_PART)
		{
			LARGE_INTEGER n64Pos;

			n64Pos.QuadPart = ((LONGLONG) stDrive.dwNTRelativeSector) * 512;
			
			nRet = SetFilePointer(hDrive, n64Pos.LowPart,&n64Pos.HighPart, FILE_BEGIN);
			if(nRet == 0xffffffff)
				return GetLastError();;

			dwBytes = 0;

			nRet = ReadFile(hDrive, szSector, 512, (DWORD *) &dwBytes, NULL);
			if(!nRet)
				return GetLastError();

			if(dwBytes != 512)
				return ERROR_READ_FAULT;
			
			PartitionTbl = (PARTITION *) (szSector+0x1BE);

			for(i=0; i<4; i++)
			{
				stDrive.wCylinder = PartitionTbl->chCylinder;
				stDrive.wHead = PartitionTbl->chHead;
				stDrive.dwNumSectors = PartitionTbl->dwNumberSectors;
				stDrive.wSector = PartitionTbl->chSector;
				stDrive.dwRelativeSector = 0;
				stDrive.wType = ((PartitionTbl->chType == PART_EXTENDED) || (PartitionTbl->chType == PART_DOSX13X)) ? EXTENDED_PART:BOOT_RECORD;
				
				if((PartitionTbl->chType == PART_EXTENDED) || (PartitionTbl->chType == PART_DOSX13X))
				{
					dwPrevRelSector = PartitionTbl->dwRelativeSector;
					stDrive.dwNTRelativeSector = dwPrevRelSector + dwMainPrevRelSector;
				}
				else
				{
					stDrive.dwNTRelativeSector = dwMainPrevRelSector + dwPrevRelSector + PartitionTbl->dwRelativeSector;
				}

				if(stDrive.wType == EXTENDED_PART)
					break;

				if(PartitionTbl->chType == 0)
					break;

				switch(PartitionTbl->chType)
				{
				case PART_DOS2_FAT:
					strcpy(szTmpStr, "FAT12");
					break;
				case PART_DOSX13:
				case PART_DOS4_FAT:
				case PART_DOS3_FAT:
					strcpy(szTmpStr, "FAT16");
					break;
				case PART_DOS32X:
				case PART_DOS32:
					strcpy(szTmpStr, "FAT32");			//Normal FAT32
					break;
				case 7:
					strcpy(szTmpStr, "NTFS");
					break;
				default:
					strcpy(szTmpStr, "Unknown");
					break;
				}
				wsprintf(szTxt, "%s Drive %d", szTmpStr, wDrive);
				AddDrive(szTxt,&stDrive);
				PartitionTbl++;
				wDrive++;
			}
			if(i==4)
				break;
		}
	}

	CloseHandle(hDrive);
	return ERROR_SUCCESS;
}

void CUndeleteDlg::OnBtnScan() 
{
	DWORD dwCode;
	CString cszTxt;

	GetDlgItemText(IDC_BTNSCAN,cszTxt);

	if(cszTxt == "Scan Files")
	{
		m_bStopScanFilesThread = false;
		m_hScanFilesThread = CreateThread(NULL,0,ScanFilesThread,this,0,0);

		SetDlgItemText(IDC_BTNSCAN,"Stop Scanning");
	}
	else
	{
		m_bStopScanFilesThread = true;
		
		dwCode = 0;
		if(GetExitCodeThread(m_hScanFilesThread,&dwCode))// make sure the thread is exited
		{
			if(dwCode == STILL_ACTIVE)
			{
				SetDlgItemText(IDC_BTNSCAN,"Stopping...");
				GetDlgItem(IDC_BTNSCAN)->EnableWindow(false);
				return;
			}
		}
		
		SetDlgItemText(IDC_BTNSCAN,"Scan Files");	
		CloseHandle(m_hScanFilesThread);
		m_hScanFilesThread = NULL;
	}
}

void CUndeleteDlg::OnRClickLstFiles(NMHDR* pNMHDR, LRESULT* pResult) 
{ // right clicking feature
	CMenu cMnu;
	cMnu.LoadMenu(IDR_MNULSTFILES);
	CMenu* pPopup = cMnu.GetSubMenu(0);

	DWORD dwPos = GetMessagePos();
	CPoint cPt;
	cPt.x = (int)LOWORD (dwPos);
	cPt.y = (int) HIWORD (dwPos);
 
	int nID = pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD,cPt.x,cPt.y,this,NULL);
	if(nID == ID_MNU_FIND_FIRST)
	{
		OnMnuFindFirst();
	}
	else if(nID == ID_MNU_FIND_NEXT)
	{
		OnMnuFindNext();
	}
	else if(nID == ID_MNU_SAVE)
	{
		OnMnuSave();
		
	}
	*pResult = 0;
}

void CUndeleteDlg::OnMnuFindFirst() 
{
	CFindDlg cFindDlg;
	CString cszTxt;
	DWORD i,dwCount;

	CListCtrl *pcList = (CListCtrl*)GetDlgItem(IDC_LSTFILES);
	dwCount = pcList->GetItemCount();

	cFindDlg.m_cszFindText = m_cszFindText;	

	if(cFindDlg.DoModal() == IDCANCEL)
		return;

	m_cszFindText = cFindDlg.m_cszFindText;
	
	if(m_cszFindText == "")
		return;

	for(i=0;i<dwCount;i++)
	{
		cszTxt = pcList->GetItemText(i,0);
		
		cszTxt.TrimRight();
		if(cszTxt == "")
			continue;

		cszTxt.MakeLower();
		m_cszFindText.MakeLower();

		if(cszTxt.Find(m_cszFindText) >=0) 
		{// string found
			pcList->EnsureVisible(i,false);
			pcList->SetItemState(i,LVIS_SELECTED,0x0000000f);
			m_dwFoundItem = i;
			break;
		}
	}
}

void CUndeleteDlg::OnMnuFindNext() 
{
	CString cszTxt;
	DWORD i,dwCount;

	CListCtrl *pcList = (CListCtrl*)GetDlgItem(IDC_LSTFILES);
	dwCount = pcList->GetItemCount();

	if(m_cszFindText == "")
		return;

	m_dwFoundItem++;
	if(m_dwFoundItem > dwCount)
	{
		MessageBox("Reached the end of list");
		m_dwFoundItem = 0;
		return;
	}

	for(i=m_dwFoundItem;i<dwCount;i++)
	{
		cszTxt = pcList->GetItemText(i,0);

		cszTxt.TrimRight();
		if(cszTxt == "")
			continue;
		
		cszTxt.MakeLower();
		m_cszFindText.MakeLower();

		if(cszTxt.Find(m_cszFindText) >=0) 
		{// string found
			pcList->EnsureVisible(i,false);
			pcList->SetItemState(i,LVIS_SELECTED,0x0000000f);
			m_dwFoundItem = i;
			break;
		}
	}
	if(i==dwCount)
	{
		MessageBox("Reached the end of list");
		m_dwFoundItem = 0;
	}
}


void CUndeleteDlg::OnMnuSave() 
{// saving tyhe files , saving the deleted & present files
	CListCtrl *pcList = (CListCtrl*)GetDlgItem(IDC_LSTFILES);

	char szPath[_MAX_PATH];
	char szTmpPath[_MAX_PATH];
	char szBuff[32];
	int nIdxSelLst;
	int nRet;
	void *lpMsgBuf;
	BYTE *pData;
	DWORD dwBytes =0;
	DWORD dwLen;

	CNTFSDrive::ST_FILEINFO stFInfo;

	POSITION pos = pcList->GetFirstSelectedItemPosition();
	if(!pos)
		return;

	GetCurrentDirectory(_MAX_PATH,szPath);
	if(!GetDirectory(this->m_hWnd, szPath,szPath))
		return;

	BeginWaitCursor();
	while(pos)
	{
		strcpy(szTmpPath,szPath);

		nIdxSelLst = pcList->GetNextSelectedItem(pos);
		pcList->GetItemText(nIdxSelLst,1,szBuff,32);
		
		nIdxSelLst = atol(szBuff);

		nRet = m_cNTFS.Read_File(nIdxSelLst+30,pData,dwLen); // read the file content in to a buffer
		if(nRet)                     
		{
			EndWaitCursor();
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,nRet, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL);
			MessageBox((LPTSTR)lpMsgBuf, "GetLastError", MB_OK|MB_ICONERROR);
			LocalFree(lpMsgBuf);
			return;
		}

		nRet = m_cNTFS.GetFileDetail(nIdxSelLst+30,stFInfo); // not get the file detail for FileName
		if(nRet)
		{
			EndWaitCursor();
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,nRet, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL);
			MessageBox((LPTSTR)lpMsgBuf, "GetLastError", MB_OK|MB_ICONERROR);
			LocalFree(lpMsgBuf);;
			return;
		}

		strcat(szTmpPath,"\\");
		strcat(szTmpPath,stFInfo.szFilename);

		/// create the new file and save the contents
		HANDLE hNewFile = CreateFile(szTmpPath,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,stFInfo.dwAttributes,0);
		if(hNewFile == INVALID_HANDLE_VALUE)
		{
			EndWaitCursor();
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL);
			MessageBox((LPTSTR)lpMsgBuf, "GetLastError", MB_OK|MB_ICONERROR);
			LocalFree(lpMsgBuf);
			return;
		}
		
		nRet = WriteFile(hNewFile,pData,dwLen,&dwBytes,NULL);
		if(!nRet)
		{
			EndWaitCursor();
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL);
			MessageBox((LPTSTR)lpMsgBuf, "GetLastError", MB_OK|MB_ICONERROR);
			LocalFree(lpMsgBuf);
			return;
		}
		delete pData;
		CloseHandle(hNewFile);
	}
}

// F3 ...find next feature
void CUndeleteDlg::OnKeyDownLstFiles(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	
	if(pLVKeyDow->wVKey == VK_F3)
	{
		OnMnuFindNext();
	}	
	*pResult = 0;
}


DWORD WINAPI CUndeleteDlg::ScanFilesThread(LPVOID lpVoid)
{
	CUndeleteDlg *pDlg = (CUndeleteDlg *)lpVoid;

	DRIVEPACKET *pDrive;	
	CNTFSDrive::ST_FILEINFO stFInfo;
	char szBuffer[255];
	DWORD dwDeleted = 0, dwBytes =0, i;
	int nRet;
	HTREEITEM hTreeItem;

	char *lpMsgBuf;

	CListCtrl *pList = (CListCtrl*)pDlg->GetDlgItem(IDC_LSTFILES);
	CTreeCtrl *pcTree = (CTreeCtrl *)pDlg->GetDlgItem(IDC_TREDISKS);

	hTreeItem = pcTree->GetSelectedItem();
	if(!hTreeItem)
		goto exitThread;;

	pDrive = (DRIVEPACKET *)pcTree->GetItemData(hTreeItem);

	if(!pDrive)
		goto exitThread;;
	
	// delete the previous files if entered
	pList->DeleteAllItems();

	if(pDlg->m_hDrive == INVALID_HANDLE_VALUE)
	{
		pDlg->m_hDrive = CreateFile(PHYSICAL_DRIVE,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,NULL);
		if(pDlg->m_hDrive == INVALID_HANDLE_VALUE)
		{
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL);
			pDlg->MessageBox((LPTSTR)lpMsgBuf, "GetLastError", MB_OK|MB_ICONERROR);
			LocalFree(lpMsgBuf);
			CloseHandle(pDlg->m_hDrive);
			goto exitThread;;
		}
	}

	pDlg->m_cNTFS.SetDriveHandle(pDlg->m_hDrive); // set the physical drive handle
	pDlg->m_cNTFS.SetStartSector(pDrive->dwNTRelativeSector,512); // set the starting sector of the NTFS

	nRet = pDlg->m_cNTFS.Initialize(); // initialize, ie. read all MFT in to the memory
	if(nRet)
	{
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,nRet, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL);
		pDlg->MessageBox((LPTSTR)lpMsgBuf, "GetLastError", MB_OK|MB_ICONERROR);
		LocalFree(lpMsgBuf);
		CloseHandle(pDlg->m_hDrive);
		pDlg->m_hDrive = INVALID_HANDLE_VALUE;
		goto exitThread;;
	}

	for(i=0;(i<0xFFFFFFFF);i++) // theoretical max file count is 0xFFFFFFFF
	{								//   but i'm not sure our CListCtrl can support this ...
		if(pDlg->m_bStopScanFilesThread)
			break;

		nRet = pDlg->m_cNTFS.GetFileDetail(i+30,stFInfo); // get the file detail one by one 
		if((nRet == ERROR_NO_MORE_FILES)||(nRet == ERROR_INVALID_PARAMETER))
			goto exitThread;;

		if(nRet)
		{
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,nRet, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL);
			pDlg->MessageBox((LPTSTR)lpMsgBuf, "GetLastError", MB_OK|MB_ICONERROR);
			LocalFree(lpMsgBuf);
			CloseHandle(pDlg->m_hDrive);
			pDlg->m_hDrive = INVALID_HANDLE_VALUE;
			goto exitThread;;
		}

		pList->InsertItem(i,stFInfo.szFilename);

		strcpy(szBuffer,"");

		if(stFInfo.dwAttributes&FILE_ATTRIBUTE_READONLY)
			strcat(szBuffer,"R-");

		if(stFInfo.dwAttributes&FILE_ATTRIBUTE_HIDDEN)
			strcat(szBuffer,"H-");

		if(stFInfo.dwAttributes&FILE_ATTRIBUTE_SYSTEM)
			strcat(szBuffer,"S-");

		if(stFInfo.dwAttributes&FILE_ATTRIBUTE_DIRECTORY)
			strcat(szBuffer,"D-");

		if(stFInfo.dwAttributes&FILE_ATTRIBUTE_ARCHIVE)
			strcat(szBuffer,"A-");

		if(stFInfo.dwAttributes&FILE_ATTRIBUTE_ENCRYPTED)
			strcat(szBuffer,"E-");

		if(stFInfo.dwAttributes&FILE_ATTRIBUTE_NORMAL)
			strcat(szBuffer,"N-");

		if(stFInfo.dwAttributes&FILE_ATTRIBUTE_TEMPORARY)
			strcat(szBuffer,"T-");

		if(stFInfo.dwAttributes&FILE_ATTRIBUTE_SPARSE_FILE)
			strcat(szBuffer,"Sp-");

		if(stFInfo.dwAttributes&FILE_ATTRIBUTE_REPARSE_POINT)
			strcat(szBuffer,"ReSp-");

		if(stFInfo.dwAttributes&FILE_ATTRIBUTE_COMPRESSED)
			strcat(szBuffer,"C-");

		if(stFInfo.dwAttributes&FILE_ATTRIBUTE_OFFLINE)
			strcat(szBuffer,"O-");

		if(stFInfo.dwAttributes&FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
			strcat(szBuffer,"I-");

		if(stFInfo.dwAttributes&0x4000)
			strcat(szBuffer,"En-");  // if it is encrypted

		if(stFInfo.dwAttributes&0x10000000)
			strcat(szBuffer,"Dir-"); // if it is directory

		if (stFInfo.dwAttributes & 0x20000000)
			strcpy(szBuffer, "In-");// if it is indexed

		szBuffer[(strlen(szBuffer)<512 ? strlen(szBuffer) : 511)] = 0;
		pList->SetItemText(i,2,szBuffer);

		sprintf(szBuffer,"%d",i);
		pList->SetItemText(i,1,szBuffer);

		strcpy(szBuffer,"");
		if(stFInfo.bDeleted)
		{
			dwDeleted++;
			strcpy(szBuffer,"Yes");
		}

		pList->SetItemText(i,3,szBuffer);

		sprintf(szBuffer,"%u",stFInfo.n64Size);
		pList->SetItemText(i,4,szBuffer);

		pDlg->SetDlgItemInt(IDC_EDTTOTFILES,i);
		pDlg->SetDlgItemInt(IDC_EDTDELFILES,dwDeleted);
	}
exitThread:
	pDlg->SetDlgItemText(IDC_BTNSCAN,"Scan Files");
	pDlg->GetDlgItem(IDC_BTNSCAN)->EnableWindow(true);
	return 0;
}