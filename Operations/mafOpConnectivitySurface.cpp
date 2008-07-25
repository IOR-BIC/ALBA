/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpConnectivitySurface.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:51 $
  Version:   $Revision: 1.3 $
  Authors:   Daniele Giunchi - Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpConnectivitySurface.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafVMESurface.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkCleanPolyData.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkDecimatePro.h"
#include "vtkStripper.h"
#include "vtkTriangleFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataConnectivityFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpConnectivitySurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpConnectivitySurface::mafOpConnectivitySurface(const wxString &label) :
mafOp(label)
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
mafOpConnectivitySurface::~mafOpConnectivitySurface()
//----------------------------------------------------------------------------
{
	vtkDEL(m_OriginalPolydata);
	for(int numVmes=0;numVmes<m_ExtractedVmes.size();numVmes++)
	{
		mafDEL(m_ExtractedVmes[numVmes]);
	}
	m_ExtractedVmes.clear();
}
//----------------------------------------------------------------------------
bool mafOpConnectivitySurface::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMESurface));
}
//----------------------------------------------------------------------------
mafOp *mafOpConnectivitySurface::Copy()   
//----------------------------------------------------------------------------
{
  return (new mafOpConnectivitySurface(m_Label));
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
void mafOpConnectivitySurface::OpRun()   
//----------------------------------------------------------------------------
{  	
	vtkNEW(m_OriginalPolydata);
	m_OriginalPolydata->DeepCopy((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());
	
	// interface:
  if(!m_TestMode)
  {
	  m_Gui = new mafGUI(this);

    
	  double bounds[6];
	  m_OriginalPolydata->GetBounds(bounds);

	  m_Gui->Label("");
	  m_Gui->Label(_("Extract the largest surface"),true);

	  m_Gui->Bool(ID_EXTRACT_BIGGEST_SURFACE,_("Enable"),&m_ExtractBiggestSurface);
	  m_Gui->Divider(2);
	 //-------------------------------------

	  m_Gui->Label(_("Filter Output by Size"),true);
	  m_Gui->Label(_("Size Thresh."));
	  m_Gui->Double(ID_THRESOLD,"", &m_Thresold,0,MAXDOUBLE,-1,_("The operation will get rid of surfaces which are under this size"));
		m_Gui->Label("Input bounds dimensions:",true);
	  mafString labelX;
	  labelX.Append(wxString::Format(_("DimX:  %.2f"),(bounds[1]-bounds[0])));
	  m_Gui->Label(labelX);

	  mafString labelY;
	  labelY.Append(wxString::Format(_("DimY:  %.2f"),(bounds[3]-bounds[2])));
	  m_Gui->Label(labelY);

	  mafString labelZ;
	  labelZ.Append(wxString::Format(_("DimZ:  %.2f"),(bounds[5]-bounds[4])));
	  m_Gui->Label(labelZ);

  	

	  m_Gui->Divider(2);
	//-------------------------------------


	  m_Gui->Button(ID_VTK_CONNECT,_("run connectivity"));

	  m_Gui->Label("");
	  m_Gui->Label(_("Extracted:"), &m_NumberOfExtractedSurfaces);
		

    vtkMAFSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
    connectivityFilter->SetInput(m_OriginalPolydata);
    connectivityFilter->SetExtractionModeToAllRegions();
    connectivityFilter->Update();

    int regionNumbers = connectivityFilter->GetNumberOfExtractedRegions();
    m_NumberOfExtractedSurfaces = wxString::Format("%d", regionNumbers);

    if(regionNumbers > 100)
    {
      m_Alert= _("Warning: process time will be heavy");
    }
    else
      m_Alert = "";

    m_Gui->Label(&m_Alert, true, true);
	  m_Gui->Label("");
  	
	  m_Gui->OkCancel();
	  m_Gui->Enable(wxOK,false);

	  m_Gui->Divider();
    //m_Gui->Update();
	  ShowGui();
  }
}
//----------------------------------------------------------------------------
void mafOpConnectivitySurface::OpDo()
//----------------------------------------------------------------------------
{
	for(int vmeShowed = 0; vmeShowed < m_ExtractedVmes.size(); vmeShowed++)
		m_ExtractedVmes[vmeShowed]->ReparentTo(m_Input);
}
//----------------------------------------------------------------------------
void mafOpConnectivitySurface::OpUndo()
//----------------------------------------------------------------------------
{
	for(int vmeShowed = 0; vmeShowed < m_ExtractedVmes.size(); vmeShowed++)
		mafEventMacro(mafEvent(this,VME_REMOVE,m_ExtractedVmes[vmeShowed]));
}
//----------------------------------------------------------------------------
void mafOpConnectivitySurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
					vtkMAFSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
					connectivityFilter->SetInput(m_OriginalPolydata);
					connectivityFilter->SetExtractionModeToAllRegions();
					connectivityFilter->Update();
					regionNumbers = connectivityFilter->GetNumberOfExtractedRegions();
				}
				m_NumberOfExtractedSurfaces = wxString::Format("%d", regionNumbers);
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
void mafOpConnectivitySurface::OpStop(int result)
//----------------------------------------------------------------------------
{
	if(result == OP_RUN_CANCEL)
	{
		for(int numVmes = 0; numVmes < m_ExtractedVmes.size(); numVmes++)
		{
			mafDEL(m_ExtractedVmes[numVmes]);
		}
		m_ExtractedVmes.clear();
	}
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mafOpConnectivitySurface::OnVtkConnect()
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
    mafDEL(m_ExtractedVmes[numVmes]);
	}
	m_ExtractedVmes.clear();

  double bounds[6];
  m_OriginalPolydata->GetBounds(bounds);

  double dimX, dimY, dimZ;
  dimX = (bounds[1] - bounds[0]);
  dimY = (bounds[3] - bounds[2]);
  dimZ = (bounds[5] - bounds[4]);

  double maxBound = (dimX >= dimY) ? (dimX >= dimZ ? dimX : dimZ) : (dimY >= dimZ ? dimY : dimZ); 
  if(m_Thresold > maxBound)
  {
    m_NumberOfExtractedSurfaces = _("0");
    m_Alert= _("Over max bound you've no extraction");
    m_Gui->Enable(ID_VTK_CONNECT,true);
    m_Gui->Enable(wxOK,true);
    m_Gui->Update();
    return;
  }
	vtkMAFSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
	connectivityFilter->SetInput(m_OriginalPolydata);
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
		
		if(m_Thresold <= maxBound)
		{
			mafVMESurface *surf;
			mafNEW(surf);
			surf->SetData(connectivityFilter->GetOutput(),surf->GetTimeStamp());
			surf->SetName(wxString::Format("%d_extr",region));
			m_ExtractedVmes.push_back(surf);
		}
	}

  m_NumberOfExtractedSurfaces = wxString::Format("%d", m_ExtractedVmes.size());

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
