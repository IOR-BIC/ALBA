/*=========================================================================

 Program: MAF2
 Module: mafOpImporterMSF1x
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterMSF1x_H__
#define __mafOpImporterMSF1x_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafMSFImporter;

//----------------------------------------------------------------------------
// mafOpImporterMSF1x :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpImporterMSF1x: public mafOp
{
public:
  mafOpImporterMSF1x(const wxString &label = "MSF1xImporter");
 ~mafOpImporterMSF1x(); 
  
  mafTypeMacro(mafOpImporterMSF1x, mafOp);

  mafOp* Copy();

	/** this operation does not depend on the selected node */
  bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
  void OpRun();

	/** Import MSF file. */
  void ImportMSF();

  /** Set the MSF filename to be imported. 
      This is used when the operation is executed not using user interface. */
  void SetFileName(const char *name) {m_File = name;};

protected:
  mafString m_File;
  mafString m_FileDir;
  
  mafMSFImporter *m_Importer;
};
#endif
