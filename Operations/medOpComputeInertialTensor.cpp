/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpComputeInertialTensor.cpp,v $
  Language:  C++
  Date:      $Date: 2011-02-16 22:45:20 $
  Version:   $Revision: 1.1.2.4 $
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

  m_Density = 1.0;
  m_Mass = 0.0;
  m_Ixx = 0.0;
  m_Iyy = 0.0;
  m_Izz = 0.0;
  m_Accuracy = 1000;

  m_MethodToUse = MONTE_CARLO;
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
   op_copy->m_Density = this->m_Density;

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
  if (!vme->GetTagArray()->IsTagPresent("INERTIAL_TENSOR_COMPONENTS_[Ixx,Iyy,Izz]"))
  {
    if (m_TagTensor.GetNumberOfComponents()>0)
    {
      vme->GetTagArray()->SetTag(m_TagTensor);
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
  vme->GetTagArray()->GetTag("INERTIAL_TENSOR_COMPONENTS_[Ixx,Iyy,Izz]",m_TagTensor);
  vme->GetTagArray()->GetTag("SURFACE_MASS",m_TagMass);
  vme->GetTagArray()->DeleteTag("INERTIAL_TENSOR_COMPONENTS_[Ixx,Iyy,Izz]");
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
  //save results in vme attributes
  std::vector<double> vec_comp;
  vec_comp.push_back(m_Ixx);
  vec_comp.push_back(m_Iyy);
  vec_comp.push_back(m_Izz);

	mafTagItem tag;
	tag.SetName("INERTIAL_TENSOR_COMPONENTS_[Ixx,Iyy,Izz]");
  tag.SetNumberOfComponents(3);
  tag.SetComponents(vec_comp);
  m_Input->GetTagArray()->SetTag(tag);

  mafTagItem tagm;
	tagm.SetName("SURFACE_MASS");
  tagm.SetValue(m_Mass);
  m_Input->GetTagArray()->SetTag(tagm);
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
  m_Gui->Integer(-1,_("Accuracy"),&m_Accuracy,0,100000);
  m_Gui->Divider(0);

  wxString choices[2]={_("Monte Carlo"),_("Geometry")};
  m_Gui->Combo(ID_COMBO,_("Method to use:"),&m_MethodToUse,2,choices,"Select method to use");
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
  return GetSurfaceMassFromVolume();
}
//----------------------------------------------------------------------------
double medOpComputeInertialTensor::GetSurfaceMassFromVolume()
//----------------------------------------------------------------------------
{
  double mass = GetSurfaceVolume()*m_Density;

  return mass;
}
//----------------------------------------------------------------------------
double medOpComputeInertialTensor::GetSurfaceMassFromArea()
//----------------------------------------------------------------------------
{
  double mass = GetSurfaceArea()*m_Density;

  return mass;
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

  // compute mass
  double distributed_mass = GetSurfaceMassFromVolume()/GetSurfaceArea();
  double tot_mass=0;

  // initialize variables
  int pId, qId, rId;
  double p[3],q[3],r[3],dp0[3],dp1[3],c[3],xp[3];
  double *a[3], a0[3], a1[3], a2[3], *v[3], v0[3], v1[3], v2[3];;
  
  tot_mass = 0.0;
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
      
      // on-diagonal terms
      a0[0] = tri_mass*(9*c[0]*c[0] + p[0]*p[0] + q[0]*q[0] + r[0]*r[0])/12;
      a1[1] = tri_mass*(9*c[1]*c[1] + p[1]*p[1] + q[1]*q[1] + r[1]*r[1])/12;
      a2[2] = tri_mass*(9*c[2]*c[2] + p[2]*p[2] + q[2]*q[2] + r[2]*r[2])/12;
      
      // off-diagonal terms
      a0[1] = tri_mass*(9*c[0]*c[1] + p[0]*p[1] + q[0]*q[1] + r[0]*r[1])/12;
      a0[2] = tri_mass*(9*c[0]*c[2] + p[0]*p[2] + q[0]*q[2] + r[0]*r[2])/12;
      a1[2] = tri_mass*(9*c[1]*c[2] + p[1]*p[2] + q[1]*q[2] + r[1]*r[2])/12;

      if (!m_TestMode)
      {
        mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)(((double) i)/((double) ncells)*100.)));
      }

    } // end foreach triangle
  }// end foreach cell
  
  // matrix is symmetric
  a1[0] = a0[1];
  a2[0] = a0[2];
  a2[1] = a1[2];
  
  // by the spectral theorem, since the moment of inertia tensor is real and symmetric, there exists a Cartesian coordinate system in which it is diagonal,
  // the coordinate axes are called the principal axes and the constants Ixx, Iyy and Izz are called the principal moments of inertia. 
  // extract eigenvalues from jacobian matrix (inertial tensor components referred to principal axes)
  double eval[3];
  v[0] = v0; v[1] = v1; v[2] = v2; 
  vtkMath::Jacobi(a,eval,v);
  
  m_Ixx += eval[0];
  m_Iyy += eval[1];
  m_Iyy += eval[2];

  m_Mass += tot_mass;

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
    wait = new wxBusyInfo("Computing inertial tensor components...");
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_TEXT,&s));
  }
  
  // get dataset
  vtkDataSet* ds = surf->GetOutput()->GetVTKData();
  vtkGeometryFilter* gfilter = vtkGeometryFilter::New();
  gfilter->SetInput(ds);
  gfilter->Update();
  vtkPolyData* pd = gfilter->GetOutput();

  // get bounds
  double bounds[6];
  ds->GetBounds(bounds);
  
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
  for (int i=0; i<m_Accuracy; i++)
  {
    // get random point
    double x = vtkMath::Random(bounds[0],bounds[1]);
    double y = vtkMath::Random(bounds[2],bounds[3]);
    double z = vtkMath::Random(bounds[4],bounds[5]);
    
    double p[3];
    p[0] = x;
    p[1] = y;
    p[2] = z;

    // is it inside surface? (the algorithm assumes the surface is closed)
    if (IsInsideSurface(pd,p))
    {
      pint++;

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
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)(((double) i)/((double) m_Accuracy)*100.)));
    }
  }

  // matrix is symmetric
  ai1[0] = ai0[1];
  ai2[0] = ai0[2];
  ai2[1] = ai1[2];

  // get hypercube volume and mass
  double hycube_v = abs(bounds[1]-bounds[0]) * abs(bounds[3]-bounds[2]) * abs(bounds[5]-bounds[4]);
  double hycube_m = m_Density*hycube_v;

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
  
  // calculates mass
  m_Mass += hycube_m * poly_hycube_mratio;

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

  // perform eigenvalues evaluation: I(symmetric)=nIn
  double eval[3];  
  vtkMath::Jacobi(a,eval,v);

  m_Ixx += eval[0];
  m_Iyy += eval[1];
  m_Izz += eval[2];

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
    if (group->GetChild(i)->IsMAFType(mafVMESurface))
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
    if (group->GetChild(i)->IsMAFType(mafVMESurface))
    {
      ComputeInertialTensor(group->GetChild(i),i+1,n_of_children);
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