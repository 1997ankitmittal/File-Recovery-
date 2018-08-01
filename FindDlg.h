#if !defined(AFX_FINDDLG_H__C5A7A090_E7DC_41A0_B93C_C4BA43EC5244__INCLUDED_)
#define AFX_FINDDLG_H__C5A7A090_E7DC_41A0_B93C_C4BA43EC5244__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog

class CFindDlg : public CDialog
{
// Construction
public:
	CFindDlg(CWnd* pParent = NULL);   // standard constructor

	CString m_cszFindText;
// Dialog Data
	//{{AFX_DATA(CFindDlg)
	enum { IDD = IDD_DLGFIND };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	

	// Generated message map functions
	//{{AFX_MSG(CFindDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDDLG_H__C5A7A090_E7DC_41A0_B93C_C4BA43EC5244__INCLUDED_)
