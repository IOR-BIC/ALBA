/*=========================================================================

 Program: MAF2Medical
 Module: medOpImporterVTK
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpImporterVTK.h"
#include <wx/busyinfo.h>

#include "mafVME.h"
#include "medVMEPolylineGraph.h"

#include "mafTagArray.h"

#include "vtkDataSet.h"
#include "vtkDataSetReader.h"
#include "vtkPolyDataReader.h"
#include "vtkMAFSmartPointer.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpImporterVTK);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpImporterVTK::medOpImporterVTK(const wxString &label) :
mafOpImporterVTK(label)
//----------------------------------------------------------------------------
{
  m_VmePolyLine = NULL;
}
//----------------------------------------------------------------------------
medOpImporterVTK::~medOpImporterVTK()
//----------------------------------------------------------------------------
{
  mafDEL(m_VmePolyLine);
}
//----------------------------------------------------------------------------
mafOp* medOpImporterVTK::Copy()   
//----------------------------------------------------------------------------
{
  medOpImporterVTK *cp = new medOpImporterVTK(m_Label);
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
int medOpImporterVTK::ImportVTK()
//----------------------------------------------------------------------------
{
  bool success = false;
  if(!this->m_TestMode)
    wxBusyInfo wait(_("Loading file: ..."));

  vtkMAFSmartPointer<vtkDataSetReader> reader;
  reader->SetFileName(m_File);

  int canRead=TRUE;
  vtkDataReader *preader = NULL;
  // workaround to avoid double reading
  switch (reader->ReadOutputType())
  {
  case VTK_POLY_DATA:
    preader = vtkPolyDataReader::New();
    break;
  default:
    return mafOpImporterVTK::ImportVTK();
  }
    
  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,preader));
  preader->SetFileName(m_File);
  preader->Update();

  if (preader->GetNumberOfOutputs()>0)
  {
    wxString path, name, ext;
    wxSplitPath(m_File.c_str(),&path,&name,&ext);

    vtkDataSet *data = vtkDataSet::SafeDownCast(preader->GetOutputs()[0]);
    if (data)
    {
      mafNEW(m_VmePolyLine);
      if (m_VmePolyLine->SetDataByDetaching(data,0) == MAF_OK)
      {
        m_Output = m_VmePolyLine;
      }
      else
      {
        vtkDEL(preader);
        return mafOpImporterVTK::ImportVTK();
      }

      mafTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");
      m_Output->GetTagArray()->SetTag(tag_Nature);
      m_Output->SetName(name.c_str());

      success = true;
    }
  }
  vtkDEL(preader);
  if(!success && !this->m_TestMode)
    mafMessage(_("Error reading VTK file."), _("I/O Error"), wxICON_ERROR );

  return MAF_OK;
}
