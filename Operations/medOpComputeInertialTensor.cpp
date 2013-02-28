/*=========================================================================

 Program: MAF2Medical
 Module: medOpComputeInertialTensor
 Authors: Simone Brazzale , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

wxString DENSITY_TAG_NAME = "DENSITY";

using namespace std;

#include "medOpComputeInertialTensor.h"

#include "vnl/vnl_cross.h"
#include "wx/busyinfo.h"

#include "vtkCell.h"
#include "vtkIdList.h"
#include "vtkIdType.h"
#include "vtkPoints.h"
#include "vtkTriangle.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkGenericCell.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMassProperties.h"
#include "vtkGeometryFilter.h"
#include "vtkMAFCellLocator.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafVMEGroup.h"
#include "mafVMESurface.h"
#include "mafVMEOutput.h"
#include "mafTagArray.h"

//----------------------------------------------------------------------------
// Defines :
//----------------------------------------------------------------------------

#define vtkCELLTRIANGLES(CELLPTIDS, TYPE, IDX, PTID0, PTID1, PTID2) \
        { switch( TYPE ) \
          { \
          case VTK_TRIANGLE: \
          case VTK_POLYGON: \
          case VTK_QUAD: \
            PTID0 = CELLPTIDS[0]; \
            PTID1 = CELLPTIDS[(IDX)+1]; \
            PTID2 = CELLPTIDS[(IDX)+2]; \
            break; \
          case VTK_TRIANGLE_STRIP: \
            PTID0 = CELLPTIDS[IDX]; \
            PTID1 = CELLPTIDS[(IDX)+1+((IDX)&1)]; \
            PTID2 = CELLPTIDS[(IDX)+2-((IDX)&1)]; \
            break; \
          default: \
            PTID0 = PTID1 = PTID2 = -1; \
          } }

#define VTK_MAX_ITER 10    //Maximum iterations for ray-firing
#define VTK_VOTE_THRESHOLD 3

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

  m_DefaultDensity = 1.0;
  m_Mass = 0.0;
  m__Principal_I1 = 0.0;
  m_Principal_I2 = 0.0;
  m_Principal_I3 = 0.0;
  m_Accuracy = 1000;
  m_Vtkcomp = 1;

  m_InertialTensor[0] = 0;
  m_InertialTensor[1] = 0;
  m_InertialTensor[2] = 0;

  m_InertialTensor[3] = 0;
  m_InertialTensor[4] = 0;
  m_InertialTensor[5] = 0;

  m_InertialTensor[6] = 0;
  m_InertialTensor[7] = 0;
  m_InertialTensor[8] = 0;

  m_CenterOfMass[0] = 0;
  m_CenterOfMass[1] = 0;
  m_CenterOfMass[2] = 0;

  m_MethodToUse = GEOMETRY;
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
	 medOpComputeInertialTensor* op_copy = new medOpComputeInertialTensor(m_Label);
   op_copy->m_DefaultDensity = this->m_DefaultDensity;

   return op_copy;
}
//----------------------------------------------------------------------------
bool medOpComputeInertialTensor::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return ( (node && node->IsMAFType(mafVMESurface)) || (node && node->IsMAFType(mafVMEGroup)) );
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
  mafVME* vme = (mafVME*) m_Input;


  if (!vme->GetTagArray()->IsTagPresent("LOCAL_CENTER_OF_MASS_COMPONENTS"))
  {
	  if (m_LocalCenterOfMassTag.GetNumberOfComponents()>0)
	  {
		  vme->GetTagArray()->SetTag(m_LocalCenterOfMassTag);
	  }
  }

  if (!vme->GetTagArray()->IsTagPresent("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS"))
  {
    if (m_PrincipalInertialTensorTag.GetNumberOfComponents()>0)
    {
      vme->GetTagArray()->SetTag(m_PrincipalInertialTensorTag);
    }
  }

  if (!vme->GetTagArray()->IsTagPresent("INERTIAL_TENSOR_COMPONENTS"))
  {
	  if (m_InertialTensorTag.GetNumberOfComponents()>0)
	  {
		  vme->GetTagArray()->SetTag(m_InertialTensorTag);
	  }
  }
  
  if (!vme->GetTagArray()->IsTagPresent("SURFACE_MASS"))
  {
    if (m_TagMass.GetNumberOfComponents()>0)
    {
      vme->GetTagArray()->SetTag(m_TagMass);
    }
  }

}
//----------------------------------------------------------------------------
void medOpComputeInertialTensor::OpUndo()
//----------------------------------------------------------------------------
{
  mafVME* vme = (mafVME*) m_Input;
  vme->GetTagArray()->GetTag("LOCAL_CENTER_OF_MASS_COMPONENTS",m_LocalCenterOfMassTag);
  vme->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",m_PrincipalInertialTensorTag);
  vme->GetTagArray()->GetTag("INERTIAL_TENSOR_COMPONENTS",m_InertialTensorTag);
  vme->GetTagArray()->GetTag("SURFACE_MASS",m_TagMass);

  vme->GetTagArray()->GetTag("LOCAL_CENTER_OF_MASS_COMPONENTS");
  vme->GetTagArray()->DeleteTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS");
  vme->GetTagArray()->DeleteTag("INERTIAL_TENSOR_COMPONENTS");
  vme->GetTagArray()->DeleteTag("SURFACE_MASS");
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
        int result = OP_RUN_CANCEL;

        if (m_Input->IsMAFType(mafVMESurface))
        {
          result = ComputeInertialTensor(m_Input);
        }
        else if (m_Input->IsMAFType(mafVMEGroup))
        {
          result = ComputeInertialTensorFromGroup();
        }

        if (result==OP_RUN_CANCEL)
        {
          OpStop(result);
        }

        AddAttributes();
        OpStop(result);
      }
			break;
		case wxCANCEL:
      { 
        OpStop(OP_RUN_CANCEL);
      }
			break;
    case ID_COMBO:
      {
        if (m_MethodToUse==1) 
        {
          m_Gui->Enable(ID_ACCURACY,false);
          m_Gui->Enable(ID_VTKCOMP,false);
        }
        else
        {
          m_Gui->Enable(ID_ACCURACY,true);
          m_Gui->Enable(ID_VTKCOMP,true);
        }
      }
    default:
      mafEventMacro(*e);
      break;
		}
	}
}
//----------------------------------------------------------------------------
void medOpComputeInertialTensor::AddAttributes()
//----------------------------------------------------------------------------
{
	std::vector<double> localCenterOfMassComponents;
	localCenterOfMassComponents.push_back(m_CenterOfMass[0]);
	localCenterOfMassComponents.push_back(m_CenterOfMass[1]);
	localCenterOfMassComponents.push_back(m_CenterOfMass[2]);

	mafTagItem tagLocalCenterOfMass;
	tagLocalCenterOfMass.SetName("LOCAL_CENTER_OF_MASS_COMPONENTS");
	tagLocalCenterOfMass.SetNumberOfComponents(localCenterOfMassComponents.size());
	tagLocalCenterOfMass.SetComponents(localCenterOfMassComponents);
	m_Input->GetTagArray()->SetTag(tagLocalCenterOfMass);

	std::vector<double> principalInertialTensorComponents;
	principalInertialTensorComponents.push_back(m__Principal_I1);
	principalInertialTensorComponents.push_back(m_Principal_I2);
	principalInertialTensorComponents.push_back(m_Principal_I3);

	mafTagItem tagPrincipalInertialTensor;
	tagPrincipalInertialTensor.SetName("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS");
	tagPrincipalInertialTensor.SetNumberOfComponents(principalInertialTensorComponents.size());
	tagPrincipalInertialTensor.SetComponents(principalInertialTensorComponents);
	m_Input->GetTagArray()->SetTag(tagPrincipalInertialTensor);

	std::vector<double> inertialTensorComponents;
	for (int i = 0 ; i < 9 ; i++)
	{
		inertialTensorComponents.push_back(m_InertialTensor[i]);
	}

	mafTagItem tagInertialTensor;
	tagInertialTensor.SetName("INERTIAL_TENSOR_COMPONENTS");
	tagInertialTensor.SetNumberOfComponents(inertialTensorComponents.size());
	tagInertialTensor.SetComponents(inertialTensorComponents);
	m_Input->GetTagArray()->SetTag(tagInertialTensor);

	mafTagItem tagMass;
	tagMass.SetName("SURFACE_MASS");
	tagMass.SetValue(m_Mass);
	m_Input->GetTagArray()->SetTag(tagMass);

	if (m_Input->IsA("mafVMEGroup")) 
	{
		vector<pair<mafNode * , double>>::iterator iter;

		for (iter = m_NodeMassPairVector.begin() ; iter != m_NodeMassPairVector.end() ; ++iter)
		{
			mafNode *node = iter->first;
			double mass = iter->second;

			mafTagItem ti;
			ti.SetName("SURFACE_MASS");
			ti.SetValue(mass);

			node->GetTagArray()->SetTag(ti);
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
  m_Gui->Label("Default Density is the density");
  m_Gui->Label("value that will be used for");
  m_Gui->Label("computation if DENSITY tag");
  m_Gui->Label("is not found in VME's tagArray.");
  m_Gui->Label("");
  m_Gui->Label("Default Density");
  m_Gui->Double(-1,_(""),&m_DefaultDensity);
  m_Gui->Divider(0);
  m_Gui->Divider(0);  
  m_Gui->Integer(ID_ACCURACY,_("Accuracy"),&m_Accuracy,0,100000);
  m_Gui->Divider(0);

  wxString choices[2]={_("Monte Carlo"),_("Geometry")};
  m_Gui->Combo(ID_COMBO,_("Method:"),&m_MethodToUse,2,choices,"Select method");
  m_Gui->Divider(0);

  m_Gui->Bool(ID_VTKCOMP,"Use VTK compatibility",&m_Vtkcomp,1,"Use VTK compatibility");
  m_Gui->Divider(0);

  if (m_MethodToUse==1) {
    m_Gui->Enable(ID_ACCURACY,false);
    m_Gui->Enable(ID_VTKCOMP,false);
  }

  m_Gui->OkCancel();
}
//----------------------------------------------------------------------------
double medOpComputeInertialTensor::GetSurfaceVolume(mafNode* node)
//----------------------------------------------------------------------------
{
  mafVMESurface* surf = mafVMESurface::SafeDownCast(node);

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
double medOpComputeInertialTensor::GetSurfaceArea(mafNode* node)
//----------------------------------------------------------------------------
{
  mafVMESurface* surf = mafVMESurface::SafeDownCast(node);

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
double medOpComputeInertialTensor::GetSurfaceMassFromVolume(mafNode* node)
//----------------------------------------------------------------------------
{
  double density = GetDensity(node);

  if (density == DENSITY_NOT_FOUND)
  {
	  density = m_DefaultDensity;
  }

  double mass = GetSurfaceVolume(node)*density;

  return mass;
}
//----------------------------------------------------------------------------
double medOpComputeInertialTensor::GetSurfaceMassFromArea(mafNode* node)
//----------------------------------------------------------------------------
{
  double density = GetDensity(node);

  if (density == DENSITY_NOT_FOUND)
  {
	  density = m_DefaultDensity;
  }
  double mass = GetSurfaceArea(node)*density;

  return mass;
}
//----------------------------------------------------------------------------
double medOpComputeInertialTensor::GetSurfaceMassFromVTK(mafNode* node)
//----------------------------------------------------------------------------
{
  // get surface
  mafVMESurface* surf = (mafVMESurface*) node;
  if (surf->GetOutput() == NULL || surf->GetOutput()->GetVTKData() == NULL)
    return 0;
  surf->GetOutput()->Update();
  surf->GetOutput()->GetVTKData()->Update();

  // get dataset
  vtkDataSet* ds = surf->GetOutput()->GetVTKData();
  int npoints = ds->GetNumberOfPoints();
  int ncells = ds->GetNumberOfCells();

  // compute mass
  double distributed_mass = GetSurfaceMassFromVolume(node)/GetSurfaceArea(node);
  double tot_mass=0;

  // initialize variables
  int pId, qId, rId;
  double p[3],q[3],r[3],dp0[3],dp1[3],c[3],xp[3];
  
  tot_mass = 0.0;
  
  // loop through cells
  for (int i=0; i<ncells;i++)
  {
    int cellId = i;
    vtkCell* cell = ds->GetCell(cellId);
    int type = cell->GetCellType();
    
    vtkIdType numPts = 0;
    vtkIdType *ptIds = 0;

    // get cell points
    switch (ds->GetDataObjectType())
      {
      case VTK_POLY_DATA:
        ((vtkPolyData *)ds)->GetCellPoints( cellId, numPts, ptIds );
        break;
      case VTK_UNSTRUCTURED_GRID:
        ((vtkUnstructuredGrid *)ds)->GetCellPoints( cellId, numPts, ptIds );
        break;
      default:
        break;
      }
    for (int j=0; j<numPts-2; j++ )
    {
      // trianglize cells
      vtkCELLTRIANGLES( ptIds, type, j, pId, qId, rId );
      if ( pId < 0 )
        {
        continue;
        }
      ds->GetPoint(pId, p);
      ds->GetPoint(qId, q);
      ds->GetPoint(rId, r);
      // p, q, and r are the oriented triangle points.
      // compute the components of the moment of inertia tensor.
      for (int k=0; k<3; k++ )
        {
        // two edge vectors
        dp0[k] = q[k] - p[k];
        dp1[k] = r[k] - p[k];
        // centroid
        c[k] = (p[k] + q[k] + r[k])/3;
        }
      vtkMath::Cross( dp0, dp1, xp );

      double tri_area = 0.5*vtkMath::Norm( xp );
      double tri_mass = distributed_mass*tri_area;
      tot_mass += tri_mass;
	}
  }
  return tot_mass;
}
//----------------------------------------------------------------------------
int medOpComputeInertialTensor::ComputeInertialTensor(mafNode* node, int current_node, int n_of_nodes)
//----------------------------------------------------------------------------
{
  if (m_MethodToUse==MONTE_CARLO)
  {
    return ComputeInertialTensorUsingMonteCarlo(node,current_node,n_of_nodes);
  }
  else
  {
    return ComputeInertialTensorUsingGeometry(node,current_node,n_of_nodes);
  }
}
//----------------------------------------------------------------------------
int medOpComputeInertialTensor::ComputeInertialTensorUsingGeometry(mafNode* node, int current_node, int n_of_nodes)
//----------------------------------------------------------------------------
{
  //tensor components
  /* 
            | (y^2+z^2)    -x*y      -x*z     |
     T_i =  |   -x*y    (x^2+z^2)    -y*z     |
            |   -x*z       -y*z    (x^2+y^2)  |

              t21 = t12; t31 = t13; t32 = t23

      T   =   sum_i (m_i*t_i)

      the tensor of a system with n points each of mass m_i is the sum of the components over the n points.

  */

  int result = OP_RUN_CANCEL;

  if (!node->IsMAFType(mafVMESurface))
  {
    return result;
  }
  
  // get surface
  mafVMESurface* surf = (mafVMESurface*) node;
  if (surf->GetOutput() == NULL || surf->GetOutput()->GetVTKData() == NULL)
    return result;
  surf->GetOutput()->Update();
  surf->GetOutput()->GetVTKData()->Update();

  // get dataset
  vtkDataSet* ds = surf->GetOutput()->GetVTKData();
  int npoints = ds->GetNumberOfPoints();
  int ncells = ds->GetNumberOfCells();

  if (ncells==0)
  {
    return result;
  }

  // wx stuff
  wxBusyInfo *wait;
  wxString str("Computing inertial tensor: surface ");
  str << current_node << "/" << n_of_nodes;
  mafString s(str.c_str());
  if(!m_TestMode)
  {
    wxSetCursor(wxCursor(wxCURSOR_WAIT));
    wait = new wxBusyInfo("Computing inertial tensor components...");
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_TEXT,&s));
  }

  // initialize variables
  int pId, qId, rId;
  double p[3],q[3],r[3];
  double *a[3], a0[3], a1[3], a2[3], *v[3], v0[3], v1[3], v2[3];

  double _xx=0; double _yy=0; double _zz=0;
  double _yx=0; double _zx=0; double _zy=0;
  double _Cx=0; double _Cy=0; double _Cz=0;
  double _m=0;
  
  a[0] = a0; a[1] = a1; a[2] = a2;
  for (int i=0; i<3; i++ )
    {
    a0[i] = a1[i] = a2[i] = 0.0;
    }
  
  // loop through cells
  for (int i=0; i<ncells;i++)
  {
    int cellId = i;
    vtkCell* cell = ds->GetCell(cellId);
    int type = cell->GetCellType();
    
    vtkIdType numPts = 0;
    vtkIdType *ptIds = 0;

    // get cell points
    switch (ds->GetDataObjectType())
      {
      case VTK_POLY_DATA:
        ((vtkPolyData *)ds)->GetCellPoints( cellId, numPts, ptIds );
        break;
      case VTK_UNSTRUCTURED_GRID:
        ((vtkUnstructuredGrid *)ds)->GetCellPoints( cellId, numPts, ptIds );
        break;
      default:
        break;
      }
    for (int j=0; j<numPts-2; j++ )
    {
      // trianglize cells
      vtkCELLTRIANGLES( ptIds, type, j, pId, qId, rId );
      if ( pId < 0 )
        {
        continue;
        }
      ds->GetPoint(pId, p);
      ds->GetPoint(qId, q);
      ds->GetPoint(rId, r);
      // p, q, and r are the oriented triangle points.

      double x1=p[0];
      double y1=p[1];
      double z1=p[2];
      double x2=q[0];
      double y2=q[1];
      double z2=q[2];
      double x3=r[0];
      double y3=r[1];
      double z3=r[2];

      // Signed volume of this tetrahedron.
      double v = x1*y2*z3 + y1*z2*x3 + x2*y3*z1 -
                  (x3*y2*z1 + x2*y1*z3 + y3*z2*x1);
        
      // Contribution to the mass
      _m += v;

      // Contribution to the centroid
      double x4 = x1 + x2 + x3;           
      _Cx += (v * x4);
      double y4 = y1 + y2 + y3;           
      _Cy += (v * y4);
      double z4 = z1 + z2 + z3;           
      _Cz += (v * z4);

      // Contribution to moment of inertia 
      _xx += v * (x1*x1 + x2*x2 + x3*x3 + x4*x4);
      _yy += v * (y1*y1 + y2*y2 + y3*y3 + y4*y4);
      _zz += v * (z1*z1 + z2*z2 + z3*z3 + z4*z4);
      _yx += v * (y1*x1 + y2*x2 + y3*x3 + y4*x4);
      _zx += v * (z1*x1 + z2*x2 + z3*x3 + z4*x4);
      _zy += v * (z1*y1 + z2*y2 + z3*y3 + z4*y4);

      if (!m_TestMode)
      {
        mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)(((double) i)/((double) ncells)*100.)));
      }

    } // end foreach triangle
  }// end foreach cell

  // Centroid.  
  // The case _m = 0 needs to be addressed here.
  double rr = 1.0 / (4 * _m);
  double Cx = _Cx * rr;
  double Cy = _Cy * rr;
  double Cz = _Cz * rr;

  m_CenterOfMass[0] += Cx;
  m_CenterOfMass[1] += Cy;
  m_CenterOfMass[2] += Cz;

  // Mass
  double m = _m / 6;

  // Moment of inertia about the centroid.
  rr = 1.0 / 120;
  double Iyx = _yx * rr - m * Cy*Cx;
  double Izx = _zx * rr - m * Cz*Cx;
  double Izy = _zy * rr - m * Cz*Cy;

  _xx = _xx * rr - m * Cx*Cx;
  _yy = _yy * rr - m * Cy*Cy;
  _zz = _zz * rr - m * Cz*Cz;

  double Ixx = _yy + _zz;
  double Iyy = _zz + _xx;
  double Izz = _xx + _yy;  

  // Fill matrix 
  a0[0] = Ixx; a0[1] = Iyx; a0[2] = Izx;
  a1[0] = Iyx; a1[1] = Iyy; a1[2] = Izy;
  a2[0] = Izx; a2[1] = Izy; a2[2] = Izz;


  // by the spectral theorem, since the moment of inertia tensor is real and symmetric, there exists a Cartesian coordinate system in which it is diagonal,
  // the coordinate axes are called the principal axes and the constants I1, I2 and I3 are called the principal moments of inertia. 
  // extract eigenvalues from jacobian matorix (inertial tensor components referred to principal axes).
  double eval[3];
  v[0] = v0; v[1] = v1; v[2] = v2; 
  vtkMath::Jacobi(a,eval,v);

  // scale by the density

  double density = GetDensity(node);
  
  if (density == DENSITY_NOT_FOUND)
  {

	  density = m_DefaultDensity;

	  std::ostringstream stringStream;
	  stringStream << DENSITY_TAG_NAME.c_str() << " tag not found. Using default density value ie " << density << std::endl;          			
	  mafLogMessage(stringStream.str().c_str());
	  
  }
  else
  {
	  std::ostringstream stringStream;
	  stringStream << DENSITY_TAG_NAME.c_str() << " tag found. Using value " << density << std::endl;          			
	  mafLogMessage(stringStream.str().c_str());

  }

  double scale = density;

  // scale the inertial tensor with the density
  m_InertialTensor[0] += scale * Ixx;
  m_InertialTensor[1] += scale * Iyx;
  m_InertialTensor[2] += scale * Izx;

  m_InertialTensor[3] += scale * Iyx;
  m_InertialTensor[4] += scale * Iyy;
  m_InertialTensor[5] += scale * Izy;

  m_InertialTensor[6] += scale * Izx;
  m_InertialTensor[7] += scale * Izy;
  m_InertialTensor[8] += scale * Izz;

  // scale the principal inertial tensor components with the density
  m__Principal_I1 += scale*eval[0];
  m_Principal_I2 += scale*eval[1];
  m_Principal_I3 += scale*eval[2];

  // store the mass for later use
  double mass = scale * m;

  pair<mafNode* , double> nodeMassPair(node , mass);

  m_NodeMassPairVector.push_back(nodeMassPair);

  m_Mass += mass;

  if(!m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
    wxSetCursor(wxCursor(wxCURSOR_DEFAULT));
    cppDEL(wait);
  }

  result = OP_RUN_OK;
  return result;
}
//----------------------------------------------------------------------------
int medOpComputeInertialTensor::ComputeInertialTensorUsingMonteCarlo(mafNode* node, int current_node, int n_of_nodes)
//----------------------------------------------------------------------------
{
  //tensor components
  /* 
            | (y^2+z^2)    -x*y      -x*z     |
     T_i =  |   -x*y    (x^2+z^2)    -y*z     |
            |   -x*z       -y*z    (x^2+y^2)  |

              t21 = t12; t31 = t13; t32 = t23

      T   =   sum_i (m_i*t_i)

      the tensor of a system with n points each of mass m_i is the sum of the components over the n points.

  */

  int result = OP_RUN_CANCEL;

  if (!node->IsMAFType(mafVMESurface))
  {
    return result;
  }
  
  // get surface
  mafVMESurface* surf = (mafVMESurface*) node;
  if (surf->GetOutput() == NULL || surf->GetOutput()->GetVTKData() == NULL)
    return result;
  surf->GetOutput()->Update();
  surf->GetOutput()->GetVTKData()->Update();

  // wx stuff
  wxBusyInfo *wait;
  wxString str("Computing inertial tensor: surface ");
  str << current_node << "/" << n_of_nodes;
  mafString s(str.c_str());
  if(!m_TestMode)
  {
    wxSetCursor(wxCursor(wxCURSOR_WAIT));
    wait = new wxBusyInfo("Computing inertial tensor components, please wait...");
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_TEXT,&s));
  }
  
  // get dataset
  vtkDataSet* ds = surf->GetOutput()->GetVTKData();
  vtkGeometryFilter* gfilter = vtkGeometryFilter::New();
  gfilter->SetInput(ds);
  gfilter->Update();
  vtkPolyData* pd = gfilter->GetOutput();

  // get vme bounds
  double vme_bounds[6];
  surf->GetOutput()->GetBounds(vme_bounds);

  // get dataset bounds
  double ds_bounds[6];
  ds->GetBounds(ds_bounds);
  
  // initialize points for Monte Carlo method
  double pint = 0; // this is the counter for the internal points
  double ptot = 0; // this is the counter for all the points

  // inizialize matrixes
  double *a[3], a0[3], a1[3], a2[3], *ai[3], ai0[3], ai1[3], ai2[3],*v[3], v0[3], v1[3], v2[3];
  ai[0] = ai0; ai[1] = ai1; ai[2] = ai2; // matrix elements for internal points
  a[0] = a0; a[1] = a1; a[2] = a2; // final matrix elements for tensor computation
  for (int i=0; i<3; i++ )
  {
    ai0[i] = ai1[i] = ai2[i] = 0.0;
    a0[i] = a1[i] = a2[i] = 0.0;
  }
  v[0] = v0; v[1] = v1; v[2] = v2;
  
  /*
    START MONTE CARLO:
    Pick a point in the containing hypercube. Add to the ptot counter. 
    Is it in the polyhedron? If yes, add to the pint counter. Do this lots (m_Accuracy).
    Then mass_polyhedron/mass_hypercube \approx pint/ptot.

    For the moment of inertia, each count is weighted by the square of the distance of the point to the reference axis.
  */
 
  // 1) Measure Center Of Mass coordinates

  double com[3];
  com[0]=0;com[1]=0;com[2]=0;
  double pcom = 0;
  for (int i=0; i<m_Accuracy; i++)
  {
    // get random point
    double x = vtkMath::Random(vme_bounds[0],vme_bounds[1]);
    double y = vtkMath::Random(vme_bounds[2],vme_bounds[3]);
    double z = vtkMath::Random(vme_bounds[4],vme_bounds[5]);
    
    // refer points to the center of the bounding box for vtk calculations
    double p[3];
    p[0] = x - (vme_bounds[0] - ds_bounds[0]);
    p[1] = y - (vme_bounds[2] - ds_bounds[2]);
    p[2] = z - (vme_bounds[4] - ds_bounds[4]);
    
    // is it inside surface? (the algorithm assumes the surface is closed)
    if (IsInsideSurface(pd,p))
    {
      com[0] += x;
      com[1] += y;
      com[2] += z;
      pcom++;
    }

    if (!m_TestMode)
    {
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)(((double) i)/((double) m_Accuracy)*50.)));
    }
  }
  com[0] /= pcom;
  com[1] /= pcom;
  com[2] /= pcom;

  m_CenterOfMass[0] += com[0];
  m_CenterOfMass[1] += com[1];
  m_CenterOfMass[2] += com[2];

  if (!m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,50.0));
  }

  // 2) Apply Monte Carlo method

  for (int i=0; i<m_Accuracy; i++)
  {
    // get random point
    double x = vtkMath::Random(vme_bounds[0],vme_bounds[1]);
    double y = vtkMath::Random(vme_bounds[2],vme_bounds[3]);
    double z = vtkMath::Random(vme_bounds[4],vme_bounds[5]);
    
    // refer points to the center of the bounding box for vtk calculations
    double p[3];
    p[0] = x - (vme_bounds[0] - ds_bounds[0]);
    p[1] = y - (vme_bounds[2] - ds_bounds[2]);
    p[2] = z - (vme_bounds[4] - ds_bounds[4]);

    // is it inside surface? (the algorithm assumes the surface is closed)
    if (IsInsideSurface(pd,p))
    {
      pint++;

      // refere points to the center of mass of the surface for Monte Carlo calculations
      x -= com[0];
      y -= com[1];
      z -= com[2];

      // perform matrix element calculation
      ai0[0] += y*y + z*z ;
      ai1[1] += x*x + z*z ;
      ai2[2] += x*x + y*y ;
      ai0[1] -= x*y;
      ai0[2] -= x*z;
      ai1[2] -= y*z;
    }

    ptot++;

    if (!m_TestMode)
    {
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)(50+(((double) i)/((double) m_Accuracy)*50.))));
    }
  }

  // 3) Get final values

  // matrix is symmetric
  ai1[0] = ai0[1];
  ai2[0] = ai0[2];
  ai2[1] = ai1[2];

  // get hypercube volume and mass
  double hycube_v = abs(ds_bounds[1]-ds_bounds[0]) * abs(ds_bounds[3]-ds_bounds[2]) * abs(ds_bounds[5]-ds_bounds[4]);
  
  double density = GetDensity(node);

  if (density == DENSITY_NOT_FOUND)
  {

	  density = m_DefaultDensity;
  }
  
  double hycube_m = density*hycube_v;

  // get Monte Carlo ratios
  double poly_hycube_mratio = pint/ptot;
  double ph_t11ratio = ai0[0]/ptot;
  double ph_t22ratio = ai1[1]/ptot;
  double ph_t33ratio = ai2[2]/ptot;
  double ph_t12ratio = ai0[1]/ptot;
  double ph_t13ratio = ai0[2]/ptot;
  double ph_t21ratio = ai1[0]/ptot;
  double ph_t23ratio = ai1[2]/ptot;
  double ph_t31ratio = ai2[0]/ptot;
  double ph_t32ratio = ai2[1]/ptot;
  
  // calculate mass, check if compatibility with VTK is required 
  // NB: the scale factor should be used when the mass is measured using VTK method (vtk compatibility) rather than MC method. 
  //     in this case also the tensor components need to be rescaled by the ratio mass_VTK/mass_MC.
  double scale = 1.0;
  double mc_mass = hycube_m * poly_hycube_mratio;
  if (!m_Vtkcomp)
  {
    m_Mass += mc_mass;
  }
  else
  {
    double vtk_mass = GetSurfaceMassFromVTK(node);

    m_Mass += vtk_mass;

    scale = vtk_mass/mc_mass;
  }

  // fill final inertia tensor
  a[0][0] = hycube_m*ph_t11ratio;
  a[0][1] = hycube_m*ph_t12ratio;
  a[0][2] = hycube_m*ph_t13ratio;
  a[1][0] = hycube_m*ph_t21ratio;
  a[1][1] = hycube_m*ph_t22ratio;
  a[1][2] = hycube_m*ph_t23ratio;
  a[2][0] = hycube_m*ph_t31ratio;
  a[2][1] = hycube_m*ph_t32ratio;
  a[2][2] = hycube_m*ph_t33ratio;

  mafString ss;
  
  // by the spectral theorem, since the moment of inertia tensor is real and symmetric, there exists a Cartesian coordinate system in which it is diagonal,
  // the coordinate axes are called the principal axes and the constants I1, I2 and I3 are called the principal moments of inertia. 
  // extract eigenvalues from jacobian matrix (inertial tensor components referred to principal axes).
  double eval[3];  
  vtkMath::Jacobi(a,eval,v);

  // scale the principal inertial tensor
  m__Principal_I1 += scale*eval[0];
  m_Principal_I2 += scale*eval[1];
  m_Principal_I3 += scale*eval[2];

  // scale the inertial tensor
  m_InertialTensor[0] += scale * a[0][0];
  m_InertialTensor[1] += scale * a[0][1];
  m_InertialTensor[2] += scale * a[0][2];
  m_InertialTensor[3] += scale * a[1][0];
  m_InertialTensor[4] += scale * a[1][1];
  m_InertialTensor[5] += scale * a[1][2];
  m_InertialTensor[6] += scale * a[2][0];
  m_InertialTensor[7] += scale * a[2][1];
  m_InertialTensor[8] += scale * a[2][2];

  if(!m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
    wxSetCursor(wxCursor(wxCURSOR_DEFAULT));
    cppDEL(wait);
  }

  vtkDEL(gfilter);
  
  result = OP_RUN_OK;
  return result;
}
//----------------------------------------------------------------------------
int medOpComputeInertialTensor::ComputeInertialTensorFromGroup()
//----------------------------------------------------------------------------
{
  mafVMEGroup* group = (mafVMEGroup*) m_Input;

  int result = OP_RUN_CANCEL;

  int n_of_children = group->GetNumberOfChildren();
  int n_of_surfaces = 0;
  
  // get number of children surfaces (only direct child!)
  for (int i=0;i<n_of_children;i++)
  {
    if (group->GetChild(i)->IsA("mafVMESurface"))
    {
      n_of_surfaces++;
    }
  }

  if (n_of_surfaces>0) 
  {
    wxString s;
    s << "Found " << n_of_surfaces << " surfaces: applying operation on all of them ..";
    mafLogMessage(s.c_str());
  }
  else
  {
    mafLogMessage("no surfaces found in the group. Quit!");
    return result;
  }

  // compute inertial tensor fro each children (results will be summed)
  for (int i=0;i<n_of_children;i++)
  {
	mafVMESurface *childSurface = mafVMESurface::SafeDownCast(group->GetChild(i));

    if (childSurface != NULL)
    {
	  
	  wxString s;
	  s << "Computing Inertial tensor for: " << childSurface->GetName();
	  mafLogMessage(s.c_str());      
	  ComputeInertialTensor(childSurface,i+1,n_of_children - 1);

	  m_CenterOfMass[0] = m_CenterOfMass[0] / n_of_children; 
	  m_CenterOfMass[1] = m_CenterOfMass[1] / n_of_children;
	  m_CenterOfMass[2] = m_CenterOfMass[2] / n_of_children;

    }
  }

  result = OP_RUN_OK;

  return result;
}
//----------------------------------------------------------------------------
int medOpComputeInertialTensor::IsInsideSurface(vtkPolyData* surface, double x[3])
//----------------------------------------------------------------------------
{
  /*
    IMPORTED FROM VTK 5.2 and modified by Simone Brazzale
  */

  if (surface==NULL)
  {
    return 0;
  }

  double bounds[6];
  surface->GetBounds(bounds);

  // do a quick bounds check
  if ( x[0] < bounds[0] || x[0] > bounds[1] ||
       x[1] < bounds[2] || x[1] > bounds[3] ||
       x[2] < bounds[4] || x[2] > bounds[5])
    {
    return 0;
    }
  
  // Set up structures for acceleration ray casting
  double tolerance = 0.001;
  double length = surface->GetLength();

  vtkMAFCellLocator* cellLocator = vtkMAFCellLocator::New();
  vtkIdList* cellIds = vtkIdList::New();
  vtkGenericCell* cell = vtkGenericCell::New();

  cellLocator->SetDataSet(surface);
  cellLocator->BuildLocator();
  
  //  Perform in/out by shooting random rays. Multiple rays are fired
  //  to improve accuracy of the result.
  //
  //  The variable iterNumber counts the number of rays fired and is
  //  limited by the defined variable VTK_MAX_ITER.
  //
  //  The variable deltaVotes keeps track of the number of votes for
  //  "in" versus "out" of the surface.  When deltaVotes > 0, more votes
  //  have counted for "in" than "out".  When deltaVotes < 0, more votes
  //  have counted for "out" than "in".  When the delta_vote exceeds or
  //  equals the defined variable VTK_VOTE_THRESHOLD, then the
  //  appropriate "in" or "out" status is returned.
  //
  double rayMag, ray[3], xray[3], t, pcoords[3], xint[3];
  int i, numInts, iterNumber, deltaVotes, subId;
  vtkIdType idx, numCells;
  double tol = tolerance*length;

  for (deltaVotes = 0, iterNumber = 1;
       (iterNumber < VTK_MAX_ITER) && (abs(deltaVotes) < VTK_VOTE_THRESHOLD);
       iterNumber++) 
    {
    //  Define a random ray to fire.
    rayMag = 0.0;
    while (rayMag == 0.0 )
      {
      for (i=0; i<3; i++)
        {
        ray[i] = vtkMath::Random(-1.0,1.0);
        }
      rayMag = vtkMath::Norm(ray);
      }

    // The ray must be appropriately sized wrt the bounding box. (It has to go
    // all the way through the bounding box.)
    for (i=0; i<3; i++)
      {
      xray[i] = x[i] + (length/rayMag)*ray[i];
      }

    // Retrieve the candidate cells from the locator
    cellLocator->FindCellsAlongLine(x,xray,tol,cellIds);

    // Intersect the line with each of the candidate cells
    numInts = 0;
    numCells = cellIds->GetNumberOfIds();
    for ( idx=0; idx < numCells; idx++ )
      {
      surface->GetCell(cellIds->GetId(idx), cell);
      if ( cell->IntersectWithLine(x, xray, tol, t, xint, pcoords, subId) )
        {
        numInts++;
        }
      } //for all candidate cells
    
    // Count the result
    if ( (numInts % 2) == 0)
      {
      --deltaVotes;
      }
    else
      {
      ++deltaVotes;
      }
    } //try another ray

  cell->Delete();
  cellIds->Delete();
  cellLocator->Delete();

  //   If the number of votes is positive, the point is inside
  //
  return ( deltaVotes < 0 ? 0 : 1 );
}

double medOpComputeInertialTensor::GetDensity( mafNode* node)
{
	double density = DENSITY_NOT_FOUND;

	mafTagItem *densityTagItem = NULL;
	densityTagItem = node->GetTagArray()->GetTag(DENSITY_TAG_NAME.c_str());

	if (densityTagItem != NULL)
	{
		density = densityTagItem->GetValueAsDouble();
		return density;
	}
	else
	{
		return DENSITY_NOT_FOUND;
	}
}

double medOpComputeInertialTensor::GetMass( mafNode* node)
{
	double mass = SURFACE_MASS_NOT_FOUND;

	wxString massTagName = "SURFACE_MASS";

	mafTagItem *massTagItem = NULL;
	massTagItem = node->GetTagArray()->GetTag(massTagName.c_str());

	if (massTagItem != NULL)
	{
		mass = massTagItem->GetValueAsDouble();
		return mass;
	}
	else
	{
		return SURFACE_MASS_NOT_FOUND;
	}
}

double medOpComputeInertialTensor::GetDefaultDensity()
{
	return m_DefaultDensity;
}

void medOpComputeInertialTensor::SetDefaultDensity( double val )
{
	m_DefaultDensity = val;
}
