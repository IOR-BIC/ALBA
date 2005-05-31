/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVTKExporter.h,v $
  Language:  C++
  Date:      $Date: 2005-05-31 09:49:12 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoVTKExporter_H__
#define __mmoVTKExporter_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafEvent;

//----------------------------------------------------------------------------
// mmoVTKExporter :
//----------------------------------------------------------------------------
/** */
class mmoVTKExporter: public mafOp
{
public:
  mmoVTKExporter(wxString label);
 ~mmoVTKExporter(); 
  mafOp* Copy();
	void OnEvent(mafEventBase *event);

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

  void SaveVTKData();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  mafString  m_File;
	mafString  m_FileDir;
  mafVME   *m_Vme; 
	int				m_Binary;
	int				m_ABSMatrixFlag;
};
#endif
