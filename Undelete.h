// Undelete.h : main header file for the UNDELETE application
//

#if !defined(AFX_UNDELETE_H__36D89CCD_73B6_4281_84F3_8BFB6C26776D__INCLUDED_)
#define AFX_UNDELETE_H__36D89CCD_73B6_4281_84F3_8BFB6C26776D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CUndeleteApp:
// See Undelete.cpp for the implementation of this class
//

class CUndeleteApp : public CWinApp
{
public:
	CUndeleteApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUndeleteApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CUndeleteApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNDELETE_H__36D89CCD_73B6_4281_84F3_8BFB6C26776D__INCLUDED_)
