// UndeleteDlg.h : header file
//

#if !defined(AFX_UNDELETEDLG_H__8B418511_9F89_47D7_9F50_B4D15178914B__INCLUDED_)
#define AFX_UNDELETEDLG_H__8B418511_9F89_47D7_9F50_B4D15178914B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NTFSDrive.h"


typedef struct
{
	WORD	wCylinder;
	WORD	wHead;
	WORD	wSector;
	DWORD	dwNumSectors;
	WORD	wType;
	DWORD	dwRelativeSector;
	DWORD	dwNTRelativeSector;
	DWORD	dwBytesPerSector;

}DRIVEPACKET;

typedef struct
{
	BYTE	chBootInd;
	BYTE	chHead;
	BYTE	chSector;
	BYTE	chCylinder;
	BYTE	chType;
	BYTE	chLastHead;
	BYTE	chLastSector;
	BYTE	chLastCylinder;
	DWORD	dwRelativeSector;
	DWORD	dwNumberSectors;

}PARTITION;

#define PART_TABLE 0
#define BOOT_RECORD 1
#define EXTENDED_PART 2

#define PART_UNKNOWN 0x00		//Unknown.  
#define PART_DOS2_FAT 0x01		//12-bit FAT.  
#define PART_DOS3_FAT 0x04		//16-bit FAT. Partition smaller than 32MB.  
#define PART_EXTENDED 0x05		//Extended MS-DOS Partition.  
#define PART_DOS4_FAT 0x06		//16-bit FAT. Partition larger than or equal to 32MB.  
#define PART_DOS32 0x0B			//32-bit FAT. Partition up to 2047GB.  
#define PART_DOS32X 0x0C		//Same as PART_DOS32(0Bh), but uses Logical Block Address Int 13h extensions.  
#define PART_DOSX13 0x0E		//Same as PART_DOS4_FAT(06h), but uses Logical Block Address Int 13h extensions.  
#define PART_DOSX13X 0x0F		//Same as PART_EXTENDED(05h), but uses Logical Block Address Int 13h extensions.  


/////////////////////////////////////////////////////////////////////////////
// CUndeleteDlg dialog


class CUndeleteDlg : public CDialog
{
// Construction
public:
	void AddDrive(char *szDrvTxt, DRIVEPACKET *pstDrive);
	CUndeleteDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CUndeleteDlg)
	enum { IDD = IDD_UNDELETE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUndeleteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int ScanLogicalDrives();
	HICON m_hIcon;
	HTREEITEM m_hTreeRoot;

	CNTFSDrive m_cNTFS;

	HANDLE m_hDrive;

	CString m_cszFindText;
	DWORD m_dwFoundItem;

	HANDLE m_hScanFilesThread;
	bool m_bStopScanFilesThread;
	static DWORD WINAPI ScanFilesThread(LPVOID lpVoid);


	// Generated message map functions
	//{{AFX_MSG(CUndeleteDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnScan();
	afx_msg void OnDestroy();
	afx_msg void OnMnuFindFirst();
	afx_msg void OnMnuFindNext();
	afx_msg void OnMnuSave();
	afx_msg void OnRClickLstFiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDownLstFiles(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNDELETEDLG_H__8B418511_9F89_47D7_9F50_B4D15178914B__INCLUDED_)
