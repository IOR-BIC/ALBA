/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpComputeInertialTensor
 Authors: Simone Brazzale , Stefano Perticoni
 
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

wxString DENSITY_TAG_NAME = "DENSITY";

using namespace std;

#include "albaOpComputeInertialTensor.h"

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
#include "vtkALBACellLocator.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkALBASmartPointer.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"


#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaVMEGroup.h"
#include "albaVMESurface.h"
#include "albaVMEOutput.h"
#include "albaTagArray.h"
#include "albaProgressBarHelper.h"


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

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpComputeInertialTensor);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpComputeInertialTensor::albaOpComputeInertialTensor(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

  m_DefaultDensity = 1.0;
  m_Mass = 0.0;
  m_Principal_I1 = 0.0;
  m_Principal_I2 = 0.0;
  m_Principal_I3 = 0.0;
  m_Accuracy = 1000;
  
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

}
//----------------------------------------------------------------------------
albaOpComputeInertialTensor::~albaOpComputeInertialTensor( ) 
//----------------------------------------------------------------------------
{
	m_LocalInertiaTensors.clear();
	m_NodeMassPairVector.clear();
}
//----------------------------------------------------------------------------
albaOp* albaOpComputeInertialTensor::Copy()   
//----------------------------------------------------------------------------
{
	 albaOpComputeInertialTensor* op_copy = new albaOpComputeInertialTensor(m_Label);
   op_copy->m_DefaultDensity = this->m_DefaultDensity;

   return op_copy;
}
//----------------------------------------------------------------------------
bool albaOpComputeInertialTensor::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return ( (node && node->IsALBAType(albaVMESurface)) || (node && node->IsALBAType(albaVMEGroup)) );
}
//----------------------------------------------------------------------------
void albaOpComputeInertialTensor::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
  ShowGui();
}
//----------------------------------------------------------------------------
void albaOpComputeInertialTensor::OpDo()
//----------------------------------------------------------------------------
{
  if (!m_Input->GetTagArray()->IsTagPresent("LOCAL_CENTER_OF_MASS_COMPONENTS"))
  {
	  if (m_LocalCenterOfMassTag.GetNumberOfComponents()>0)
	  {
		  m_Input->GetTagArray()->SetTag(m_LocalCenterOfMassTag);
	  }
  }

  if (!m_Input->GetTagArray()->IsTagPresent("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS"))
  {
    if (m_PrincipalInertialTensorTag.GetNumberOfComponents()>0)
    {
      m_Input->GetTagArray()->SetTag(m_PrincipalInertialTensorTag);
    }
  }

  if (!m_Input->GetTagArray()->IsTagPresent("INERTIAL_TENSOR_COMPONENTS"))
  {
	  if (m_InertialTensorTag.GetNumberOfComponents()>0)
	  {
		  m_Input->GetTagArray()->SetTag(m_InertialTensorTag);
	  }
  }
  
  if (!m_Input->GetTagArray()->IsTagPresent("SURFACE_MASS"))
  {
    if (m_TagMass.GetNumberOfComponents()>0)
    {
      m_Input->GetTagArray()->SetTag(m_TagMass);
    }
  }

}
//----------------------------------------------------------------------------
void albaOpComputeInertialTensor::OpUndo()
//----------------------------------------------------------------------------
{
  m_Input->GetTagArray()->GetTag("LOCAL_CENTER_OF_MASS_COMPONENTS",m_LocalCenterOfMassTag);
  m_Input->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",m_PrincipalInertialTensorTag);
  m_Input->GetTagArray()->GetTag("INERTIAL_TENSOR_COMPONENTS",m_InertialTensorTag);
  m_Input->GetTagArray()->GetTag("SURFACE_MASS",m_TagMass);

  m_Input->GetTagArray()->DeleteTag("LOCAL_CENTER_OF_MASS_COMPONENTS");
  m_Input->GetTagArray()->DeleteTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS");
  m_Input->GetTagArray()->DeleteTag("INERTIAL_TENSOR_COMPONENTS");
  m_Input->GetTagArray()->DeleteTag("SURFACE_MASS");
}
//----------------------------------------------------------------------------
void albaOpComputeInertialTensor::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId())
		{

		case wxOK:
      {
        int result = OP_RUN_CANCEL;

        if (m_Input->IsALBAType(albaVMESurface))
        {
          result = ComputeInertialTensor(m_Input);
        }
        else if (m_Input->IsALBAType(albaVMEGroup))
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
      albaEventMacro(*e);
      break;
		}
	}
}
//----------------------------------------------------------------------------
void albaOpComputeInertialTensor::AddAttributes()
//----------------------------------------------------------------------------
{
	std::vector<double> localCenterOfMassComponents;
	localCenterOfMassComponents.push_back(m_CenterOfMass[0]);
	localCenterOfMassComponents.push_back(m_CenterOfMass[1]);
	localCenterOfMassComponents.push_back(m_CenterOfMass[2]);

	albaTagItem tagLocalCenterOfMass;
	tagLocalCenterOfMass.SetName("LOCAL_CENTER_OF_MASS_COMPONENTS");
	tagLocalCenterOfMass.SetNumberOfComponents(localCenterOfMassComponents.size());
	tagLocalCenterOfMass.SetComponents(localCenterOfMassComponents);
	m_Input->GetTagArray()->SetTag(tagLocalCenterOfMass);

	std::vector<double> principalInertialTensorComponents;
	principalInertialTensorComponents.push_back(m_Principal_I1);
	principalInertialTensorComponents.push_back(m_Principal_I2);
	principalInertialTensorComponents.push_back(m_Principal_I3);

	albaTagItem tagPrincipalInertialTensor;
	tagPrincipalInertialTensor.SetName("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS");
	tagPrincipalInertialTensor.SetNumberOfComponents(principalInertialTensorComponents.size());
	tagPrincipalInertialTensor.SetComponents(principalInertialTensorComponents);
	m_Input->GetTagArray()->SetTag(tagPrincipalInertialTensor);

	std::vector<double> inertialTensorComponents;
	for (int i = 0 ; i < 9 ; i++)
	{
		inertialTensorComponents.push_back(m_InertialTensor[i]);
	}

	albaTagItem tagInertialTensor;
	tagInertialTensor.SetName("INERTIAL_TENSOR_COMPONENTS");
	tagInertialTensor.SetNumberOfComponents(inertialTensorComponents.size());
	tagInertialTensor.SetComponents(inertialTensorComponents);
	m_Input->GetTagArray()->SetTag(tagInertialTensor);

	albaTagItem tagMass;
	tagMass.SetName("SURFACE_MASS");
	tagMass.SetValue(m_Mass);
	m_Input->GetTagArray()->SetTag(tagMass);

	if (m_Input->IsA("albaVMEGroup")) 
	{
		vector<pair<albaVME * , double>>::iterator iter;

		for (iter = m_NodeMassPairVector.begin() ; iter != m_NodeMassPairVector.end() ; ++iter)
		{
			albaVME *node = iter->first;
			double mass = iter->second;

			albaTagItem ti;
			ti.SetName("SURFACE_MASS");
			ti.SetValue(mass);

			node->GetTagArray()->SetTag(ti);
		}
	}
}
//----------------------------------------------------------------------------
void albaOpComputeInertialTensor::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  albaEventMacro(albaEvent(this,result)); 
}
//----------------------------------------------------------------------------
void albaOpComputeInertialTensor::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);

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
	  
  m_Gui->OkCancel();
}


//----------------------------------------------------------------------------
int albaOpComputeInertialTensor::ComputeLocalInertialTensor(albaVME* node, int current_node, int n_of_nodes)
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

  if (!node->IsALBAType(albaVMESurface))
  {
    return OP_RUN_CANCEL;
  }
   
	// get surface
	albaVMESurface* surf = (albaVMESurface*) node;
	if (surf->GetOutput() == NULL || surf->GetOutput()->GetVTKData() == NULL)
		return OP_RUN_CANCEL;
	surf->GetOutput()->Update();
	surf->GetOutput()->GetVTKData()->Update();
	
	vtkALBASmartPointer<vtkTransformPolyDataFilter> tranformFilter;
  tranformFilter->SetInput((vtkPolyData *)surf->GetOutput()->GetVTKData());
  tranformFilter->SetTransform(surf->GetOutput()->GetAbsTransform()->GetVTKTransform());
  tranformFilter->Update();

	// get dataset
	vtkDataSet* ds = tranformFilter->GetOutput();

  int npoints = ds->GetNumberOfPoints();
  int ncells = ds->GetNumberOfCells();

  if (ncells==0)
  {
    return OP_RUN_CANCEL;
  }

  // wx stuff
  wxString str("Computing inertial tensor: surface ");
  str << current_node << "/" << n_of_nodes;

	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Computing inertial tensor components...");
	progressHelper.SetBarText(str);

  // initialize variables
  int pId, qId, rId;
  double p[3],q[3],r[3];

	LocalInertiaTensor lit; 
  
  lit._xx=0; lit._yy=0; lit._zz=0;
  lit._yx=0; lit._zx=0; lit._zy=0;
  lit._Cx=0; lit._Cy=0; lit._Cz=0;
  lit._m=0;
	lit._node=node;
  
 
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
      double vol = x1*y2*z3 + y1*z2*x3 + x2*y3*z1 -
                  (x3*y2*z1 + x2*y1*z3 + y3*z2*x1);
        
      // Contribution to the mass
      lit._m += vol;

      // Contribution to the centroid
      double x4 = x1 + x2 + x3;           
      lit._Cx += (vol * x4);
      double y4 = y1 + y2 + y3;           
      lit._Cy += (vol * y4);
      double z4 = z1 + z2 + z3;           
      lit._Cz += (vol * z4);

      // Contribution to moment of inertia 
      lit._xx += vol * (x1*x1 + x2*x2 + x3*x3 + x4*x4);
      lit._yy += vol * (y1*y1 + y2*y2 + y3*y3 + y4*y4);
      lit._zz += vol * (z1*z1 + z2*z2 + z3*z3 + z4*z4);
      lit._yx += vol * (y1*x1 + y2*x2 + y3*x3 + y4*x4);
      lit._zx += vol * (z1*x1 + z2*x2 + z3*x3 + z4*x4);
      lit._zy += vol * (z1*y1 + z2*y2 + z3*y3 + z4*y4);

			progressHelper.UpdateProgressBar(((double) i)/((double) ncells)*100.);
    } // end foreach triangle
  }// end foreach cell

	lit._density = GetDensity(node);

	if (lit._density == DENSITY_NOT_FOUND)
	{

		lit._density = m_DefaultDensity;

		std::ostringstream stringStream;
		stringStream << DENSITY_TAG_NAME.c_str() << " tag not found. Using default density value ie " << lit._density << std::endl;          			
		albaLogMessage(stringStream.str().c_str());

	}
	else
	{
		std::ostringstream stringStream;
		stringStream << DENSITY_TAG_NAME.c_str() << " tag found. Using value " << lit._density << std::endl;          			
		albaLogMessage(stringStream.str().c_str());
	}

	m_LocalInertiaTensors.push_back(lit);

	return OP_RUN_OK;
}
//----------------------------------------------------------------------------
void albaOpComputeInertialTensor::ComputeGlobalInertiaTensor()
//----------------------------------------------------------------------------
{
	double *a[3], a0[3], a1[3], a2[3], *v[3], v0[3], v1[3], v2[3];
	a[0] = a0; a[1] = a1; a[2] = a2;
	for (int i=0; i<3; i++ )
	{
		a0[i] = a1[i] = a2[i] = 0.0;
	}

	//Calculating Center Of Mass
	for (int i=0;i<m_LocalInertiaTensors.size();i++)
	{
		LocalInertiaTensor lit=m_LocalInertiaTensors[i];
		
		// Centroid.  
		// The case _m = 0 needs to be addressed here.
		double rr = 1.0 / (4 * lit._m);
		double Cx = lit._Cx * rr;
		double Cy = lit._Cy * rr;
		double Cz = lit._Cz * rr;

		// Mass
		double m = lit._m / 6;

		// store the mass for later use
		double mass = lit._density * m;

		m_CenterOfMass[0] += Cx * mass;
		m_CenterOfMass[1] += Cy * mass;
		m_CenterOfMass[2] += Cz * mass;

		pair<albaVME* , double> nodeMassPair(lit._node , mass);
		m_NodeMassPairVector.push_back(nodeMassPair);

		m_Mass += mass;
	}
  
	//Rescaling Center Of Mass
	m_CenterOfMass[0] /= m_Mass; 
	m_CenterOfMass[1] /= m_Mass;
	m_CenterOfMass[2] /= m_Mass;

	//simple address global center of mass
	double gCx=m_CenterOfMass[0];
	double gCy=m_CenterOfMass[1];
	double gCz=m_CenterOfMass[2];

	//Calculating Global Inertia Tensor
	for (int i=0;i<m_LocalInertiaTensors.size();i++)
	{
		LocalInertiaTensor lit=m_LocalInertiaTensors[i];

		// Mass
		double m = lit._m / 6;

		// Moment of inertia about the center of mass.
		double rr = 1.0 / 120;
		double Iyx = lit._yx * rr - m * gCy*gCx;
		double Izx = lit._zx * rr - m * gCz*gCx;
		double Izy = lit._zy * rr - m * gCz*gCy;

		double _xx = lit._xx * rr - m * gCx*gCx;
		double _yy = lit._yy * rr - m * gCy*gCy;
		double _zz = lit._zz * rr - m * gCz*gCz;

		double Ixx = _yy + _zz;
		double Iyy = _zz + _xx;
		double Izz = _xx + _yy;  

		// Fill matrix 
		a0[0] = Ixx; a0[1] = Iyx; a0[2] = Izx;
		a1[0] = Iyx; a1[1] = Iyy; a1[2] = Izy;
		a2[0] = Izx; a2[1] = Izy; a2[2] = Izz;


		// by the spectral theorem, since the moment of inertia tensor is real and symmetric, there exists a Cartesian coordinate system in which it is diagonal,
		// the coordinate axes are called the principal axes and the constants I1, I2 and I3 are called the principal moments of inertia. 
		// extract eigenvalues from jacobian matrix (inertial tensor components referred to principal axes).
		double eval[3];
		v[0] = v0; v[1] = v1; v[2] = v2; 
		vtkMath::Jacobi(a,eval,v);

		// scale by the density
		double scale = lit._density;

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
		m_Principal_I1 += scale*eval[0];
		m_Principal_I2 += scale*eval[1];
		m_Principal_I3 += scale*eval[2];
	}
}

//----------------------------------------------------------------------------
int albaOpComputeInertialTensor::ComputeInertialTensorFromGroup()
//----------------------------------------------------------------------------
{
  albaVMEGroup* group = (albaVMEGroup*) m_Input;

  int result = OP_RUN_CANCEL;

  int n_of_children = group->GetNumberOfChildren();
  int n_of_surfaces = 0;
  
  // get number of children surfaces (only direct child!)
  for (int i=0;i<n_of_children;i++)
  {
    if (group->GetChild(i)->IsA("albaVMESurface"))
    {
      n_of_surfaces++;
    }
  }

  if (n_of_surfaces>0) 
  {
    wxString s;
    s << "Found " << n_of_surfaces << " surfaces: applying operation on all of them ..";
    albaLogMessage(s.c_str());
  }
  else
  {
    albaLogMessage("no surfaces found in the group. Quit!");
    return result;
  }

  // compute inertial tensor fro each children (results will be summed)
  for (int i=0;i<n_of_children;i++)
  {
	albaVMESurface *childSurface = albaVMESurface::SafeDownCast(group->GetChild(i));

    if (childSurface != NULL)
    {
	  
	  wxString s;
	  s << "Computing Inertial tensor for: " << childSurface->GetName();
	  albaLogMessage(s.c_str());      
	  ComputeLocalInertialTensor(childSurface,i+1,n_of_surfaces);
    }
  }

	//merge local calculation to the global inertia tensor
	ComputeGlobalInertiaTensor();

  result = OP_RUN_OK;

  return result;
}

//----------------------------------------------------------------------------
double albaOpComputeInertialTensor::GetDensity( albaVME* node)
//----------------------------------------------------------------------------
{
	double density = DENSITY_NOT_FOUND;

	albaTagItem *densityTagItem = NULL;
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

//----------------------------------------------------------------------------
double albaOpComputeInertialTensor::GetMass( albaVME* node)
//----------------------------------------------------------------------------
{
	double mass = SURFACE_MASS_NOT_FOUND;

	wxString massTagName = "SURFACE_MASS";

	albaTagItem *massTagItem = NULL;
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

//----------------------------------------------------------------------------
double albaOpComputeInertialTensor::GetDefaultDensity()
//----------------------------------------------------------------------------
{
	return m_DefaultDensity;
}

//----------------------------------------------------------------------------
void albaOpComputeInertialTensor::SetDefaultDensity( double val )
//----------------------------------------------------------------------------
{
	m_DefaultDensity = val;
}

//----------------------------------------------------------------------------
int albaOpComputeInertialTensor::ComputeInertialTensor(albaVME* node)
//----------------------------------------------------------------------------
{
	 int result = ComputeLocalInertialTensor(node);
	 if (result!=OP_RUN_CANCEL)
	 {
		 ComputeGlobalInertiaTensor();
	 }
	 return result;
}
