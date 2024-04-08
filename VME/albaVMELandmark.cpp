/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMELandmark
 Authors: Marco Petrone, Paolo Quadrani
 
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

#include "albaVMELandmark.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEOutputPointSet.h"
#include "albaSmartPointer.h"
#include "albaDataPipeCustom.h"
#include "albaOBB.h"
#include "albaIndent.h"
#include "albaTransform.h"
#include "albaGUI.h"
#include "mmaMaterial.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBADataPipe.h"
#include "vtkTransform.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkBitArray.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMELandmark);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMELandmark::albaVMELandmark()
//-------------------------------------------------------------------------
{
  SetOutput(albaVMEOutputPointSet::New()); // create the output

  vtkNEW(m_Polydata);
  vtkALBASmartPointer<vtkPoints> points;
  points->InsertNextPoint(0,0,0);
  m_Polydata->SetPoints(points);

  // add cellarray for visibility
  vtkALBASmartPointer<vtkCellArray> cells;
  m_Polydata->SetVerts(cells);

  // add scalars for visibility attribute
  vtkPointData* point_data = m_Polydata->GetPointData();
  vtkALBASmartPointer<vtkBitArray> scalars;
  scalars->SetNumberOfValues(1);
  point_data->SetScalars(scalars);

  // attach a datapipe which creates a bridge between VTK and ALBA
  albaDataPipeCustom *dpipe = albaDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);

  dpipe->GetVTKDataPipe()->SetNthInput(0, m_Polydata);
  m_Position[0] = m_Position[1] = m_Position[2] = "0.0";
}

//-------------------------------------------------------------------------
albaVMELandmark::~albaVMELandmark()
//-------------------------------------------------------------------------
{
  vtkDEL(m_Polydata);
}
//-------------------------------------------------------------------------
int albaVMELandmark::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    albaVMELandmark *lm = albaVMELandmark::SafeDownCast(a);
    SetRadius(lm->GetRadius());
    SetSphereResolution(lm->GetSphereResolution());
    SetLandmarkVisibility(lm->GetLandmarkVisibility());

    albaDataPipeCustom *dpipe = albaDataPipeCustom::SafeDownCast(GetDataPipe());

    if (dpipe)
      dpipe->SetInput(m_Polydata);

    return ALBA_OK;
  }  
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
bool albaVMELandmark::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    albaVMELandmark *lm = albaVMELandmark::SafeDownCast(vme);
    ret = (GetRadius() == lm->GetRadius() &&
           GetSphereResolution() == lm->GetSphereResolution() &&
           GetLandmarkVisibility() == lm->GetLandmarkVisibility());
  }
  return ret;
}

//-------------------------------------------------------------------------
int albaVMELandmark::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==ALBA_OK)
  {
    // force material allocation
    GetMaterial();

    return ALBA_OK;
  }

  return ALBA_ERROR;
}

//-----------------------------------------------------------------------
void albaVMELandmark::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  // update the scalar value according to landmark visibility
  m_Polydata->GetPointData()->GetScalars()->SetTuple1(0,GetLandmarkVisibility()?1:0);
  m_Polydata->Modified();
}

//-------------------------------------------------------------------------
bool albaVMELandmark::CanReparentTo(albaVME *parent)
//-------------------------------------------------------------------------
{
  if (albaVMELandmarkCloud *vlmc = albaVMELandmarkCloud::SafeDownCast(parent))
  {  
    if ( vlmc->GetLandmarkIndex(this->GetName())<0  )
      return true;
  }
  else if (parent == NULL)
  {
    return Superclass::CanReparentTo(parent);
  }

  return false;
}

//----------------------------------------------------------------------------
void albaVMELandmark::SetName(const char *name)
{
	albaVMELandmarkCloud *parent = albaVMELandmarkCloud::SafeDownCast(this->GetParent());
	
	if (!m_Name.Equals(name) && parent && parent->GetLandmarkIndex(name) >= 0)
	{
		albaWarningMessageMacro("There is already a landmatk with this name in the cloud\nOld name will be restored");
		Superclass::SetName(GetName());
	}
	else
		Superclass::SetName(name);
}

//-------------------------------------------------------------------------
void albaVMELandmark::SetRadius(double rad)
//-------------------------------------------------------------------------
{
  albaVMELandmarkCloud *parent = albaVMELandmarkCloud::SafeDownCast(this->GetParent());
  if (parent)
  {
    parent->SetRadius(rad);
  }
}

//-------------------------------------------------------------------------
double albaVMELandmark::GetRadius()
//-------------------------------------------------------------------------
{
  albaVMELandmarkCloud *parent = albaVMELandmarkCloud::SafeDownCast(this->GetParent());
  if (parent)
  {
    return parent->GetRadius();
  }
  else
  {
    return -1;
  }
}

//-------------------------------------------------------------------------
void albaVMELandmark::SetSphereResolution(int res)
//-------------------------------------------------------------------------
{
  albaVMELandmarkCloud *parent = albaVMELandmarkCloud::SafeDownCast(this->GetParent());
  if (parent)
  {
    parent->SetSphereResolution(res);
  }
}

//-------------------------------------------------------------------------
int albaVMELandmark::GetSphereResolution()
//-------------------------------------------------------------------------
{
  albaVMELandmarkCloud *parent = albaVMELandmarkCloud::SafeDownCast(this->GetParent());
  if (parent)
  {
    return parent->GetSphereResolution();
  }
  else
  {
    return -1;
  }
}

//-------------------------------------------------------------------------
void albaVMELandmark::SetMatrix(albaMatrix &mat)
//-------------------------------------------------------------------------
{
  bool a = this->GetLandmarkVisibility(mat.GetTimeStamp());
  double pos[3];
  albaTransform::GetPosition(mat,pos);
  mat.Identity();
  mat.GetElements()[0][0]=a;
  mat.GetElements()[1][1]=a;
  mat.GetElements()[2][2]=a;
  albaTransform::SetPosition(mat,pos);
  this->Superclass::SetMatrix(mat);
}

//----------------------------------------------------------------------------
void albaVMELandmark::SetAbsMatrix(const albaMatrix &matrix)
{
	Superclass::SetAbsMatrix(matrix);

	albaVMELandmarkCloud *parentCloud = albaVMELandmarkCloud::SafeDownCast(m_Parent);
	if (parentCloud)
	{
		double pose[3],rPose[3];
		GetOutput()->GetPose(pose, rPose);
		parentCloud->SetLandmark(this, pose[0], pose[1], pose[2], m_CurrentTime);
	}
	
}

//-------------------------------------------------------------------------
int albaVMELandmark::SetPoint(double x,double y,double z,albaTimeStamp t)
//-------------------------------------------------------------------------
{
  t = t < 0 ? this->m_CurrentTime : t;
  this->SetPose(x,y,z,0,0,0,t);
  return ALBA_OK;
}

//-------------------------------------------------------------------------
int albaVMELandmark::SetPoint(double xyz[3],albaTimeStamp t)
//-------------------------------------------------------------------------
{
  t = t < 0 ? this->m_CurrentTime : t;
  this->SetPose(xyz[0],xyz[1],xyz[2],0,0,0,t);
  return ALBA_OK;
}

//-------------------------------------------------------------------------
int albaVMELandmark::GetPoint(double &x,double &y,double &z,albaTimeStamp t)
//-------------------------------------------------------------------------
{
  t = t < 0 ? this->m_CurrentTime : t;
  
  double ori[3];
  this->GetOutput()->GetAbsPose(x,y,z,ori[0],ori[1],ori[2],t);
  return ALBA_OK;
}

//-------------------------------------------------------------------------
int albaVMELandmark::GetPoint(double xyz[3],albaTimeStamp t)
//-------------------------------------------------------------------------
{
  t=t<0?this->m_CurrentTime:t;
  double ori[3];
  this->GetOutput()->GetAbsPose(xyz,ori,t);
  return ALBA_OK;
}

//-------------------------------------------------------------------------
int albaVMELandmark::SetLandmarkVisibility(bool a,albaTimeStamp t)
//-------------------------------------------------------------------------
{
  if (this->GetLandmarkVisibility(t)!=a)
  {
    albaMatrix mat;
    this->GetOutput()->GetMatrix(mat, t);
    mat.GetElements()[0][0]=a;   // Paolo 04-04-05
    mat.GetElements()[1][1]=a;   // added 'a' instead of '0'
    mat.GetElements()[2][2]=a;
    this->Superclass::SetMatrix(mat); // Call Superclass::SetMatrix(mat) instead SetMatrix(mat) to avoid visibility overwriting
  }
  return ALBA_OK;
}
//-------------------------------------------------------------------------
bool albaVMELandmark::GetLandmarkVisibility(albaTimeStamp t)
//-------------------------------------------------------------------------
{
  albaMatrix mat;
  this->GetOutput()->GetMatrix(mat,t);

  //if (mat->GetElements()[0][0]!=mat->GetElements()[1][1]||mat->GetElements()[0][0]!=mat->GetElements()[2][2]) // DEBUG Test
  //  vtkErrorMacro("GetLandmarkVisibility: corrupted visibility information for landmark "<<this->GetName()<<" a time "<<t);

  bool ret = mat.GetElements()[0][0] != 0;
  return ret;
}
/*//-------------------------------------------------------------------------
void albaVMELandmark::GetVMELocalSpaceBounds(mflBounds &bounds,albaTimeStamp t, mflVMEIterator *iter)
//-------------------------------------------------------------------------
{
  if (t<0)
    t=this->m_CurrentTime;

  if (this->GetLandmarkVisibility(t))
  {
    double radius=this->GetRadius();
    for (int i=0;i<3;i++)
    {
      bounds.Bounds[2*i]=-radius;
      bounds.Bounds[2*i+1]=+radius;
    }
  }
  else
  {
    bounds.Reset();
  }
}
*/
//-------------------------------------------------------------------------
void albaVMELandmark::Print(std::ostream &os, const int tabs)
//-------------------------------------------------------------------------
{
  albaIndent indent(tabs);
  this->Superclass::Print(os, indent);
  
  double x,y,z,rx,ry,rz;
  this->GetOutput()->GetPose(x,y,z,rx,ry,rz,this->m_CurrentTime);

  os << indent << "Current Landmark State: (" \
    << x <<", "<< y <<", "<< z <<") Visibility = " \
    << this->GetLandmarkVisibility(this->m_CurrentTime)<<std::endl;
}
//-------------------------------------------------------------------------
albaGUI* albaVMELandmark::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  
	double xyz[3],rxyz[3];
  this->GetOutput()->GetAbsPose(xyz,rxyz);
  m_Gui->Label("Abs pose:", true);
  m_Position[0] = albaString::Format("%f",xyz[0]);
  m_Position[1] = albaString::Format("%f",xyz[1]);
  m_Position[2] = albaString::Format("%f",xyz[2]);

  m_Gui->Label("x:", &m_Position[0]);
  m_Gui->Label("y:", &m_Position[1]);
  m_Gui->Label("z:", &m_Position[2]);
  m_Gui->Divider();
	
	return m_Gui;
}

//-------------------------------------------------------------------------
mmaMaterial *albaVMELandmark::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material;
  if (m_Parent)
  {
    material = (mmaMaterial *)m_Parent->GetAttribute("MaterialAttributes");
  }
  else
  {
    material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  }
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
  }
  if (m_Output)
  {
    ((albaVMEOutputPointSet *)m_Output)->SetMaterial(material);
  }
  return material;
}
//-------------------------------------------------------------------------
char** albaVMELandmark::GetIcon() 
//-------------------------------------------------------------------------
{
#include "albaVMELandmark.xpm"
  return albaVMELandmark_xpm;
}
//-------------------------------------------------------------------------
void albaVMELandmark::SetTimeStamp(albaTimeStamp t)
//-------------------------------------------------------------------------
{
  Superclass::SetTimeStamp(t);
  this->GetOutput()->Update();
  double xyz[3],rxyz[3];
  this->GetOutput()->GetAbsPose(xyz,rxyz);  
  m_Position[0] = albaString::Format("x: %f",xyz[0]);
  m_Position[1] = albaString::Format("y: %f",xyz[1]);
  m_Position[2] = albaString::Format("z: %f",xyz[2]);
  if(m_Gui)
    m_Gui->Update();
}
//-------------------------------------------------------------------------
void albaVMELandmark::InternalUpdate()
//-------------------------------------------------------------------------
{
  double xyz[3],rxyz[3];
  this->GetOutput()->GetAbsPose(xyz,rxyz);

  m_Position[0] = albaString::Format("x: %f",xyz[0]);
  m_Position[1] = albaString::Format("y: %f",xyz[1]);
  m_Position[2] = albaString::Format("z: %f",xyz[2]);

  if(m_Gui)
    m_Gui->Update();
}
