// MFTRecord.h: interface for the CMFTRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFTRECORD_H__04A1B8DF_0EB0_4B72_8587_2703342C5675__INCLUDED_)
#define AFX_MFTRECORD_H__04A1B8DF_0EB0_4B72_8587_2703342C5675__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma pack(push, curAlignment)

#pragma pack(1)

////////////////////////////MFT record header and attribute header //////////////////////
struct NTFS_MFT_FILE
{
	char		szSignature[4];		// Signature "FILE"
	WORD		wFixupOffset;		// offset to fixup pattern
	WORD		wFixupSize;			// Size of fixup-list +1
	LONGLONG	n64LogSeqNumber;	// log file seq number
	WORD		wSequence;			// sequence nr in MFT
	WORD		wHardLinks;			// Hard-link count
	WORD		wAttribOffset;		// Offset to seq of Attributes
	WORD		wFlags;				// 0x01 = NonRes; 0x02 = Dir
	DWORD		dwRecLength;		// Real size of the record
	DWORD		dwAllLength;		// Allocated size of the record
	LONGLONG	n64BaseMftRec;		// ptr to base MFT rec or 0
	WORD		wNextAttrID;		// Minimum Identificator +1
	WORD		wFixupPattern;		// Current fixup pattern
	DWORD		dwMFTRecNumber;		// Number of this MFT Record
								// followed by resident and
								// part of non-res attributes
};

typedef struct	// if resident then + RESIDENT
{					//  else + NONRESIDENT
	DWORD	dwType;
	DWORD	dwFullLength;
	BYTE	uchNonResFlag;
	BYTE	uchNameLength;
	WORD	wNameOffset;
	WORD	wFlags;
	WORD	wID;

	union ATTR
	{
		struct RESIDENT
		{
			DWORD	dwLength;
			WORD	wAttrOffset;
			BYTE	uchIndexedTag;
			BYTE	uchPadding;
		} Resident;

		struct NONRESIDENT
		{
			LONGLONG	n64StartVCN;
			LONGLONG	n64EndVCN;
			WORD		wDatarunOffset;
			WORD		wCompressionSize; // compression unit size
			BYTE		uchPadding[4];
			LONGLONG	n64AllocSize;
			LONGLONG	n64RealSize;
			LONGLONG	n64StreamSize;
			// data runs...
		}NonResident;
	}Attr;
} NTFS_ATTRIBUTE;

//////////////////////////////////////////////////////////////////////////////////////////

///////////////////////// Attributes /////////////////////////////////////////////////////
typedef struct
{
	LONGLONG	n64Create;		// Creation time
	LONGLONG	n64Modify;		// Last Modify time
	LONGLONG	n64Modfil;		// Last modify of record
	LONGLONG	n64Access;		// Last Access time
	DWORD		dwFATAttributes;// As FAT + 0x800 = compressed
	DWORD		dwReserved1;	// unknown

} ATTR_STANDARD;   
  
typedef struct
{
	LONGLONG	dwMftParentDir;            // Seq-nr parent-dir MFT entry
	LONGLONG	n64Create;                  // Creation time
	LONGLONG	n64Modify;                  // Last Modify time
	LONGLONG	n64Modfil;                  // Last modify of record
	LONGLONG	n64Access;                  // Last Access time
	LONGLONG	n64Allocated;               // Allocated disk space
	LONGLONG	n64RealSize;                // Size of the file
	DWORD		dwFlags;					// attribute
	DWORD		dwEAsReparsTag;				// Used by EAs and Reparse
	BYTE		chFileNameLength;
	BYTE		chFileNameType;            // 8.3 / Unicode
	WORD		wFilename[512];             // Name (in Unicode ?)

}ATTR_FILENAME; 
//////////////////////////////////////////////////////////////////////////////////////////

#pragma pack(pop, curAlignment)



class CMFTRecord  
{
protected:
	HANDLE	m_hDrive;

	BYTE *m_pMFTRecord;
	DWORD m_dwMaxMFTRecSize;
	DWORD m_dwCurPos;
	DWORD m_dwBytesPerCluster;
	LONGLONG m_n64StartPos;

	int ReadRaw(LONGLONG n64LCN, BYTE *chData, DWORD &dwLen);
	int ExtractData(NTFS_ATTRIBUTE ntfsAttr, BYTE *&puchData, DWORD &dwDataLen);

public:
///////// attributes //////////////////////
	ATTR_STANDARD m_attrStandard;
	ATTR_FILENAME m_attrFilename;

	BYTE *m_puchFileData; // collected file data buffer
	DWORD m_dwFileDataSz; // file data size , ie. m_pchFileData buffer length

	bool m_bInUse;

public:
	int SetRecordInfo(LONGLONG n64StartPos, DWORD dwRecSize, DWORD dwBytesPerCluster);
	void SetDriveHandle(HANDLE hDrive);
	
	int ExtractFile(BYTE *puchMFTBuffer, DWORD dwLen, bool bExcludeData=false);

	CMFTRecord();
	virtual ~CMFTRecord();

};

#endif // !defined(AFX_MFTRECORD_H__04A1B8DF_0EB0_4B72_8587_2703342C5675__INCLUDED_)
