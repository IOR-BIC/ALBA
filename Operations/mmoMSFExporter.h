/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMSFExporter.h,v $
  Language:  C++
  Date:      $Date: 2005-09-19 11:35:58 $
  Version:   $Revision: 1.1 $
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
  mmoMSFExporter(wxString label);
 ~mmoMSFExporter(); 
  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *vme) { return vme!= NULL;};

	/** Builds operation's interface. */
  void OpRun();

protected:
  /** Export selected subtree-tree to a .msf file*/
  void ExportMSF();

  mafString m_MSFFile;
	mafString m_MSFFileDir;
};
#endif
