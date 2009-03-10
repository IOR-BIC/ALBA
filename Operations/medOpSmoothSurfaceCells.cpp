/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpSmoothSurfaceCells.cpp,v $
Language:  C++
Date:      $Date: 2009-03-10 14:07:14 $
Version:   $Revision: 1.3.2.3 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2007
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

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpSmoothSurfaceCells.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"

#include "mafRWI.h"
#include "mmdMouse.h"

#include "mafGUIDialog.h"
#include "mafGUIButton.h"
#include "mafGUIValidator.h"

#include "mafVME.h"
#include "mafVMESurface.h"
#include "mmiSelectCell.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkPointData.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkTriangle.h"
#include "vtkFloatArray.h"
#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkMAFCellsFilter.h"

#include "vtkMAFRemoveCellsFilter.h"

#include "vtkAppendPolyData.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkCleanPolyData.h"


const int ID_REGION = 0;

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpSmoothSurfaceCells);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpSmoothSurfaceCells::medOpSmoothSurfaceCells(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType  = OPTYPE_OP;
	m_Canundo = true;
	m_InputPreserving = false;
	m_Dialog = NULL;
	m_Rwi = NULL;

	m_SelectCellInteractor  = NULL;

	m_CellFilter = NULL;

	m_Diameter = 1;
	m_MinBrushSize = 0;
	m_MaxBrushMSize = 100;

	m_Mesh = NULL;
	m_UnselectCells = 0;

	m_NeighborCellPointIds = NULL;

	m_ResultPolydata	  = NULL;
	m_OriginalPolydata  = NULL;

  m_PolydataMapper = NULL;
  m_PolydataActor = NULL;

  m_SmoothParameterNumberOfInteractions = 250;
  m_SmoothParameterBoundary = 0;
  m_SmoothParameterFeatureAngle = 45.;

  m_RemoveSelectedCells = NULL;
  m_RemoveUnSelectedCells = NULL;
  m_SelectCellInteractor = NULL;
  m_VisitedCells = NULL;
  
  	// only one region with ID 0
  m_RegionNumber = ID_REGION;

	
}
//----------------------------------------------------------------------------
medOpSmoothSurfaceCells::~medOpSmoothSurfaceCells()
//----------------------------------------------------------------------------
{
	if (m_VisitedCells) delete []m_VisitedCells;
	
	vtkDEL(m_Mesh);
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);
  vtkDEL(m_RemoveSelectedCells);
  vtkDEL(m_RemoveUnSelectedCells);
  vtkDEL(m_CellFilter);
  mafDEL(m_SelectCellInteractor);
}
//----------------------------------------------------------------------------
mafOp* medOpSmoothSurfaceCells::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new medOpSmoothSurfaceCells(m_Label);
}
//----------------------------------------------------------------------------
bool medOpSmoothSurfaceCells::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	return vme != NULL && vme->IsMAFType(mafVMESurface);
}
//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::OpRun()
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
  m_Diameter = vtkMath::Distance2BetweenPoints(bounds, bounds + 3)/ 2.0;

	CreateSurfacePipeline();
	
	InitializeMesh();
	int maxVisitedCells = m_OriginalPolydata->GetNumberOfCells();
	m_VisitedCells = new int[maxVisitedCells];
	for ( int i=0; i < maxVisitedCells; i++ )
	{
		m_VisitedCells[i] = -1;
	}


	if (m_TestMode == false)
	{
		CreateOpDialog();

		int ret_dlg = m_Dialog->ShowModal();
		if( ret_dlg == wxID_OK )
		{
			result = OP_RUN_OK;
		}
		else 
		{
			result = OP_RUN_CANCEL;
		}

		DeleteOpDialog();

		mafEventMacro(mafEvent(this,result));
	}
}
//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::OpDo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::OpUndo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_OriginalPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
// widget ID's
//----------------------------------------------------------------------------
enum EXTRACT_ISOSURFACE_ID
{
	ID_FIT = MINID,
	ID_DIAMETER,
	ID_DELETE,
	ID_OK,
	ID_UNSELECT,
	ID_CANCEL,
	ID_SMOOTH,
  ID_BOUNDARY,
  ID_FEATURE_ANGLE,
	ID_RESET,
	ID_ITERATIONS,
};
//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::CreateOpDialog()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	//===== setup interface ====
	m_Dialog = new mafGUIDialog("Smooth Cells", mafCLOSEWINDOW | mafRESIZABLE);

	m_Rwi = new mafRWI(m_Dialog,ONE_LAYER,false);
	m_Rwi->SetListener(this);
	m_Rwi->CameraSet(CAMERA_PERSPECTIVE);

	m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.0001f);
	m_Rwi->SetSize(0,0,800,800);
	m_Rwi->Show(true);
	m_Rwi->m_RwiBase->SetMouse(m_Mouse);

	m_Rwi->m_RenFront->AddActor(m_PolydataActor);


	vtkPolyData *polydata = m_ResultPolydata;

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
  wxStaticText *foo1  = new wxStaticText(m_Dialog,-1, " ");

  //wxStaticText *smoothNumberIterations  = new wxStaticText(m_Dialog,-1, "number of iterations: ");
  //wxTextCtrl   *smoothIterations = new wxTextCtrl  (m_Dialog,ID_ITERATIONS, "",								 		p,wxSize(50, 16 ), wxNO_BORDER );
	
  //wxStaticText *foo2  = new wxStaticText(m_Dialog,-1, " ");
  //wxCheckBox *boundaryEnable =   new wxCheckBox(m_Dialog, ID_BOUNDARY,         "boundary", p, wxSize(80,20));

  //wxStaticText *featureAngleTitle  = new wxStaticText(m_Dialog,-1, " feature angle:");
  //wxTextCtrl   *featureAngle = new wxTextCtrl  (m_Dialog,ID_FEATURE_ANGLE, "",								 		p,wxSize(50, 16 ), wxNO_BORDER );


  wxStaticText *help  = new wxStaticText(m_Dialog,-1, "Use CTRL to select cells");

	mafGUIButton  *unselectAllButton =    new mafGUIButton(m_Dialog, ID_UNSELECT,    "unselect all", p,wxSize(80,20));
	mafGUIButton  *b_fit =    new mafGUIButton(m_Dialog, ID_FIT,    "reset camera", p,wxSize(80,20));
	mafGUIButton  *smoothButton =    new mafGUIButton(m_Dialog, ID_SMOOTH, "smooth", p,wxSize(80,20));

	mafGUIButton  *resetButton =    new mafGUIButton(m_Dialog, ID_RESET, "reset", p,wxSize(80,20));
	
	mafGUIButton  *ok =     new mafGUIButton(m_Dialog, ID_OK,     "ok", p, wxSize(80,20));
	mafGUIButton  *cancel = new mafGUIButton(m_Dialog, ID_CANCEL, "cancel", p, wxSize(80,20));

     // AACC: 10-03-2009 please add more decimal digits...
    diameter->SetValidator(mafGUIValidator(this,ID_DIAMETER,diameter,&m_Diameter,m_MinBrushSize,m_MaxBrushMSize, 4));
	unselect->SetValidator(mafGUIValidator(this, ID_DELETE, unselect, &m_UnselectCells));

  //smoothIterations->SetValidator(mafGUIValidator(this,ID_ITERATIONS,smoothIterations,&m_SmoothParameterNumberOfInteractions,0));
  //boundaryEnable->SetValidator(mafGUIValidator(this, ID_BOUNDARY, boundaryEnable, &m_SmoothParameterBoundary));
  //featureAngle->SetValidator(mafGUIValidator(this, ID_FEATURE_ANGLE, featureAngle, &m_SmoothParameterFeatureAngle));

	unselectAllButton->SetValidator(mafGUIValidator(this,ID_UNSELECT,unselectAllButton));
	b_fit->SetValidator(mafGUIValidator(this,ID_FIT,b_fit));
	smoothButton->SetValidator(mafGUIValidator(this,ID_SMOOTH,smoothButton));
	resetButton->SetValidator(mafGUIValidator(this,ID_RESET,resetButton));
	
	ok->SetValidator(mafGUIValidator(this,ID_OK,ok));
	cancel->SetValidator(mafGUIValidator(this,ID_CANCEL,cancel));

	wxBoxSizer *h_sizer0 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer0->Add(help,     0,wxRIGHT);	

	wxBoxSizer *h_sizer1 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer1->Add(brushSize,     0,wxRIGHT);	
	h_sizer1->Add(diameter,     0,wxRIGHT);	
	h_sizer1->Add(foo,     0,wxRIGHT);	
	h_sizer1->Add(unselect,     0,wxRIGHT);
  
  h_sizer1->Add(foo1,     0,wxRIGHT);
  //h_sizer1->Add(smoothNumberIterations,     0,wxRIGHT);
  //h_sizer1->Add(smoothIterations, 0, wxRIGHT);
  //h_sizer1->Add(foo2, 0, wxRIGHT);
  //h_sizer1->Add(boundaryEnable, 0, wxRIGHT);
  //h_sizer1->Add(featureAngleTitle, 0, wxRIGHT);
  //h_sizer1->Add(featureAngle, 0, wxRIGHT);
  

	wxBoxSizer *h_sizer2 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer2->Add(unselectAllButton,     0,wxRIGHT);	
	h_sizer2->Add(b_fit,			0,wxRIGHT);	
	h_sizer2->Add(smoothButton,	0,wxRIGHT);	
	h_sizer2->Add(resetButton,0,wxRIGHT);
	h_sizer2->Add(ok,					0,wxRIGHT);
	h_sizer2->Add(cancel,			0,wxRIGHT);

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
void medOpSmoothSurfaceCells::CreateCellFilters()
//----------------------------------------------------------------------------
{
  m_CellFilter = vtkMAFCellsFilter::New();
  m_CellFilter->SetInput(m_ResultPolydata);
  m_CellFilter->Update();

  m_RemoveSelectedCells = vtkMAFRemoveCellsFilter::New();
  m_RemoveSelectedCells->SetInput(m_ResultPolydata);
  m_RemoveSelectedCells->Update();

  m_RemoveUnSelectedCells = vtkMAFRemoveCellsFilter::New();
  m_RemoveUnSelectedCells->SetInput(m_ResultPolydata);
  m_RemoveUnSelectedCells->Update();
}

//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::DestroyCellFilters()
//----------------------------------------------------------------------------
{
  m_RemoveUnSelectedCells->Delete();
  m_RemoveUnSelectedCells=NULL;
  m_RemoveSelectedCells->Delete();
  m_RemoveSelectedCells=NULL;
  m_CellFilter->Delete();
  m_CellFilter=NULL;
}

//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::CreateSurfacePipeline()
//----------------------------------------------------------------------------
{
	CreateCellFilters();

  if(m_PolydataMapper	==NULL)
	  m_PolydataMapper	= vtkPolyDataMapper::New();
	
  m_PolydataMapper->SetInput(m_CellFilter->GetOutput());
	m_PolydataMapper->ScalarVisibilityOn();

  if(m_PolydataActor == NULL)
	  m_PolydataActor = vtkActor::New();

	m_PolydataActor->SetMapper(m_PolydataMapper);

}
//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::DeleteOpDialog()
//----------------------------------------------------------------------------
{
	m_Mouse->RemoveObserver(m_SelectCellInteractor);

	mafDEL(m_SelectCellInteractor);

	vtkDEL(m_PolydataMapper);
	vtkDEL(m_PolydataActor);
	vtkDEL(m_CellFilter);
  vtkDEL(m_RemoveSelectedCells);
  vtkDEL(m_RemoveUnSelectedCells);

	cppDEL(m_Rwi); 
	cppDEL(m_Dialog);
}


//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::OnEvent(mafEventBase *maf_event)
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
      m_RemoveSelectedCells->UndoMarks();
      m_RemoveUnSelectedCells->UndoMarks();
			m_CellFilter->UndoMarks();
			m_Rwi->m_RenderWindow->Render();
			break;

		case ID_FIT:
			{
				vtkPolyData *polydata = m_ResultPolydata;

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
            m_RegionNumber = -m_UnselectCells;
			}     
			break ;

		case ID_SMOOTH:
			{
				SmoothCells();
				m_Rwi->m_RenderWindow->Render();
			}     
			break ;
		case ID_RESET:
			{
				m_ResultPolydata->DeepCopy(m_OriginalPolydata);
				m_ResultPolydata->Update();
				DestroyCellFilters();
				CreateSurfacePipeline();
				InitializeMesh();
				int maxVisitedCells = m_OriginalPolydata->GetNumberOfCells();
				m_VisitedCells = new int[maxVisitedCells];
				for ( int i=0; i < maxVisitedCells; i++ )
				{
					m_VisitedCells[i] = -1;
				}

				m_Rwi->m_RenderWindow->Render();
			}     
			break ;
		
		default:
			mafEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::TraverseMeshAndMark( double radius )
//----------------------------------------------------------------------------
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
			if ( m_VisitedCells[cellId] != m_RegionNumber )
			{
				// mark it as visited
				m_VisitedCells[cellId] = m_RegionNumber;

				double currentCellCenter[3] = {0,0,0};
				FindTriangleCellCenter(cellId, currentCellCenter);

				// mark the cell if the distance criterion is ok
				if (vtkMath::Distance2BetweenPoints(seedCenter, currentCellCenter)
					< (m_Diameter*m_Diameter / 4))
				{
					m_UnselectCells ? m_CellFilter->UnmarkCell(cellId) : m_CellFilter->MarkCell(cellId);
          m_UnselectCells ? m_RemoveSelectedCells->UnmarkCell(cellId) : m_RemoveSelectedCells->MarkCell(cellId);
          m_UnselectCells ? m_RemoveUnSelectedCells->UnmarkCell(cellId) : m_RemoveUnSelectedCells->MarkCell(cellId);
				}

				// get its points
				m_Mesh->GetCellPoints(cellId, numCellPoints, cellPointsList);

				// for each cell point
				for (idPoint=0; idPoint < numCellPoints; idPoint++) 
				{
					// if the point has not been yet visited
					ptId=cellPointsList[idPoint];
					
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
//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::MarkCellsInRadius(double radius)
//----------------------------------------------------------------------------
{

	vtkNEW(m_NeighborCellPointIds);

	m_NeighborCellPointIds->Allocate(3);

	//vtkIdType i;
	vtkIdType numPts, numCells;
	vtkPoints *inPts;
	vtkPolyData *polydata = m_ResultPolydata;

	//  Check input/allocate storage
	//
	inPts = polydata->GetPoints();

	if (inPts == NULL)
	{
		mafLogMessage("No points!");
		return;
	}

	numPts = inPts->GetNumberOfPoints();
	numCells = polydata->GetNumberOfCells();

	if ( numPts < 1 || numCells < 1 )
	{
		mafLogMessage("No data to connect!");
		return;
	}

	// Initialize.  Keep track of points and cells visited.
	//

	// heuristic
	//int maxVisitedCells = numCells;
	////m_VisitedCells = new int[maxVisitedCells];
	//for ( i=0; i < maxVisitedCells; i++ )
	//{
	//	m_VisitedCells[i] = -1;
	//}

	// Traverse all cells marking those visited. Connected region grows 
	// using a connected wave propagation.

	vtkNEW(m_Wave);
	m_Wave->Allocate(numPts);

	vtkNEW(m_Wave2);
	m_Wave2->Allocate(numPts);

	// only one region with ID 0
	//m_RegionNumber = ID_REGION;

	m_Wave->InsertNextId(m_CellSeed);

	//mark the seeded region
	TraverseMeshAndMark(m_Diameter/2);

	m_Wave->Reset();
	m_Wave2->Reset();

	//delete [] m_VisitedCells;

	vtkDEL(m_NeighborCellPointIds);
	vtkDEL(m_Wave);
	vtkDEL(m_Wave2);
}
//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::SetSeed( vtkIdType cellSeed )
//----------------------------------------------------------------------------
{
	m_CellSeed = cellSeed;
}
//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::FindTriangleCellCenter(vtkIdType id, double center[3])
//----------------------------------------------------------------------------
{
	double p0[3] = {0,0,0};
	double p1[3] = {0,0,0};
	double p2[3] = {0,0,0};

	assert(m_Mesh->GetCell(id)->GetNumberOfPoints() == 3);

	vtkIdList *list = vtkIdList::New();
	list->SetNumberOfIds(3);

	m_Mesh->GetCellPoints(id, list);

	m_Mesh->GetPoint(list->GetId(0),p0);
	m_Mesh->GetPoint(list->GetId(1),p1);
	m_Mesh->GetPoint(list->GetId(2),p2);

	vtkTriangle::TriangleCenter(p0,p1,p2,center);

	list->Delete();
}
//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::InitializeMesh()
//----------------------------------------------------------------------------
{
	// Build cell structure
	//
	vtkNEW(m_Mesh);
	vtkPolyData *polydata = m_ResultPolydata;
	assert(polydata);

	this->m_Mesh->CopyStructure(polydata);
	this->m_Mesh->BuildLinks();
}
//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::SmoothCells()
//----------------------------------------------------------------------------
{
	// perform cells removing...
	if (m_TestMode == false)
	{
		wxBusyInfo("Smooth selection...");
	}

  m_RemoveUnSelectedCells->ReverseRemoveOn();
  m_RemoveUnSelectedCells->RemoveMarkedCells();
  m_RemoveUnSelectedCells->Update();

  vtkMAFSmartPointer<vtkPolyData> toSmoothPolyData;
  toSmoothPolyData->DeepCopy(m_RemoveUnSelectedCells->GetOutput());
  toSmoothPolyData->Update();

  m_RemoveSelectedCells->RemoveMarkedCells();
  m_RemoveSelectedCells->Update();

  vtkMAFSmartPointer<vtkPolyData> polyData;
  polyData->DeepCopy(m_RemoveSelectedCells->GetOutput());
  polyData->Update();


  /*vtkMAFSmartPointer<vtkLinearSubdivisionFilter> linearSubdivisionFilter;
  linearSubdivisionFilter->SetInput(toSmoothPolyData);
  linearSubdivisionFilter->SetNumberOfSubdivisions(1);
  linearSubdivisionFilter->Update();

  int num = linearSubdivisionFilter->GetOutput()->GetNumberOfPoints();*/

  vtkMAFSmartPointer<vtkSmoothPolyDataFilter> smoothFilter;
  smoothFilter->SetInput(toSmoothPolyData);
  smoothFilter->SetNumberOfIterations(m_SmoothParameterNumberOfInteractions);
  smoothFilter->BoundarySmoothingOff();//always true
  smoothFilter->FeatureEdgeSmoothingOn();
  smoothFilter->SetFeatureAngle(m_SmoothParameterFeatureAngle);
  smoothFilter->Update();

  vtkMAFSmartPointer<vtkAppendPolyData> appendFilter; 
  appendFilter->AddInput(smoothFilter->GetOutput());
  appendFilter->AddInput(polyData);
  appendFilter->Update();

  vtkMAFSmartPointer<vtkCleanPolyData> cleanFilter; 
  cleanFilter->SetInput(appendFilter->GetOutput());
  cleanFilter->Update();


  m_ResultPolydata->DeepCopy(cleanFilter->GetOutput());

	m_ResultPolydata->Modified();
	m_ResultPolydata->Update();

  DestroyCellFilters();

  CreateSurfacePipeline();
  InitializeMesh();
  // reset structures
  int maxVisitedCells = m_OriginalPolydata->GetNumberOfCells();
  m_VisitedCells = new int[maxVisitedCells];
	for ( int i=0; i < maxVisitedCells; i++ )
	{
		m_VisitedCells[i] = -1;
	}

}
//----------------------------------------------------------------------------
void medOpSmoothSurfaceCells::MarkCells()
//----------------------------------------------------------------------------
{
	MarkCellsInRadius(m_Diameter/2);  
}
