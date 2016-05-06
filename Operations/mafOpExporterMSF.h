/*=========================================================================

 Program: MAF2
 Module: mafOpExporterMSF
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class mafVME;
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
  bool Accept(mafVME*vme);

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
