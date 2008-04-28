/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterRawMotionData.h,v $
  Language:  C++
  Date:      $Date: 2008-04-28 08:47:16 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpImporterRawMotionData_H__
#define __medOpImporterRawMotionData_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;                      //era mflVME
class mafEvent;
//----------------------------------------------------------------------------
// medOpImporterRawMotionData :
//----------------------------------------------------------------------------
/** */
class medOpImporterRawMotionData : public mafOp
{
public:
	medOpImporterRawMotionData(wxString label);
	~medOpImporterRawMotionData(); 
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	//void OpUndo();                       // gia' implementata in mafOp

protected:
  wxString m_FileDir;
	wxString m_DictDir;
	wxString m_File;
	wxString m_Dict;
	mafVME  *m_Vme; 						// era mflVME *m_vme
	int m_DictionaryAvailable;
};
#endif
