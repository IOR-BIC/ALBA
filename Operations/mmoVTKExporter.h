/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVTKExporter.h,v $
  Language:  C++
  Date:      $Date: 2005-10-11 12:32:55 $
  Version:   $Revision: 1.5 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoVTKExporter_H__
#define __mmoVTKExporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;

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
	void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();

	/** Export vtk data. */
  void ExportVTK();

  /** vtk pipeline to save data on disk. */
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
