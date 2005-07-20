/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMSFImporter.h,v $
  Language:  C++
  Date:      $Date: 2005-07-20 12:14:29 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoMSFImporter_H__
#define __mmoMSFImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMEPolyline;
class mafVMESurface;
class mafVMEVolumeGray;
class mafVMEGeneric;
class mafMSFImporter;

//----------------------------------------------------------------------------
// mmoMSFImporter :
//----------------------------------------------------------------------------
/** */
class mmoMSFImporter: public mafOp
{
public:
  mmoMSFImporter(wxString label);
 ~mmoMSFImporter(); 
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
