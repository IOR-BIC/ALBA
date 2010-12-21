/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterVTKXML.cpp,v $
Language:  C++
Date:      $Date: 2010-12-21 15:59:19 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2001/2005 
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
* Modified source versions must be plainly marked fas such, and must not be misrepresented 
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

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpImporterVTKXML.h"
#include <wx/busyinfo.h>

#include "medVMEPolylineGraph.h"

#include "mafVMEGeneric.h"
#include "mafVMEImage.h"
#include "mafVMEPointSet.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEVolumeRGB.h"
#include "mafVMEMesh.h"

#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkXMLImageDataReader.h"
#include "vtkMAFSmartPointer.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpImporterVTKXML);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpImporterVTKXML::medOpImporterVTKXML(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_Input   = NULL;

  m_VmePointSet = NULL;
  m_VmePolyLine = NULL;
  m_VmeSurface  = NULL;
  m_VmeGrayVol  = NULL;
  m_VmeRGBVol  = NULL;
  m_VmeMesh     = NULL;
  m_VmeGeneric  = NULL;

  m_File    = "";
  m_FileDir = "";
}
//----------------------------------------------------------------------------
medOpImporterVTKXML::~medOpImporterVTKXML()
//----------------------------------------------------------------------------
{
  mafDEL(m_VmePointSet);
  mafDEL(m_VmePolyLine);
  mafDEL(m_VmeSurface);
  mafDEL(m_VmeGrayVol);
  mafDEL(m_VmeRGBVol);
  mafDEL(m_VmeMesh);
  mafDEL(m_VmeGeneric);
}
//----------------------------------------------------------------------------
bool medOpImporterVTKXML::Accept(mafNode* node)   
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
mafOp* medOpImporterVTKXML::Copy()   
//----------------------------------------------------------------------------
{
  medOpImporterVTKXML *cp = new medOpImporterVTKXML(m_Label);
  cp->m_File = m_File;
  return cp;
}
//----------------------------------------------------------------------------
void medOpImporterVTKXML::OpRun()   
//----------------------------------------------------------------------------
{
  mafString wildc = "vtk xml Data (*.vtp)|*.vtp";
  mafString f;
  if (m_File.IsEmpty())
  {
    f = mafGetOpenFile(m_FileDir, wildc, _("Choose VTK XML file")).c_str();
    m_File = f;
  }

  int result = OP_RUN_CANCEL;
  if(!m_File.IsEmpty())
  {
    if (ImportVTKXML() == MAF_OK)
    {
      result = OP_RUN_OK;
    }
    else
    {
      if(!this->m_TestMode)
        mafMessage(_("Unsupported file format"), _("I/O Error"), wxICON_ERROR );
    }
  }
  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
int medOpImporterVTKXML::ImportVTKXML()
//----------------------------------------------------------------------------
{
  vtkXMLImageDataReader *imageReader;
  vtkNEW(imageReader);
  imageReader->SetFileName(m_File.GetCStr());
  imageReader->Update();

  if (imageReader->GetOutput()->IsTypeOf("vtkPolyData"))
  {
    return MAF_OK;
  }


  return MAF_ERROR;
}
