/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterVTKXML
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterVTKXML_H__
#define __albaOpImporterVTKXML_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOpImporterFile.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaString;
class albaVMEPointSet;
class albaVMEPolylineGraph;
class albaVMESurface;
class albaVMEVolumeGray;
class albaVMEVolumeRGB;
class albaVMEMesh;
class albaVMEGeneric;

class vtkCallbackCommand;

/**
class name: albaOpImporterVTKXMLXML
Operation to importer VTK XML format (.vtp extension)*/
class ALBA_EXPORT albaOpImporterVTKXML: public albaOpImporterFile 
{
public:
  /** constructor */
  albaOpImporterVTKXML(const wxString &label = "medImporter VTK XML");
  /** destructor */
  ~albaOpImporterVTKXML(); 

  /** RTTI macro*/
  albaTypeMacro(albaOpImporterVTKXML, albaOpImporterFile);

  /** clone the current object */
  /*virtual*/ albaOp* Copy();

  /** Builds operation's interface. */
  /*virtual*/ void OpRun();

  /** Import VTK XML data. */
  virtual int ImportFile();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** Function to trace error events from the vtk readers.*/
  static void ErrorProcessEvents(vtkObject* sender, unsigned long channel, void* clientdata, void* calldata);

  /** Set m_ErrorCount to 0.*/
  static void ResetErrorCount();

  albaString m_FileDir;

  albaVMEPointSet *m_VmePointSet;
  albaVMEPolylineGraph *m_VmePolyLine;
  albaVMESurface *m_VmeSurface;
  albaVMEVolumeGray *m_VmeGrayVol;
  albaVMEVolumeRGB *m_VmeRGBVol;
  albaVMEMesh *m_VmeMesh;  
  albaVMEGeneric *m_VmeGeneric;

  vtkCallbackCommand *m_EventRouter;

};
#endif
