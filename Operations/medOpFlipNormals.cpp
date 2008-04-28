/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpFlipNormals.cpp,v $
Language:  C++
Date:      $Date: 2008-04-28 08:37:52 $
Version:   $Revision: 1.1 $
Authors:   Matteo Giacomoni - Daniele Giunchi
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

#include "medOpFlipNormals.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"

#include "mafRWI.h"
#include "mmdMouse.h"

#include "mmgDialog.h"
#include "mmgButton.h"
#include "mmgValidator.h"

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
#include "vtkCellsFilter.h"
#include "vtkCellCenters.h" 
#include "vtkGlyph3D.h"
#include "vtkArrowSource.h"
#include "vtkOBBTree.h"
#include "vtkPolyDataNormals.h"

const int ID_REGION = 0;

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpFlipNormals);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpFlipNormals::medOpFlipNormals(wxString label) :
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

	m_NormalGlyph = NULL;
	m_NormalMapper = NULL;
	m_NormalActor = NULL;
	m_CenterPointsFilter = NULL;
	m_NormalArrow = NULL;
}
//----------------------------------------------------------------------------
medOpFlipNormals::~medOpFlipNormals()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Mesh);
	vtkDEL(m_ResultPolydata);
	vtkDEL(m_OriginalPolydata);

	vtkDEL(m_NormalActor);
	vtkDEL(m_NormalMapper);
	vtkDEL(m_CenterPointsFilter);
	vtkDEL(m_NormalGlyph);
}
//----------------------------------------------------------------------------
mafOp* medOpFlipNormals::Copy()
//----------------------------------------------------------------------------
{
	/** return a copy of itself, needs to put it into the undo stack */
	return new medOpFlipNormals(m_Label);
}
//----------------------------------------------------------------------------
bool medOpFlipNormals::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	return vme != NULL && vme->IsMAFType(mafVMESurface);
}
//----------------------------------------------------------------------------
void medOpFlipNormals::OpRun()
//----------------------------------------------------------------------------
{

	if(!((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData()->GetCellData()->GetNormals()))
	{
		vtkMAFSmartPointer<vtkPolyDataNormals> normalFilter;
		normalFilter->SetInput((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());

		normalFilter->ComputeCellNormalsOn();
		normalFilter->SplittingOff();
		normalFilter->FlipNormalsOff();
		normalFilter->SetFeatureAngle(30);
		normalFilter->Update();

		((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData())->DeepCopy(normalFilter->GetOutput());
	}

	vtkNEW(m_ResultPolydata);
	m_ResultPolydata->DeepCopy((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());

	vtkNEW(m_OriginalPolydata);
	m_OriginalPolydata->DeepCopy((vtkPolyData*)((mafVME *)m_Input)->GetOutput()->GetVTKData());

	int result = OP_RUN_CANCEL;

	CreateSurfacePipeline();
	CreateNormalsPipe();
	InitializeMesh();

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
void medOpFlipNormals::OpDo()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->SetData(m_ResultPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpFlipNormals::OpUndo()
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
	ID_FLIP,
	ID_RESET,
	ID_ALL_NORMAL,
};
//----------------------------------------------------------------------------
void medOpFlipNormals::CreateOpDialog()
//----------------------------------------------------------------------------
{
	wxBusyCursor wait;

	//===== setup interface ====
	m_Dialog = new mmgDialog("Flip Normals", mafCLOSEWINDOW | mafRESIZABLE);

	m_Rwi = new mafRWI(m_Dialog,ONE_LAYER,false);
	m_Rwi->SetListener(this);
	m_Rwi->CameraSet(CAMERA_PERSPECTIVE);

	m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.0001f);
	m_Rwi->SetSize(0,0,800,800);
	m_Rwi->Show(true);
	m_Rwi->m_RwiBase->SetMouse(m_Mouse);

	m_Rwi->m_RenFront->AddActor(m_PolydataActor);
	m_Rwi->m_RenFront->AddActor(m_NormalActor);

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

	wxStaticText *help  = new wxStaticText(m_Dialog,-1, "Use CTRL to select cells");

	mmgButton  *unselectAllButton =    new mmgButton(m_Dialog, ID_UNSELECT,    "unselect all", p,wxSize(80,20));
	mmgButton  *b_fit =    new mmgButton(m_Dialog, ID_FIT,    "reset camera", p,wxSize(80,20));
	mmgButton  *flipButton =    new mmgButton(m_Dialog, ID_FLIP, "flip", p,wxSize(80,20));
	mmgButton  *resetButton =    new mmgButton(m_Dialog, ID_RESET, "reset", p,wxSize(80,20));
	mmgButton  *allNormal =    new mmgButton(m_Dialog, ID_ALL_NORMAL, "All Normal", p,wxSize(80,20));
	mmgButton  *ok =     new mmgButton(m_Dialog, ID_OK,     "ok", p, wxSize(80,20));
	mmgButton  *cancel = new mmgButton(m_Dialog, ID_CANCEL, "cancel", p, wxSize(80,20));

	diameter->SetValidator(mmgValidator(this,ID_DIAMETER,diameter,&m_Diameter,m_MinBrushSize,m_MaxBrushMSize));
	unselect->SetValidator(mmgValidator(this, ID_DELETE, unselect, &m_UnselectCells));

	unselectAllButton->SetValidator(mmgValidator(this,ID_UNSELECT,unselectAllButton));
	b_fit->SetValidator(mmgValidator(this,ID_FIT,b_fit));
	flipButton->SetValidator(mmgValidator(this,ID_FLIP,flipButton));
	resetButton->SetValidator(mmgValidator(this,ID_RESET,resetButton));
	allNormal->SetValidator(mmgValidator(this,ID_ALL_NORMAL,allNormal));
	ok->SetValidator(mmgValidator(this,ID_OK,ok));
	cancel->SetValidator(mmgValidator(this,ID_CANCEL,cancel));

	wxBoxSizer *h_sizer0 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer0->Add(help,     0,wxRIGHT);	

	wxBoxSizer *h_sizer1 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer1->Add(brushSize,     0,wxRIGHT);	
	h_sizer1->Add(diameter,     0,wxRIGHT);	
	h_sizer1->Add(foo,     0,wxRIGHT);	
	h_sizer1->Add(unselect,     0,wxRIGHT);	

	wxBoxSizer *h_sizer2 = new wxBoxSizer(wxHORIZONTAL);
	h_sizer2->Add(unselectAllButton,     0,wxRIGHT);	
	h_sizer2->Add(b_fit,			0,wxRIGHT);	
	h_sizer2->Add(flipButton,	0,wxRIGHT);	
	h_sizer2->Add(resetButton,0,wxRIGHT);
	h_sizer2->Add(allNormal,	0,wxRIGHT);
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
void medOpFlipNormals::CreateSurfacePipeline()
//----------------------------------------------------------------------------
{
	m_CellFilter = vtkCellsFilter::New();
	m_CellFilter->SetInput(m_ResultPolydata);
	m_CellFilter->Update();

	m_PolydataMapper	= vtkPolyDataMapper::New();
	m_PolydataMapper->SetInput(m_CellFilter->GetOutput());
	m_PolydataMapper->ScalarVisibilityOn();

	m_PolydataActor = vtkActor::New();
	m_PolydataActor->SetMapper(m_PolydataMapper);

}
//----------------------------------------------------------------------------
void medOpFlipNormals::CreateNormalsPipe()
//----------------------------------------------------------------------------
{
	vtkNEW(m_CenterPointsFilter);
	m_CenterPointsFilter->SetInput(m_ResultPolydata);
	m_CenterPointsFilter->Update();
	
	m_Centers = m_CenterPointsFilter->GetOutput();
	m_Centers->Update();
	m_Centers->GetPointData()->SetNormals(m_ResultPolydata->GetCellData()->GetNormals());
	m_Centers->Update();

	double bounds[6];
	m_ResultPolydata->GetBounds(bounds);
	double maxBounds = (bounds[1]-bounds[0] < bounds[3]-bounds[2])?bounds[1]-bounds[0]:bounds[3]-bounds[2];
	maxBounds = (maxBounds<bounds[5]-bounds[4])?maxBounds:bounds[5]-bounds[4];

	vtkNEW(m_NormalArrow);
	m_NormalArrow->SetTipLength(0.0);
	m_NormalArrow->SetTipRadius(0.0);
	m_NormalArrow->SetShaftRadius(0.005*maxBounds);
	m_NormalArrow->SetTipResolution(16);
	m_NormalArrow->SetShaftResolution(16);
	m_NormalArrow->Update();

	vtkNEW(m_NormalGlyph);
	m_NormalGlyph->SetInput(m_Centers);
	m_NormalGlyph->SetSource(m_NormalArrow->GetOutput());
	m_NormalGlyph->SetVectorModeToUseNormal();
	m_NormalGlyph->Update();

	vtkNEW(m_NormalMapper);
	m_NormalMapper->SetInput(m_NormalGlyph->GetOutput());
	m_NormalMapper->Update();

	vtkNEW(m_NormalActor);
	m_NormalActor->SetMapper(m_NormalMapper);
	m_NormalActor->SetVisibility(true);
	m_NormalActor->PickableOff();
	m_NormalActor->Modified();
}
//----------------------------------------------------------------------------
void medOpFlipNormals::DeleteOpDialog()
//----------------------------------------------------------------------------
{
	m_Mouse->RemoveObserver(m_SelectCellInteractor);

	mafDEL(m_SelectCellInteractor);

	vtkDEL(m_PolydataMapper);
	vtkDEL(m_PolydataActor);
	vtkDEL(m_CellFilter);

	cppDEL(m_Rwi); 
	cppDEL(m_Dialog);
}


//----------------------------------------------------------------------------
void medOpFlipNormals::OnEvent(mafEventBase *maf_event)
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
			m_CellFilter->UndoMarks();
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

		case ID_FLIP:
			{
				FlipNormals();
				m_CellFilter->UndoMarks();
				m_Rwi->m_RenderWindow->Render();
			}     
			break ;
		case ID_RESET:
			{
				m_ResultPolydata->DeepCopy(m_OriginalPolydata);
				m_ResultPolydata->Update();
				m_Rwi->m_RenderWindow->Render();
			}     
			break ;
		case ID_ALL_NORMAL:
			{
				ModifyAllNormal();
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
void medOpFlipNormals::ModifyAllNormal()
//----------------------------------------------------------------------------
{
	vtkOBBTree *OBBFilter;
	vtkNEW(OBBFilter);
	OBBFilter->SetDataSet(m_ResultPolydata);
	OBBFilter->CacheCellBoundsOn();
	OBBFilter->BuildLocator();

	double bounds[6];
	m_ResultPolydata->GetBounds(bounds);

	double dimX, dimY, dimZ;
	dimX = (bounds[1] - bounds[0]);
	dimY = (bounds[3] - bounds[2]);
	dimZ = (bounds[5] - bounds[4]);

	double maxBound = (dimX >= dimY) ? (dimX >= dimZ ? dimX : dimZ) : (dimY >= dimZ ? dimY : dimZ); 

	//Check direction of the first normal
	vtkPoints *p;
	vtkNEW(p);

  if(m_CellFilter->GetNumberOfMarkedCells() == 0)
  {
    wxMessageBox("Must select at least one cell");
    return;
  }

	double *normal=m_ResultPolydata->GetCellData()->GetNormals()->GetTuple3(m_CellFilter->GetIdMarkedCell(0));
	int sign[3];
	sign[0]=normal[0]<0? -1 :1;
	sign[1]=normal[1]<0? -1 :1;
	sign[2]=normal[2]<0? -1 :1;

	double point1[3];
	FindTriangleCellCenter(m_CellFilter->GetIdMarkedCell(0),point1);
	point1[0]+=(normal[0]*0.001);
	point1[1]+=(normal[1]*0.001);
	point1[2]+=(normal[2]*0.001);
	//point1=m_Centers->GetPoint(m_CellFilter->GetIdMarkedCell(0));
	double point2[3];
	point2[0]=(point1[0])+((normal[0]*10)*maxBound);
	point2[1]=(point1[1])+((normal[1]*10)*maxBound);
	point2[2]=(point1[2])+((normal[2]*10)*maxBound);
	OBBFilter->IntersectWithLine(point1,point2,p,NULL);

	int direction=p->GetNumberOfPoints()%2;
	vtkDEL(p);

	for(int i=0;i<m_Centers->GetNumberOfPoints();i++)
	{
		vtkNEW(p);
		FindTriangleCellCenter(i,point1);
		normal=m_ResultPolydata->GetCellData()->GetNormals()->GetTuple3(i);
		point1[0]+=(normal[0]*0.001);
		point1[1]+=(normal[1]*0.001);
		point1[2]+=(normal[2]*0.001);
		int sign[3];
		sign[0]=normal[0]<0? -1 :1;
		sign[1]=normal[1]<0? -1 :1;
		sign[2]=normal[2]<0? -1 :1;

		point2[0]=(point1[0])+((normal[0]*10)*maxBound);
		point2[1]=(point1[1])+((normal[1]*10)*maxBound);
		point2[2]=(point1[2])+((normal[2]*10)*maxBound);
		OBBFilter->IntersectWithLine(point1,point2,p,NULL);

		if((p->GetNumberOfPoints()%2)!=direction)
		{
			double *new_normal=m_ResultPolydata->GetCellData()->GetNormals()->GetTuple3(i);
			new_normal[0]=-new_normal[0];
			new_normal[1]=-new_normal[1];
			new_normal[2]=-new_normal[2];
			m_ResultPolydata->GetCellData()->GetNormals()->SetTuple3(i,new_normal[0],new_normal[1],new_normal[2]);
		}
		vtkDEL(p);
	}
	m_ResultPolydata->Modified();
	m_ResultPolydata->Update();
}
//----------------------------------------------------------------------------
void medOpFlipNormals::TraverseMeshAndMark( double radius )
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
					m_UnselectCells ? m_CellFilter->UnmarkCell(cellId) : m_CellFilter->MarkCell(cellId);
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
//----------------------------------------------------------------------------
void medOpFlipNormals::MarkCellsInRadius(double radius)
//----------------------------------------------------------------------------
{

	vtkNEW(m_NeighborCellPointIds);

	m_NeighborCellPointIds->Allocate(3);

	vtkIdType i;
	vtkIdType numPts, numCells;
	vtkPoints *inPts;
	vtkPolyData *polydata = vtkPolyData::SafeDownCast(((mafVME *)m_Input)->GetOutput()->GetVTKData());

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
	int maxVisitedCells = numCells;
	m_VisitedCells = new int[maxVisitedCells];
	for ( i=0; i < maxVisitedCells; i++ )
	{
		m_VisitedCells[i] = -1;
	}

	// heuristic
	int maxVisitedPoints = numPts;
	m_VisitedPoints = new vtkIdType[maxVisitedPoints];  
	for ( i=0; i < maxVisitedPoints; i++ )
	{
		m_VisitedPoints[i] = -1;
	}

	// Traverse all cells marking those visited. Connected region grows 
	// using a connected wave propagation.

	vtkNEW(m_Wave);
	m_Wave->Allocate(numPts);

	vtkNEW(m_Wave2);
	m_Wave2->Allocate(numPts);

	m_PointNumber = 0;

	// only one region with ID 0
	m_RegionNumber = ID_REGION;

	m_Wave->InsertNextId(m_CellSeed);

	//mark the seeded region
	TraverseMeshAndMark(m_Diameter/2);

	m_Wave->Reset();
	m_Wave2->Reset();

	delete [] m_VisitedCells;
	delete [] m_VisitedPoints;

	vtkDEL(m_NeighborCellPointIds);
	vtkDEL(m_Wave);
	vtkDEL(m_Wave2);
}
//----------------------------------------------------------------------------
void medOpFlipNormals::SetSeed( vtkIdType cellSeed )
//----------------------------------------------------------------------------
{
	m_CellSeed = cellSeed;
}
//----------------------------------------------------------------------------
void medOpFlipNormals::FindTriangleCellCenter(vtkIdType id, double center[3])
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
void medOpFlipNormals::InitializeMesh()
//----------------------------------------------------------------------------
{
	// Build cell structure
	//
	vtkNEW(m_Mesh);
	vtkPolyData *polydata = vtkPolyData::SafeDownCast(((mafVME *)m_Input)->GetOutput()->GetVTKData());
	assert(polydata);

	this->m_Mesh->CopyStructure(polydata);
	this->m_Mesh->BuildLinks();
}
//----------------------------------------------------------------------------
void medOpFlipNormals::FlipNormals()
//----------------------------------------------------------------------------
{
	// perform cells removing...
	if (m_TestMode == false)
	{
		wxBusyInfo("Flip normals...");
	}

	for(int i=0;i<m_CellFilter->GetNumberOfMarkedCells();i++)
	{
		int j=0;
		bool Find=false;
		while(j<i && !Find)
		{
			if(m_CellFilter->GetIdMarkedCell(i) == m_CellFilter->GetIdMarkedCell(j))
				Find=true;
			j++;
		}
		if(Find)
			continue;

		vtkFloatArray *array;
		array=vtkFloatArray::SafeDownCast(m_ResultPolydata->GetCellData()->GetNormals());
		double *normal;
		normal=array->GetTuple3(m_CellFilter->GetIdMarkedCell(i));
		normal[0]=-normal[0];
		normal[1]=-normal[1];
		normal[2]=-normal[2];
		m_ResultPolydata->GetCellData()->GetNormals()->SetTuple3(m_CellFilter->GetIdMarkedCell(i),normal[0],normal[1],normal[2]);
	}
	m_ResultPolydata->Modified();
	m_ResultPolydata->Update();
}
//----------------------------------------------------------------------------
void medOpFlipNormals::MarkCells()
//----------------------------------------------------------------------------
{
	MarkCellsInRadius(m_Diameter/2);  
}
