/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpSegmentationRegionGrowingLocalAndGlobalThreshold.cpp,v $
Language:  C++
Date:      $Date: 2009-10-21 11:55:58 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2009
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

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpSegmentationRegionGrowingLocalAndGlobalThreshold.h"
#include "wx/busyinfo.h"

#include "medDecl.h"
#include "mafGUI.h"
#include "mafNode.h"
#include "mafVMEVolumeGray.h"
#include "mafGUILutSlider.h"
#include "mafGUIHistogramWidget.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMEDRegionGrowingLocalGlobalThreshold.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkMAFHistogram.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpSegmentationRegionGrowingLocalAndGlobalThreshold);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpSegmentationRegionGrowingLocalAndGlobalThreshold::medOpSegmentationRegionGrowingLocalAndGlobalThreshold(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;
  m_InputPreserving = false;

  m_LowerThreshold = 10000;
  m_UpperThreshold = 50000;
  m_LowerLabel = -1;
  m_UpperLabel = 1;

  m_VolumeOutput = NULL;
  
}
//----------------------------------------------------------------------------
medOpSegmentationRegionGrowingLocalAndGlobalThreshold::~medOpSegmentationRegionGrowingLocalAndGlobalThreshold()
//----------------------------------------------------------------------------
{
  mafDEL(m_VolumeOutput);
}
//----------------------------------------------------------------------------
mafOp* medOpSegmentationRegionGrowingLocalAndGlobalThreshold::Copy()
//----------------------------------------------------------------------------
{
  /** return a copy of itself, needs to put it into the undo stack */
  return new medOpSegmentationRegionGrowingLocalAndGlobalThreshold(m_Label);
}
//----------------------------------------------------------------------------
bool medOpSegmentationRegionGrowingLocalAndGlobalThreshold::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  return vme && vme->IsA("mafVMEVolumeGray");
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingLocalAndGlobalThreshold::OpRun()
//----------------------------------------------------------------------------
{
  m_Volume = mafVMEVolumeGray::SafeDownCast(m_Input);
  mafNEW(m_VolumeOutput);

  double sr[2];
  m_Volume->GetOutput()->GetVTKData()->GetScalarRange(sr);

  m_UpperThreshold = sr[1];
  m_LowerThreshold = sr[0];

  CreateGui();
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingLocalAndGlobalThreshold::LocalSegmentation()
//----------------------------------------------------------------------------
{
  vtkImageData *imageData = vtkImageData::SafeDownCast(m_Volume->GetOutput()->GetVTKData());
  imageData->Update();
  int dims[3];
  double spacing[3];
  imageData->GetDimensions(dims);
  imageData->GetSpacing(spacing);
  
  vtkMAFSmartPointer<vtkMEDRegionGrowingLocalGlobalThreshold> localFilter;
  localFilter->SetInput(imageData);
  localFilter->SetLowerLabel(m_LowerLabel);
  localFilter->SetLowerThreshold(m_LowerThreshold);
  localFilter->SetUpperLabel(m_UpperLabel);
  localFilter->SetUpperThreshold(m_UpperThreshold);
  localFilter->Update();

  m_VolumeOutput->SetData(vtkImageData::SafeDownCast(localFilter->GetOutput()),m_Volume->GetTimeStamp());
  m_VolumeOutput->SetName(_("Segmentation Output"));
  m_VolumeOutput->Update();

  m_Output = m_VolumeOutput;
  
}
//----------------------------------------------------------------------------
// widget ID's
//----------------------------------------------------------------------------
enum REGION_GROWING_ID
{
  ID_TO_START = MINID,
  ID_LOWER_THRESHOLD,
  ID_UPPER_THRESHOLD,
  ID_LOWER_LABEL,
  ID_UPPER_LABEL,
  ID_SEGMENTATION,
  ID_SLIDER_LABELS,
  ID_SLIDER_THRESHOLD,
  ID_ROLLOUT_LABELS,
};
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingLocalAndGlobalThreshold::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
//   m_Gui->Label(_("Lower threshold"));
//   m_Gui->Integer(ID_LOWER_THRESHOLD,"",&m_LowerThreshold);
//   m_Gui->Label(_("Upper threshold"));
//   m_Gui->Integer(ID_UPPER_THRESHOLD,"",&m_UpperThreshold);

  m_Gui->Divider(1);
  wxBoxSizer *sizer1 = new wxBoxSizer(wxHORIZONTAL);
  m_SliderThresholds = new mafGUILutSlider(m_Gui,ID_SLIDER_LABELS ,wxPoint(0,0),wxSize(10,24), 0, "Thresholds");
  m_SliderThresholds->SetListener(this);
  m_SliderThresholds->SetSize(5,24);
  m_SliderThresholds->SetMinSize(wxSize(5,24));
  m_SliderThresholds->SetRange(m_LowerThreshold,m_UpperThreshold);
  m_SliderThresholds->SetSubRange(m_LowerThreshold,m_UpperThreshold);

  sizer1->Add(m_SliderThresholds,wxALIGN_CENTER|wxRIGHT);
  m_Gui->Add(sizer1);
  m_Gui->Divider(1);

  wxBoxSizer *sizer3 = new wxBoxSizer(wxHORIZONTAL);
  mafGUIHistogramWidget *histogram = new mafGUIHistogramWidget(m_Gui,-1,wxPoint(0,0),wxSize(20,20));
  histogram->SetListener(m_Gui);
  histogram->SetRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);
  vtkImageData *hd = vtkImageData::SafeDownCast(m_Volume->GetOutput()->GetVTKData());
  histogram->SetData(hd->GetPointData()->GetScalars());

  sizer3->Add(histogram,wxALIGN_CENTER|wxRIGHT);
  m_Gui->Add(sizer3,1);

  m_Gui->Divider(1);

  m_GuiLabels = new mafGUI(this);
  m_Gui->RollOut(ID_ROLLOUT_LABELS, _("Labels"), m_GuiLabels, false);

  m_LowerLabel = VTK_UNSIGNED_SHORT_MIN;
  m_UpperLabel = VTK_UNSIGNED_SHORT_MAX;

  wxBoxSizer *sizer2 = new wxBoxSizer(wxHORIZONTAL);
  m_SliderLabels = new mafGUILutSlider(m_GuiLabels,ID_SLIDER_LABELS ,wxPoint(0,0),wxSize(10,24), 0, "Label");
  m_SliderLabels->SetListener(this);
  m_SliderLabels->SetSize(5,24);
  m_SliderLabels->SetMinSize(wxSize(5,24));
  m_SliderLabels->SetRange(m_LowerLabel,m_UpperLabel);
  m_SliderLabels->SetSubRange(m_LowerLabel,m_UpperLabel);
  //m_SliderLabels->EnableMiddleButton(false);

  sizer2->Add(m_SliderLabels,wxALIGN_CENTER|wxRIGHT);

  m_GuiLabels->Add(sizer2);
  m_GuiLabels->FitGui();

//   m_Gui->Label(_("Lower label"));
//   m_Gui->Integer(ID_LOWER_LABEL,"",&m_LowerLabel);
//   m_Gui->Label(_("Upper label"));
//   m_Gui->Integer(ID_UPPER_LABEL,"",&m_UpperLabel);
//   m_Gui->Divider(1);

  m_Gui->Divider(1);
  // m_Gui->Button(ID_SEGMENTATION,_("Segmentation"));
  m_Gui->OkCancel();
  m_Gui->Divider(1);

  m_Gui->FitGui();
  ShowGui();
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingLocalAndGlobalThreshold::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_RANGE_MODIFIED:
      {
        if (e->GetSender() == m_SliderLabels)
        {
          m_SliderLabels->GetSubRange(&m_LowerLabel,&m_UpperLabel);
        }
        if (e->GetSender() == m_SliderThresholds)
        {
          m_SliderThresholds->GetSubRange(&m_LowerThreshold,&m_UpperThreshold);
        }
      }
      break;
//     case ID_SEGMENTATION:
//       {
//         LocalSegmentation();
//       }
//       break;
    case wxOK:
      {
        LocalSegmentation();
        this->OpStop(OP_RUN_OK);
        return;
      }
      break;
    case wxCANCEL:
      {
        this->OpStop(OP_RUN_CANCEL);
        return;
      }
      break;
    default:
      mafEventMacro(*e);
      break; 
    }
  }
}
