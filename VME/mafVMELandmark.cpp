/*=========================================================================

 Program: MAF2
 Module: mafVMELandmark
 Authors: Marco Petrone, Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEOutputPointSet.h"
#include "mafSmartPointer.h"
#include "mafDataPipeCustom.h"
#include "mafOBB.h"
#include "mafIndent.h"
#include "mafTransform.h"
#include "mafGUI.h"
#include "mmaMaterial.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFDataPipe.h"
#include "vtkTransform.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkBitArray.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMELandmark);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMELandmark::mafVMELandmark()
//-------------------------------------------------------------------------
{
  SetOutput(mafVMEOutputPointSet::New()); // create the output

  vtkNEW(m_Polydata);
  vtkMAFSmartPointer<vtkPoints> points;
  points->InsertNextPoint(0,0,0);
  m_Polydata->SetPoints(points);

  // add cellarray for visibility
  vtkMAFSmartPointer<vtkCellArray> cells;
  m_Polydata->SetVerts(cells);

  // add scalars for visibility attribute
  vtkPointData* point_data = m_Polydata->GetPointData();
  vtkMAFSmartPointer<vtkBitArray> scalars;
  scalars->SetNumberOfValues(1);
  point_data->SetScalars(scalars);

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);

  dpipe->GetVTKDataPipe()->SetNthInput(0, m_Polydata);
  m_Position[0] = m_Position[1] = m_Position[2] = "0.0";
}

//-------------------------------------------------------------------------
mafVMELandmark::~mafVMELandmark()
//-------------------------------------------------------------------------
{
  vtkDEL(m_Polydata);
}
//-------------------------------------------------------------------------
int mafVMELandmark::DeepCopy(mafVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMELandmark *lm = mafVMELandmark::SafeDownCast(a);
    SetRadius(lm->GetRadius());
    SetSphereResolution(lm->GetSphereResolution());
    SetLandmarkVisibility(lm->GetLandmarkVisibility());

    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_Polydata);
      m_Polydata->Update();
    }

    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMELandmark::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    mafVMELandmark *lm = mafVMELandmark::SafeDownCast(vme);
    ret = (GetRadius() == lm->GetRadius() &&
           GetSphereResolution() == lm->GetSphereResolution() &&
           GetLandmarkVisibility() == lm->GetLandmarkVisibility());
  }
  return ret;
}

//-------------------------------------------------------------------------
int mafVMELandmark::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==MAF_OK)
  {
    // force material allocation
    GetMaterial();

    return MAF_OK;
  }

  return MAF_ERROR;
}

//-----------------------------------------------------------------------
void mafVMELandmark::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  // update the scalar value according to landmark visibility
  m_Polydata->GetPointData()->GetScalars()->SetTuple1(0,GetLandmarkVisibility()?1:0);
  m_Polydata->Modified();
}

//-------------------------------------------------------------------------
bool mafVMELandmark::CanReparentTo(mafVME *parent)
//-------------------------------------------------------------------------
{
  if (mafVMELandmarkCloud *vlmc = mafVMELandmarkCloud::SafeDownCast(parent))
  {  
    if ( vlmc->FindLandmarkIndex(this->GetName())<0  )
      return true;
  }
  else if (parent == NULL)
  {
    return Superclass::CanReparentTo(parent);
  }

  return false;
}

//-------------------------------------------------------------------------
void mafVMELandmark::SetRadius(double rad)
//-------------------------------------------------------------------------
{
  mafVMELandmarkCloud *parent = mafVMELandmarkCloud::SafeDownCast(this->GetParent());
  if (parent)
  {
    parent->SetRadius(rad);
  }
}

//-------------------------------------------------------------------------
double mafVMELandmark::GetRadius()
//-------------------------------------------------------------------------
{
  mafVMELandmarkCloud *parent = mafVMELandmarkCloud::SafeDownCast(this->GetParent());
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
void mafVMELandmark::SetSphereResolution(int res)
//-------------------------------------------------------------------------
{
  mafVMELandmarkCloud *parent = mafVMELandmarkCloud::SafeDownCast(this->GetParent());
  if (parent)
  {
    parent->SetSphereResolution(res);
  }
}

//-------------------------------------------------------------------------
int mafVMELandmark::GetSphereResolution()
//-------------------------------------------------------------------------
{
  mafVMELandmarkCloud *parent = mafVMELandmarkCloud::SafeDownCast(this->GetParent());
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
void mafVMELandmark::SetMatrix(mafMatrix &mat)
//-------------------------------------------------------------------------
{
  bool a = this->GetLandmarkVisibility(mat.GetTimeStamp());
  double pos[3];
  mafTransform::GetPosition(mat,pos);
  mat.Identity();
  mat.GetElements()[0][0]=a;
  mat.GetElements()[1][1]=a;
  mat.GetElements()[2][2]=a;
  mafTransform::SetPosition(mat,pos);
  this->Superclass::SetMatrix(mat);
}

//----------------------------------------------------------------------------
void mafVMELandmark::SetAbsMatrix(const mafMatrix &matrix)
{
	Superclass::SetAbsMatrix(matrix);

	mafVMELandmarkCloud *parentCloud = mafVMELandmarkCloud::SafeDownCast(m_Parent);
	if (parentCloud)
	{
		double pose[3];
		GetPoint(pose);
		parentCloud->SetLandmark(this, pose[0], pose[1], pose[2], m_CurrentTime);
	}
	
}

//-------------------------------------------------------------------------
int mafVMELandmark::SetPoint(double x,double y,double z,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t = t < 0 ? this->m_CurrentTime : t;
  this->SetPose(x,y,z,0,0,0,t);
  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafVMELandmark::SetPoint(double xyz[3],mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t = t < 0 ? this->m_CurrentTime : t;
  this->SetPose(xyz[0],xyz[1],xyz[2],0,0,0,t);
  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafVMELandmark::GetPoint(double &x,double &y,double &z,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t = t < 0 ? this->m_CurrentTime : t;
  
  double ori[3];
  this->GetOutput()->GetPose(x,y,z,ori[0],ori[1],ori[2],t);
  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafVMELandmark::GetPoint(double xyz[3],mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t=t<0?this->m_CurrentTime:t;
  double ori[3];
  this->GetOutput()->GetPose(xyz,ori,t);
  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafVMELandmark::SetLandmarkVisibility(bool a,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  if (this->GetLandmarkVisibility(t)!=a)
  {
    mafMatrix mat;
    this->GetOutput()->GetMatrix(mat, t);
    mat.GetElements()[0][0]=a;   // Paolo 04-04-05
    mat.GetElements()[1][1]=a;   // added 'a' instead of '0'
    mat.GetElements()[2][2]=a;
    this->Superclass::SetMatrix(mat); // Call Superclass::SetMatrix(mat) instead SetMatrix(mat) to avoid visibility overwriting
  }
  return MAF_OK;
}
//-------------------------------------------------------------------------
bool mafVMELandmark::GetLandmarkVisibility(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  mafMatrix mat;
  this->GetOutput()->GetMatrix(mat,t);

  //if (mat->GetElements()[0][0]!=mat->GetElements()[1][1]||mat->GetElements()[0][0]!=mat->GetElements()[2][2]) // DEBUG Test
  //  vtkErrorMacro("GetLandmarkVisibility: corrupted visibility information for landmark "<<this->GetName()<<" a time "<<t);

  bool ret = mat.GetElements()[0][0] != 0;
  return ret;
}
/*//-------------------------------------------------------------------------
void mafVMELandmark::GetVMELocalSpaceBounds(mflBounds &bounds,mafTimeStamp t, mflVMEIterator *iter)
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
void mafVMELandmark::Print(std::ostream &os, const int tabs)
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);
  this->Superclass::Print(os, indent);
  
  double x,y,z,rx,ry,rz;
  this->GetOutput()->GetPose(x,y,z,rx,ry,rz,this->m_CurrentTime);

  os << indent << "Current Landmark State: (" \
    << x <<", "<< y <<", "<< z <<") Visibility = " \
    << this->GetLandmarkVisibility(this->m_CurrentTime)<<std::endl;
}
//-------------------------------------------------------------------------
mafGUI* mafVMELandmark::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafVME::CreateGui(); // Called to show info about vmes' type and name
  double xyz[3],rxyz[3];
  this->GetOutput()->GetAbsPose(xyz,rxyz);
  m_Gui->Label("abs pose:");
  m_Position[0] = wxString::Format("x: %f",xyz[0]);
  m_Position[1] = wxString::Format("y: %f",xyz[1]);
  m_Position[2] = wxString::Format("z: %f",xyz[2]);
  m_Gui->Label("", &m_Position[0]);
  m_Gui->Label("", &m_Position[1]);
  m_Gui->Label("", &m_Position[2]);
  m_Gui->Divider();
	return m_Gui;
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMELandmark::GetMaterial()
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
    ((mafVMEOutputPointSet *)m_Output)->SetMaterial(material);
  }
  return material;
}
//-------------------------------------------------------------------------
char** mafVMELandmark::GetIcon() 
//-------------------------------------------------------------------------
{
#include "mafVMELandmark.xpm"
  return mafVMELandmark_xpm;
}
//-------------------------------------------------------------------------
void mafVMELandmark::SetTimeStamp(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  Superclass::SetTimeStamp(t);
  this->GetOutput()->Update();
  double xyz[3],rxyz[3];
  this->GetOutput()->GetAbsPose(xyz,rxyz);  
  m_Position[0] = wxString::Format("x: %f",xyz[0]);
  m_Position[1] = wxString::Format("y: %f",xyz[1]);
  m_Position[2] = wxString::Format("z: %f",xyz[2]);
  if(m_Gui)
    m_Gui->Update();
}
//-------------------------------------------------------------------------
void mafVMELandmark::InternalUpdate()
//-------------------------------------------------------------------------
{
  double xyz[3],rxyz[3];
  this->GetOutput()->GetAbsPose(xyz,rxyz);

  m_Position[0] = wxString::Format("x: %f",xyz[0]);
  m_Position[1] = wxString::Format("y: %f",xyz[1]);
  m_Position[2] = wxString::Format("z: %f",xyz[2]);

  if(m_Gui)
    m_Gui->Update();
}
