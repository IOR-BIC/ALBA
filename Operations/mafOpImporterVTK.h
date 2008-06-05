/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpImporterVTK.h,v $
  Language:  C++
  Date:      $Date: 2008-06-05 15:52:50 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpImporterVTK_H__
#define __mafOpImporterVTK_H__

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
class mafVMEVolumeRGB;
class mafVMEMesh;
class mafVMEGeneric;

//----------------------------------------------------------------------------
// mafOpImporterVTK :
//----------------------------------------------------------------------------
/** Import operation that try to read VTK data file and set it into the corresponding VME 
that accept the VTK data format. If no VME can accept the format a message box will be shown 
to the user and no data will be imported.*/
class mafOpImporterVTK: public mafOp 
{
public:
  mafOpImporterVTK(const wxString &label = "VTKImporter");
 ~mafOpImporterVTK(); 
  
  mafTypeMacro(mafOpImporterVTK, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
  void OpRun();

	/** Import vtk data, return MAF_OK on success. */
  virtual int ImportVTK();

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
  mafVMEVolumeRGB *m_VmeRGBVol;
  mafVMEMesh       *m_VmeMesh;  
  mafVMEGeneric    *m_VmeGeneric;
};
#endif
