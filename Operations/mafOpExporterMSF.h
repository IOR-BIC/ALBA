/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpExporterMSF.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 11:49:23 $
  Version:   $Revision: 1.1.2.3 $
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
class MAF_EXPORT mafOpExporterMSF: public mafOp
{
public:
  mafOpExporterMSF(const wxString &label = "MSFExporter");
 ~mafOpExporterMSF(); 
  
  mafTypeMacro(mafOpExporterMSF, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *vme);

	/** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .msf to export */
  void SetFileName(const char *file_name) {m_MSFFile = file_name;};

  /** Get the filename for the .msf to export */
  mafString GetFileName() {return m_MSFFile;};

  /** Export selected subtree-tree to a .msf file*/
  int ExportMSF();

  typedef struct 
  {
    int oldID;
    int newID;
  } idValues;

protected:
  mafString m_MSFFile;
	mafString m_MSFFileDir;
};
#endif
