/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpInteractionDebugger.cpp,v $
  Language:  C++
  Date:      $Date: 2008-09-05 11:12:35 $
  Version:   $Revision: 1.3 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpInteractionDebugger.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "mafSmartPointer.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"

#include "mafVMEMeshAnsysTextImporter.h"
#include "vtkMAFSmartPointer.h"
#include <fstream>

// new stuff

#include "vtkMAFSmartPointer.h"
#include "mafVMEPolyline.h"
#include "mafVMEOutputPolyline.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "medGizmoInteractionDebugger.h"
#include "medVMEPolylineGraph.h"

// Operation constants
enum Mesh_Importer_ID
{
  ID_FIRST = MINID,
  ID_RESET,
  ID_OK,
  ID_CANCEL,
  ID_CHOOSE,
};

mafCxxTypeMacro(medOpInteractionDebugger);


medOpInteractionDebugger::medOpInteractionDebugger(const wxString &label) :
mafOp(label)
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;
  m_GizmoDebugger = NULL;
  m_Constrain = NULL;
  m_ActiveBranchId = 0;
}

medOpInteractionDebugger::~medOpInteractionDebugger()
{
  cppDEL(m_GizmoDebugger);
}

bool medOpInteractionDebugger::Accept(mafNode *node)
{
  return true;
}

mafOp* medOpInteractionDebugger::Copy()   

{
  medOpInteractionDebugger *cp = new medOpInteractionDebugger(m_Label);
  return cp;
}

void medOpInteractionDebugger::OpRun()   
{
  CreateGui();
  ShowGui();
}

void medOpInteractionDebugger::CreateGui()
{  
  this->AddMEDVMEPolylineGraphTestConstrain1ToTree();
  this->AddMEDGizmoDebuggerToTree();

  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);
  m_Gui->Divider();
  m_Gui->Button(ID_RESET, "Reset", "");
  m_Gui->Divider();
  m_Gui->Button(ID_CHOOSE, "Choose Constrain", "");
  m_Gui->Divider();
  m_Gui->AddGui(m_GizmoDebugger->GetGui());
  m_Gui->Divider();
  m_Gui->OkCancel();

  m_Gui->Update();

}

void medOpInteractionDebugger::OnEvent(mafEventBase *maf_event) 
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case wxOK:
      {
        this->OpStop(OP_RUN_OK);
      }
      break;
      case wxCANCEL:
      {
        this->OpStop(OP_RUN_OK);
      }
      break;
      case ID_RESET:
      {
        m_ActiveBranchId = 0;
        m_GizmoDebugger->SetCurvilinearAbscissa(m_ActiveBranchId, 0.0);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
      case ID_CHOOSE:
      {
        mafString s(_("Choose Constrain"));
        mafEvent e(this,VME_CHOOSE, &s, (long)&medOpInteractionDebugger::ConstrainAccept);
        mafEventMacro(e);
        mafNode *vme = e.GetVme();
        if(vme != NULL)
        {
          OnChooseConstrainVme(vme);

          assert(TRUE);
        }
      }
      break;

      default:
        mafEventMacro(*e);
      break;
    }	
  }
}


void medOpInteractionDebugger::BuildPolyline1(vtkPoints *in_points)
{
  double point[3];
  // point 1
  point[0] = 0.0;
  point[1] = 0.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  // point 2
  point[0] = 1.0;
  point[1] = 0.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  // point 3
  point[0] = 1.0;
  point[1] = 1.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  // point 4
  point[0] = 2.0;
  point[1] = 1.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  in_points->Modified();
}

void medOpInteractionDebugger::BuildPolyline2(vtkPoints *in_points)
{
  /** 
  # vtk DataFile Version 3.0
  vtk output
  ASCII
  DATASET POLYDATA
  POINTS 6 float
  -1 0 0 0 0 0 1 1 0 
  2 1 0 3 0 0 4 0 0 

  LINES 5 15
  2 0 1 
  2 1 2 
  2 2 3 
  2 3 4 
  2 4 5 

  CELL_DATA 5
  POINT_DATA 6

  */
  double point[3];
  // point 0
  point[0] = -1.0;
  point[1] = 0.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  // point 1
  point[0] = 0.0;
  point[1] = 0.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  // point 2
  point[0] = 1.0;
  point[1] = 1.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  // point 3
  point[0] = 2.0;
  point[1] = 1.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  // point 4
  point[0] = 3.0;
  point[1] = 0.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  // point 5
  point[0] = 4.0;
  point[1] = 0.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  in_points->Modified();
}

void medOpInteractionDebugger::BuildVMEPolyline( vtkPoints * in_points, mafVMEPolyline *polyline )
{
  vtkMAFSmartPointer<vtkPolyData> in_data;
  vtkMAFSmartPointer<vtkCellArray> in_cells;

  int pointId[2];
  for(int i = 0; i< in_points->GetNumberOfPoints();i++)
  {
    if (i > 0)
    {             
      pointId[0] = i - 1;
      pointId[1] = i;
      in_cells->InsertNextCell(2 , pointId);  
    }
  }

  in_data->SetPoints(in_points);
  in_data->SetLines(in_cells);
  in_data->Modified();
  in_data->Update();


  // try to set this data to the volume
  int returnValue = -1;

  returnValue = polyline->SetData(vtkPolyData::SafeDownCast(in_data),0.0,mafVMEGeneric::MAF_VME_REFERENCE_DATA);
  assert(returnValue == MAF_OK);

  polyline->Modified();
  polyline->Update();
}

void medOpInteractionDebugger::BuildPolyline3(vtkPoints *in_points)
{
  double point[3];
  // point 1
  point[0] = 0.0;
  point[1] = 0.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  // point 2
  point[0] = 1.0;
  point[1] = 0.0;
  point[2] = 0.0;
  in_points->InsertNextPoint(point);

  // point 3
  point[0] = 0.0;
  point[1] = 0.0;
  point[2] = 1.0;
  in_points->InsertNextPoint(point);

  // point 4
  point[0] = 0.0;
  point[1] = 1.0;
  point[2] = 1.0;
  in_points->InsertNextPoint(point);

  in_points->Modified();
}

void medOpInteractionDebugger::AddMAFVMEPolylineTestConstrain1ToTree()
{
  vtkMAFSmartPointer<vtkPoints> in_points;
  mafVMEPolyline *polyline;
  mafNEW(polyline);

  BuildPolyline1(in_points);
  BuildVMEPolyline(in_points, polyline);

  polyline->SetName("pippo constrain");
  polyline->ReparentTo(m_Input);
  
  mafDEL(polyline);

}

void medOpInteractionDebugger::AddMEDVMEPolylineGraphTestConstrain1ToTree()
{
  vtkMAFSmartPointer<vtkPolyData> pd;
  BuildGraph1(pd);

  medVMEPolylineGraph *polyline;
  mafNEW(polyline);
  
  polyline->SetData(pd, -1);
  polyline->GetOutput()->GetVTKData()->Update();
  polyline->Update();

  polyline->SetName("test graph");
  polyline->ReparentTo(m_Input);

  mafDEL(polyline);
}


void medOpInteractionDebugger::RemoveTestConstraintGraph1FromTree()
{

  mafVME *vmeRoot = mafVME::SafeDownCast(m_Input->GetRoot());

  mafVME *constraint = mafVME::SafeDownCast(vmeRoot->FindInTreeByName("test graph"));
  assert(constraint != NULL);
  
  constraint->ReparentTo(NULL);
}

void medOpInteractionDebugger::AddMAFVMEPolylineTestConstrain2ToTree()
{
  vtkMAFSmartPointer<vtkPoints> in_points;
  mafVMEPolyline *polyline;
  mafNEW(polyline);

  BuildPolyline2(in_points);
  BuildVMEPolyline(in_points, polyline);

  polyline->SetName("test constrain");
  polyline->ReparentTo(m_Input);

  mafDEL(polyline);
}

void medOpInteractionDebugger::AddMAFVMEPolylineTestConstrain3ToTree()
{
  vtkMAFSmartPointer<vtkPoints> in_points;
  mafVMEPolyline *polyline;
  mafNEW(polyline);

  BuildPolyline3(in_points);
  BuildVMEPolyline(in_points, polyline);

  polyline->SetName("pluto constrain");
  polyline->ReparentTo(m_Input);

  mafDEL(polyline);

}

void medOpInteractionDebugger::AddMEDGizmoDebuggerToTree()
{

  mafVME *vmeRoot = mafVME::SafeDownCast(m_Input->GetRoot());

  mafVME *constraint = mafVME::SafeDownCast(vmeRoot->FindInTreeByName("test graph"));
  assert(constraint != NULL);
  
  // Create the output vtk data...
  mafEventMacro(mafEvent(this,VME_SHOW,constraint,true));
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));

  // build constrained gizmo
  m_GizmoDebugger = new medGizmoInteractionDebugger(vmeRoot, this, "test gizmo path");

  m_GizmoDebugger->SetGizmoLength(2);

  if (constraint->IsA("medVMEPolylineGraph"))
  {
    medVMEPolylineGraph *polylineGraph = medVMEPolylineGraph::SafeDownCast(constraint);
    m_GizmoDebugger->SetConstraintPolylineGraph(polylineGraph);
  } 
  else
  {
    assert(false);
  }

  m_GizmoDebugger->SetCurvilinearAbscissa(m_ActiveBranchId, 0.0);
  m_GizmoDebugger->Show(true);
  
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));

}

void medOpInteractionDebugger::OnChooseConstrainVme( mafNode *vme )
{
  m_Constrain = mafVME::SafeDownCast(vme);
  assert(m_Constrain);

  mafEventMacro(mafEvent(this,VME_SHOW, m_Constrain, true));

  m_GizmoDebugger->SetGizmoLength(2);
  m_GizmoDebugger->SetConstraintPolylineGraph(medVMEPolylineGraph::SafeDownCast(m_Constrain));
  m_GizmoDebugger->SetCurvilinearAbscissa(m_ActiveBranchId, 0.0);
  m_GizmoDebugger->Show(true);
  
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

void medOpInteractionDebugger::BuildGraph1( vtkPolyData *inputPolyData )
{
  //------------------------------------------------------------------------------
/* Create example polydata.  This is a connected tree of lines and polylines.

												  17
												  |
												  16
												  |
												  15 - 18 - 19 - 20 - 21
												  |
												  14
	  2									  	|
	 /                      13
  /                       |                        
  1 - 3 - 6 - 7 - 8 - 9 - 10 - 11 - 12
  |   |\
  |   | \
  0   5  4


  # vtk DataFile Version 3.0
  vtk output
  ASCII
  DATASET POLYDATA
  POINTS 22 float
  0 0 0      1 2 0      2 4 0 
  2 1 0      3 1 0      3 0 0 
  3 2 0      4 1 0      5 2 0 
  6 1 0      7 2 0      8 1 0 
  9 2 0      7 4 0      9 6 0 
  10 8 0    13 10 0     14 12 0 
  11 9 0    12 8 0      13 8 0 
  16 10 0 

  LINES 7 35
  2 0 1 
  2 1 2 
  2 3 4 
  2 3 5 
  9 1 3 6 7 8 9 10 11 12 
  6 10 13 14 15 16 17 
  5 15 18 19 20 21 

  CELL_DATA 7
  POINT_DATA 22

*/
  vtkPoints *points = vtkPoints::New() ;
  vtkCellArray *lines = vtkCellArray::New() ;

  int i ;

  // coordinates of vertices
  static double vertices[22][3] ={
    {0,0,0},
    {1,2,0},
    {2,4,0},
    {2,1,0},
    {2,0,0},
    {3,0,0},

    {3,1,0},
    {4,1,0},
    {5,2,0},
    {6,1,0},
    {7,2,0},
    {8,1,0},
    {9,2,0},

    {7,4,0},
    {9,6,0},
    {10,8,0},
    {13,10,0},
    {14,12,0},

    {11,9,0},
    {12,8,0},
    {13,8,0},
    {16,10,0}
  } ;

  // indices of simple lines and polylines
  static vtkIdType lineids[7][10] = {
    {0,1},
    {1,2},
    {3,4},
    {3,5},
    {1, 3, 6, 7, 8, 9, 10, 11, 12},
    {10, 13, 14, 15, 16, 17},
    {15, 18, 19, 20, 21}
  };

  // insert points
  for (i = 0 ;  i < 22 ;  i++)
    points->InsertNextPoint(vertices[i]) ;

  // insert lines and polylines
  lines->InsertNextCell(2, lineids[0]) ;
  lines->InsertNextCell(2, lineids[1]) ;
  lines->InsertNextCell(2, lineids[2]) ;
  lines->InsertNextCell(2, lineids[3]) ;
  lines->InsertNextCell(9, lineids[4]) ;
  lines->InsertNextCell(6, lineids[5]) ;
  lines->InsertNextCell(5, lineids[6]) ;

  // put points and cells in polydata
  inputPolyData->SetPoints(points) ;
  inputPolyData->SetLines(lines) ;

  points->Delete() ;
  lines->Delete() ;

}

//----------------------------------------------------------------------------
void medOpInteractionDebugger::OpDo()
//----------------------------------------------------------------------------
{
  cppDEL(m_GizmoDebugger);
  // RemoveTestConstraintGraph1FromTree();
}
