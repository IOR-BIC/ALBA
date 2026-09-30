/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpRemoveCells
 Authors: Stefano Perticoni
 
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

#include "albaOpRemoveCells.h"
#include "albaGUIBusyInfo.h"

#include "albaDecl.h"
#include "albaGUI.h"

#include "albaGUIDialog.h"

#include "albaRWIBase.h"
#include "albaRWI.h"
#include "albaDeviceButtonsPadMouse.h"

#include "albaGUIButton.h"
#include "albaGUIValidator.h"
#include "albaGUIPicButton.h"
#include "albaGUIFloatSlider.h"

#include "albaVME.h"
#include "albaVMESurface.h"
#include "albaVMEOutput.h"
#include "albaInteractorSelectCell.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include "vtkCell.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkVolume.h"
#include "vtkALBARemoveCellsFilter.h"
#include "vtkMath.h"
#include "vtkIdList.h"
#include "vtkTriangle.h"
#include "vtkCamera.h"
#include "vtkInteractorStyle.h"

const int ID_REGION = 0;

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpRemoveCells);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpRemoveCells::albaOpRemoveCells(wxString label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;
  m_Dialog = NULL;
  m_Rwi = NULL;

  m_SelectCellInteractor  = NULL;

  m_Rcf = NULL;

  m_Diameter = 20;
  m_MinBrushSize = 0;
  m_MaxBrushMSize = 100;

  m_Mesh = NULL;
  m_UnselectCells = 0;
  
  m_NeighborCellPointIdList = NULL;
  m_InputPreserving = false;

  m_ResultPolydata	  = NULL;
  m_OriginalPolydata  = NULL;
  m_PolydataMapper    = NULL;
  m_PolydataActor     = NULL;
  m_Rcf               = NULL;


  m_TriangeCentreComputationList = NULL;
  m_AutoClip = 1;
}
//----------------------------------------------------------------------------
albaOpRemoveCells::~albaOpRemoveCells()
//----------------------------------------------------------------------------
{
  if (m_Mesh)
    DestroyHelperStructures();

  if (m_PolydataMapper)
    vtkDEL(m_PolydataMapper);
  
  if (m_PolydataActor)
    vtkDEL(m_PolydataActor);

  if (m_Rcf)
    vtkDEL(m_Rcf);

  vtkDEL(m_Mesh);
  vtkDEL(m_ResultPolydata);
  vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
albaOp* albaOpRemoveCells::Copy()
//----------------------------------------------------------------------------
{
  /** return a copy of itself, needs to put it into the undo stack */
  return new albaOpRemoveCells(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpRemoveCells::InternalAccept(albaVME* vme)
//----------------------------------------------------------------------------
{
  return vme != NULL && vme->IsALBAType(albaVMESurface);
}
//----------------------------------------------------------------------------
void albaOpRemoveCells::OpRun()
//----------------------------------------------------------------------------
{
  
  vtkNEW(m_ResultPolydata);
  m_ResultPolydata->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());

  vtkNEW(m_OriginalPolydata);
  m_OriginalPolydata->DeepCopy((vtkPolyData*)m_Input->GetOutput()->GetVTKData());

  int result = OP_RUN_CANCEL;
  // default size for the brush (depends on the input dimensions)
  double bounds[6]= {0.,0.,0.,0.,0.,0.};
  m_Input->GetOutput()->GetVTKData()->GetBounds(bounds);
   // bounds x0 x1 y0 y1 z0 z1
   m_Diameter = sqrt((bounds[1]-bounds[0])*(bounds[1]-bounds[0])+(bounds[3]-bounds[2])*(bounds[3]-bounds[2])+(bounds[5]-bounds[4])*(bounds[5]-bounds[4]))/10.0;


  CreateSurfacePipeline();
  CreateHelperStructures();

  if (m_TestMode == false)
  {
    CreateOpDialog();

    int ret_dlg = m_Dialog->ShowModal();
    if( ret_dlg == wxID_OK )
    {
      result = OP_RUN_OK;

      RemoveCells();
    }
    else 
    {
      result = OP_RUN_CANCEL;
    }

    DeleteOpDialog();

    albaEventMacro(albaEvent(this,result));
  }
  
}
//----------------------------------------------------------------------------
void albaOpRemoveCells::OpDo()
//----------------------------------------------------------------------------
{
  ((albaVMESurface *)m_Input)->SetData(m_ResultPolydata,m_Input->GetTimeStamp());
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpRemoveCells::OpUndo()
//----------------------------------------------------------------------------
{
  ((albaVMESurface *)m_Input)->SetData(m_OriginalPolydata,m_Input->GetTimeStamp());
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
// widget ID's
//----------------------------------------------------------------------------
enum REMOVE_CELL_ID
{
  ID_FIT = MINID,
  ID_DIAMETER,
  ID_DELETE,
  ID_OK,
  ID_UNSELECT,
  ID_CANCEL,
  ID_CLIP,
  ID_AUTOCLIP,
};
//----------------------------------------------------------------------------
void albaOpRemoveCells::CreateOpDialog()
//----------------------------------------------------------------------------
{
  wxBusyCursor wait;

  //===== setup interface ====
  m_Dialog = new albaGUIDialog("Remove Cells", albaCLOSEWINDOW | albaRESIZABLE);
  
  m_Rwi = new albaRWI(m_Dialog,ONE_LAYER,false);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(CAMERA_PERSPECTIVE);

  m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.0001f);
  m_Rwi->SetSize(0,0,800,800);
  m_Rwi->Show(true);
  m_Rwi->m_RwiBase->SetMouse(m_Mouse);

  m_Rwi->m_RenFront->AddActor(m_PolydataActor);

  m_Rwi->SetAxesVisibility(1);

  
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(m_Input->GetOutput()->GetVTKData());
 
  double bounds[6] = {0,0,0,0,0,0};
  polydata->GetBounds(bounds);

  m_Rwi->m_RenFront->ResetCamera(polydata->GetBounds());
  m_Rwi->m_RenFront->ResetCameraClippingRange(bounds);

  albaNEW(m_SelectCellInteractor);

  m_SelectCellInteractor->SetListener(this);
  m_Mouse->AddObserver(m_SelectCellInteractor, MCH_INPUT);

 
  wxPoint p = wxDefaultPosition;

  wxStaticText *brushSize  = new wxStaticText(m_Dialog,-1, "brush size: ");
  wxStaticText *foo  = new wxStaticText(m_Dialog,-1, " ");
  wxTextCtrl   *diameter = new wxTextCtrl  (m_Dialog,ID_DIAMETER, "",								 		p,wxSize(50, 16 ), wxNO_BORDER );
  wxCheckBox *unselect =   new wxCheckBox(m_Dialog, ID_DELETE,         "unselect", p, wxSize(80,20));
  wxCheckBox *b_clip =   new wxCheckBox(m_Dialog, ID_AUTOCLIP,         "autoclipping", p, wxSize(80,20));

  wxStaticText *help  = new wxStaticText(m_Dialog,-1, "Use CTRL to select cells");

  albaGUIButton  *unselectAllButton =    new albaGUIButton(m_Dialog, ID_UNSELECT,    "unselect all", p,wxSize(80,20));
  albaGUIButton  *b_fit =    new albaGUIButton(m_Dialog, ID_FIT,    "reset camera", p,wxSize(80,20));
  albaGUIButton  *ok =     new albaGUIButton(m_Dialog, ID_OK,     "ok", p, wxSize(80,20));
  albaGUIButton  *cancel = new albaGUIButton(m_Dialog, ID_CANCEL, "cancel", p, wxSize(80,20));

	
  b_clip->SetValidator(albaGUIValidator(this,ID_AUTOCLIP,b_clip,(int*)&m_AutoClip));
  // AACC: 10-03-2009 please add more decimal digits...
  diameter->SetValidator(albaGUIValidator(this,ID_DIAMETER,diameter,&m_Diameter,m_MinBrushSize,m_MaxBrushMSize, 4));
  unselect->SetValidator(albaGUIValidator(this, ID_DELETE, unselect, &m_UnselectCells));

  unselectAllButton->SetValidator(albaGUIValidator(this,ID_UNSELECT,unselectAllButton));
  b_fit->SetValidator(albaGUIValidator(this,ID_FIT,b_fit));
  ok->SetValidator(albaGUIValidator(this,ID_OK,ok));
  cancel->SetValidator(albaGUIValidator(this,ID_CANCEL,cancel));

  wxBoxSizer *h_sizer0 = new wxBoxSizer(wxHORIZONTAL);
  h_sizer0->Add(help,     0,wxRIGHT);	

  wxBoxSizer *h_sizer1 = new wxBoxSizer(wxHORIZONTAL);
  h_sizer1->Add(brushSize,     0,wxRIGHT);	
  h_sizer1->Add(diameter,     0,wxRIGHT);	
  h_sizer1->Add(foo,     0,wxRIGHT);	
  h_sizer1->Add(unselect,     0,wxRIGHT);	

  h_sizer1->Add(b_clip,0,wxRIGHT);


  wxBoxSizer *h_sizer2 = new wxBoxSizer(wxHORIZONTAL);
  h_sizer2->Add(unselectAllButton,     0,wxRIGHT);	
  h_sizer2->Add(b_fit,     0,wxRIGHT);	
  h_sizer2->Add(ok,      0,wxRIGHT);
  h_sizer2->Add(cancel,  0,wxRIGHT);
 
  wxBoxSizer *v_sizer =  new wxBoxSizer( wxVERTICAL );
  v_sizer->Add(m_Rwi->m_RwiBase, 1,wxEXPAND);
  v_sizer->Add(h_sizer0,     0,wxEXPAND | wxALL,5);
  v_sizer->Add(h_sizer1,     0,wxEXPAND | wxALL,5);
  v_sizer->Add(h_sizer2,     0,wxEXPAND | wxALL,5);

  m_Dialog->Add(v_sizer, 1, wxEXPAND);

  int x_pos,y_pos,w,h;
  albaGetFrame()->GetPosition(&x_pos,&y_pos);
  m_Dialog->GetSize(&w,&h);
  m_Dialog->SetSize(x_pos+5,y_pos+5,w,h);

  m_Rwi->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpRemoveCells::CreateSurfacePipeline()
//----------------------------------------------------------------------------
{
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(m_Input->GetOutput()->GetVTKData());

  m_Rcf = vtkALBARemoveCellsFilter::New();
  m_Rcf->SetInputData(polydata);
  m_Rcf->Update();

  m_PolydataMapper	= vtkPolyDataMapper::New();
  m_PolydataMapper->SetInputConnection(m_Rcf->GetOutputPort());
  m_PolydataMapper->ScalarVisibilityOn();

  m_PolydataActor = vtkActor::New();
  m_PolydataActor->SetMapper(m_PolydataMapper);
  
}
//----------------------------------------------------------------------------
void albaOpRemoveCells::DeleteOpDialog()
//----------------------------------------------------------------------------
{
  m_Mouse->RemoveObserver(m_SelectCellInteractor);

  albaDEL(m_SelectCellInteractor);

  cppDEL(m_Rwi); 
  cppDEL(m_Dialog);
}


//----------------------------------------------------------------------------
void albaOpRemoveCells::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {	
      case ID_OK:
        m_Dialog->EndModal(wxID_OK);
      break;
   
      case ID_CANCEL:
        m_Dialog->EndModal(wxID_CANCEL);
      break;
   
      case ID_UNSELECT:
        m_Rcf->UndoMarks();
        m_Rwi->m_RenderWindow->Render();
      break;

      case ID_FIT:
      {
        vtkPolyData *polydata = vtkPolyData::SafeDownCast(m_Input->GetOutput()->GetVTKData());

        double bounds[6] = {0,0,0,0,0,0};
        polydata->GetBounds(bounds);

        m_Rwi->m_RenFront->ResetCamera(polydata->GetBounds());
        m_Rwi->m_RenFront->ResetCameraClippingRange(bounds);

        m_Rwi->m_RenderWindow->Render();
      }
      break;

      case VME_PICKED:
      {
        
        double pos[3];
        vtkPoints *pts = NULL; 
        pts = (vtkPoints *)e->GetVtkObj();
        pts->GetPoint(0,pos);
        
        // get the picked cell and mark it as selected
        int cellID = e->GetArg();

        if (cellID == m_CellSeed)
        {
          return;
        }

        SetSeed(cellID);

        // select circle region by pick
        MarkCellsInRadius(m_Diameter/2);
        
        m_Rwi->m_RenderWindow->Render();
      }
      break;
      
      case ID_DELETE:
      {

      }     
      break ;
	  case ID_AUTOCLIP:
		  if(m_AutoClip)
		  {
			  //achiarini (27.11.2007)
			  //this is to guarantee that the clippingplanes are not reset during interaction
			  vtkInteractorStyle::SafeDownCast(m_Rwi->m_RwiBase->GetInteractorStyle())->AutoAdjustCameraClippingRangeOn(); 
			  m_SelectCellInteractor->AutoResetClippingRangeOn();

		  }
		  else{
			  //this is to guarantee that the clippingplanes are not reset during interaction
			  vtkInteractorStyle::SafeDownCast(m_Rwi->m_RwiBase->GetInteractorStyle())->AutoAdjustCameraClippingRangeOff(); 
			  m_SelectCellInteractor->AutoResetClippingRangeOff();
			  m_Rwi->CameraUpdate();
		  }

		  break;

	 default:
        albaEventMacro(*e);
      break; 
    }
  }
}

void albaOpRemoveCells::ExecuteMark( double radius )
{
  
  vtkIdType cellId, ptId, numIds, idCellInWave;
  int idPoint, k;
  vtkIdType *cellPointsList, *cellsFromPoint, numCellPoints;
  vtkIdList *tmpWave;
  unsigned short ncells = 0;

  double seedCenter[3] = {0,0,0};
  FindTriangleCellCenter(m_CellSeed, seedCenter);

  while ( (numIds=m_Wave->GetNumberOfIds()) > 0 )
  {
    // for all the cells in the wave
    for ( idCellInWave=0; idCellInWave < numIds; idCellInWave++ )
    {
      cellId = m_Wave->GetId(idCellInWave);
      if (m_VisitedCells[cellId] < 0)
      {
        // mark it as visited
        m_VisitedCells[cellId] = m_RegionNumber;

        double currentCellCenter[3] = { 0,0,0 };
        FindTriangleCellCenter(cellId, currentCellCenter);

        // mark the cell if the distance criterion is ok
        if (vtkMath::Distance2BetweenPoints(seedCenter, currentCellCenter)
          < (m_Diameter * m_Diameter / 4))
        {
          m_UnselectCells ? m_Rcf->UnmarkCell(cellId) : m_Rcf->MarkCell(cellId);
        }

        // get its points
        vtkNew<vtkIdList> cellPointsList;
        m_Mesh->GetCellPoints(cellId, cellPointsList);
        numCellPoints = cellPointsList->GetNumberOfIds();

        // for each cell point
        for (idPoint = 0; idPoint < numCellPoints; idPoint++)
        {
          // if the point has not been yet visited
          ptId = cellPointsList->GetId(idPoint);
          if (m_VisitedPoints[ptId] < 0)
          {
            // mark it as visited
            m_VisitedPoints[ptId] = m_PointNumber++;
          }

          // get neighbor cells from cell point
          vtkNew<vtkIdList> cellsFromPoint;
          m_Mesh->GetPointCells(ptId, cellsFromPoint);
          ncells = cellsFromPoint->GetNumberOfIds();

          // check connectivity criterion (geometric + distance)
          for (k = 0; k < ncells; k++)
          {
            cellId = cellsFromPoint->GetId(k);

            FindTriangleCellCenter(cellId, currentCellCenter);
            if (vtkMath::Distance2BetweenPoints(seedCenter, currentCellCenter)
              < (m_Diameter * m_Diameter / 4))
            {
              // insert next cells to be visited in the other wave
              m_Wave2->InsertNextId(cellId);
            }
          }//for all cells using this point
        }//for all points of this cell
      }//if cell not yet visited
    }//for all cells in this wave

    tmpWave = m_Wave;
    m_Wave = m_Wave2;
    m_Wave2 = tmpWave;
    tmpWave->Reset();
  } //while wave is not empty
}

void albaOpRemoveCells::MarkCellsInRadius(double radius){

  vtkIdType i;
  int numPts, numCells;
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(m_Input->GetOutput()->GetVTKData());

  // Initialize.  Keep track of points and cells visited.
  
  // heuristic
  numPts = polydata->GetNumberOfCells();
  
  for ( i=numPts-1; i >= 0; --i )
  {
    m_VisitedCells[i] = -1;
  }

  // heuristic
  numCells = polydata->GetNumberOfPoints();
  
  for ( i=numCells-1; i >=  0; --i )
  {
    m_VisitedPoints[i] = -1;
  }

  // Traverse all cells marking those visited. Connected region grows 
  // using a connected wave propagation.

  m_PointNumber = 0;

  // only one region with ID 0
  m_RegionNumber = ID_REGION;
   
  m_Wave->InsertNextId(m_CellSeed);
 
  //mark the seeded region
  ExecuteMark(m_Diameter/2);
   
  m_Wave->Reset();
  m_Wave2->Reset();
  
  
}
  

void albaOpRemoveCells::SetSeed( vtkIdType cellSeed )
{
	m_CellSeed = cellSeed;
}

void albaOpRemoveCells::FindTriangleCellCenter(vtkIdType id, double center[3])
{

  m_Mesh->GetCellPoints(id, m_TriangeCentreComputationList);

  m_Mesh->GetPoint(m_TriangeCentreComputationList->GetId(0),m_P0);
  m_Mesh->GetPoint(m_TriangeCentreComputationList->GetId(1),m_P1);
  m_Mesh->GetPoint(m_TriangeCentreComputationList->GetId(2),m_P2);

  vtkTriangle::TriangleCenter(m_P0,m_P1,m_P2,center);

}

void albaOpRemoveCells::CreateHelperStructures()
{
  // Build cell structure
  //
  vtkNEW(m_Mesh);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(m_Input->GetOutput()->GetVTKData());
  assert(polydata);
  

  if (polydata->GetPoints() == NULL)
  {
    albaLogMessage("No points!");
    assert(false);
  }


  if ( polydata->GetNumberOfPoints() < 1 || polydata->GetNumberOfCells() < 1 )
  {
    albaLogMessage("No data to connect!");
    assert(false);
  }

  this->m_Mesh->CopyStructure(polydata);
  this->m_Mesh->BuildLinks();
  
  int maxVisitedCells = m_Mesh->GetNumberOfCells();
  m_VisitedCells = new int[maxVisitedCells];

  int maxVisitedPoints = m_Mesh->GetNumberOfPoints();
  m_VisitedPoints = new vtkIdType[maxVisitedPoints];  
  
  vtkNEW(m_Wave);
  m_Wave->Allocate(maxVisitedPoints);

  vtkNEW(m_Wave2);
  m_Wave2->Allocate(maxVisitedPoints);


  vtkNEW(m_NeighborCellPointIdList);

  m_NeighborCellPointIdList->Allocate(3);
  

  m_TriangeCentreComputationList = vtkIdList::New();
  m_TriangeCentreComputationList->SetNumberOfIds(3);

}

void albaOpRemoveCells::RemoveCells()
{
  	// perform cells removing...
    albaGUIBusyInfo("removing cells...",m_TestMode);
    
    m_Rcf->RemoveMarkedCells();
    m_Rcf->Update();

    m_ResultPolydata->DeepCopy(m_Rcf->GetOutput());
}

void albaOpRemoveCells::MarkCells()
{
  MarkCellsInRadius(m_Diameter/2);  
}

void albaOpRemoveCells::DestroyHelperStructures()
{
  vtkDEL(m_Wave);
  vtkDEL(m_Wave2);

	delete [] m_VisitedCells;
  delete [] m_VisitedPoints;

  vtkDEL(m_NeighborCellPointIdList);
  
  m_TriangeCentreComputationList->Delete();
}
