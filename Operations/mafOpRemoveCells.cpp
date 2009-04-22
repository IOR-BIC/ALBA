/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpRemoveCells.cpp,v $
Language:  C++
Date:      $Date: 2009-04-22 09:42:30 $
Version:   $Revision: 1.3.2.2 $
Authors:   Stefano Perticoni
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

#include "mafOpRemoveCells.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "mafGUIDialog.h"

#include "mafRWIBase.h"
#include "mafRWI.h"
#include "mmdMouse.h"

#include "mafGUIButton.h"
#include "mafGUIValidator.h"
#include "mafGUIPicButton.h"
#include "mafGUIFloatSlider.h"

#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMEOutput.h"
#include "mmiSelectCell.h"

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
#include "vtkMAFRemoveCellsFilter.h"
#include "vtkMath.h"
#include "vtkIdList.h"
#include "vtkTriangle.h"
#include "vtkCamera.h"
#include "vtkInteractorStyle.h"

const int ID_REGION = 0;

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpRemoveCells);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpRemoveCells::mafOpRemoveCells(wxString label) :
mafOp(label)
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

  m_TriangeCentreComputationList = NULL;
  m_AutoClip = 1;
}
//----------------------------------------------------------------------------
mafOpRemoveCells::~mafOpRemoveCells()
//----------------------------------------------------------------------------
{
  vtkDEL(m_Mesh);
  vtkDEL(m_ResultPolydata);
  vtkDEL(m_OriginalPolydata);
}
//----------------------------------------------------------------------------
mafOp* mafOpRemoveCells::Copy()
//----------------------------------------------------------------------------
{
  /** return a copy of itself, needs to put it into the undo stack */
  return new mafOpRemoveCells(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpRemoveCells::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  return vme != NULL && vme->IsMAFType(mafVMESurface);
}
//----------------------------------------------------------------------------
void mafOpRemoveCells::OpRun()
//----------------------------------------------------------------------------
{
  
  vtkNEW(m_ResultPolydata);
  m_ResultPolydata->DeepCopy((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());

  vtkNEW(m_OriginalPolydata);
  m_OriginalPolydata->DeepCopy((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());

  int result = OP_RUN_CANCEL;
  // default size for the brush (depends on the input dimensions)
  double bounds[6]= {0.,0.,0.,0.,0.,0.};
  ((mafVME *)m_Input)->GetOutput()->GetVTKData()->GetBounds(bounds);
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

    DestroyHelperStructures();
    mafEventMacro(mafEvent(this,result));
  }
}
//----------------------------------------------------------------------------
void mafOpRemoveCells::OpDo()
//----------------------------------------------------------------------------
{
  ((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,((mafVME *)m_Input)->GetTimeStamp());
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpRemoveCells::OpUndo()
//----------------------------------------------------------------------------
{
  ((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,((mafVME *)m_Input)->GetTimeStamp());
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
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
void mafOpRemoveCells::CreateOpDialog()
//----------------------------------------------------------------------------
{
  wxBusyCursor wait;

  //===== setup interface ====
  m_Dialog = new mafGUIDialog("Remove Cells", mafCLOSEWINDOW | mafRESIZABLE);
  
  m_Rwi = new mafRWI(m_Dialog,ONE_LAYER,false);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(CAMERA_PERSPECTIVE);

  m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.0001f);
  m_Rwi->SetSize(0,0,800,800);
  m_Rwi->Show(true);
  m_Rwi->m_RwiBase->SetMouse(m_Mouse);

  m_Rwi->m_RenFront->AddActor(m_PolydataActor);

  m_Rwi->SetAxesVisibility(1);

  
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(((mafVME *)m_Input)->GetOutput()->GetVTKData());
 
  double bounds[6] = {0,0,0,0,0,0};
  polydata->GetBounds(bounds);

  m_Rwi->m_RenFront->ResetCamera(polydata->GetBounds());
  m_Rwi->m_RenFront->ResetCameraClippingRange(bounds);

  mafNEW(m_SelectCellInteractor);

  m_SelectCellInteractor->SetListener(this);
  m_Mouse->AddObserver(m_SelectCellInteractor, MCH_INPUT);

 
  wxPoint p = wxDefaultPosition;

  wxStaticText *brushSize  = new wxStaticText(m_Dialog,-1, "brush size: ");
  wxStaticText *foo  = new wxStaticText(m_Dialog,-1, " ");
  wxTextCtrl   *diameter = new wxTextCtrl  (m_Dialog,ID_DIAMETER, "",								 		p,wxSize(50, 16 ), wxNO_BORDER );
  wxCheckBox *unselect =   new wxCheckBox(m_Dialog, ID_DELETE,         "unselect", p, wxSize(80,20));
  wxCheckBox *b_clip =   new wxCheckBox(m_Dialog, ID_AUTOCLIP,         "autoclipping", p, wxSize(80,20));

  wxStaticText *help  = new wxStaticText(m_Dialog,-1, "Use CTRL to select cells");

  mafGUIButton  *unselectAllButton =    new mafGUIButton(m_Dialog, ID_UNSELECT,    "unselect all", p,wxSize(80,20));
  mafGUIButton  *b_fit =    new mafGUIButton(m_Dialog, ID_FIT,    "reset camera", p,wxSize(80,20));
  mafGUIButton  *ok =     new mafGUIButton(m_Dialog, ID_OK,     "ok", p, wxSize(80,20));
  mafGUIButton  *cancel = new mafGUIButton(m_Dialog, ID_CANCEL, "cancel", p, wxSize(80,20));

	
  b_clip->SetValidator(mafGUIValidator(this,ID_AUTOCLIP,b_clip,(int*)&m_AutoClip));
  // AACC: 10-03-2009 please add more decimal digits...
  diameter->SetValidator(mafGUIValidator(this,ID_DIAMETER,diameter,&m_Diameter,m_MinBrushSize,m_MaxBrushMSize, 4));
  unselect->SetValidator(mafGUIValidator(this, ID_DELETE, unselect, &m_UnselectCells));

  unselectAllButton->SetValidator(mafGUIValidator(this,ID_UNSELECT,unselectAllButton));
  b_fit->SetValidator(mafGUIValidator(this,ID_FIT,b_fit));
  ok->SetValidator(mafGUIValidator(this,ID_OK,ok));
  cancel->SetValidator(mafGUIValidator(this,ID_CANCEL,cancel));

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
  mafGetFrame()->GetPosition(&x_pos,&y_pos);
  m_Dialog->GetSize(&w,&h);
  m_Dialog->SetSize(x_pos+5,y_pos+5,w,h);

  m_Rwi->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafOpRemoveCells::CreateSurfacePipeline()
//----------------------------------------------------------------------------
{
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(((mafVME *)m_Input)->GetOutput()->GetVTKData());

  m_Rcf = vtkMAFRemoveCellsFilter::New();
  m_Rcf->SetInput(polydata);
  m_Rcf->Update();

  m_PolydataMapper	= vtkPolyDataMapper::New();
  m_PolydataMapper->SetInput(m_Rcf->GetOutput());
  m_PolydataMapper->ScalarVisibilityOn();

  m_PolydataActor = vtkActor::New();
  m_PolydataActor->SetMapper(m_PolydataMapper);
  
}
//----------------------------------------------------------------------------
void mafOpRemoveCells::DeleteOpDialog()
//----------------------------------------------------------------------------
{
  m_Mouse->RemoveObserver(m_SelectCellInteractor);

  mafDEL(m_SelectCellInteractor);

  vtkDEL(m_PolydataMapper);
  vtkDEL(m_PolydataActor);
  vtkDEL(m_Rcf);

  cppDEL(m_Rwi); 
  cppDEL(m_Dialog);
}


//----------------------------------------------------------------------------
void mafOpRemoveCells::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        vtkPolyData *polydata = vtkPolyData::SafeDownCast(((mafVME *)m_Input)->GetOutput()->GetVTKData());

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
        mafEventMacro(*e);
      break; 
    }
  }
}

void mafOpRemoveCells::ExecuteMark( double radius )
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
      if ( m_VisitedCells[cellId] < 0 )
      {
        // mark it as visited
        m_VisitedCells[cellId] = m_RegionNumber;
        
        double currentCellCenter[3] = {0,0,0};
        FindTriangleCellCenter(cellId, currentCellCenter);

        // mark the cell if the distance criterion is ok
        if (vtkMath::Distance2BetweenPoints(seedCenter, currentCellCenter)
          < (m_Diameter*m_Diameter / 4))
        {
          m_UnselectCells ? m_Rcf->UnmarkCell(cellId) : m_Rcf->MarkCell(cellId);
        }

        // get its points
        m_Mesh->GetCellPoints(cellId, numCellPoints, cellPointsList);

        // for each cell point
        for (idPoint=0; idPoint < numCellPoints; idPoint++) 
        {
          // if the point has not been yet visited
          if ( m_VisitedPoints[ptId=cellPointsList[idPoint]] < 0 )
          {
            // mark it as visited
            m_VisitedPoints[ptId] = m_PointNumber++;
          }

          // get neighbor cells from cell point
          m_Mesh->GetPointCells(ptId,ncells,cellsFromPoint);

          // check connectivity criterion (geometric + distance)
          for (k=0; k < ncells; k++)
          {
            cellId = cellsFromPoint[k];
            
            FindTriangleCellCenter(cellId,currentCellCenter);
            if (vtkMath::Distance2BetweenPoints(seedCenter, currentCellCenter)
              < (m_Diameter*m_Diameter / 4))
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

void mafOpRemoveCells::MarkCellsInRadius(double radius){

  vtkIdType i;
  int numPts, numCells;
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(((mafVME *)m_Input)->GetOutput()->GetVTKData());

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
  

void mafOpRemoveCells::SetSeed( vtkIdType cellSeed )
{
	m_CellSeed = cellSeed;
}

void mafOpRemoveCells::FindTriangleCellCenter(vtkIdType id, double center[3])
{

  m_Mesh->GetCellPoints(id, m_TriangeCentreComputationList);

  m_Mesh->GetPoint(m_TriangeCentreComputationList->GetId(0),m_P0);
  m_Mesh->GetPoint(m_TriangeCentreComputationList->GetId(1),m_P1);
  m_Mesh->GetPoint(m_TriangeCentreComputationList->GetId(2),m_P2);

  vtkTriangle::TriangleCenter(m_P0,m_P1,m_P2,center);

}

void mafOpRemoveCells::CreateHelperStructures()
{
  // Build cell structure
  //
  vtkNEW(m_Mesh);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(((mafVME *)m_Input)->GetOutput()->GetVTKData());
  assert(polydata);
  

  if (polydata->GetPoints() == NULL)
  {
    mafLogMessage("No points!");
    assert(false);
  }


  if ( polydata->GetNumberOfPoints() < 1 || polydata->GetNumberOfCells() < 1 )
  {
    mafLogMessage("No data to connect!");
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

void mafOpRemoveCells::RemoveCells()
{
  	// perform cells removing...
    if (m_TestMode == false)
    {
      wxBusyInfo("removing cells...");
    }

    m_Rcf->RemoveMarkedCells();
    m_Rcf->Update();

    m_ResultPolydata->DeepCopy(m_Rcf->GetOutput());
}

void mafOpRemoveCells::MarkCells()
{
  MarkCellsInRadius(m_Diameter/2);  
}

void mafOpRemoveCells::DestroyHelperStructures()
{
  vtkDEL(m_Wave);
  vtkDEL(m_Wave2);

	delete [] m_VisitedCells;
  delete [] m_VisitedPoints;

  vtkDEL(m_NeighborCellPointIdList);
  
  m_TriangeCentreComputationList->Delete();
}
