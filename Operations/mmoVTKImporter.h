/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVTKImporter.h,v $
  Language:  C++
  Date:      $Date: 2005-05-31 23:54:49 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoVTKImporter_H__
#define __mmoVTKImporter_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVME;

//----------------------------------------------------------------------------
// mmoVTKImporter :
//----------------------------------------------------------------------------
/** */
class mmoVTKImporter: public mafOp
{
public:
  mmoVTKImporter(wxString label);
 ~mmoVTKImporter(); 
  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

  /** Set the vtk filename to be imported. 
      This is used when the operation is executed not using user interface. */
  void SetFileName(const char *name) {m_File = name;};

protected:
  wxString m_File;
  wxString m_FileDir;

  mafVME   *m_Vme;
};
#endif
