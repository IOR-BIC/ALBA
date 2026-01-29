/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterVTK
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterVTK_H__
#define __albaOpImporterVTK_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOpImporterFile.h"

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
class vtkUnstructuredGrid;

//----------------------------------------------------------------------------
// albaOpImporterVTK :
//----------------------------------------------------------------------------
/** Import operation that try to read VTK data file and set it into the corresponding VME 
that accept the VTK data format. If no VME can accept the format a message box will be shown 
to the user and no data will be imported.*/
class ALBA_EXPORT albaOpImporterVTK : public albaOpImporterFile
{
public:
	albaOpImporterVTK(const wxString &label = "VTKImporter");
	~albaOpImporterVTK();

	albaTypeMacro(albaOpImporterVTK, albaOpImporterFile);

	albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop(int result);

	/** Import VTK data, return ALBA_OK on success. */
	virtual int ImportFile();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

	wxString m_FileDir;

	albaVMELandmarkCloud *m_VmeLandmarkCloud;
	//albaVMEPointSet   *m_VmePointSet;
	albaVMEPolyline   *m_VmePolyLine;
	albaVMESurface    *m_VmeSurface;
	albaVMEImage      *m_VmeImage;
	albaVMEVolumeGray *m_VmeGrayVol;
	albaVMEVolumeRGB *m_VmeRGBVol;
	albaVMEMesh       *m_VmeMesh;
	albaVMEGeneric    *m_VmeGeneric;
private:
	void CheckAndAddIDsToUnstructuredGrid(vtkUnstructuredGrid* ug);
};
#endif