/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpImporterMSF1x.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
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
class mafOpImporterMSF1x: public mafOp
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
