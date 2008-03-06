/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpExporterMSF.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpExporterMSF_H__
#define __mafOpExporterMSF_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafEvent;

//----------------------------------------------------------------------------
// mafOpExporterMSF :
//----------------------------------------------------------------------------
/** */
class mafOpExporterMSF: public mafOp
{
public:
  mafOpExporterMSF(const wxString &label = "MSFExporter");
 ~mafOpExporterMSF(); 
  
  mafTypeMacro(mafOpExporterMSF, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *vme) { return vme!= NULL;};

	/** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .msf to export */
  void SetFileName(const char *file_name) {m_MSFFile = file_name;};

  /** Export selected subtree-tree to a .msf file*/
  void ExportMSF();

protected:
  mafString m_MSFFile;
	mafString m_MSFFileDir;
};
#endif
