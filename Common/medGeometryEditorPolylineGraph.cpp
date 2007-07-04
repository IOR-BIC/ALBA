/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGeometryEditorPolylineGraph.cpp,v $
Language:  C++
Date:      $Date: 2007-07-04 09:50:25 $
Version:   $Revision: 1.4 $
Authors:   Matteo Giacomoni
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


#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medGeometryEditorPolylineGraph.h"
#include "mafDecl.h"

#include "mmgGui.h"
#include "mafPolylineGraph.h"
#include "medVMEPolylineEditor.h"
#include "mmiPicker.h"
#include "mafInteractor.h"

#include "vtkMAFSmartPointer.h"
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
#include "vtkCell.h"
#include "vtkCellArray.h"

enum POINT_TOOL_ID
{
	ID_ADD_POINT = 0,
	ID_SELECT_POINT,
	ID_INSERT_POINT,
	ID_MOVE_POINT,
};
enum BRANCH_TOOL_ID
{
	ID_SELECT_BRANCH = 0,
	ID_ADD_BRANCH,
	ID_DELETE_BRANCH,
};
enum ACTION_ID
{
	ID_POINT_ACTION = 0,
	ID_BRANCH_ACTION,
};

//----------------------------------------------------------------------------
medGeometryEditorPolylineGraph::medGeometryEditorPolylineGraph(mafVME *input, mafObserver *listener)
//----------------------------------------------------------------------------
{
	m_PolylineGraph = new mafPolylineGraph;
	m_PolylineGraph->AddNewBranch();

	vtkMAFSmartPointer<vtkPolyData> data;
	m_PolylineGraph->CopyToPolydata(data);

	mafNEW(m_VMEPolylineEditor);
	m_VMEPolylineEditor->SetName("VME Editor");
	m_VMEPolylineEditor->ReparentTo(mafVME::SafeDownCast(input->GetRoot()));

	m_InputVME	=	input;
	m_Listener	= listener;

	m_CurrentBranch = 0;

	//Gui Variables
	m_PointTool		= ID_ADD_POINT;
	m_Action			= ID_POINT_ACTION;
	m_BranchTool	= ID_SELECT_BRANCH;

	CreateGui();
	CreateISA();
	CreatePipe();

	m_VMEPolylineEditor->SetData(m_AppendPolydata->GetOutput(),0.0);

	mafNEW(m_VMEPolylineSelection);
	m_VMEPolylineSelection->ReparentTo(mafVME::SafeDownCast(input->GetRoot()));

	m_SelectedPoint = -1;
	m_SelectedPointVTK = -1;
}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraph::CreatePipe() 
//----------------------------------------------------------------------------
{
	vtkMAFSmartPointer<vtkPolyData> data;
	m_PolylineGraph->CopyToPolydata(data);

	vtkMAFSmartPointer<vtkSphereSource> Sphere;
	Sphere->SetRadius(2.0);
	Sphere->SetPhiResolution(10);
	Sphere->SetThetaResolution(10);

	vtkNEW(m_Glyph);
	m_Glyph->SetInput(data);
	m_Glyph->SetSource(Sphere->GetOutput());
	m_Glyph->SetScaleModeToDataScalingOff();
	m_Glyph->SetRange(0.0,1.0);
	m_Glyph->Update();

	vtkNEW(m_Tube);
	m_Tube->UseDefaultNormalOff();
	m_Tube->SetInput(data);
	m_Tube->SetRadius(1.0);
	m_Tube->SetCapping(true);
	m_Tube->SetNumberOfSides(10);

	vtkNEW(m_AppendPolydata);
	m_AppendPolydata->AddInput(m_Tube->GetOutput());
	m_AppendPolydata->AddInput(m_Glyph->GetOutput());
	m_AppendPolydata->Update();
}
//----------------------------------------------------------------------------
medGeometryEditorPolylineGraph::~medGeometryEditorPolylineGraph() 
//----------------------------------------------------------------------------
{
	m_InputVME->SetBehavior(m_OldBehavior);

	m_VMEPolylineEditor->SetBehavior(NULL);
	m_VMEPolylineEditor->ReparentTo(NULL);

	m_VMEPolylineSelection->SetBehavior(NULL);
	m_VMEPolylineSelection->ReparentTo(NULL);
	
	mafDEL(m_Picker);

	vtkDEL(m_AppendPolydata);
	vtkDEL(m_Tube);
	vtkDEL(m_Glyph);
}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraph::Show(bool show)
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,VME_SHOW,m_VMEPolylineEditor,show));
}
//----------------------------------------------------------------------------
enum EDITOR_GRAPH_ID
//----------------------------------------------------------------------------
{
	ID_POINT_TOOL = MINID,
	ID_BRANCH_TOOL,
	ID_ACTION,
};
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraph::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
			case ID_ACTION:
				{
					m_Gui->Enable(ID_POINT_TOOL,m_Action==ID_POINT_ACTION);
					m_Gui->Enable(ID_BRANCH_TOOL,m_Action==ID_BRANCH_ACTION);

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
		default:
			mafEventMacro(*e);
			break; 
		}
	}
}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraph::BehaviorUpdate()
//----------------------------------------------------------------------------
{
	if(m_Action==ID_POINT_ACTION)
	{
		if(m_PointTool==ID_ADD_POINT)//If the user want to add a point
		{
			m_VMEPolylineEditor->SetBehavior(NULL);
			m_InputVME->SetBehavior(m_Picker);
		}
		else if( m_PointTool==ID_MOVE_POINT || m_PointTool==ID_SELECT_POINT )//If the user want to move or select a point
		{
			m_VMEPolylineEditor->SetBehavior(m_Picker);
			m_InputVME->SetBehavior(NULL);
		}
	}
	else if(m_Action==ID_BRANCH_ACTION)
	{
		if(m_BranchTool==ID_SELECT_BRANCH)//If the user want select a branch
		{
			m_VMEPolylineEditor->SetBehavior(m_Picker);
			m_InputVME->SetBehavior(NULL);
		}
		else if(m_BranchTool==ID_ADD_BRANCH)//If the user want add a branch
		{
			m_VMEPolylineEditor->SetBehavior(NULL);
			m_InputVME->SetBehavior(m_Picker);
		}
	}
}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraph::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui =new mmgGui(this);
	wxString choices_action[2]={_("Point"),_("Branch")};
	m_Gui->Radio(ID_ACTION,_("Action"),&m_Action,2,choices_action);

	wxString choices_point_tool[4]={_("Add Point"),_("Select Point"),_("Insert Point"),_("Move Point")};
	m_Gui->Radio(ID_POINT_TOOL,_("Point Tool"),&m_PointTool,4,choices_point_tool);
	m_Gui->Enable(ID_POINT_TOOL,m_Action==ID_POINT_ACTION);

	wxString choices_branch_tool[3]={_("Select Branch"),_("Add Branch"),_("Delete Branch")};
	m_Gui->Radio(ID_BRANCH_TOOL,_("Branch Tool"),&m_BranchTool,3,choices_branch_tool);
	m_Gui->Enable(ID_BRANCH_TOOL,m_Action==ID_BRANCH_ACTION);

}
//----------------------------------------------------------------------------
mmgGui* medGeometryEditorPolylineGraph::GetGui()
//----------------------------------------------------------------------------
{
	return m_Gui;
}
//-------------------------------------------------------------------------
int medGeometryEditorPolylineGraph::AddNewVertex(double vertex[3],vtkIdType branch)
//-------------------------------------------------------------------------
{
	vtkMAFSmartPointer<vtkPolyData> polydata;
	int result;
	if(m_PolylineGraph->GetNumberOfBranches()!=0)//If there are at least a branch
	{
		if(m_PolylineGraph->GetNumberOfBranches()>branch && branch >=0)//if variable branch is a right ID
		{
			m_PolylineGraph->AddNewVertexToBranch(branch,vertex);
			m_PolylineGraph->CopyToPolydata(polydata);

			result=UpdateVMEEditorData(polydata);
		}
		else if(branch<0)//if variables branch is -1 the new vertex is added to the current branch
		{
			m_PolylineGraph->AddNewVertexToBranch(m_CurrentBranch,vertex);
			m_PolylineGraph->CopyToPolydata(polydata);
			
			result=UpdateVMEEditorData(polydata);
		}
		else
		{
			result = MAF_ERROR;
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
		m_PolylineGraph->AddNewVertex(vertex);
		m_PolylineGraph->CopyToPolydata(polydata);

		result=UpdateVMEEditorData(polydata);
	}
	else
	{
		result = MAF_ERROR;
	}

	return result;
}
//-------------------------------------------------------------------------
int medGeometryEditorPolylineGraph::UpdateVMEEditorData(vtkPolyData *polydata)
//-------------------------------------------------------------------------
{
	m_Glyph->SetInput(polydata);
	m_Glyph->Modified();
	m_Glyph->Update();
	m_Tube->SetInput(polydata);
	m_Tube->Modified();
	m_Tube->Update();

	m_AppendPolydata->RemoveAllInputs();
	m_AppendPolydata->AddInput(m_Glyph->GetOutput());
	m_AppendPolydata->AddInput(m_Tube->GetOutput());
	m_AppendPolydata->Update();

	int result = m_VMEPolylineEditor->SetData(m_AppendPolydata->GetOutput(),m_VMEPolylineEditor->GetTimeStamp());
	m_VMEPolylineEditor->Update();

	return result;
}
//-------------------------------------------------------------------------
void medGeometryEditorPolylineGraph::CreateISA()
//-------------------------------------------------------------------------
{
	m_OldBehavior = m_InputVME->GetBehavior();

	m_Picker = mmiPicker::New();
	m_Picker->SetListener(this);

	m_InputVME->SetBehavior(m_Picker);
	m_VMEPolylineEditor->SetBehavior(NULL);
}
//----------------------------------------------------------------------------
vtkPolyData* medGeometryEditorPolylineGraph::GetOutput()
//----------------------------------------------------------------------------
{
	vtkPolyData *polydata;
	vtkNEW(polydata);
	m_PolylineGraph->CopyToPolydata(polydata);
	return polydata;
}
//----------------------------------------------------------------------------
void medGeometryEditorPolylineGraph::VmePicked(mafEvent *e)
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

				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			else if(m_PointTool==ID_MOVE_POINT)
			{
				if(m_SelectedPoint!=-1)
				{
					m_Picker->EnableContinuousPicking(true);
					double vertexCoord[3];
					vtkPoints *pts = NULL; 
					pts = (vtkPoints *)e->GetVtkObj();
					pts->GetPoint(0,vertexCoord);
					m_PolylineGraph->SetVertexCoords(m_SelectedPoint,vertexCoord);

					vtkMAFSmartPointer<vtkPolyData> poly;
					m_PolylineGraph->CopyToPolydata(poly);

					vtkMAFSmartPointer<vtkCharArray> scalar;
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

					mafEventMacro(mafEvent(this,CAMERA_UPDATE));
				}
			}
			else if(m_PointTool==ID_SELECT_POINT)
			{
				double vertexCoord[3];
				vtkPoints *pts = NULL; 
				pts = (vtkPoints *)e->GetVtkObj();
				pts->GetPoint(0,vertexCoord);

				double minDistance=VTK_DOUBLE_MAX;
				int iMin;
				for(int i=0;i<m_PolylineGraph->GetNumberOfVertices();i++)
				{
					double point[3];
					m_PolylineGraph->GetVertexCoords(i,point);
					double distance = sqrt(vtkMath::Distance2BetweenPoints(vertexCoord,point));
					if(distance<minDistance)
					{
						iMin=i;
						minDistance=distance;
					}
				}
				m_SelectedPoint = iMin;

				vtkMAFSmartPointer<vtkPolyData> poly;
				m_PolylineGraph->CopyToPolydata(poly);
				m_SelectedPointVTK=poly->FindPoint(vertexCoord);
				vtkMAFSmartPointer<vtkCharArray> scalar;
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
				mafEventMacro(mafEvent(this,VME_SHOW,m_VMEPolylineSelection,true));
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
		}
		else if(m_Action==ID_BRANCH_ACTION)
		{
			if(m_BranchTool==ID_SELECT_BRANCH)
			{
				double vertexCoord[3],vertexCoord2[3];
				vtkPoints *pts = NULL; 
				pts = (vtkPoints *)e->GetVtkObj();
				pts->GetPoint(0,vertexCoord);

				vtkMAFSmartPointer<vtkPolyData> poly;
				m_PolylineGraph->CopyToPolydata(poly);
				UpdateVMEEditorData(poly);

				int SelectedTubePointVTK=m_Tube->GetOutput()->FindPoint(vertexCoord);
				
				double *normal;
				normal=m_Tube->GetOutput()->GetPointData()->GetNormals()->GetTuple3(SelectedTubePointVTK);
				vtkMath::Normalize(normal);

				//Create a line by two points on the normal to intersect the polyline 
				vertexCoord[0]+=(normal[0]*0.001);
				vertexCoord[1]+=(normal[1]*0.001);
				vertexCoord[2]+=(normal[2]*0.001);
				vertexCoord2[0]=(vertexCoord[0])-(normal[0]*10.0);
				vertexCoord2[1]=(vertexCoord[1])-(normal[1]*10.0);
				vertexCoord2[2]=(vertexCoord[2])-(normal[2]*10.0);

				vtkMAFSmartPointer<vtkOBBTree> locator;
				locator->SetDataSet(poly);
				locator->BuildLocator();
				double t, ptline[3], pcoords[3];
				int subId;
				vtkIdType	CellID;
				int n=locator->IntersectWithLine(vertexCoord, vertexCoord2,1, t, ptline, pcoords, subId,CellID);

				//Polydata for VMEEditorSelection
				//This is needed to avoid scalar interpolation problem
				vtkMAFSmartPointer<vtkPolyData> poly_selected;
				vtkMAFSmartPointer<vtkPoints> points;
				vtkMAFSmartPointer<vtkCellArray> lines;
				vtkMAFSmartPointer<vtkIdList> idlist;

				vtkIdList *IDS=poly->GetCell(CellID)->GetPointIds();
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
				vtkMAFSmartPointer<vtkCharArray> scalar_sel;
				scalar_sel->SetNumberOfComponents(1);
				scalar_sel->SetNumberOfTuples(poly_selected->GetNumberOfPoints());
				for (int i=0;i<poly_selected->GetNumberOfPoints();i++)
				{
					scalar_sel->SetTuple1(i,1.0);
				}
				poly_selected->GetPointData()->SetScalars(scalar_sel);

				vtkMAFSmartPointer<vtkTubeFilter> tube;
				tube->UseDefaultNormalOff();
				tube->SetInput(poly_selected);
				tube->SetRadius(1.5);
				tube->SetCapping(true);
				tube->SetNumberOfSides(5);

				m_VMEPolylineSelection->SetData(tube->GetOutput(),0.0);

				m_CurrentBranch = CellID;

				mafEventMacro(mafEvent(this,VME_SHOW,m_VMEPolylineSelection,false));
				mafEventMacro(mafEvent(this,VME_SHOW,m_VMEPolylineSelection,true));
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			}
			else if(m_BranchTool==ID_ADD_BRANCH)
			{
				if(m_SelectedPoint != -1)
				{
					m_Picker->EnableContinuousPicking(false);
					double pos[3];
					vtkPoints *pts = NULL; 
					pts = (vtkPoints *)e->GetVtkObj();
					pts->GetPoint(0,pos);

					m_PolylineGraph->AddNewBranch(m_SelectedPoint);
					m_CurrentBranch = m_PolylineGraph->GetNumberOfBranches()-1;

					AddNewVertex(pos);

					m_VMEPolylineSelection->SetData(m_Tube->GetOutput(),0.0);

					m_SelectedPoint = -1;

					mafEventMacro(mafEvent(this,VME_SHOW,m_VMEPolylineSelection,false));
					mafEventMacro(mafEvent(this,CAMERA_UPDATE));
				}
			}
		}
	}
}