/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpOpenExternalFile
 Authors: Stefano Perticoni    Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpOpenExternalFile_H__
#define __albaOpOpenExternalFile_H__


#include "albaOp.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaOp;
class albaEvent;

//----------------------------------------------------------------------------
// albaOpOpenExternalFile :
//----------------------------------------------------------------------------
/** Open with external programs the data previously imported with the albaOpImporterExternalFile */
class ALBA_EXPORT albaOpOpenExternalFile: public albaOp
{
public:
  albaOpOpenExternalFile(const wxString &label = "OpenExternalFile");
 ~albaOpOpenExternalFile();

 albaTypeMacro(albaOpOpenExternalFile, albaOp);

 albaOp* Copy();

 /** Builds operation's interface. */
 void OpRun();

 /** Return the "pid" of the wxExecute() */
 long GetPid();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

private:
  long m_Pid;
};
#endif
