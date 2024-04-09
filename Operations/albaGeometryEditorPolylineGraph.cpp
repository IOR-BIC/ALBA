/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGeometryEditorPolylineGraph
 Authors: Matteo Giacomoni
 
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

#include "albaGeometryEditorPolylineGraph.h"
#include "albaDecl.h"

#include "albaGUI.h"
#include "albaVMEPolylineEditor.h"
#include "albaVMEPolylineGraph.h"
#include "albaInteractorPicker.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkCharArray.h"
#include "vtkSphereSource.h"
#include "vtkGlyph3D.h"
#include "vtkTubeFilter.h"
#include "vtkAppendPolyData.h"
#include "vtkCellData.h"
#include "vtkCellLocator.h"
#include "vtkOBBTree.h"
#include "vtkCellArray.h"
#include "vtkCell.h"
#include "vtkCellLocator.h"

//----------------------------------------------------------------------------
albaGeometryEditorPolylineGraph::albaGeometryEditorPolylineGraph(albaVME *input, albaObserver *listener,albaVMEPolylineGraph *polyline,bool testMode)
//----------------------------------------------------------------------------
{
	m_PolylineGraph = new albaPolylineGraph;

	m_TestMode = testMode;
	
	if(polyline)
  {
		m_PolylineGraph->CopyFromPolydata(vtkPolyData::SafeDownCast(polyline->GetOutput()->GetVTKData()));
    m_PolylineGraph->MergeSimpleJoinedBranches();
  }

	vtkALBASmartPointer<vtkPolyData> data;
	m_PolylineGraph->CopyToPolydata(data);

	albaNEW(m_VMEPolylineEditor);
	m_VMEPolylineEditor->SetName("VME Editor");
	if(input)
		m_VMEPolylineEditor->ReparentTo(input->GetRoot());

	m_InputVME	=	input;
	m_Listener	= listener;

	m_CurrentBranch = 0;

	//Gui Variables
	m_Action			= ID_POINT_ACTION;
	m_PointTool		= ID_ADD_POINT;
	m_BranchTool	= ID_ADD_BRANCH;
  m_SphereRadius = 1.0;

	m_SelectedPoint = UNDEFINED_POINT_ID;
	m_SelectedPointVTK = UNDEFINED_POINT_ID;
	m_SelectedBranch = UNDEFINED_BRANCH_ID;

	m_OldBehavior = NULL;
	m_Picker			= NULL;

	if(!m_TestMode)
	{
		CreateGui();
		CreateISA();
	}

	CreatePipe();

	UpdateVMEEditorData(data);
	//m_VMEPolylineEditor->SetData(m_AppendPolydata->GetOutput(),0.0);

	albaNEW(m_VMEPolylineSelection);
	if(input)
		m_VMEPolylineSelection->ReparentTo(input->GetRoot());

	m_SelectedPoint1ForInserting = UNDEFINED_POINT_ID;
	m_SelectedPoint2ForInserting = UNDEFINED_POINT_ID; 
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraph::CreatePipe() 
//----------------------------------------------------------------------------
{
	vtkALBASmartPointer<vtkPolyData> data;
	m_PolylineGraph->CopyToPolydata(data);

	//vtkALBASmartPointer<vtkSphereSource> Sphere;
  vtkNEW(m_Sphere);
	m_Sphere->SetRadius(m_SphereRadius);
	m_Sphere->SetPhiResolution(5);
	m_Sphere->SetThetaResolution(5);

	vtkNEW(m_Glyph);
	m_Glyph->SetInputData(data);
	m_Glyph->SetSourceConnection(m_Sphere->GetOutputPort());
	m_Glyph->SetScaleModeToDataScalingOff();
	m_Glyph->SetRange(0.0,1.0);
	m_Glyph->Update();

	vtkNEW(m_Tube);
	m_Tube->UseDefaultNormalOff();
	m_Tube->SetInputData(data);
	m_Tube->SetRadius(m_SphereRadius/2);
	m_Tube->SetCapping(true);
	m_Tube->SetNumberOfSides(5);

	vtkNEW(m_AppendPolydata);
	m_AppendPolydata->AddInputConnection(m_Tube->GetOutputPort());
	m_AppendPolydata->AddInputConnection(m_Glyph->GetOutputPort());
	m_AppendPolydata->Update();
}
//----------------------------------------------------------------------------
albaGeometryEditorPolylineGraph::~albaGeometryEditorPolylineGraph() 
//----------------------------------------------------------------------------
{
	if(m_InputVME)
		m_InputVME->SetBehavior(m_OldBehavior);

	m_VMEPolylineEditor->SetBehavior(NULL);
	m_VMEPolylineEditor->ReparentTo(NULL);

	m_VMEPolylineSelection->SetBehavior(NULL);
	m_VMEPolylineSelection->ReparentTo(NULL);


	albaDEL(m_VMEPolylineSelection);
	albaDEL(m_VMEPolylineEditor);
	
	albaDEL(m_Picker);

	vtkDEL(m_AppendPolydata);
	vtkDEL(m_Tube);
	vtkDEL(m_Glyph);
  vtkDEL(m_Sphere);

	delete m_PolylineGraph;
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraph::Show(bool show)
//----------------------------------------------------------------------------
{
	GetLogicManager()->VmeShow(m_VMEPolylineEditor, show);
	if(show==false)
		GetLogicManager()->VmeShow(m_VMEPolylineSelection, show);

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraph::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{
      case ID_SPHERE_RADIUS:
        {
          m_Sphere->SetRadius(m_SphereRadius);
          m_Sphere->Update();
          m_Tube->SetRadius(m_SphereRadius/2);
          m_Tube->Update();
          vtkALBASmartPointer<vtkPolyData> poly_new;
          m_PolylineGraph->CopyToPolydata(poly_new);
          UpdateVMEEditorData(poly_new);
					GetLogicManager()->CameraUpdate();
        }
      break;
			case ID_ACTION:
				{
					m_Gui->Enable(ID_POINT_TOOL,m_Action==ID_POINT_ACTION);
          m_Gui->Enable(ID_SPHERE_RADIUS,m_Action==ID_POINT_ACTION);
					m_Gui->Enable(ID_BRANCH_TOOL,m_Action==ID_BRANCH_ACTION);
					m_Gui->Enable(ID_BUTTON_POINT_DELETE,m_Action==ID_POINT_ACTION && m_SelectedPoint!=UNDEFINED_POINT_ID);
					m_Gui->Enable(ID_BUTTON_BRANCH_DELETE,m_Action==ID_BRANCH_ACTION && m_SelectedBranch!=UNDEFINED_BRANCH_ID);

					BehaviorUpdate();
				}
			break;
			case VME_PICKING:
			case VME_PICKED:
				{
					VmePicked(e);
				}
			break;
			case ID_POINT_TOOL:
			case ID_BRANCH_TOOL:
				{
					BehaviorUpdate();
				}
				break;
			case ID_BUTTON_POINT_DELETE:
				{
					if(DeletePoint(m_SelectedPoint)==ALBA_OK)
					{
						m_SelectedPoint = UNDEFINED_POINT_ID;
						
						if(!m_TestMode)
							GetLogicManager()->VmeShow(m_VMEPolylineSelection, false);

						GetLogicManager()->CameraUpdate();
					}
					else
						albaLogMessage("It's impossible to delete a point of degree > 2");
				}
				break;
			case ID_BUTTON_BRANCH_DELETE:
				{

					DeleteBranch(m_SelectedBranch);
					m_SelectedBranch=UNDEFINED_BRANCH_ID;

					vtkALBASmartPointer<vtkPolyData> poly_new;
					m_PolylineGraph->CopyToPolydata(poly_new);
					UpdateVMEEditorData(poly_new);

					GetLogicManager()->VmeShow(m_VMEPolylineSelection, false);

					GetLogicManager()->VmeVisualModeChanged(m_VMEPolylineEditor);

					GetLogicManager()->CameraUpdate();

				}
		default:
			albaEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraph::BehaviorUpdate()
//----------------------------------------------------------------------------
{
	if(m_Action==ID_POINT_ACTION)
	{
		if(m_PointTool==ID_ADD_POINT)//If the user want to add a point
		{
			m_VMEPolylineEditor->SetBehavior(NULL);
			m_InputVME->SetBehavior(m_Picker);
		}
		else if(m_PointTool==ID_INSERT_POINT||m_PointTool==ID_SELECT_POINT)
		{
			m_VMEPolylineEditor->SetBehavior(m_Picker);
			m_InputVME->SetBehavior(NULL);
		}
		else if(m_PointTool==ID_MOVE_POINT)
		{
			m_VMEPolylineEditor->SetBehavior(m_Picker);
			m_InputVME->SetBehavior(m_Picker);
		}
	}
	else if(m_Action==ID_BRANCH_ACTION)
	{
		if(m_BranchTool==ID_ADD_BRANCH)//If the user want add a branch
		{
			m_VMEPolylineEditor->SetBehavior(NULL);
			m_InputVME->SetBehavior(m_Picker);
		}
		else if(m_BranchTool==ID_SELECT_BRANCH)
		{
			m_VMEPolylineEditor->SetBehavior(m_Picker);
			m_InputVME->SetBehavior(m_Picker);
		}
	}

  albaEventMacro(albaEvent(this,ID_VME_BEHAVIOR_UPDATE,m_InputVME));
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraph::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui =new albaGUI(this);
	wxString choices_action[2]={_("Point"),_("Branch")};
	m_Gui->Radio(ID_ACTION,_("Action"),&m_Action,2,choices_action);

	wxString choices_point_tool[4]={_("Add Point"),_("Insert Point"),_("Move Point"),_("Select Point")};
	m_Gui->Radio(ID_POINT_TOOL,_("Point Tool"),&m_PointTool,4,choices_point_tool);
	m_Gui->Enable(ID_POINT_TOOL,m_Action==ID_POINT_ACTION);

	m_Gui->Double(ID_SPHERE_RADIUS, "Point Radius", &m_SphereRadius, MINDOUBLE, MAXDOUBLE, -1, "", false, 0.35);
  m_Gui->Enable(ID_SPHERE_RADIUS,m_Action==ID_POINT_ACTION);

	m_Gui->Button(ID_BUTTON_POINT_DELETE,_("Delete"));
	m_Gui->Enable(ID_BUTTON_POINT_DELETE,m_Action==ID_POINT_ACTION && m_SelectedPoint!=UNDEFINED_POINT_ID);

	wxString choices_branch_tool[2]={_("Add Branch"),_("Select Branch")};
	m_Gui->Radio(ID_BRANCH_TOOL,_("Branch Tool"),&m_BranchTool,2,choices_branch_tool);
	m_Gui->Enable(ID_BRANCH_TOOL,m_Action==ID_BRANCH_ACTION);

	m_Gui->Button(ID_BUTTON_BRANCH_DELETE,_("Delete"));
	m_Gui->Enable(ID_BUTTON_BRANCH_DELETE,m_Action==ID_BRANCH_ACTION && m_SelectedBranch!=UNDEFINED_BRANCH_ID);
}
//----------------------------------------------------------------------------
albaGUI* albaGeometryEditorPolylineGraph::GetGui()
//----------------------------------------------------------------------------
{
	return m_Gui;
}
//-------------------------------------------------------------------------
int albaGeometryEditorPolylineGraph::AddNewVertex(double vertex[3],vtkIdType branch)
//-------------------------------------------------------------------------
{
	vtkALBASmartPointer<vtkPolyData> polydata;
	int result;
	if(m_PolylineGraph->GetNumberOfBranches()!=0)//If there are at least a branch
	{
		if(m_PolylineGraph->GetNumberOfBranches()>branch && branch >=0)//if variable branch is a right ID
		{
			m_PolylineGraph->AddNewVertexToBranch(branch,vertex);
			m_PolylineGraph->CopyToPolydata(polydata);

			result=UpdateVMEEditorData(polydata);
		}
		else if(branch==UNDEFINED_BRANCH_ID)//if variables branch is -1 the new vertex is added to the current branch
		{
			m_PolylineGraph->AddNewVertexToBranch(m_CurrentBranch,vertex);
			m_PolylineGraph->CopyToPolydata(polydata);
			
			result=UpdateVMEEditorData(polydata);
		}
		else
		{
			result = ALBA_ERROR;
		}
	}
	else if(m_PolylineGraph->GetNumberOfVertices()!=0)//If there are already some vertices new vertex are added to the last vertex
	{
		m_PolylineGraph->AddNewVertex(m_PolylineGraph->GetMaxVertexId(),vertex);
		m_PolylineGraph->CopyToPolydata(polydata);

		result=UpdateVMEEditorData(polydata);
	}
	else if(m_PolylineGraph->GetNumberOfVertices()==0)//If the new vertex is the first of the graph
	{
		m_PolylineGraph->AddNewBranch();
		m_CurrentBranch = m_PolylineGraph->GetNumberOfBranches()-1;
		m_PolylineGraph->AddNewVertexToBranch(m_CurrentBranch,vertex);
		m_PolylineGraph->CopyToPolydata(polydata);

		result=UpdateVMEEditorData(polydata);
	}
	else
	{
		result = ALBA_ERROR;
	}

	return result;
}
//-------------------------------------------------------------------------
int albaGeometryEditorPolylineGraph::UpdateVMEEditorData(vtkPolyData *polydata)
//-------------------------------------------------------------------------
{
	m_Glyph->SetInputData(polydata);
	m_Glyph->Modified();
	m_Glyph->Update();
	m_Tube->SetInputData(polydata);
	m_Tube->Modified();
	m_Tube->Update();

	m_AppendPolydata->RemoveAllInputs();
	m_AppendPolydata->AddInputConnection(m_Glyph->GetOutputPort());
	m_AppendPolydata->AddInputConnection(m_Tube->GetOutputPort());
	m_AppendPolydata->Update();

	vtkALBASmartPointer<vtkCharArray> scalar;
	scalar->SetNumberOfComponents(1);
	scalar->SetNumberOfTuples(m_AppendPolydata->GetOutput()->GetNumberOfPoints());
	for (int i=0;i<m_AppendPolydata->GetOutput()->GetNumberOfPoints();i++)
	{
		scalar->SetTuple1(i,0.0);
	}
	m_AppendPolydata->GetOutput()->GetPointData()->SetScalars(scalar);
	m_AppendPolydata->Update();

	int result = m_VMEPolylineEditor->SetData(m_AppendPolydata->GetOutput(),m_VMEPolylineEditor->GetTimeStamp());
	m_VMEPolylineEditor->Update();

	return result;
}
//-------------------------------------------------------------------------
void albaGeometryEditorPolylineGraph::CreateISA()
//-------------------------------------------------------------------------
{
	m_OldBehavior = m_InputVME->GetBehavior();

	m_Picker = albaInteractorPicker::New();
	m_Picker->SetListener(this);

	m_InputVME->SetBehavior(m_Picker);
	m_VMEPolylineEditor->SetBehavior(m_Picker);

  albaEventMacro(albaEvent(this,ID_VME_BEHAVIOR_UPDATE,m_InputVME));
}
//----------------------------------------------------------------------------
vtkPolyData* albaGeometryEditorPolylineGraph::GetOutput()
//----------------------------------------------------------------------------
{
	vtkPolyData *polydata;
	vtkNEW(polydata);
	m_PolylineGraph->CopyToPolydata(polydata);
	return polydata;
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraph::VmePicked(albaEvent *e)
//----------------------------------------------------------------------------
{
	if(e->GetSender()==m_Picker)
	{
		if(m_Action==ID_POINT_ACTION)
		{
			if(m_PointTool==ID_ADD_POINT)
			{
				m_Picker->EnableContinuousPicking(false);
				double pos[3];
				vtkPoints *pts = NULL; 
				pts = (vtkPoints *)e->GetVtkObj();
				pts->GetPoint(0,pos);

				AddNewVertex(pos);

				if(!m_TestMode)
					GetLogicManager()->VmeShow(m_VMEPolylineSelection, false);
			}
			else if(m_PointTool==ID_MOVE_POINT)
			{
				double vertexCoord[3];
				vtkPoints *pts = NULL; 
				pts = (vtkPoints *)e->GetVtkObj();
				pts->GetPoint(0,vertexCoord);

        if(e->GetId()==VME_PICKED)
          m_Picker->EnableContinuousPicking(!m_Picker->IsContinuousPicking());

        if(e->GetId()==VME_PICKED)
        {
          SelectPoint(vertexCoord);
          if(!m_TestMode)
          {
						GetLogicManager()->VmeVisualModeChanged(m_VMEPolylineSelection);
          }
        }
        MovePoint(vertexCoord);

        #ifndef _DEBUG
          albaLogMessage(albaString::Format("%.3f %.3f %.3f",vertexCoord[0],vertexCoord[1],vertexCoord[2]));
        #endif
			}
			else if(m_PointTool==ID_SELECT_POINT)
			{
				m_Picker->EnableContinuousPicking(false);
				double vertexCoord[3];
				vtkPoints *pts = NULL; 
				pts = (vtkPoints *)e->GetVtkObj();
				pts->GetPoint(0,vertexCoord);

				SelectPoint(vertexCoord);

				if(!m_TestMode)
				{
					GetLogicManager()->VmeVisualModeChanged(m_VMEPolylineSelection);
				}

				m_Gui->Enable(ID_BUTTON_POINT_DELETE,m_Action==ID_POINT_ACTION && m_SelectedPoint!=UNDEFINED_POINT_ID);
			}
			else if(m_PointTool==ID_INSERT_POINT)
			{
				double vertexCoord[3];
				vtkPoints *pts = NULL; 
				pts = (vtkPoints *)e->GetVtkObj();
				pts->GetPoint(0,vertexCoord);

        SelectPoint(vertexCoord);

        if(!m_TestMode)
        {
					GetLogicManager()->VmeVisualModeChanged(m_VMEPolylineSelection);
        }

				//m_Picker->EnableContinuousPicking(true);

				if(InsertPoint(vertexCoord)==ALBA_OK)
				{
					//m_PointTool = ID_MOVE_POINT;
					m_Gui->Update();
				}
			}
		}
		else if(m_Action==ID_BRANCH_ACTION)
		{
			if(m_BranchTool==ID_SELECT_BRANCH)
			{
				m_Picker->EnableContinuousPicking(false);
				double pos[3];
				vtkPoints *pts = NULL; 
				pts = (vtkPoints *)e->GetVtkObj();
				pts->GetPoint(0,pos);

				SelectBranch(pos);

        albaLogMessage(albaString::Format("Branch Select = %d",m_CurrentBranch));

				m_Gui->Enable(ID_BUTTON_BRANCH_DELETE,m_Action==ID_BRANCH_ACTION && m_SelectedBranch!=UNDEFINED_BRANCH_ID);
			}
			else if(m_BranchTool==ID_ADD_BRANCH)
			{
				if(m_SelectedPoint!=UNDEFINED_POINT_ID)
				{
					m_Picker->EnableContinuousPicking(false);
					double pos[3];
					vtkPoints *pts = NULL; 
					pts = (vtkPoints *)e->GetVtkObj();
					pts->GetPoint(0,pos);
					
					if(AddBranch(pos)==ALBA_OK)
					{

						m_Action = ID_POINT_ACTION;
						m_PointTool = ID_ADD_POINT;

						m_Gui->Enable(ID_POINT_TOOL,m_Action==ID_POINT_ACTION);
            m_Gui->Enable(ID_SPHERE_RADIUS,m_Action==ID_POINT_ACTION);
						m_Gui->Enable(ID_BRANCH_TOOL,m_Action==ID_BRANCH_ACTION);

						m_Gui->Update();

						if(!m_TestMode)
							GetLogicManager()->VmeShow(m_VMEPolylineSelection, false);
					}
					else
					{
						albaLogMessage("Point must have degree > 1");
					}
				}
			}
		}
	}
	GetLogicManager()->CameraUpdate();
	//albaLogMessage(albaString::Format("current branch %d di %d",m_CurrentBranch,m_PolylineGraph->GetNumberOfBranches()));
}
//----------------------------------------------------------------------------
int albaGeometryEditorPolylineGraph::InsertPoint(double position[3])
//----------------------------------------------------------------------------
{
  int idEdgeNearst=-1;
  double minDistance=VTK_DOUBLE_MAX;
	for(int i=0;i<m_PolylineGraph->GetNumberOfEdges();i++)
	{
		//check if the 2 points of i-th edge are equal to selected points
		if(m_PolylineGraph->GetConstEdgePtr(i)->GetVertexId(1)==m_SelectedPoint||m_PolylineGraph->GetConstEdgePtr(i)->GetVertexId(0)==m_SelectedPoint)
		{
			int P0=m_PolylineGraph->GetConstEdgePtr(i)->GetVertexId(0);
      int P1=m_PolylineGraph->GetConstEdgePtr(i)->GetVertexId(1);

      double coordP0[3];
      double coordP1[3];

      m_PolylineGraph->GetConstVertexPtr(P0)->GetCoords(coordP0);
      m_PolylineGraph->GetConstVertexPtr(P1)->GetCoords(coordP1);

      double distance=ComputeDistancePointLine(coordP0,coordP1,position);
      if(minDistance>distance)
      {
        idEdgeNearst=i;
        minDistance=distance;
      }
    }
  }

  if(idEdgeNearst==-1)//If no edged was found
  {
    return ALBA_ERROR;
  }

  int branch=m_PolylineGraph->GetConstEdgePtr(idEdgeNearst)->GetBranchId();

  int P0=m_PolylineGraph->GetConstEdgePtr(idEdgeNearst)->GetVertexId(0);
  int P1=m_PolylineGraph->GetConstEdgePtr(idEdgeNearst)->GetVertexId(1);

	m_PolylineGraph->DeleteEdge(idEdgeNearst);

	m_PolylineGraph->AddNewVertex(position);

	m_PolylineGraph->AddNewEdge(P0,m_PolylineGraph->GetMaxVertexId());
	m_PolylineGraph->AddNewEdge(m_PolylineGraph->GetMaxVertexId(),P1);

	m_PolylineGraph->AddExistingEdgeToBranch(branch,m_PolylineGraph->GetMaxEdgeId()-1);
	m_PolylineGraph->AddExistingEdgeToBranch(branch,m_PolylineGraph->GetMaxEdgeId());

	int nEdge=m_PolylineGraph->GetConstBranchPtr(m_PolylineGraph->GetMaxBranchId())->GetNumberOfEdges();
	int *eList=new int[nEdge];

	for(int i=0;i<nEdge;i++)
		eList[i]=m_PolylineGraph->GetConstBranchPtr(m_PolylineGraph->GetMaxBranchId())->GetEdgeId(i);

	m_PolylineGraph->DeleteBranch(m_PolylineGraph->GetMaxBranchId());

	for(int i=0;i<nEdge;i++)
		m_PolylineGraph->AddExistingEdgeToBranch(branch,eList[i]);

	vtkALBASmartPointer<vtkPolyData> poly;
	m_PolylineGraph->CopyToPolydata(poly);

	m_SelectedPoint=m_PolylineGraph->GetMaxVertexId();

	SelectPoint(m_SelectedPoint);

	UpdateVMEEditorData(poly);

	delete [] eList;

	return ALBA_OK;

}
//----------------------------------------------------------------------------
double albaGeometryEditorPolylineGraph::ComputeDistancePointLine(double lineP0[3],double lineP1[3],double point[3])
//----------------------------------------------------------------------------
{
  double v[3];
  v[0]=(lineP0[2]-lineP1[2])*point[1]+(lineP1[1]-lineP0[1])*point[2]+(lineP0[1]*lineP1[2]-lineP1[1]*lineP0[2]);
  v[1]=(lineP0[0]-lineP1[0])*point[2]+(lineP1[2]-lineP0[2])*point[0]+(lineP0[2]*lineP1[0]-lineP1[2]*lineP0[0]);
  v[2]=(lineP0[1]-lineP1[1])*point[0]+(lineP1[0]-lineP0[0])*point[1]+(lineP0[0]*lineP1[1]-lineP1[0]*lineP0[1]);

  double mod=sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);

  double lenght=0;
  for(int i=0;i<3;i++)
  {
    lenght+=(lineP0[i]-lineP1[i])*(lineP0[i]-lineP1[i]);
  }
  lenght=sqrt(lenght);

  double distance;
  distance=mod/lenght;
  return distance;
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraph::DeleteBranch(vtkIdType branchID)
//----------------------------------------------------------------------------
{
	int *vList=new int[m_PolylineGraph->GetConstBranchPtr(branchID)->GetNumberOfVertices()];
	for(int i=0;i<m_PolylineGraph->GetConstBranchPtr(branchID)->GetNumberOfVertices();i++)
		vList[i]=m_PolylineGraph->GetConstBranchPtr(branchID)->GetVertexId(i);

	int nEdge=m_PolylineGraph->GetConstBranchPtr(branchID)->GetNumberOfEdges();

  const albaPolylineGraph::Branch *branchToDelete=m_PolylineGraph->GetConstBranchPtr(branchID);

	for(int i=0;i<nEdge;i++)
	{
		int pID=m_PolylineGraph->GetConstEdgePtr(m_PolylineGraph->GetConstBranchPtr(branchID)->GetLastEdgeId())->GetVertexId(1);
		m_PolylineGraph->DeleteEdge(m_PolylineGraph->GetConstBranchPtr(branchID)->GetLastEdgeId());
    m_PolylineGraph->DeleteBranch(m_PolylineGraph->GetMaxBranchId());//when an edge is deleted a new branch is created
    if(m_PolylineGraph->GetConstVertexPtr(pID)->GetDegree()==0)//only vertex with degree == 0 can be delete
		{
			m_PolylineGraph->DeleteVertex(pID);
		}
		/*else
		{
			for(int k=0;k<m_PolylineGraph->GetNumberOfBranches()-1;k++)
			{
				for(int j=0;j<m_PolylineGraph->GetConstBranchPtr(k)->GetNumberOfVertices();j++)
				{
					if(pID==m_PolylineGraph->GetConstBranchPtr(k)->GetVertexId(j) && k!=branchID)
          {
						//DeleteBranch(k);
            break;
          }
				}
			}
		}*/
	}
	if(m_PolylineGraph->GetConstVertexPtr(m_PolylineGraph->GetConstBranchPtr(branchID)->GetLastVertexId())->GetDegree()==0)
		m_PolylineGraph->DeleteVertex(m_PolylineGraph->GetConstBranchPtr(branchID)->GetLastVertexId());

  
  for (int i=0;i<m_PolylineGraph->GetNumberOfBranches();i++)
  {
    if(m_PolylineGraph->GetConstBranchPtr(i)==m_PolylineGraph->GetConstBranchPtr(branchID))
      m_PolylineGraph->DeleteBranch(i);

  }

  m_PolylineGraph->MergeSimpleJoinedBranches();

  delete [] vList;
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraph::MovePoint(double newPosition[3],int pointID)
//----------------------------------------------------------------------------
{
	if(pointID==UNDEFINED_POINT_ID)
  {
    double *p = new double[3];
    m_PolylineGraph->GetConstVertexPtr(m_SelectedPoint)->GetCoords(p);
    p[0]=newPosition[0];
    p[1]=newPosition[1];
    p[2]=newPosition[2];
		m_PolylineGraph->SetVertexCoords(m_SelectedPoint,p);
    delete [] p;
  }
	else
  {
    double *p = new double[3];
    m_PolylineGraph->GetConstVertexPtr(pointID)->GetCoords(p);
    p[0]=newPosition[0];
    p[1]=newPosition[1];
    p[2]=newPosition[2];
		m_PolylineGraph->SetVertexCoords(pointID,p);
    delete [] p;
  }

	SelectPoint(pointID);
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraph::SelectPoint(double position[3])
//----------------------------------------------------------------------------
{
	double minDistance=VTK_DOUBLE_MAX;
	int iMin;
	for(int i=0;i<m_PolylineGraph->GetNumberOfVertices();i++)
	{
		double point[3];
		m_PolylineGraph->GetVertexCoords(i,point);
		double distance = sqrt(vtkMath::Distance2BetweenPoints(position,point));
		if(distance<minDistance)
		{
			iMin=i;
			minDistance=distance;
		}
	}
	m_SelectedPoint = iMin;

	SelectPoint(m_SelectedPoint);
	
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraph::SelectPoint(int pointID)
//----------------------------------------------------------------------------
{
	double position[3];
	m_PolylineGraph->GetVertexCoords(m_SelectedPoint,position);
	vtkALBASmartPointer<vtkPolyData> poly;
	m_PolylineGraph->CopyToPolydata(poly);
	m_SelectedPointVTK=poly->FindPoint(position);

	vtkALBASmartPointer<vtkCharArray> scalar;
	scalar->SetNumberOfComponents(1);
	scalar->SetNumberOfTuples(m_PolylineGraph->GetNumberOfVertices());
	for (int i=0;i<m_PolylineGraph->GetNumberOfVertices();i++)
	{
		scalar->SetTuple1(i,0);
	}

	scalar->SetTuple1(m_SelectedPointVTK,1);

	poly->GetPointData()->SetScalars(scalar);

	UpdateVMEEditorData(poly);

	//VME Selection data are composed by sphere
	m_VMEPolylineSelection->SetData(m_Glyph->GetOutput(),0.0);
}
//----------------------------------------------------------------------------
int albaGeometryEditorPolylineGraph::DeletePoint(int pointID)
//----------------------------------------------------------------------------
{
	if(pointID==UNDEFINED_POINT_ID)
		pointID=m_SelectedPoint;

	if(m_PolylineGraph->GetConstVertexPtr(pointID)->GetDegree()<3)//a point could be delete only if has degree < 3
	{
		vtkALBASmartPointer<vtkIdList> vList;
		m_PolylineGraph->GetConstVertexPtr(pointID)->GetVerticesIdList(vList);

		int branch;
		bool branchFound=false;
		for(int i=0;i<m_PolylineGraph->GetNumberOfBranches();i++)
			for(int j=0;j<m_PolylineGraph->GetConstBranchPtr(i)->GetNumberOfVertices();j++)
				if(m_PolylineGraph->GetConstBranchPtr(i)->GetVertexId(j)==pointID)
				{
					branch=i;
					branchFound=true;
				}
		if(!branchFound)
			return ALBA_ERROR;

		bool branchMin=false;//check if in the branch there are only 2 vertices
		if(m_PolylineGraph->GetConstBranchPtr(branch)->GetNumberOfVertices()<=2)
			branchMin=true;

		while (m_PolylineGraph->GetConstVertexPtr(pointID)->GetDegree()>0)
		{
			m_PolylineGraph->DeleteEdge(m_PolylineGraph->GetConstVertexPtr(pointID)->GetEdgeId(0));
		}

		if(!branchMin)
			m_PolylineGraph->AddNewEdge(vList->GetId(0),vList->GetId(1));

		m_PolylineGraph->DeleteVertex(pointID);

		if(!branchMin)
		{
			m_PolylineGraph->AddExistingEdgeToBranch(branch,m_PolylineGraph->GetMaxEdgeId());

			int num=m_PolylineGraph->GetConstBranchPtr(m_PolylineGraph->GetMaxBranchId())->GetNumberOfEdges();
			int *eList=new int[num];

			for(int i=0;i<num;i++)
				eList[i]=m_PolylineGraph->GetConstBranchPtr(m_PolylineGraph->GetMaxBranchId())->GetEdgeId(i);

			//when a point is deleted 2 new branch are created because a branch should be connected.
			m_PolylineGraph->DeleteBranch(m_PolylineGraph->GetMaxBranchId());
			m_PolylineGraph->DeleteBranch(m_PolylineGraph->GetMaxBranchId());

			//attach to the original branch the edge no connected
			for(int i=0;i<num;i++)
				m_PolylineGraph->AddExistingEdgeToBranch(branch,eList[i]);

			delete [] eList;
		}
		else
		{
			m_PolylineGraph->DeleteBranch(m_PolylineGraph->GetMaxBranchId());
		}
   

		vtkALBASmartPointer<vtkPolyData> poly;
		m_PolylineGraph->CopyToPolydata(poly);
		vtkALBASmartPointer<vtkCharArray> scalar;
		scalar->SetNumberOfComponents(1);
		scalar->SetNumberOfTuples(m_PolylineGraph->GetNumberOfVertices());
		for (int i=0;i<m_PolylineGraph->GetNumberOfVertices();i++)
		{
			scalar->SetTuple1(i,0);
		}
		poly->GetPointData()->SetScalars(scalar);

		UpdateVMEEditorData(poly);
		
		return ALBA_OK;
	}
	else
	{		
		return ALBA_ERROR;
	}
}
//----------------------------------------------------------------------------
int albaGeometryEditorPolylineGraph::DeletePoint(double position[3])
//----------------------------------------------------------------------------
{
	double minDistance=VTK_DOUBLE_MAX;
	int iMin;
	for(int i=0;i<m_PolylineGraph->GetNumberOfVertices();i++)
	{
		double point[3];
		m_PolylineGraph->GetVertexCoords(i,point);
		double distance = sqrt(vtkMath::Distance2BetweenPoints(position,point));
		if(distance<minDistance)
		{
			iMin=i;
			minDistance=distance;
		}
	}
	
	return DeletePoint(iMin);
}
//----------------------------------------------------------------------------
void albaGeometryEditorPolylineGraph::SelectBranch(double position[3])
//----------------------------------------------------------------------------
{
	vtkALBASmartPointer<vtkPolyData> poly;
	m_PolylineGraph->CopyToPolydata(poly);
	UpdateVMEEditorData(poly);

	int SelectedTubePointVTK=m_Tube->GetOutput()->FindPoint(position);

	double *normal;
	normal=m_Tube->GetOutput()->GetPointData()->GetNormals()->GetTuple3(SelectedTubePointVTK);
	vtkMath::Normalize(normal);

	//Create a line by two points on the normal to intersect the polyline 
	double position2[3];
  double position1[3]={position[0],position[1],position[2]};
	position1[0]+=(normal[0]*m_SphereRadius);
	position1[1]+=(normal[1]*m_SphereRadius);
	position1[2]+=(normal[2]*m_SphereRadius);
	position2[0]=(position[0])-(normal[0]*m_SphereRadius);
	position2[1]=(position[1])-(normal[1]*m_SphereRadius);
	position2[2]=(position[2])-(normal[2]*m_SphereRadius);

	vtkALBASmartPointer<vtkCellLocator> locator;
	locator->SetDataSet(poly);
	locator->BuildLocator();
	double t;
	int subId;
	vtkIdType	CellID;
	//int n=locator->IntersectWithLine(position, position2,1, t, ptline, pcoords, subId,CellID);
  locator->FindClosestPoint(position,position2,CellID,subId,t);

  int n=3;
	if(n==0)
		return;

	//Polydata for VMEEditorSelection
	//This is needed to avoid scalar interpolation problem
	vtkALBASmartPointer<vtkPolyData> poly_selected;
	vtkALBASmartPointer<vtkPoints> points;
	vtkALBASmartPointer<vtkCellArray> lines;
	vtkALBASmartPointer<vtkIdList> idlist;

	vtkIdList *IDS=poly->GetCell(CellID)->GetPointIds();
  albaLogMessage(albaString::Format("Points %.3f %.3f %.3f",position[0],position[1],position[2])),
  albaLogMessage(albaString::Format("CellID %d N* Points %d",CellID,IDS->GetNumberOfIds()));
	for(int i=0;i<IDS->GetNumberOfIds();i++)
	{
		int ID=IDS->GetId(i);
		points->InsertNextPoint(poly->GetPoint(ID));
		idlist->InsertId(i,i);
	}
	lines->InsertNextCell(idlist);
	poly_selected->SetLines(lines);
	poly_selected->SetPoints(points);
  
	//All point of poly_selected must have scalar 1.0 in way to color the tubes
	vtkALBASmartPointer<vtkCharArray> scalar_sel;
	scalar_sel->SetNumberOfComponents(1);
	scalar_sel->SetNumberOfTuples(poly_selected->GetNumberOfPoints());
	for (int i=0;i<poly_selected->GetNumberOfPoints();i++)
	{
		scalar_sel->SetTuple1(i,1.0);
	}
	poly_selected->GetPointData()->SetScalars(scalar_sel);

	vtkALBASmartPointer<vtkTubeFilter> tube;
	tube->UseDefaultNormalOff();
	tube->SetInputData(poly_selected);
	tube->SetRadius(m_SphereRadius/1.8);
	tube->SetCapping(true);
	tube->SetNumberOfSides(5);

	m_VMEPolylineSelection->SetData(tube->GetOutput(),0.0);

	m_CurrentBranch = m_PolylineGraph->GetBranchCorrespondingToOutputCell(CellID);

	m_SelectedBranch = m_CurrentBranch;
  
	GetLogicManager()->VmeVisualModeChanged(m_VMEPolylineSelection);
}
//----------------------------------------------------------------------------
int albaGeometryEditorPolylineGraph::AddBranch(double position[3])
//----------------------------------------------------------------------------
{
	if(m_PolylineGraph->GetConstVertexPtr(m_SelectedPoint)->GetDegree()>1)
	{
		if(m_PolylineGraph->AddNewBranch(m_SelectedPoint))
		{
			m_CurrentBranch = m_PolylineGraph->GetNumberOfBranches()-1;

			AddNewVertex(position);

			m_VMEPolylineSelection->SetData(m_Tube->GetOutput(),0.0);

			m_SelectedPoint = UNDEFINED_POINT_ID;

			return ALBA_OK;
		}
	}

	return ALBA_ERROR;
}