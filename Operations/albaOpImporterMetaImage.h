/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMetaImage
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterMetaImage_H__
#define __albaOpImporterMetaImage_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMELandmarkCloud;
//class albaVMEPointSet;
class albaVMEPolyline;
class albaVMESurface;
class albaVMEVolumeGray;
class albaVMEVolumeRGB;
class albaVMEMesh;
class albaVMEGeneric;
class albaVMEImage;

//----------------------------------------------------------------------------
// albaOpImporterMetaImage :
//----------------------------------------------------------------------------
/** Import operation that try to read VTK data file and set it into the corresponding VME 
that accept the VTK data format. If no VME can accept the format a message box will be shown 
to the user and no data will be imported.*/
class ALBA_EXPORT albaOpImporterMetaImage: public albaOp 
{
public:
  albaOpImporterMetaImage(const wxString &label = "Importer Meta Image");
 ~albaOpImporterMetaImage(); 
  
  albaTypeMacro(albaOpImporterMetaImage, albaOp);

  albaOp* Copy();

	/** Builds operation's interface. */
  void OpRun();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop(int result);

	/** Import vtk data, return ALBA_OK on success. */
  virtual int ImportMetaImage();

  /** Set the vtk filename to be imported. 
      This is used when the operation is executed not using user interface. */
  void SetFileName(const char *name) {m_File = name;};

	/** retrieve the file name*/
	wxString GetFileName() { return m_File; };

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  wxString m_File;
  wxString m_FileDir;
  
  albaVMEImage      *m_VmeImage;
  albaVMEVolumeGray *m_VmeGrayVol;
};
#endif
