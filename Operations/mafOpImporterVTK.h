/*=========================================================================

 Program: MAF2
 Module: mafOpImporterVTK
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class mafVME;
class mafVMELandmarkCloud;
//class mafVMEPointSet;
class mafVMEPolyline;
class mafVMESurface;
class mafVMEVolumeGray;
class mafVMEVolumeRGB;
class mafVMEMesh;
class mafVMEGeneric;
class mafVMEImage;

//----------------------------------------------------------------------------
// mafOpImporterVTK :
//----------------------------------------------------------------------------
/** Import operation that try to read VTK data file and set it into the corresponding VME 
that accept the VTK data format. If no VME can accept the format a message box will be shown 
to the user and no data will be imported.*/
class MAF_EXPORT mafOpImporterVTK: public mafOp 
{
public:
  mafOpImporterVTK(const wxString &label = "VTKImporter");
 ~mafOpImporterVTK(); 
  
  mafTypeMacro(mafOpImporterVTK, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafVME* node) {return true;};

	/** Builds operation's interface. */
  void OpRun();

		/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
		virtual void OpStop(int result);

	/** Import vtk data, return MAF_OK on success. */
  virtual int ImportVTK();

  /** Set the vtk filename to be imported. 
      This is used when the operation is executed not using user interface. */
  void SetFileName(const char *name) {m_File = name;};

	/** retrieve the file name*/
	wxString GetFileName() { return m_File; };
protected:
  wxString m_File;
  wxString m_FileDir;
  
  mafVMELandmarkCloud *m_VmeLandmarkCloud;
  //mafVMEPointSet   *m_VmePointSet;
  mafVMEPolyline   *m_VmePolyLine;
  mafVMESurface    *m_VmeSurface;
  mafVMEImage      *m_VmeImage;
  mafVMEVolumeGray *m_VmeGrayVol;
  mafVMEVolumeRGB *m_VmeRGBVol;
  mafVMEMesh       *m_VmeMesh;  
  mafVMEGeneric    *m_VmeGeneric;
};
#endif
