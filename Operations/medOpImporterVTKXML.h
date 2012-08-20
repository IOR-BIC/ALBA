/*=========================================================================

 Program: MAF2Medical
 Module: medOpImporterVTKXML
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpImporterVTKXML_H__
#define __medOpImporterVTKXML_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medOperationsDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafString;
class mafVMEPointSet;
class medVMEPolylineGraph;
class mafVMESurface;
class mafVMEVolumeGray;
class mafVMEVolumeRGB;
class mafVMEMesh;
class mafVMEGeneric;

class vtkCallbackCommand;

/**
class name: medOpImporterVTKXMLXML
Operation to importer VTK XML format (.vtp extension)*/
class MED_OPERATION_EXPORT medOpImporterVTKXML: public mafOp 
{
public:
  /** constructor */
  medOpImporterVTKXML(const wxString &label = "medImporter VTK XML");
  /** destructor */
  ~medOpImporterVTKXML(); 

  /** RTTI macro*/
  mafTypeMacro(medOpImporterVTKXML, mafOp);

  /** clone the current object */
  /*virtual*/ mafOp* Copy();

  /** Builds operation's interface. */
  /*virtual*/ void OpRun();

  /** Return true for the acceptable vme type. */
  /*virtual*/ bool Accept(mafNode* node);

  /** Import VTK XML data. */
  virtual int ImportVTKXML();

  /** retrieve the file name*/
  mafString GetFileName(){return m_File;};

  /** Set the file name*/
  void SetFileName(mafString filename);

protected:

  /** Function to trace error events from the vtk readers.*/
  static void ErrorProcessEvents(vtkObject* sender, unsigned long channel, void* clientdata, void* calldata);

  /** Set m_ErrorCount to 0.*/
  static void ResetErrorCount();

  mafString m_File;
  mafString m_FileDir;

  mafVMEPointSet *m_VmePointSet;
  medVMEPolylineGraph *m_VmePolyLine;
  mafVMESurface *m_VmeSurface;
  mafVMEVolumeGray *m_VmeGrayVol;
  mafVMEVolumeRGB *m_VmeRGBVol;
  mafVMEMesh *m_VmeMesh;  
  mafVMEGeneric *m_VmeGeneric;

  vtkCallbackCommand *m_EventRouter;

};
#endif
