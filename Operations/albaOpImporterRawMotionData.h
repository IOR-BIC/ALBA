/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)Medical
 Module: medOpImporterRawMotionData
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpImporterRawMotionData_H__
#define __medOpImporterRawMotionData_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;                      //era mflVME
class albaEvent;
//----------------------------------------------------------------------------
// medOpImporterRawMotionData :
//----------------------------------------------------------------------------
/** 
class name: medOpImporterRawMotionData
raw motion data are represented from landmark cloud, and this importer will create a landmark cloud with childs.
*/
class medOpImporterRawMotionData : public albaOp
{
public:
  /** constructor */
	medOpImporterRawMotionData(wxString label);
  /** destructor */
	~medOpImporterRawMotionData();
  /** clone the current object */
	albaOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(albaNode* node) {return true;};

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	//void OpUndo();                       // gia' implementata in albaOp

protected:
  wxString m_FileDir;
	wxString m_DictDir;
	wxString m_File;
	wxString m_Dict;
	albaVME  *m_Vme; 						// era mflVME *m_vme
	int m_DictionaryAvailable;
};
#endif
