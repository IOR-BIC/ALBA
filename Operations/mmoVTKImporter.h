/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVTKImporter.h,v $
  Language:  C++
  Date:      $Date: 2005-07-14 12:33:00 $
  Version:   $Revision: 1.6 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoVTKImporter_H__
#define __mmoVTKImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMEPointSet;
class mafVMEPolyline;
class mafVMESurface;
class mafVMEVolumeGray;
class mafVMEGeneric;

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

	/** Import vtk data. */
  void ImportVTK();

  /** Set the vtk filename to be imported. 
      This is used when the operation is executed not using user interface. */
  void SetFileName(const char *name) {m_File = name;};

protected:
  wxString m_File;
  wxString m_FileDir;
  
  mafVMEPointSet   *m_VmePointSet;
  mafVMEPolyline   *m_VmePolyLine;
  mafVMESurface    *m_VmeSurface;
  mafVMEVolumeGray *m_VmeGrayVol;
  mafVMEGeneric    *m_VmeGeneric;
};
#endif
