/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterRawMotionData.h,v $
  Language:  C++
  Date:      $Date: 2010-10-07 06:54:49 $
  Version:   $Revision: 1.1.2.1 $
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
/** 
class name: medOpImporterRawMotionData
raw motion data are represented from landmark cloud, and this importer will create a landmark cloud with childs.
*/
class medOpImporterRawMotionData : public mafOp
{
public:
  /** constructor */
	medOpImporterRawMotionData(wxString label);
  /** destructor */
	~medOpImporterRawMotionData();
  /** clone the current object */
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
