/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpConnectivitySurface
 Authors: Daniele Giunchi - Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpConnectivitySurface.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaVMESurface.h"

#include "vtkALBASmartPointer.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCleanPolyData.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkDecimatePro.h"
#include "vtkStripper.h"
#include "vtkTriangleFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkCleanPolyData.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpConnectivitySurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpConnectivitySurface::albaOpConnectivitySurface(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = true;
	m_OriginalPolydata  = NULL;

	m_Thresold = 0.0;
	m_NumberOfExtractedSurfaces = "0";
  m_Alert="";

	m_ExtractBiggestSurface = 0;
}
//----------------------------------------------------------------------------
albaOpConnectivitySurface::~albaOpConnectivitySurface()
//----------------------------------------------------------------------------
{
	vtkDEL(m_OriginalPolydata);
	for(int numVmes=0;numVmes<m_ExtractedVmes.size();numVmes++)
	{
		albaDEL(m_ExtractedVmes[numVmes]);
	}
	m_ExtractedVmes.clear();
}
//----------------------------------------------------------------------------
bool albaOpConnectivitySurface::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVMESurface));
}
//----------------------------------------------------------------------------
albaOp *albaOpConnectivitySurface::Copy()   
//----------------------------------------------------------------------------
{
  return (new albaOpConnectivitySurface(m_Label));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum FILTER_SURFACE_ID
{
	ID_CLEAN = MINID,
	ID_THRESOLD,
	ID_VTK_CONNECT,
	ID_EXTRACT_BIGGEST_SURFACE,
};
//----------------------------------------------------------------------------
void albaOpConnectivitySurface::CreateGui()   
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);


  double bounds[6];
  m_OriginalPolydata->GetBounds(bounds);

  m_Gui->Label("");
  m_Gui->Label(_("Extract the largest surface"),true);

  m_Gui->Bool(ID_EXTRACT_BIGGEST_SURFACE,_("Enable"),&m_ExtractBiggestSurface);
  m_Gui->Divider(2);
  //-------------------------------------

  m_Gui->Label(_("Filter Output by Size"),true);
  m_Gui->Label(_("Size Thresh."));
  //m_Gui->Double(ID_THRESOLD,"", &m_Thresold,0,MAXDOUBLE,-1,_("The operation will get rid of surfaces which are under this size"));
  m_Gui->Slider(ID_THRESOLD,"",&m_Thresold,0,100);

  m_Gui->Label("Input bounds dimensions:",true);

  albaString labelX;
  labelX.Append(albaString::Format(_("DimX:  %.2f"),(bounds[1]-bounds[0])));
  m_Gui->Label(labelX);

  albaString labelY;
  labelY.Append(albaString::Format(_("DimY:  %.2f"),(bounds[3]-bounds[2])));
  m_Gui->Label(labelY);

  albaString labelZ;
  labelZ.Append(albaString::Format(_("DimZ:  %.2f"),(bounds[5]-bounds[4])));
  m_Gui->Label(labelZ);



  m_Gui->Divider(2);
  //-------------------------------------


  m_Gui->Button(ID_VTK_CONNECT,_("run connectivity"));

  m_Gui->Label("");
  m_Gui->Label(_("Extracted:"), &m_NumberOfExtractedSurfaces);


  vtkALBASmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
  connectivityFilter->SetInputData(m_OriginalPolydata);
  connectivityFilter->SetExtractionModeToAllRegions();
  connectivityFilter->Update();

  int regionNumbers = connectivityFilter->GetNumberOfExtractedRegions();
  m_NumberOfExtractedSurfaces = albaString::Format("%d", regionNumbers);

  if(regionNumbers > 100)
  {
    m_Alert= _("Warning: process time will be heavy");
  }
  else
    m_Alert = "";

  m_Gui->Label(&m_Alert, true, true);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	m_Gui->Enable(wxOK, false);
	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpConnectivitySurface::OpRun()   
//----------------------------------------------------------------------------
{  	
	vtkNEW(m_OriginalPolydata);
	m_OriginalPolydata->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());
	
	// interface:
  if(!m_TestMode)
  {
	  CreateGui();
  }
}
//----------------------------------------------------------------------------
void albaOpConnectivitySurface::OpDo()
//----------------------------------------------------------------------------
{
	for(int vmeShowed = 0; vmeShowed < m_ExtractedVmes.size(); vmeShowed++)
		m_ExtractedVmes[vmeShowed]->ReparentTo(m_Input);
}
//----------------------------------------------------------------------------
void albaOpConnectivitySurface::OpUndo()
//----------------------------------------------------------------------------
{
	for(int vmeShowed = 0; vmeShowed < m_ExtractedVmes.size(); vmeShowed++)
		GetLogicManager()->VmeRemove(m_ExtractedVmes[vmeShowed]);
}
//----------------------------------------------------------------------------
void albaOpConnectivitySurface::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {	
		case ID_EXTRACT_BIGGEST_SURFACE:
			{
				int regionNumbers;
				if(m_ExtractBiggestSurface == 1)
				{
					regionNumbers=1;
				}
				else
				{
					vtkALBASmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
					connectivityFilter->SetInputData(m_OriginalPolydata);
					connectivityFilter->SetExtractionModeToAllRegions();
					connectivityFilter->Update();
					regionNumbers = connectivityFilter->GetNumberOfExtractedRegions();
				}
				m_NumberOfExtractedSurfaces = albaString::Format("%d", regionNumbers);
				m_Gui->Update();
			}
			break;
      case ID_VTK_CONNECT:
        OnVtkConnect();
      break;
      case wxOK:
        OpStop(OP_RUN_OK);        
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);        
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaOpConnectivitySurface::OpStop(int result)
//----------------------------------------------------------------------------
{
	if(result == OP_RUN_CANCEL)
	{
		for(int numVmes = 0; numVmes < m_ExtractedVmes.size(); numVmes++)
		{
			albaDEL(m_ExtractedVmes[numVmes]);
		}
		m_ExtractedVmes.clear();
	}
	HideGui();
	albaEventMacro(albaEvent(this,result));        
}
//----------------------------------------------------------------------------
void albaOpConnectivitySurface::OnVtkConnect()
//----------------------------------------------------------------------------
{
  if(!m_TestMode)
  {
	  wxBusyCursor wait;
	  m_Gui->Enable(ID_VTK_CONNECT,false);
	  m_Gui->Update();
  }

	for(int numVmes = 0; numVmes < m_ExtractedVmes.size(); numVmes++)
	{
    albaDEL(m_ExtractedVmes[numVmes]);
	}
	m_ExtractedVmes.clear();

  double bounds[6];
  m_OriginalPolydata->GetBounds(bounds);

  double dimX, dimY, dimZ;
  dimX = (bounds[1] - bounds[0]);
  dimY = (bounds[3] - bounds[2]);
  dimZ = (bounds[5] - bounds[4]);

  double maxBound = (dimX >= dimY) ? (dimX >= dimZ ? dimX : dimZ) : (dimY >= dimZ ? dimY : dimZ); 
  double valueBoundThreshold = m_Thresold*maxBound/100;

//   if(m_Thresold > maxBound)
//   {
//     m_NumberOfExtractedSurfaces = _("0");
//     m_Alert= _("Over max bound you've no extraction");
//     m_Gui->Enable(ID_VTK_CONNECT,true);
//     m_Gui->Enable(wxOK,true);
//     m_Gui->Update();
//     return;
//   }
	vtkALBASmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
	connectivityFilter->SetInputData(m_OriginalPolydata);
	int regionNumbers;
	if(m_ExtractBiggestSurface == 1)
	{
		connectivityFilter->SetExtractionModeToLargestRegion();
		connectivityFilter->Update();
		regionNumbers = 1;
	}
	else
	{
		connectivityFilter->SetExtractionModeToSpecifiedRegions();
		connectivityFilter->Update();
		regionNumbers = connectivityFilter->GetNumberOfExtractedRegions();
	}

	for(int region = 0, deleteRegion = 0; region < regionNumbers; region++)
	{
    connectivityFilter->InitializeSpecifiedRegionList();
		connectivityFilter->AddSpecifiedRegion(region);
		connectivityFilter->Update();

		connectivityFilter->GetOutput()->GetBounds(bounds);
		
		dimX = (bounds[1] - bounds[0]);
		dimY = (bounds[3] - bounds[2]);
		dimZ = (bounds[5] - bounds[4]);

		double maxBound = (dimX >= dimY) ? (dimX >= dimZ ? dimX : dimZ) : (dimY >= dimZ ? dimY : dimZ); 
		
		if(valueBoundThreshold <= maxBound)
		{

      vtkALBASmartPointer<vtkCleanPolyData> clean;
      clean->SetInputConnection(connectivityFilter->GetOutputPort());
      clean->Update();

			albaVMESurface *surf;
			albaNEW(surf);
			surf->SetData(clean->GetOutput(),surf->GetTimeStamp());
			surf->SetName(albaString::Format("%d_extr",region));
			m_ExtractedVmes.push_back(surf);
		}
	}

  m_NumberOfExtractedSurfaces = albaString::Format("%d", m_ExtractedVmes.size());

  if(regionNumbers > 100)
  {
    m_Alert= _("Warning: process time will be heavy");
  }
  else
    m_Alert = "";

  if(!m_TestMode)
  {
	  m_Gui->Enable(ID_VTK_CONNECT,true);
	  m_Gui->Enable(wxOK,true);
	  m_Gui->Update();
  }
}
