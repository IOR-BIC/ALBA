/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoSTLExporter.h,v $
  Language:  C++
  Date:      $Date: 2005-06-21 11:35:30 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani     
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoSTLExporter_H__
#define __mmoSTLExporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;

//----------------------------------------------------------------------------
// mmoSTLExporter :
//----------------------------------------------------------------------------
/** */
class mmoSTLExporter: public mafOp
{
public:
  mmoSTLExporter(wxString label);
 ~mmoSTLExporter(); 
  mafOp* Copy();
	void OnEvent(mafEventBase *maf_event);

 	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();

	/** Export the surface. */
  void ExportSurface();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  mafString  m_File;
	mafString  m_FileDir;
	int				 m_Binary;
	int				 m_ABSMatrixFlag;
};
#endif
