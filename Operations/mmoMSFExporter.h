/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMSFExporter.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoMSFExporter_H__
#define __mmoMSFExporter_H__

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
// mmoMSFExporter :
//----------------------------------------------------------------------------
/** */
class mmoMSFExporter: public mafOp
{
public:
  mmoMSFExporter(const wxString &label = "MSFExporter");
 ~mmoMSFExporter(); 
  
  mafTypeMacro(mmoMSFExporter, mafOp);

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
