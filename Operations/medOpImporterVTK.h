/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterVTK.h,v $
Language:  C++
Date:      $Date: 2008-01-31 16:43:15 $
Version:   $Revision: 1.1 $
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

#ifndef __medOpImporterVTK_H__
#define __medOpImporterVTK_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMEPointSet;
class medVMEPolylineGraph;
class mafVMESurface;
class mafVMEVolumeGray;
class mafVMEMesh;
class mafVMEGeneric;

//----------------------------------------------------------------------------
// medOpImporterVTK :
//----------------------------------------------------------------------------
/**The same importer of the MAF , but polylines are stored inside 
the medVMEPolylineGraph instead mafVMEPolyline*/
class medOpImporterVTK: public mafOp 
{
public:
  medOpImporterVTK(const wxString &label = "medVTKImporter");
  ~medOpImporterVTK(); 

  mafTypeMacro(medOpImporterVTK, mafOp);

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
  wxString GetFileName(){return m_File;};

protected:
  wxString m_File;
  wxString m_FileDir;

  mafVMEPointSet        *m_VmePointSet;
  medVMEPolylineGraph   *m_VmePolyLine;
  mafVMESurface         *m_VmeSurface;
  mafVMEVolumeGray      *m_VmeGrayVol;
  mafVMEMesh            *m_VmeMesh;  
  mafVMEGeneric         *m_VmeGeneric;
};
#endif
