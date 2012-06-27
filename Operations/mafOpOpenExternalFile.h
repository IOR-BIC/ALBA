/*=========================================================================

 Program: MAF2
 Module: mafOpOpenExternalFile
 Authors: Stefano Perticoni    Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpOpenExternalFile_H__
#define __mafOpOpenExternalFile_H__


#include "mafOp.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafOp;
class mafEvent;

//----------------------------------------------------------------------------
// mafOpOpenExternalFile :
//----------------------------------------------------------------------------
/** Open with external programs the data previously imported with the mafOpImporterExternalFile */
class MAF_EXPORT mafOpOpenExternalFile: public mafOp
{
public:
  mafOpOpenExternalFile(const wxString &label = "OpenExternalFile");
 ~mafOpOpenExternalFile();

 mafTypeMacro(mafOpOpenExternalFile, mafOp);

 mafOp* Copy();

 /** Return true for the acceptable vme type. */
 bool Accept(mafNode* node);

 /** Builds operation's interface. */
 void OpRun();

 /** Return the "pid" of the wxExecute() */
 long GetPid();

private:
  long m_Pid;
};
#endif
