/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMERefSys.cpp,v $
  Language:  C++
  Date:      $Date: 2007-01-26 09:10:29 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone, Paolo Quadrani
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


#include "mafVMERefSys.h"
#include "mafTransform.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"
#include "mafTagArray.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mmaMaterial.h"
#include "mmgGui.h"
#include "mafVMELandmarkCloud.h"
#include "mafMatrix.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFDataPipe.h"
#include "vtkPoints.h"
#include "vtkGlyph3D.h"
#include "vtkPolyData.h"
#include "vtkArrowSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAppendPolyData.h"
#include "vtkPointData.h"
#include "vtkLineSource.h"
#include "vtkUnsignedCharArray.h"
#include "vtkAxes.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMERefSys)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMERefSys::mafVMERefSys()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  SetDataPipe(dpipe);

	DependsOnLinkedNodeOn();

	m_Radio = 0;
  m_ScaleFactor = 1.0;

	m_VMEAccept = new mafVMEAccept();

  vtkUnsignedCharArray *data;
  float scalar_red[3]   = {255,0,0};
  float scalar_green[3] = {0,255,0};
  float scalar_blu[3]   = {0,0,255};

  m_XArrow = vtkArrowSource::New();
  m_XArrow->SetShaftRadius(m_XArrow->GetTipRadius()/5);
  m_XArrow->SetTipResolution(40);
  m_XArrow->SetTipRadius(m_XArrow->GetTipRadius()/2);
  m_XArrow->Update();

  m_XAxisTransform = vtkTransform::New();
  m_XAxisTransform->PostMultiply();
  m_XAxisTransform->Update();

  m_XAxis = vtkTransformPolyDataFilter::New();
  m_XAxis->SetInput(m_XArrow->GetOutput());
  m_XAxis->SetTransform(m_XAxisTransform);
  m_XAxis->Update();	

  int points = m_XArrow->GetOutput()->GetNumberOfPoints();  

  m_YArrow = vtkArrowSource::New();
  m_YArrow->SetShaftRadius(m_YArrow->GetTipRadius() / 5);
  m_YArrow->SetTipResolution(40);
  m_YArrow->SetTipRadius(m_YArrow->GetTipRadius() / 2);

  m_YAxisTransform = vtkTransform::New();
  m_YAxisTransform->PostMultiply();
  m_YAxisTransform->RotateZ(90);
  m_YAxisTransform->Update();

  m_YAxis = vtkTransformPolyDataFilter::New();
  m_YAxis->SetInput(m_YArrow->GetOutput());
  m_YAxis->SetTransform(m_YAxisTransform);
  m_YAxis->Update();

  m_ZArrow = vtkArrowSource::New();
  m_ZArrow->SetShaftRadius(m_ZArrow->GetTipRadius() / 5);
  m_ZArrow->SetTipResolution(40);
  m_ZArrow->SetTipRadius(m_ZArrow->GetTipRadius() / 2);
  m_ZArrow->Update();

  m_ZAxisTransform = vtkTransform::New();
  m_ZAxisTransform->PostMultiply();
  m_ZAxisTransform->RotateY(-90);
  m_ZAxisTransform->Update();

  m_ZAxis  = vtkTransformPolyDataFilter::New();
  m_ZAxis->SetInput(m_ZArrow->GetOutput());
  m_ZAxis->SetTransform(m_ZAxisTransform);
  m_ZAxis->Update();

  data = vtkUnsignedCharArray::New();
  data->SetName("AXES");
  data->SetNumberOfComponents(3);
  data->SetNumberOfTuples(points * 3);
  int i;
  for (i = 0; i < points; i++)
    data->SetTuple(i, scalar_red);
  for (i = points; i < 2*points; i++)
    data->SetTuple(i, scalar_green);
  for (i = 2*points; i < 3*points; i++)
    data->SetTuple(i, scalar_blu);

  // this filter do not copy the scalars also if all input 
  m_Axes = vtkAppendPolyData::New();    
  m_Axes->AddInput(m_XAxis->GetOutput()); // data has the scalars.
  m_Axes->AddInput(m_YAxis->GetOutput());
  m_Axes->AddInput(m_ZAxis->GetOutput());

  m_Axes->Update();

  m_ScaleAxisTransform = vtkTransform::New();
  m_ScaleAxisTransform->Scale(m_ScaleFactor,m_ScaleFactor,m_ScaleFactor);
  m_ScaleAxisTransform->Update();

  vtkMAFSmartPointer<vtkPolyData> axes_surface;
  axes_surface = m_Axes->GetOutput();
  axes_surface->SetSource(NULL);
  axes_surface->GetPointData()->SetScalars(data);
  vtkDEL(data);

  m_ScaleAxis  = vtkTransformPolyDataFilter::New();
  m_ScaleAxis->SetInput(axes_surface.GetPointer());
  m_ScaleAxis->SetTransform(m_ScaleAxisTransform);
  m_ScaleAxis->Update();

  dpipe->SetInput(m_ScaleAxis->GetOutput());

	m_Fixed = 0;
}

//-------------------------------------------------------------------------
mafVMERefSys::~mafVMERefSys()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  SetOutput(NULL);

  vtkDEL(m_XArrow);
  vtkDEL(m_XAxisTransform);
  m_XAxis->SetTransform(NULL);
  vtkDEL(m_XAxis);

  vtkDEL(m_YArrow);
  vtkDEL(m_YAxisTransform);
  m_YAxis->SetTransform(NULL);
  vtkDEL(m_YAxis);

  vtkDEL(m_ZArrow);
  vtkDEL(m_ZAxisTransform);
  m_ZAxis->SetTransform(NULL);
  vtkDEL(m_ZAxis);

  vtkDEL(m_Axes);	
  vtkDEL(m_ScaleAxisTransform);	
  m_ScaleAxis->SetTransform(NULL);
  vtkDEL(m_ScaleAxis);

	cppDEL(m_VMEAccept);
}
//-------------------------------------------------------------------------
int mafVMERefSys::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMERefSys *vme_ref_sys=mafVMERefSys::SafeDownCast(a);
    m_Transform->SetMatrix(vme_ref_sys->m_Transform->GetMatrix());
    SetScaleFactor(vme_ref_sys->GetScaleFactor());
    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_ScaleAxis->GetOutput());
    }
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMERefSys::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    return (m_Transform->GetMatrix() == ((mafVMERefSys *)vme)->m_Transform->GetMatrix() &&
            m_ScaleFactor == ((mafVMERefSys *)vme)->GetScaleFactor());
  }
  return false;
}
//-------------------------------------------------------------------------
int mafVMERefSys::InternalInitialize()
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

//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMERefSys::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputSurface *)GetOutput();
}

//-------------------------------------------------------------------------
void mafVMERefSys::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool mafVMERefSys::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
void mafVMERefSys::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();
}

//-------------------------------------------------------------------------
void mafVMERefSys::SetScaleFactor(double scale)
//-------------------------------------------------------------------------
{
  m_ScaleFactor = scale;
  if (m_Gui)
  {
    m_Gui->Update();
  }
  m_ScaleAxisTransform->Identity();
  m_ScaleAxisTransform->Scale(m_ScaleFactor,m_ScaleFactor,m_ScaleFactor);
  m_ScaleAxisTransform->Update();
  m_ScaleAxis->Update();
  mafEvent cam_event(this,CAMERA_UPDATE);
  this->ForwardUpEvent(cam_event);
  Modified();
}

//-------------------------------------------------------------------------
double mafVMERefSys::GetScaleFactor()
//-------------------------------------------------------------------------
{
  return m_ScaleFactor;
}
//-----------------------------------------------------------------------
int mafVMERefSys::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    parent->StoreDouble("m_ScaleFactor", m_ScaleFactor);
		parent->StoreInteger("Fixed", m_Fixed);
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMERefSys::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      node->RestoreDouble("m_ScaleFactor", m_ScaleFactor);
			node->RestoreInteger("Fixed", m_Fixed);
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *mafVMERefSys::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((mafVMEOutputSurface *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}
//-------------------------------------------------------------------------
mmgGui* mafVMERefSys::CreateGui()
//-------------------------------------------------------------------------
{

	mafID sub_id = -1;

  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  m_Gui->Double(ID_SCALE_FACTOR,_("scale"),&m_ScaleFactor);
  m_Gui->Divider();

	wxString choises[3]={"Normal","Select Origin","Select Plane"};
	m_Gui->Radio(ID_RADIO,"",&m_Radio,3,choises);

	mafVME *origin_vme = GetOriginVME();
  if (origin_vme && origin_vme->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("OriginVME");
		m_OriginVmeName = (sub_id != -1) ? ((mafVMELandmarkCloud *)origin_vme)->GetLandmarkName(sub_id) : _("none");
  }
  else
    m_OriginVmeName = origin_vme ? origin_vme->GetName() : _("none");
  m_Gui->Button(ID_REF_SYS_ORIGIN,&m_OriginVmeName,_("Origin"), _("Select the vme origin for the ref sys"));
	m_Gui->Enable(ID_REF_SYS_ORIGIN,origin_vme!=NULL);

  mafVME *point1_vme = GetPoint1VME();
  if (point1_vme && point1_vme->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("Point1VME");
    m_Point1VmeName = (sub_id != -1) ? ((mafVMELandmarkCloud *)point1_vme)->GetLandmarkName(sub_id) : _("none");
  }
  else
    m_Point1VmeName = point1_vme ? point1_vme->GetName() : _("none");
  m_Gui->Button(ID_POINT1,&m_Point1VmeName,_("Point 1"), _("Select the Point 1"));
	m_Gui->Enable(ID_POINT1,point1_vme!=NULL);

  mafVME *point2_vme = GetPoint2VME();
  if (point2_vme && point2_vme->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("Point2VME");
    m_Point2VmeName = (sub_id != -1) ? ((mafVMELandmarkCloud *)point2_vme)->GetLandmarkName(sub_id) : _("none");
  }
  else
    m_Point2VmeName = point2_vme ? point2_vme->GetName() : _("none");
  m_Gui->Button(ID_POINT2,&m_Point2VmeName,_("Point 2"), _("Select the Point 2"));
	m_Gui->Enable(ID_POINT2,point2_vme!=NULL);

	if(point2_vme && origin_vme && point2_vme)
		m_Radio=2;
	else if(origin_vme)
		m_Radio=1;
	else
		m_Radio=0;

  // vme ref sys fixed
	m_Gui->Bool(ID_FIXED, _("Click for fix the refsys"), &m_Fixed, 1);

	m_Gui->Enable(ID_SCALE_FACTOR, m_Fixed == 0);
	m_Gui->Enable(ID_REF_SYS_ORIGIN, m_Fixed == 0);
	m_Gui->Enable(ID_POINT1, m_Fixed == 0);
	m_Gui->Enable(ID_POINT2, m_Fixed == 0);
	m_Gui->Enable(ID_RADIO, m_Fixed == 0);
	m_Gui->Enable(ID_FIXED, m_Fixed == 0);

	m_Gui->Update();
	//this->InternalUpdate();

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMERefSys::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_SCALE_FACTOR:
      {
        SetScaleFactor(m_ScaleFactor);
      }
      break;
			case ID_REF_SYS_ORIGIN:
      case ID_POINT1:
      case ID_POINT2:
      {
        mafID button_id = e->GetId();
        mafString title = _("Choose vme");
        e->SetId(VME_CHOOSE);
        e->SetArg((long)m_VMEAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();
        if (n != NULL)
        {
          if (button_id == ID_REF_SYS_ORIGIN)
          {
						SetRefSysLink("OriginVME", n);
						m_OriginVmeName = n->GetName();
          }
          else if (button_id == ID_POINT1)
          {
            SetRefSysLink("Point1VME", n);
						m_Point1VmeName = n->GetName();
          }
          else
          {
            SetRefSysLink("Point2VME", n);
						m_Point2VmeName = n->GetName();
          }
					InternalUpdate();
					m_Gui->Update();
        }
      }
      break;
			case ID_RADIO:
				{
					if(m_Radio==0)
					{
						this->RemoveAllLinks();
						m_Gui->Enable(ID_REF_SYS_ORIGIN,false);
						m_Gui->Enable(ID_POINT1,false);
						m_Gui->Enable(ID_POINT2,false);
					}
					else if(m_Radio==1)
					{
						this->RemoveLink("Point1VME");
						this->RemoveLink("Point2VME");
						m_Gui->Enable(ID_REF_SYS_ORIGIN,true);
						m_Gui->Enable(ID_POINT1,false);
						m_Gui->Enable(ID_POINT2,false);
					}
					else if(m_Radio==2)
					{
						m_Gui->Enable(ID_REF_SYS_ORIGIN,true);
						m_Gui->Enable(ID_POINT1,true);
						m_Gui->Enable(ID_POINT2,true);
					}
					InternalUpdate();
					m_Gui->Update();
				}
			break;
			case ID_FIXED:
				{
          m_Gui->Enable(ID_SCALE_FACTOR, m_Fixed == 0);
					m_Gui->Enable(ID_REF_SYS_ORIGIN, m_Fixed == 0);

					m_Gui->Enable(ID_POINT1, m_Fixed == 0);
					m_Gui->Enable(ID_POINT2, m_Fixed == 0);
					m_Gui->Enable(ID_RADIO, m_Fixed == 0);

          m_Gui->Enable(ID_FIXED, m_Fixed == 0);
				}
			break;
      default:
      mafNode::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-----------------------------------------------------------------------
void mafVMERefSys::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
char **mafVMERefSys::GetIcon()
//-------------------------------------------------------------------------
{
#include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}
//-----------------------------------------------------------------------
void mafVMERefSys::InternalPreUpdate()
//-----------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------
void mafVMERefSys::InternalUpdate()
//-----------------------------------------------------------------------
{
	mafVME *point1_vme = GetPoint1VME();
	mafVME *point2_vme = GetPoint2VME();
	mafVME *origin_vme = GetOriginVME();

	if(point1_vme && point2_vme && origin_vme)
	{
		double point1[3],point2[3],origin[3],orientation[3];
		mafSmartPointer<mafTransform> TmpTransform;

		//Get the position of the origin
		if(origin_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("OriginVME") != -1)
    {
			mafVMELandmark *o=((mafVMELandmarkCloud *)origin_vme)->GetLandmark(GetLinkSubId("OriginVME"));
			o->GetPoint(origin);
    }
    else if(origin_vme->IsMAFType(mafVMELandmark))
    {
			origin_vme->GetOutput()->Update();  
      origin_vme->GetOutput()->GetAbsPose(origin, orientation);
    }

		//Get the position of the point 1
		if(point1_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("Point1VME") != -1)
    {
			mafVMELandmark *l1=((mafVMELandmarkCloud *)point1_vme)->GetLandmark(GetLinkSubId("Point1VME"));
			l1->GetPoint(point1);
    }
    else if(point1_vme->IsMAFType(mafVMELandmark))
    {
			point1_vme->GetOutput()->Update();  
      point1_vme->GetOutput()->GetAbsPose(point1, orientation);
    }

		//Get the position of the point 2
		if(point2_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("Point2VME") != -1)
    {
			mafVMELandmark *l2=((mafVMELandmarkCloud *)point2_vme)->GetLandmark(GetLinkSubId("Point2VME"));
			l2->GetPoint(point2);
    }
    else if(point2_vme->IsMAFType(mafVMELandmark))
    {
			point2_vme->GetOutput()->Update();  
      point2_vme->GetOutput()->GetAbsPose(point2, orientation);
    }

		double v1[3],v2[3],v3[3];

		v1[0] = point1[0] - origin[0];
		v1[1] = point1[1] - origin[1];
		v1[2] = point1[2] - origin[2];

		v2[0] = point2[0] - origin[0];
		v2[1] = point2[1] - origin[1];
		v2[2] = point2[2] - origin[2];

		vtkMath::Normalize(v1);
		vtkMath::Normalize(v2);

		vtkMath::Cross(v1,v2,v3);
		vtkMath::Normalize(v3);
		vtkMath::Cross(v3,v1,v2);

		vtkMatrix4x4 *matrix_translation=vtkMatrix4x4::New();
		matrix_translation->Identity();
		for(int i=0;i<3;i++)
			matrix_translation->SetElement(i,3,origin[i]);

		vtkMatrix4x4 *matrix_rotation=vtkMatrix4x4::New();
		matrix_rotation->Identity();
		for(int i=0;i<3;i++)
			matrix_rotation->SetElement(i,0,v1[i]);
		for(int i=0;i<3;i++)
			matrix_rotation->SetElement(i,1,v2[i]);
		for(int i=0;i<3;i++)
			matrix_rotation->SetElement(i,2,v3[i]);

		mafMatrix a;
		a.SetVTKMatrix(matrix_rotation);
		mafMatrix b;
		b.SetVTKMatrix(matrix_translation);
		mafMatrix c;
		mafMatrix::Multiply4x4(b,a,c);

		this->SetMatrix(c);
		
	}
	else if(origin_vme)
	{
		double origin[3],orientation[3];

		//Get the position of the origin
		if(origin_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("OriginVME") != -1)
    {
			mafVMELandmark *o=((mafVMELandmarkCloud *)origin_vme)->GetLandmark(GetLinkSubId("OriginVME"));
			o->GetPoint(origin);
    }
    else if(origin_vme->IsMAFType(mafVMELandmark))
    {
			origin_vme->GetOutput()->Update();  
      origin_vme->GetOutput()->GetAbsPose(origin, orientation);
    }

		vtkMatrix4x4 *matrix_translation=vtkMatrix4x4::New();
		matrix_translation->Identity();
		for(int i=0;i<3;i++)
			matrix_translation->SetElement(i,3,origin[i]);

		mafMatrix b;
		b.SetVTKMatrix(matrix_translation);
		
		this->SetMatrix(b);
	}
	else
	{
		vtkMatrix4x4 *matrix_translation=vtkMatrix4x4::New();
		matrix_translation->Identity();

		mafMatrix b;
		b.SetVTKMatrix(matrix_translation);
		
		this->SetMatrix(b);
	}

	mafEvent *e	= new mafEvent(this,CAMERA_UPDATE);
  ForwardUpEvent(e);
	this->Modified();
}
//-------------------------------------------------------------------------
void mafVMERefSys::SetRefSysLink(const char *link_name, mafNode *n)
//-------------------------------------------------------------------------
{
	if (n->IsMAFType(mafVMELandmark))
  {
    SetLink(link_name,n->GetParent(),((mafVMELandmarkCloud *)n->GetParent())->FindLandmarkIndex(n->GetName()));
  }
  else
    SetLink(link_name, n);
}
//-------------------------------------------------------------------------
mafVME *mafVMERefSys::GetPoint1VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("Point1VME"));
}
//-------------------------------------------------------------------------
mafVME *mafVMERefSys::GetPoint2VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("Point2VME"));
}
//-------------------------------------------------------------------------
mafVME *mafVMERefSys::GetOriginVME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("OriginVME"));
}
