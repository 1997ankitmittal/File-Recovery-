// FindDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Undelete.h"
#include "FindDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog


CFindDlg::CFindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindDlg, CDialog)
	//{{AFX_MSG_MAP(CFindDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindDlg message handlers

BOOL CFindDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetDlgItemText(IDC_EDTFINDTEXT, m_cszFindText);

	GetDlgItem(IDC_EDTFINDTEXT)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindDlg::OnOK() 
{	
	GetDlgItemText(IDC_EDTFINDTEXT, m_cszFindText);
	CDialog::OnOK();
}