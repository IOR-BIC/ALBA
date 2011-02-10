/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpComputeInertialTensor.cpp,v $
  Language:  C++
  Date:      $Date: 2011-02-10 14:26:59 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Simone Brazzale
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "medOpComputeInertialTensor.h"

#include "vnl/vnl_cross.h"
#include "wx/busyinfo.h"

#include "vtkPoints.h"
#include "vtkTriangle.h"
#include "vtkDataSet.h"
#include "vtkMassProperties.h"
#include "vtkGeometryFilter.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafVMESurface.h"
#include "mafVMEOutput.h"
#include "mafTagArray.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpComputeInertialTensor);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpComputeInertialTensor::medOpComputeInertialTensor(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

  m_Density = 1.0;
  m_Mass = 0.0;
}
//----------------------------------------------------------------------------
medOpComputeInertialTensor::~medOpComputeInertialTensor( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* medOpComputeInertialTensor::Copy()   
//----------------------------------------------------------------------------
{
	return new medOpComputeInertialTensor(m_Label);
}
//----------------------------------------------------------------------------
bool medOpComputeInertialTensor::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMESurface));
}
//----------------------------------------------------------------------------
void medOpComputeInertialTensor::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
  ShowGui();
}
//----------------------------------------------------------------------------
void medOpComputeInertialTensor::OpDo()
//----------------------------------------------------------------------------
{
  mafVMESurface* surf = (mafVMESurface*) m_Input;
  if (!surf->GetTagArray()->IsTagPresent("INERTIAL_TENSOR_COMPONENTS_[t11,t12,t13,t22,t23,t33]"))
  {
    surf->GetTagArray()->SetTag(m_TagTensor);
  }
  if (!surf->GetTagArray()->IsTagPresent("SURFACE_MASS"))
  {
    surf->GetTagArray()->SetTag(m_TagMass);
  }
}
//----------------------------------------------------------------------------
void medOpComputeInertialTensor::OpUndo()
//----------------------------------------------------------------------------
{
  mafVMESurface* surf = (mafVMESurface*) m_Input;
  surf->GetTagArray()->GetTag("INERTIAL_TENSOR_COMPONENTS_[t11,t12,t13,t22,t23,t33]",m_TagTensor);
  surf->GetTagArray()->GetTag("SURFACE_MASS",m_TagMass);
  surf->GetTagArray()->DeleteTag("INERTIAL_TENSOR_COMPONENTS_[t11,t12,t13,t22,t23,t33]");
  surf->GetTagArray()->DeleteTag("SURFACE_MASS");
}
//----------------------------------------------------------------------------
void medOpComputeInertialTensor::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
		case wxOK:
      {
        int result = ComputeInertialTensor();

        OpStop(result);
      }
			break;
		case wxCANCEL:
      { 
        OpStop(OP_RUN_CANCEL);
      }
			break;
    default:
      mafEventMacro(*e);
      break;
		}
	}
}
//----------------------------------------------------------------------------
void medOpComputeInertialTensor::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  mafEventMacro(mafEvent(this,result)); 
}
//----------------------------------------------------------------------------
void medOpComputeInertialTensor::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

  m_Gui->Divider(0);
  m_Gui->Divider(0);
  m_Gui->Double(-1,_("Density"),&m_Density);
  m_Gui->Divider(0);
  m_Gui->Divider(0);

  m_Gui->OkCancel();
}
//----------------------------------------------------------------------------
double medOpComputeInertialTensor::GetSurfaceVolume()
//----------------------------------------------------------------------------
{
  mafVMESurface* surf = mafVMESurface::SafeDownCast(m_Input);

  if (surf->GetOutput() == NULL)
    return -1;

  if (surf->GetOutput()->GetVTKData() == NULL)
    return -1;

  vtkDataSet* data_set = (vtkDataSet*)surf->GetOutput()->GetVTKData();

  //convert to polydata
  vtkGeometryFilter* poly_filter;
  vtkNEW(poly_filter);
  poly_filter->SetInput(data_set);
  poly_filter->Update();

  //get mass properties
  vtkMassProperties* mass;
	vtkNEW(mass);
	mass->SetInput(poly_filter->GetOutput());
	double volume = mass->GetVolume();

  vtkDEL(mass);
  vtkDEL(poly_filter);

  return volume;
}
//----------------------------------------------------------------------------
double medOpComputeInertialTensor::GetSurfaceArea()
//----------------------------------------------------------------------------
{
  mafVMESurface* surf = mafVMESurface::SafeDownCast(m_Input);

  if (surf->GetOutput() == NULL)
    return -1;

  if (surf->GetOutput()->GetVTKData() == NULL)
    return -1;

  vtkDataSet* data_set = (vtkDataSet*)surf->GetOutput()->GetVTKData();

  //convert to polydata
  vtkGeometryFilter* poly_filter;
  vtkNEW(poly_filter);
  poly_filter->SetInput(data_set);
  poly_filter->Update();

  //get mass properties
  vtkMassProperties* mass;
	vtkNEW(mass);
	mass->SetInput(poly_filter->GetOutput());
	double area = mass->GetSurfaceArea();

  vtkDEL(mass);
  vtkDEL(poly_filter);

  return area;
}
//----------------------------------------------------------------------------
double medOpComputeInertialTensor::GetSurfaceMass()
//----------------------------------------------------------------------------
{
  double mass = GetSurfaceArea()*m_Density;

  return mass;
}
//----------------------------------------------------------------------------
int medOpComputeInertialTensor::ComputeInertialTensor()
//----------------------------------------------------------------------------
{
  int result = OP_RUN_CANCEL;

  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wxSetCursor(wxCursor(wxCURSOR_WAIT));
    wait = new wxBusyInfo("Computing inertial tensor components...");
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  }
  
  long current_progress = 0;
  long current_percentage = 0;

  //tensor components
  /* 
            | (y^2+z^2)    -x*y      -x*z     |
     T_i =  |   -x*y    (x^2+z^2)    -y*z     |
            |   -x*z       -y*z    (x^2+y^2)  |

              t21 = t12; t31 = t13; t32 = t23

      T   =   sum_i (m_i*t_i)

      the tensor of a system with n points each of mass m_i is the sum of the components over the n points.

  */
  double t11,t12,t13,t22,t23,t33;
  t11 = 0;
  t12 = 0;
  t13 = 0;
  t22 = 0;
  t23 = 0;
  t33 = 0;

  mafVMESurface* surf = (mafVMESurface*) m_Input;
  if (surf->GetOutput() == NULL || surf->GetOutput()->GetVTKData() == NULL)
    return result;

  vtkDataSet* ds = surf->GetOutput()->GetVTKData();
  int npoints = ds->GetNumberOfPoints();
  int ncells = ds->GetNumberOfCells();

  if (ncells==0)
  {
    return result;
  }

  //get points
  /*

      We can consider two ways of implementing points: 
        1) Take the points defined in the VTK structure
        2) Take the cells defined in the VTK structure and get the centroid point of each cell.
      The first is easier but can lack of information.
      Here the second method is implemented.

  */
  for (int i=0; i<ncells;i++)
  {
    vtkCell* cell = ds->GetCell(i);

    double xyz[3];
    double pcoords[3];
    double a;

    //compute cell area for mass evaluation
    switch ( cell->GetCellType() )
        {
      case VTK_TRIANGLE:
        a = TriangleArea( cell );
        break;
      case VTK_QUAD:
        a = QuadArea( cell );
        break;
      case VTK_TETRA:
        a = TetVolume( cell );
        break;
      case VTK_HEXAHEDRON:
        a = HexVolume( cell );
        break;
        }

    //get the centroid of the cell
    int subId = cell->GetParametricCenter(pcoords);
    int ncellpoints = cell->GetNumberOfPoints();
    double* w = new double[ncellpoints];
    cell->EvaluateLocation(subId,pcoords,xyz,w);

    //compute mass
    double cell_mass = a*m_Density;
      
    //compute tensor components
    double temp_t11 = cell_mass*((xyz[1]*xyz[1])+(xyz[2]*xyz[2]));
    double temp_t12 = cell_mass*(-1*(xyz[0]*xyz[1]));
    double temp_t13 = cell_mass*(-1*(xyz[0]*xyz[2]));
    double temp_t22 = cell_mass*((xyz[0]*xyz[0])+(xyz[2]*xyz[2]));
    double temp_t23 = cell_mass*(-1*(xyz[1]*xyz[2]));
    double temp_t33 = cell_mass*((xyz[0]*xyz[0])+(xyz[1]*xyz[1]));

    //sum over points
    t11 += temp_t11;
    t12 += temp_t12;
    t13 += temp_t13;
    t22 += temp_t22;
    t23 += temp_t23;
    t33 += temp_t33;

    m_Mass += cell_mass;

    current_progress++;
    current_percentage = current_progress*100 / ncells;
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,current_percentage));
  
    delete[] w;
  }

  std::vector<double> vec_comp;
  vec_comp.push_back(t11);
  vec_comp.push_back(t12);
  vec_comp.push_back(t13);
  vec_comp.push_back(t22);
  vec_comp.push_back(t23);
  vec_comp.push_back(t33);

  //save results in surface attributes
	mafTagItem tag;
	tag.SetName("INERTIAL_TENSOR_COMPONENTS_[t11,t12,t13,t22,t23,t33]");
  tag.SetNumberOfComponents(6);
  tag.SetComponents(vec_comp);
  surf->GetTagArray()->SetTag(tag);

  mafTagItem tagm;
	tagm.SetName("SURFACE_MASS");
  tagm.SetValue(m_Mass);
  surf->GetTagArray()->SetTag(tagm);

  if(!m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
    wxSetCursor(wxCursor(wxCURSOR_DEFAULT));
    cppDEL(wait);
  }

  result = OP_RUN_OK;
  return result;
}
// internal function
double vnl_dot(vnl_vector<double> v1,vnl_vector<double> v2)
{
  assert (v1.size()==3 && v2.size()==3);

  return (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
}
//----------------------------------------------------------------------------
double medOpComputeInertialTensor::TriangleArea( vtkCell* cell )
//----------------------------------------------------------------------------
{
  //get the area of a triangle cell
  double pc[3][3];

  vtkPoints *p = cell->GetPoints();
  p->GetPoint(0, pc[0]);
  p->GetPoint(1, pc[1]);
  p->GetPoint(2, pc[2]);

  vnl_vector<double> v1(3),v2(3);

  v1[0] = pc[1][0]-pc[0][0];
  v1[1] = pc[1][1]-pc[0][1];
  v1[2] = pc[1][2]-pc[0][2];

  v2[0] = pc[2][0]-pc[0][0];
  v2[1] = pc[2][1]-pc[0][1];
  v2[2] = pc[2][2]-pc[0][2];

  vnl_vector<double> cross(3);
  cross = vnl_cross_3d(v1,v2);

  return 0.5*(cross.magnitude());
}
//----------------------------------------------------------------------------
double medOpComputeInertialTensor::QuadArea( vtkCell* cell )
//----------------------------------------------------------------------------
{
  //get the area of a quad cell
  double pc[4][3];

  vtkPoints *p = cell->GetPoints();
  for ( int i = 0; i < 4; ++i )
    p->GetPoint( i, pc[i] );

  double corn[4];
  vnl_vector<double> edg1(3),edg2(3),edg3(3),edg4(3);

  edg1[0] = pc[1][0]-pc[0][0];
  edg1[1] = pc[1][1]-pc[0][1];
  edg1[2] = pc[1][2]-pc[0][2];

  edg2[0] = pc[2][0]-pc[1][0];
  edg2[1] = pc[2][1]-pc[1][1];
  edg2[2] = pc[2][2]-pc[1][2];

  edg3[0] = pc[3][0]-pc[2][0];
  edg3[1] = pc[3][1]-pc[2][1];
  edg3[2] = pc[3][2]-pc[2][2];

  edg4[0] = pc[0][0]-pc[3][0];
  edg4[1] = pc[0][1]-pc[3][1];
  edg4[2] = pc[0][2]-pc[3][2];

  vnl_vector<double> norm1(3),norm2(3),norm3(3),norm4(3);

  //normals
  norm1= vnl_cross_3d(edg4,edg1);
  norm2= vnl_cross_3d(edg1,edg2);
  norm3= vnl_cross_3d(edg2,edg3);
  norm4= vnl_cross_3d(edg3,edg4);

  //principal axes
  vnl_vector<double> ax1(3),ax2(3);
  ax1 = edg1 - edg2;
  ax2 = edg2 - edg4;

  //quad center unit normal
  vnl_vector<double> center(3);
  center = vnl_cross_3d(ax1,ax2);
  center.normalize();

  //corners area
  corn[0] = vnl_dot(center,norm1);
  corn[1] = vnl_dot(center,norm2);
  corn[2] = vnl_dot(center,norm3);
  corn[3] = vnl_dot(center,norm4);

  double area = 0.25*(corn[0]+corn[1]+corn[2]+corn[3]);

  return area;
}
//----------------------------------------------------------------------------
double medOpComputeInertialTensor::TetVolume( vtkCell* cell )
//----------------------------------------------------------------------------
{
  //get the volume of a tet cell
  double pc[4][3];

  vtkPoints* p = cell->GetPoints();
  for ( int i = 0; i < 4; ++i )
    p->GetPoint( i, pc[i] );
    
  vnl_vector<double> v1(3),v2(3),v3(3);

  v1[0] = pc[1][0] - pc[0][0];
  v1[1] = pc[1][1] - pc[0][1];
  v1[2] = pc[1][2] - pc[0][2];

  v2[0] = pc[0][0] - pc[2][0];
  v2[1] = pc[0][1] - pc[2][1];
  v2[2] = pc[0][2] - pc[2][2];

  v3[0] = pc[3][0] - pc[0][0];
  v3[1] = pc[3][1] - pc[0][1];
  v3[2] = pc[3][2] - pc[0][2];

  return (double) ((vnl_dot(v3,vnl_cross_3d(v1,v2)))/6.0);

}
//----------------------------------------------------------------------------
double medOpComputeInertialTensor::HexVolume( vtkCell* cell )
//----------------------------------------------------------------------------
{
  //get the volume of a hex cell
  double pc[8][3];
  vtkPoints* p = cell->GetPoints();

  for ( int i = 0; i < 8; ++i )
    p->GetPoint( i, pc[i] );

  //node pos
  vnl_vector<double> np1(3),np2(3),np3(3),np4(3),np5(3),np6(3),np7(3),np8(3);

  FillVnlVector(np1,pc[0]);
  FillVnlVector(np2,pc[1]);
  FillVnlVector(np3,pc[2]);
  FillVnlVector(np4,pc[3]);
  FillVnlVector(np5,pc[4]);
  FillVnlVector(np6,pc[5]);
  FillVnlVector(np7,pc[6]);
  FillVnlVector(np8,pc[7]);

  //efg
  vnl_vector<double> efg1(3),efg2(3),efg3(3);

  efg1 =  np2;
  efg1 += np3;
  efg1 += np6;
  efg1 += np7;
  efg1 -= np1;
  efg1 -= np4;
  efg1 -= np5;
  efg1 -= np8;

  efg2 =  np3;
  efg2 += np4;
  efg2 += np7;
  efg2 += np8;
  efg2 -= np1;
  efg2 -= np2;
  efg2 -= np5;
  efg2 -= np6;

  efg3 =  np5;
  efg3 += np6;
  efg3 += np7;
  efg3 += np8;
  efg3 -= np1;
  efg3 -= np2;
  efg3 -= np3;
  efg3 -= np4;

  return (double) ((vnl_dot(efg1,vnl_cross_3d(efg2,efg3)))/64.0);
}
//----------------------------------------------------------------------------
void medOpComputeInertialTensor::FillVnlVector(vnl_vector<double> &v, double coords[3])
//----------------------------------------------------------------------------
{
  for (int i=0;i<3;i++)
  {
    v[0] = coords[0];
    v[1] = coords[1];
    v[2] = coords[2];
  }
}