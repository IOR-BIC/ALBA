/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMSF1xImporter.h,v $
  Language:  C++
  Date:      $Date: 2005-10-21 10:06:07 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoMSF1xImporter_H__
#define __mmoMSF1xImporter_H__

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
// mmoMSF1xImporter :
//----------------------------------------------------------------------------
/** */
class mmoMSF1xImporter: public mafOp
{
public:
  mmoMSF1xImporter(wxString label);
 ~mmoMSF1xImporter(); 
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
