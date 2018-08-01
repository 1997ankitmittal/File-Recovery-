// MFTRecord.cpp: implementation of the CMFTRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MFTRecord.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMFTRecord::CMFTRecord()
{
	m_hDrive = 0;

	m_dwMaxMFTRecSize = 1023; // usual size
	m_pMFTRecord = 0;
	m_dwCurPos = 0;

	m_puchFileData = 0; // collected file data buffer
	m_dwFileDataSz = 0; // file data size , ie. m_pchFileData buffer length

	memset(&m_attrStandard,0,sizeof(ATTR_STANDARD));
	memset(&m_attrFilename,0,sizeof(ATTR_FILENAME));

	m_bInUse = false;;
}

CMFTRecord::~CMFTRecord()
{
	if(m_puchFileData)
		delete m_puchFileData;
	m_puchFileData = 0;
	m_dwFileDataSz = 0;
}

// set the drive handle
void CMFTRecord::SetDriveHandle(HANDLE hDrive)
{
	m_hDrive = hDrive;
}

// set the detail
//  n64StartPos is the byte from the starting of the physical disk
//  dwRecSize is the record size in the MFT table
//  dwBytesPerCluster is the bytes per cluster
int CMFTRecord::SetRecordInfo(LONGLONG  n64StartPos, DWORD dwRecSize, DWORD dwBytesPerCluster)
{
	if(!dwRecSize)
		return ERROR_INVALID_PARAMETER;

	if(dwRecSize%2)
		return ERROR_INVALID_PARAMETER;

	if(!dwBytesPerCluster)
		return ERROR_INVALID_PARAMETER;

	if(dwBytesPerCluster%2)
		return ERROR_INVALID_PARAMETER;

	m_dwMaxMFTRecSize = dwRecSize;
	m_dwBytesPerCluster = dwBytesPerCluster;
	m_n64StartPos =  n64StartPos;
	return ERROR_SUCCESS;
}


/// puchMFTBuffer is the MFT record buffer itself (normally 1024 bytes)
//  dwLen is the MFT record buffer length
//  bExcludeData = if true the file data will not be extracted
//                 This is useful for only file browsing
int CMFTRecord::ExtractFile(BYTE *puchMFTBuffer, DWORD dwLen, bool bExcludeData)
{
	if(m_dwMaxMFTRecSize > dwLen)
		return ERROR_INVALID_PARAMETER;
	if(!puchMFTBuffer)
		return ERROR_INVALID_PARAMETER;

	NTFS_MFT_FILE	ntfsMFT;
	NTFS_ATTRIBUTE	ntfsAttr;

	BYTE *puchTmp = 0;
	BYTE *uchTmpData =0;
	DWORD dwTmpDataLen;
	int nRet;
	
	m_pMFTRecord = puchMFTBuffer;
	m_dwCurPos = 0;

	if(m_puchFileData)
		delete m_puchFileData;
	m_puchFileData = 0;
	m_dwFileDataSz = 0;

	// read the record header in MFT table
	memcpy(&ntfsMFT,&m_pMFTRecord[m_dwCurPos],sizeof(NTFS_MFT_FILE));

	if(memcmp(ntfsMFT.szSignature,"FILE",4))
		return ERROR_INVALID_PARAMETER; // not the right signature

	m_bInUse = (ntfsMFT.wFlags&0x01); //0x01  	Record is in use
									  //0x02 	Record is a directory

	//m_dwCurPos = (ntfsMFT.wFixupOffset + ntfsMFT.wFixupSize*2); 
	m_dwCurPos = ntfsMFT.wAttribOffset;

	do
	{	// extract the attribute header
		memcpy(&ntfsAttr,&m_pMFTRecord[m_dwCurPos],sizeof(NTFS_ATTRIBUTE));

		switch(ntfsAttr.dwType) // extract the attribute data 
		{
			// here I haven' implemented the processing of all the attributes.
			//  I have implemented attributes necessary for file & file data extraction
		case 0://UNUSED
			break;

		case 0x10: //STANDARD_INFORMATION
			nRet = ExtractData(ntfsAttr,uchTmpData,dwTmpDataLen);
			if(nRet)
				return nRet;
			memcpy(&m_attrStandard,uchTmpData,sizeof(ATTR_STANDARD));

			delete uchTmpData;
			uchTmpData = 0;
			dwTmpDataLen = 0;
			break;

		case 0x30: //FILE_NAME
			nRet = ExtractData(ntfsAttr,uchTmpData,dwTmpDataLen);
			if(nRet)
				return nRet;
			memcpy(&m_attrFilename,uchTmpData,dwTmpDataLen);

			delete uchTmpData;
			uchTmpData = 0;
			dwTmpDataLen = 0;

			break;

		case 0x40: //OBJECT_ID
			break;
		case 0x50: //SECURITY_DESCRIPTOR
			break;
		case 0x60: //VOLUME_NAME
			break;
		case 0x70: //VOLUME_INFORMATION
			break;
		case 0x80: //DATA
			if(!bExcludeData) 
			{
				nRet = ExtractData(ntfsAttr,uchTmpData,dwTmpDataLen);
				if(nRet)
					return nRet;

				if(!m_puchFileData)
				{
					m_dwFileDataSz = dwTmpDataLen;
					m_puchFileData = new BYTE[dwTmpDataLen];
					
					memcpy(m_puchFileData,uchTmpData,dwTmpDataLen);
				}
				else
				{
					puchTmp = new BYTE[m_dwFileDataSz+dwTmpDataLen];
					memcpy(puchTmp,m_puchFileData,m_dwFileDataSz);
					memcpy(puchTmp+m_dwFileDataSz,uchTmpData,dwTmpDataLen);

					m_dwFileDataSz += dwTmpDataLen;
					delete m_puchFileData;
					m_puchFileData = puchTmp;
				}

				delete uchTmpData;
				uchTmpData = 0;
				dwTmpDataLen = 0;
			}
			break;

		case 0x90: //INDEX_ROOT
		case 0xa0: //INDEX_ALLOCATION
			// todo: not implemented to read the index mapped records
			return ERROR_SUCCESS;
			continue;
			break;
		case 0xb0: //BITMAP
			break;
		case 0xc0: //REPARSE_POINT
			break;
		case 0xd0: //EA_INFORMATION
			break;
		case 0xe0: //EA
			break;
		case 0xf0: //PROPERTY_SET
			break;
		case 0x100: //LOGGED_UTILITY_STREAM
			break;
		case 0x1000: //FIRST_USER_DEFINED_ATTRIBUTE
			break;

		case 0xFFFFFFFF: // END 
			if(uchTmpData)
				delete uchTmpData;
			uchTmpData = 0;
			dwTmpDataLen = 0;
			return ERROR_SUCCESS;

		default:
			break;
		};

		m_dwCurPos += ntfsAttr.dwFullLength; // go to the next location of attribute
	}
	while(ntfsAttr.dwFullLength);

	if(uchTmpData)
		delete uchTmpData;
	uchTmpData = 0;
	dwTmpDataLen = 0;
	return ERROR_SUCCESS;
}

// extract the attribute data from the MFT table
//   Data can be Resident & non-resident
int CMFTRecord::ExtractData(NTFS_ATTRIBUTE ntfsAttr, BYTE *&puchData, DWORD &dwDataLen)
{
	DWORD dwCurPos = m_dwCurPos;

	if(!ntfsAttr.uchNonResFlag)
	{// residence attribute, this always resides in the MFT table itself
		
		puchData = new BYTE[ntfsAttr.Attr.Resident.dwLength];
		dwDataLen = ntfsAttr.Attr.Resident.dwLength;

		memcpy(puchData,&m_pMFTRecord[dwCurPos+ntfsAttr.Attr.Resident.wAttrOffset],dwDataLen);
	}
	else
	{// non-residence attribute, this resides in the other part of the physical drive

		if(!ntfsAttr.Attr.NonResident.n64AllocSize) // i don't know Y, but fails when its zero
			ntfsAttr.Attr.NonResident.n64AllocSize = (ntfsAttr.Attr.NonResident.n64EndVCN - ntfsAttr.Attr.NonResident.n64StartVCN) + 1;

		// ATTR_STANDARD size may not be correct
		dwDataLen = ntfsAttr.Attr.NonResident.n64RealSize;

		// allocate for reading data
		puchData = new BYTE[ntfsAttr.Attr.NonResident.n64AllocSize];

		BYTE chLenOffSz; // length & offset sizes
		BYTE chLenSz; // length size
		BYTE chOffsetSz; // offset size
		LONGLONG n64Len, n64Offset; // the actual lenght & offset
		LONGLONG n64LCN =0; // the pointer pointing the actual data on a physical disk
		BYTE *pTmpBuff = puchData;
		int nRet;

		dwCurPos += ntfsAttr.Attr.NonResident.wDatarunOffset;;

		for(;;)
		{
			///// read the length of LCN/VCN and length ///////////////////////
			chLenOffSz = 0;

			memcpy(&chLenOffSz,&m_pMFTRecord[dwCurPos],sizeof(BYTE));

			dwCurPos += sizeof(BYTE);

			if(!chLenOffSz)
				break;

			chLenSz		= chLenOffSz & 0x0F;
			chOffsetSz	= (chLenOffSz & 0xF0) >> 4;

			///// read the data length ////////////////////////////////////////
	
			n64Len = 0;

			memcpy(&n64Len,&m_pMFTRecord[dwCurPos],chLenSz);

			dwCurPos += chLenSz;

			///// read the LCN/VCN offset //////////////////////////////////////
	
			n64Offset = 0;

			memcpy(&n64Offset,&m_pMFTRecord[dwCurPos],chOffsetSz);
			
			dwCurPos += chOffsetSz;

			////// if the last bit of n64Offset is 1 then its -ve so u got to make it -ve /////
			if((((char*)&n64Offset)[chOffsetSz-1])&0x80)
				for(int i=sizeof(LONGLONG)-1;i>(chOffsetSz-1);i--)
					((char*)&n64Offset)[i] = 0xff;
			
			n64LCN += n64Offset;
			
			n64Len *= m_dwBytesPerCluster;
			///// read the actual data /////////////////////////////////////////
			/// since the data is available out side the MFT table, physical drive should be accessed
			nRet = ReadRaw(n64LCN,pTmpBuff,(DWORD&)n64Len);
			if(nRet)
				return nRet;

			pTmpBuff += n64Len;
		}
	}
	return ERROR_SUCCESS;
}

// read the data from the physical drive
int CMFTRecord::ReadRaw(LONGLONG n64LCN, BYTE *chData, DWORD &dwLen)
{
	int nRet;

	LARGE_INTEGER n64Pos;
	
	n64Pos.QuadPart = (n64LCN)*m_dwBytesPerCluster;
	n64Pos.QuadPart += m_n64StartPos;

	//   data is available in the relative sector from the begining od the drive	
	//    so point that data
	nRet = SetFilePointer(m_hDrive,n64Pos.LowPart,&n64Pos.HighPart,FILE_BEGIN);
	if(nRet == 0xFFFFFFFF)
		return GetLastError();

	BYTE *pTmp			= chData;
	DWORD dwBytesRead	=0;
	DWORD dwBytes		=0;
	DWORD dwTotRead		=0;

	while(dwTotRead <dwLen)
	{
		// v r reading a cluster at a time
		dwBytesRead = m_dwBytesPerCluster;

		// this can not read partial sectors
		nRet = ReadFile(m_hDrive,pTmp,dwBytesRead,&dwBytes,NULL);
		if(!nRet)
			return GetLastError();

		dwTotRead += dwBytes;
		pTmp += dwBytes;
	}

	dwLen = dwTotRead;

	return ERROR_SUCCESS;
}



