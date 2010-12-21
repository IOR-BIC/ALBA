/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpImporterVTKXML.cpp,v $
Language:  C++
Date:      $Date: 2010-12-21 16:55:01 $
Version:   $Revision: 1.1.2.3 $
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
#include "mafTagArray.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkUnstructuredGrid.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkXMLImageDataReader.h"
#include "vtkXMLRectilinearGridReader.h"
#include "vtkXMLUnstructuredGridReader.h"
#include "vtkCallbackCommand.h"
#include "vtkMAFSmartPointer.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpImporterVTKXML);
//----------------------------------------------------------------------------

static int m_ErrorCount = 0;

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

  m_EventRouter = NULL;
  vtkNEW(m_EventRouter);
  m_EventRouter->SetCallback(ErrorProcessEvents);
  m_EventRouter->SetClientData(this);

  m_File    = "";
  m_FileDir = "";

  ResetErrorCount();
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

  vtkDEL(m_EventRouter);
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
      wxString path, name, ext;
      wxSplitPath(m_File.GetCStr(),&path,&name,&ext);
      mafTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");
      m_Output->GetTagArray()->SetTag(tag_Nature);
      m_Output->ReparentTo(m_Input);
      m_Output->SetName(name.c_str());

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
void medOpImporterVTKXML::ErrorProcessEvents(vtkObject* sender, unsigned long channel, void* clientdata, void* calldata)
//----------------------------------------------------------------------------
{
  m_ErrorCount++;
}
//----------------------------------------------------------------------------
void medOpImporterVTKXML::ResetErrorCount()
//----------------------------------------------------------------------------
{
  m_ErrorCount = 0;
}
//----------------------------------------------------------------------------
int medOpImporterVTKXML::ImportVTKXML()
//----------------------------------------------------------------------------
{
  vtkDataSet *data = NULL;

  mafNEW(m_VmePointSet);
  mafNEW(m_VmePolyLine);
  mafNEW(m_VmeSurface);
  mafNEW(m_VmeGrayVol);
  mafNEW(m_VmeRGBVol);
  mafNEW(m_VmeMesh);
  mafNEW(m_VmeGeneric);

  vtkMAFSmartPointer<vtkXMLImageDataReader> imageReader;
  imageReader->AddObserver(vtkCommand::ErrorEvent,m_EventRouter);
  imageReader->SetFileName(m_File.GetCStr());
  imageReader->Update();

  if (m_ErrorCount == 0)
  {
    data = imageReader->GetOutput();

    if (m_VmeGrayVol->SetDataByDetaching(data,0) == MAF_OK)
    {
      m_Output = m_VmeGrayVol;
    }
    else if (m_VmeRGBVol->SetDataByDetaching(data,0) == MAF_OK)
    {
      m_Output = m_VmeRGBVol;
    }
    else
    {
      m_VmeGeneric->SetDataByDetaching(data,0);
      m_Output = m_VmeGeneric;
    }

    return MAF_OK;   
  }

  ResetErrorCount();

	vtkMAFSmartPointer<vtkXMLPolyDataReader> polydataReader;
	polydataReader->AddObserver(vtkCommand::ErrorEvent,m_EventRouter);
	polydataReader->SetFileName(m_File.GetCStr());
	polydataReader->Update();

	if (m_ErrorCount == 0)
	{
	  data = polydataReader->GetOutput();

    if (m_VmePointSet->SetDataByDetaching(data,0) == MAF_OK)
    {
      m_Output = m_VmePointSet;
    }
    else if (m_VmePolyLine->SetDataByDetaching(data,0) == MAF_OK)
    {
      m_Output = m_VmePolyLine;
    }
    else if (m_VmeSurface->SetDataByDetaching(data,0) == MAF_OK)
    {
      m_Output = m_VmeSurface;
    }
    else
    {
      m_VmeGeneric->SetDataByDetaching(data,0);
      m_Output = m_VmeGeneric;
    }

    return MAF_OK;
	}

	ResetErrorCount();

  vtkMAFSmartPointer<vtkXMLRectilinearGridReader> rgReader;
  rgReader->AddObserver(vtkCommand::ErrorEvent,m_EventRouter);
  rgReader->SetFileName(m_File.GetCStr());
  rgReader->Update();

  if (m_ErrorCount == 0)
  {
    data = rgReader->GetOutput();

    if (m_VmeGrayVol->SetDataByDetaching(data,0) == MAF_OK)
    {
      m_Output = m_VmeGrayVol;
    }
    else if (m_VmeRGBVol->SetDataByDetaching(data,0) == MAF_OK)
    {
      m_Output = m_VmeRGBVol;
    }
    else
    {
      m_VmeGeneric->SetDataByDetaching(data,0);
      m_Output = m_VmeGeneric;
    }

    return MAF_OK;   
  }

  ResetErrorCount();

  vtkMAFSmartPointer<vtkXMLUnstructuredGridReader> ugReader;
  ugReader->AddObserver(vtkCommand::ErrorEvent,m_EventRouter);
  ugReader->SetFileName(m_File.GetCStr());
  ugReader->Update();

  if (m_ErrorCount == 0)
  {
    data = ugReader->GetOutput();

    if (m_VmeMesh->SetDataByDetaching(data,0) == MAF_OK)
    {
      m_Output = m_VmeMesh;
    }
    else
    {
      m_VmeGeneric->SetDataByDetaching(data,0);
      m_Output = m_VmeGeneric;
    }

    return MAF_OK;   
  }

  ResetErrorCount();


  return MAF_ERROR;
}
