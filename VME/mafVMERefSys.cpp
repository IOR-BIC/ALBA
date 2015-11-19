/*=========================================================================

 Program: MAF2
 Module: mafVMERefSys
 Authors: Marco Petrone, Paolo Quadrani, Stefano Perticoni
 
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


#include "mafVMERefSys.h"
#include "mafGUI.h"

#include "mmaMaterial.h"
#include "mafTagArray.h"
#include "mafTransform.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
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

const bool DEBUG_MODE = false;

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

	m_Modality = REFSYS_FREE;
  m_ScaleFactor = 1.0;

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

	//m_Fixed = 0;
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
    parent->StoreDouble("ScaleFactor", m_ScaleFactor);
		parent->StoreInteger("Modality", m_Modality);
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
      node->RestoreDouble("ScaleFactor", m_ScaleFactor);
			node->RestoreInteger("Modality", m_Modality);
			SetScaleFactor(m_ScaleFactor);
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
mafGUI* mafVMERefSys::CreateGui()
//-------------------------------------------------------------------------
{

	mafID sub_id = -1;

  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();

  m_Gui->Double(ID_SCALE_FACTOR,_("scale"),&m_ScaleFactor);
  m_Gui->Divider();

	wxString choises[3]={"Normal (free to move)","Select Origin","Select Plane"};
	m_Gui->Radio(ID_RADIO,"",&m_Modality,3,choises);

	mafVME *origin_vme = GetOriginVME();
  if (origin_vme && origin_vme->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("OriginVME");
		m_OriginVmeName = (sub_id != -1) ? ((mafVMELandmarkCloud *)origin_vme)->GetLandmarkName(sub_id) : _("none");
  }
  else
    m_OriginVmeName = origin_vme ? origin_vme->GetName() : _("none");
  m_Gui->Button(ID_REF_SYS_ORIGIN,&m_OriginVmeName,_("Origin"), _("Select the vme originABSPosition for the ref sys"));
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
	
 	m_Gui->Update();

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
        e->SetArg((long)&mafVMERefSys::LandmarkAccept);
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
				if(m_Modality==REFSYS_FREE)
				{
          // Normal RefSys
					m_Gui->Enable(ID_REF_SYS_ORIGIN,false);
					m_Gui->Enable(ID_POINT1,false);
					m_Gui->Enable(ID_POINT2,false);
				}
				else if(m_Modality==REFSYS_ORIGIN)
				{
          // RefSys with Origin link
					m_Gui->Enable(ID_REF_SYS_ORIGIN,true);
					m_Gui->Enable(ID_POINT1,false);
					m_Gui->Enable(ID_POINT2,false);
				}
				else if(m_Modality==REFSYS_PLANE)
				{
          // RefSys with all the link enabled: originABSPosition, point1ABSPosition and point2ABSPosition
					m_Gui->Enable(ID_REF_SYS_ORIGIN,true);
					m_Gui->Enable(ID_POINT1,true);
					m_Gui->Enable(ID_POINT2,true);
				}
				InternalUpdate();

				//Workaround to update accept on move operations
				ForwardUpEvent( mafEvent( this, VME_MODIFIED,this ) );
				m_Gui->Update();
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
  #include "mafVMEProcedural.xpm"
  return mafVMEProcedural_xpm;
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
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Entering InternalUpdate for: " << this->GetName() << " VME"  << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }
	mafVME *point1VME = GetPoint1VME();
	mafVME *point2VME = GetPoint2VME();
	mafVME *originVME = GetOriginVME();

	if(m_Modality == REFSYS_PLANE && point1VME && point2VME && originVME)
	{
		double point1ABSPosition[3],point2ABSPosition[3],originABSPosition[3],useless[3];
		mafSmartPointer<mafTransform> TmpTransform;

		//Get the position of the originABSPosition
		if(originVME->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("OriginVME") != -1)
    {
      ((mafVMELandmarkCloud *)originVME)->GetLandmarkPosition(GetLinkSubId("OriginVME"), originABSPosition, -1);
      mafTransform t;
      t.SetMatrix(*originVME->GetOutput()->GetAbsMatrix());
      t.TransformPoint(originABSPosition, originABSPosition);
    
    }
    else if(originVME->IsMAFType(mafVMELandmark))
    {
			originVME->GetOutput()->Update();  
      originVME->GetOutput()->GetAbsPose(originABSPosition, useless);
    }

    if (DEBUG_MODE)
    {
      LogVector3(originABSPosition, "originABSPosition abs position");
    }


		//Get the position of the point 1
		if(point1VME->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("Point1VME") != -1)
    {
      ((mafVMELandmarkCloud *)point1VME)->GetLandmarkPosition(GetLinkSubId("Point1VME"),point1ABSPosition,-1);
      mafTransform t;
      t.SetMatrix(*point1VME->GetOutput()->GetAbsMatrix());
      t.TransformPoint(point1ABSPosition, point1ABSPosition);
    }
    else if(point1VME->IsMAFType(mafVMELandmark))
    {
			point1VME->GetOutput()->Update();  
      point1VME->GetOutput()->GetAbsPose(point1ABSPosition, useless);
    }
    if (DEBUG_MODE)
    {
      LogVector3(point1ABSPosition, "point1ABSPosition abs position");
    }

		//Get the position of the point 2
		if(point2VME->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("Point2VME") != -1)
    {
      ((mafVMELandmarkCloud *)point2VME)->GetLandmarkPosition(GetLinkSubId("Point2VME"),point2ABSPosition,-1);
      mafTransform t;
      t.SetMatrix(*point2VME->GetOutput()->GetAbsMatrix());
      t.TransformPoint(point2ABSPosition, point2ABSPosition);
    }
    else if(point2VME->IsMAFType(mafVMELandmark))
    {
			point2VME->GetOutput()->Update();  
      point2VME->GetOutput()->GetAbsPose(point2ABSPosition, useless);
    }
    if (DEBUG_MODE)
    {
      LogVector3(point1ABSPosition, "point2ABSPosition abs position");
    }

		double point1OriginVector[3],point2OriginVector[3],point1Point2CrossProductVector[3];

		point1OriginVector[0] = point1ABSPosition[0] - originABSPosition[0];
		point1OriginVector[1] = point1ABSPosition[1] - originABSPosition[1];
		point1OriginVector[2] = point1ABSPosition[2] - originABSPosition[2];

		point2OriginVector[0] = point2ABSPosition[0] - originABSPosition[0];
		point2OriginVector[1] = point2ABSPosition[1] - originABSPosition[1];
		point2OriginVector[2] = point2ABSPosition[2] - originABSPosition[2];

		vtkMath::Normalize(point1OriginVector);
		vtkMath::Normalize(point2OriginVector);

		vtkMath::Cross(point1OriginVector,point2OriginVector,point1Point2CrossProductVector);
		vtkMath::Normalize(point1Point2CrossProductVector);
		vtkMath::Cross(point1Point2CrossProductVector,point1OriginVector,point2OriginVector);

		vtkMatrix4x4 *matrix_translation=vtkMatrix4x4::New();
		matrix_translation->Identity();
		for(int i=0;i<3;i++)
			matrix_translation->SetElement(i,3,originABSPosition[i]);

		vtkMatrix4x4 *matrix_rotation=vtkMatrix4x4::New();
		matrix_rotation->Identity();
		for(int i=0;i<3;i++)
			matrix_rotation->SetElement(i,0,point1OriginVector[i]);
		for(int i=0;i<3;i++)
			matrix_rotation->SetElement(i,1,point2OriginVector[i]);
		for(int i=0;i<3;i++)
			matrix_rotation->SetElement(i,2,point1Point2CrossProductVector[i]);

		mafMatrix a;
		a.SetVTKMatrix(matrix_rotation);
		mafMatrix b;
		b.SetVTKMatrix(matrix_translation);
		mafMatrix c;
		mafMatrix::Multiply4x4(b,a,c);

    vtkDEL(matrix_rotation);
    vtkDEL(matrix_translation);

    this->SetAbsMatrix(c);
		this->Modified();
	}
	else if(m_Modality== REFSYS_ORIGIN && originVME)
	{
		double origin[3],orientation[3];

		//Get the position of the origin
		if(originVME->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("OriginVME") != -1)
    {
      ((mafVMELandmarkCloud *)originVME)->GetLandmarkPosition(GetLinkSubId("OriginVME"),origin,-1);
      mafTransform t;
      t.SetMatrix(*originVME->GetOutput()->GetAbsMatrix());
      t.TransformPoint(origin,origin);
    }
    else if(originVME->IsMAFType(mafVMELandmark))
    {
			originVME->GetOutput()->Update();  
      originVME->GetOutput()->GetAbsPose(origin, orientation);
    }

		vtkMatrix4x4 *matrix_translation=vtkMatrix4x4::New();
		matrix_translation->Identity();
		for(int i=0;i<3;i++)
			matrix_translation->SetElement(i,3,origin[i]);

		mafMatrix b;
		b.SetVTKMatrix(matrix_translation);

    vtkDEL(matrix_translation);
		
    this->SetAbsMatrix(b);
		this->Modified();
	}


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
  {
    SetLink(link_name, n);
  }
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

//----------------------------------------------------------------------------
bool mafVMERefSys::IsMovable()
{
	return (m_Modality==REFSYS_FREE);
}

//-------------------------------------------------------------------------
mafVME *mafVMERefSys::GetOriginVME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("OriginVME"));
}

void mafVMERefSys::LogVector3( double *vector , const char *logMessage /*= NULL*/ )
{
  std::ostringstream stringStream;
  if (logMessage)stringStream << logMessage << std::endl;
  stringStream << "Vector components: [" << vector[0] << " , " << vector[1] << " , " << vector[2] << " ]" << std::endl;
  stringStream << "Vector module: " << vtkMath::Norm(vector) << std::endl; 

  mafLogMessage(stringStream.str().c_str());
}

void mafVMERefSys::LogPoint3( double *point, const char *logMessage )
{
  std::ostringstream stringStream;
  if (logMessage) stringStream << logMessage << std::endl;
  stringStream << "Point coordinates: [" << point[0] << " , " << point[1] << " , " << point[2] << " ]" << std::endl;
  mafLogMessage(stringStream.str().c_str());
}

void mafVMERefSys::LogMAFMatrix4x4( mafMatrix *mat, const char *logMessage )
{
  std::ostringstream stringStream;
  if (logMessage) stringStream << logMessage << std::endl;
  mat->Print(stringStream);
  mafLogMessage(stringStream.str().c_str());
}

void mafVMERefSys::LogVTKMatrix4x4( vtkMatrix4x4 *mat, const char *logMessage )
{
  std::ostringstream stringStream;
  if (logMessage) stringStream << logMessage << std::endl;
  mat->Print(stringStream);
  mafLogMessage(stringStream.str().c_str());
}
