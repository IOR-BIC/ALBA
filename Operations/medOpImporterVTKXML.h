/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterVTKXML.h,v $
Language:  C++
Date:      $Date: 2012-04-06 09:15:55 $
Version:   $Revision: 1.1.2.5 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
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
