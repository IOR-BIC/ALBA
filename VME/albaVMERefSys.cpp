/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMERefSys
 Authors: Marco Petrone, Paolo Quadrani, Stefano Perticoni
 
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


#include "albaVMERefSys.h"
#include "albaGUI.h"

#include "mmaMaterial.h"
#include "albaTagArray.h"
#include "albaTransform.h"
#include "albaVMEOutputSurface.h"
#include "albaDataPipeCustom.h"
#include "albaStorageElement.h"
#include "albaIndent.h"
#include "albaVMELandmarkCloud.h"
#include "albaMatrix.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBADataPipe.h"
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

const bool DEBUG_MODE = TRUE;

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMERefSys)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMERefSys::albaVMERefSys()
//-------------------------------------------------------------------------
{
  albaNEW(m_Transform);
  albaVMEOutputSurface *output=albaVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  // attach a datapipe which creates a bridge between VTK and ALBA
  albaDataPipeCustom *dpipe = albaDataPipeCustom::New();
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
  m_XAxis->SetInputConnection(m_XArrow->GetOutputPort());
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
  m_YAxis->SetInputConnection(m_YArrow->GetOutputPort());
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
  m_ZAxis->SetInputConnection(m_ZArrow->GetOutputPort());
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
  m_Axes->AddInputConnection(m_XAxis->GetOutputPort()); // data has the scalars.
  m_Axes->AddInputConnection(m_YAxis->GetOutputPort());
  m_Axes->AddInputConnection(m_ZAxis->GetOutputPort());

  m_Axes->Update();

  m_ScaleAxisTransform = vtkTransform::New();
  m_ScaleAxisTransform->Scale(m_ScaleFactor,m_ScaleFactor,m_ScaleFactor);
  m_ScaleAxisTransform->Update();

  vtkALBASmartPointer<vtkPolyData> axes_surface;
  axes_surface = m_Axes->GetOutput();
  axes_surface->GetPointData()->SetScalars(data);
  vtkDEL(data);

  m_ScaleAxis  = vtkTransformPolyDataFilter::New();
  m_ScaleAxis->SetInputData(axes_surface.GetPointer());
  m_ScaleAxis->SetTransform(m_ScaleAxisTransform);
  m_ScaleAxis->Update();

  dpipe->SetInput(m_ScaleAxis->GetOutput());

	//m_Fixed = 0;
}

//-------------------------------------------------------------------------
albaVMERefSys::~albaVMERefSys()
//-------------------------------------------------------------------------
{
  albaDEL(m_Transform);
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
int albaVMERefSys::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    albaVMERefSys *vme_ref_sys=albaVMERefSys::SafeDownCast(a);
    m_Transform->SetMatrix(vme_ref_sys->m_Transform->GetMatrix());
    SetScaleFactor(vme_ref_sys->GetScaleFactor());
    albaDataPipeCustom *dpipe = albaDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_ScaleAxis->GetOutput());
    }
    return ALBA_OK;
  }  
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
bool albaVMERefSys::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    return (m_Transform->GetMatrix() == ((albaVMERefSys *)vme)->m_Transform->GetMatrix() &&
            m_ScaleFactor == ((albaVMERefSys *)vme)->GetScaleFactor());
  }
  return false;
}
//-------------------------------------------------------------------------
int albaVMERefSys::InternalInitialize()
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

//-------------------------------------------------------------------------
albaVMEOutputSurface *albaVMERefSys::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
  return (albaVMEOutputSurface *)GetOutput();
}

//-------------------------------------------------------------------------
void albaVMERefSys::SetMatrix(const albaMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool albaVMERefSys::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
void albaVMERefSys::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();
}

//-------------------------------------------------------------------------
void albaVMERefSys::SetScaleFactor(double scale)
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
  albaEvent cam_event(this,CAMERA_UPDATE);
  this->ForwardUpEvent(cam_event);
  Modified();
}

//-------------------------------------------------------------------------
double albaVMERefSys::GetScaleFactor()
//-------------------------------------------------------------------------
{
  return m_ScaleFactor;
}
//-----------------------------------------------------------------------
int albaVMERefSys::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    parent->StoreDouble("ScaleFactor", m_ScaleFactor);
		parent->StoreInteger("Modality", m_Modality);
    return ALBA_OK;
  }
  return ALBA_ERROR;
}

//-----------------------------------------------------------------------
int albaVMERefSys::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==ALBA_OK)
  {
    albaMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==ALBA_OK)
    {
      m_Transform->SetMatrix(matrix);
      node->RestoreDouble("ScaleFactor", m_ScaleFactor);
			node->RestoreInteger("Modality", m_Modality);
			SetScaleFactor(m_ScaleFactor);
      return ALBA_OK;
    }
  }
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *albaVMERefSys::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((albaVMEOutputSurface *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}
//-------------------------------------------------------------------------
albaGUI* albaVMERefSys::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();

  m_Gui->Double(ID_SCALE_FACTOR,_("Scale"),&m_ScaleFactor);
  m_Gui->Divider();

	wxString choises[3]={"Normal (free to move)","Select Origin","Select Plane"};
	m_Gui->Radio(ID_RADIO,"",&m_Modality,3,choises);

	albaVME *origin_vme = GetOriginVME();
  m_OriginVmeName = origin_vme ? origin_vme->GetName() : _("None");
  m_Gui->Button(ID_REF_SYS_ORIGIN,&m_OriginVmeName,_("Origin"), _("Select the vme origin ABS position for the reference system"));
	m_Gui->Enable(ID_REF_SYS_ORIGIN,origin_vme!=NULL);

  albaVME *point1_vme = GetPoint1VME();
  m_Point1VmeName = point1_vme ? point1_vme->GetName() : _("None");
  m_Gui->Button(ID_POINT1,&m_Point1VmeName,_("Point 1"), _("Select the Point 1"));
	m_Gui->Enable(ID_POINT1,point1_vme!=NULL);

  albaVME *point2_vme = GetPoint2VME();
  m_Point2VmeName = point2_vme ? point2_vme->GetName() : _("None");
  m_Gui->Button(ID_POINT2,&m_Point2VmeName,_("Point 2"), _("Select the Point 2"));
	m_Gui->Enable(ID_POINT2,point2_vme!=NULL);
	
 	m_Gui->Update();

  return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMERefSys::OnEvent(albaEventBase *alba_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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
        albaID button_id = e->GetId();
        albaString title = _("Choose vme");
        e->SetId(VME_CHOOSE);
        e->SetPointer(&albaVMERefSys::LandmarkAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        albaVME *n = e->GetVme();
        if (n != NULL)
        {
          if (button_id == ID_REF_SYS_ORIGIN)
          {
						SetMandatoryLink("OriginVME", n);
						m_OriginVmeName = n->GetName();
          }
          else if (button_id == ID_POINT1)
          {
            SetMandatoryLink("Point1VME", n);
						m_Point1VmeName = n->GetName();
          }
          else
          {
            SetMandatoryLink("Point2VME", n);
						m_Point2VmeName = n->GetName();
          }
					InternalUpdate();
					m_Gui->Update();
        }
      }
      break;
			case ID_RADIO:
			{
				albaVME *linkedVME;
				if(m_Modality==REFSYS_FREE)
				{
          // Normal RefSys

					//Set all links to non mandatory
					SetLinkToNonMandatory("OriginVME");
          SetLinkToNonMandatory("Point1VME");
          SetLinkToNonMandatory("Point2VME");
					
					m_Gui->Enable(ID_REF_SYS_ORIGIN,false);
					m_Gui->Enable(ID_POINT1,false);
					m_Gui->Enable(ID_POINT2,false);
				}
				else if(m_Modality==REFSYS_ORIGIN)
				{
          // RefSys with Origin link

					//Make origin link mandatory and remove other links
					SetLinkToMandatory("OriginVME");
          SetLinkToNonMandatory("Point1VME");
          SetLinkToNonMandatory("Point2VME");

					m_Gui->Enable(ID_REF_SYS_ORIGIN,true);
					m_Gui->Enable(ID_POINT1,false);
					m_Gui->Enable(ID_POINT2,false);
				}
				else if(m_Modality==REFSYS_PLANE)
				{
          // RefSys with all the link enabled: originABSPosition, point1ABSPosition and point2ABSPosition

					//Make all link mandatory
          SetLinkToMandatory("OriginVME");
          SetLinkToMandatory("Point1VME");
          SetLinkToMandatory("Point2VME");

					m_Gui->Enable(ID_REF_SYS_ORIGIN,true);
					m_Gui->Enable(ID_POINT1,true);
					m_Gui->Enable(ID_POINT2,true);
				}
				InternalUpdate();

				//Workaround to update accept on move operations
				GetLogicManager()->VmeModified(this);
        GetLogicManager()->CameraUpdate();
				m_Gui->Update();
			}
			break;
      default:
        albaVME::OnEvent(alba_event);
    }
  }
  else
  {
    Superclass::OnEvent(alba_event);
  }
}
//-----------------------------------------------------------------------
void albaVMERefSys::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);

  albaMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
char **albaVMERefSys::GetIcon()
//-------------------------------------------------------------------------
{
  #include "albaVMERefSys.xpm"
  return albaVMERefSys_xpm;
}
//-----------------------------------------------------------------------
void albaVMERefSys::InternalPreUpdate()
//-----------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------
void albaVMERefSys::InternalUpdate()
//-----------------------------------------------------------------------
{
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Entering InternalUpdate for: " << this->GetName() << " VME"  << std::endl;
    albaLogMessage(stringStream.str().c_str());
  }
	albaVME *point1VME = GetPoint1VME();
	albaVME *point2VME = GetPoint2VME();
	albaVME *originVME = GetOriginVME();

	if(m_Modality == REFSYS_PLANE && point1VME && point2VME && originVME)
	{
		double point1ABSPosition[3],point2ABSPosition[3],originABSPosition[3],useless[3];
		albaSmartPointer<albaTransform> TmpTransform;

		//Get the position of the originABSPosition
		originVME->GetOutput()->Update();  
    originVME->GetOutput()->GetAbsPose(originABSPosition, useless);

    if (DEBUG_MODE)
    {
      LogVector3(originABSPosition, "originABSPosition abs position");
    }


		//Get the position of the point 1
		point1VME->GetOutput()->Update();  
    point1VME->GetOutput()->GetAbsPose(point1ABSPosition, useless);
    if (DEBUG_MODE)
    {
      LogVector3(point1ABSPosition, "point1ABSPosition abs position");
    }

		//Get the position of the point 2
		point2VME->GetOutput()->Update();  
    point2VME->GetOutput()->GetAbsPose(point2ABSPosition, useless);
    if (DEBUG_MODE)
    {
      LogVector3(point1ABSPosition, "point2ABSPosition abs position");
    }

    this->SetAbsMatrix(GetAbsMatrixFromPoints(originABSPosition, point1ABSPosition, point2ABSPosition));
		this->Modified();
	}
	else if(m_Modality== REFSYS_ORIGIN && originVME)
	{
		double origin[3],orientation[3];

		//Get the position of the origin
		originVME->GetOutput()->Update();  
    originVME->GetOutput()->GetAbsPose(origin, orientation);
    
		vtkMatrix4x4 *matrix_translation=vtkMatrix4x4::New();
		matrix_translation->Identity();
		for(int i=0;i<3;i++)
			matrix_translation->SetElement(i,3,origin[i]);

		albaMatrix b;
		b.SetVTKMatrix(matrix_translation);

    vtkDEL(matrix_translation);
		
    this->SetAbsMatrix(b);
		this->Modified();
	}


}

//----------------------------------------------------------------------------
albaMatrix albaVMERefSys::GetAbsMatrixFromPoints(double * origin, double * point1,  double * point2)
{
	double point1OriginVector[3], point2OriginVector[3], point1Point2CrossProductVector[3];

	point1OriginVector[0] = point1[0] - origin[0];
	point1OriginVector[1] = point1[1] - origin[1];
	point1OriginVector[2] = point1[2] - origin[2];

	point2OriginVector[0] = point2[0] - origin[0];
	point2OriginVector[1] = point2[1] - origin[1];
	point2OriginVector[2] = point2[2] - origin[2];

	vtkMath::Normalize(point1OriginVector);
	vtkMath::Normalize(point2OriginVector);

	vtkMath::Cross(point1OriginVector, point2OriginVector, point1Point2CrossProductVector);
	vtkMath::Normalize(point1Point2CrossProductVector);
	vtkMath::Cross(point1Point2CrossProductVector, point1OriginVector, point2OriginVector);

	vtkMatrix4x4 *matrix_translation = vtkMatrix4x4::New();
	matrix_translation->Identity();
	for (int i = 0; i < 3; i++)
		matrix_translation->SetElement(i, 3, origin[i]);

	vtkMatrix4x4 *matrix_rotation = vtkMatrix4x4::New();
	matrix_rotation->Identity();
	for (int i = 0; i < 3; i++)
		matrix_rotation->SetElement(i, 0, point1OriginVector[i]);
	for (int i = 0; i < 3; i++)
		matrix_rotation->SetElement(i, 1, point2OriginVector[i]);
	for (int i = 0; i < 3; i++)
		matrix_rotation->SetElement(i, 2, point1Point2CrossProductVector[i]);

	albaMatrix a;
	a.SetVTKMatrix(matrix_rotation);
	albaMatrix b;
	b.SetVTKMatrix(matrix_translation);
	albaMatrix c;
	albaMatrix::Multiply4x4(b, a, c);

	vtkDEL(matrix_rotation);
	vtkDEL(matrix_translation);

	return c;
}

//-------------------------------------------------------------------------
albaVME *albaVMERefSys::GetPoint1VME()
//-------------------------------------------------------------------------
{
  return GetLink("Point1VME");
}
//-------------------------------------------------------------------------
albaVME *albaVMERefSys::GetPoint2VME()
//-------------------------------------------------------------------------
{
  return GetLink("Point2VME");
}

//----------------------------------------------------------------------------
bool albaVMERefSys::IsMovable()
{
	return (m_Modality==REFSYS_FREE);
}

//-------------------------------------------------------------------------
albaVME *albaVMERefSys::GetOriginVME()
//-------------------------------------------------------------------------
{
  return GetLink("OriginVME");
}

void albaVMERefSys::LogVector3( double *vector , const char *logMessage /*= NULL*/ )
{
  std::ostringstream stringStream;
  if (logMessage)stringStream << logMessage << std::endl;
  stringStream << "Vector components: [" << vector[0] << " , " << vector[1] << " , " << vector[2] << " ]" << std::endl;
  stringStream << "Vector module: " << vtkMath::Norm(vector) << std::endl; 

  albaLogMessage(stringStream.str().c_str());
}

void albaVMERefSys::LogPoint3( double *point, const char *logMessage )
{
  std::ostringstream stringStream;
  if (logMessage) stringStream << logMessage << std::endl;
  stringStream << "Point coordinates: [" << point[0] << " , " << point[1] << " , " << point[2] << " ]" << std::endl;
  albaLogMessage(stringStream.str().c_str());
}

void albaVMERefSys::LogALBAMatrix4x4( albaMatrix *mat, const char *logMessage )
{
  std::ostringstream stringStream;
  if (logMessage) stringStream << logMessage << std::endl;
  mat->Print(stringStream);
  albaLogMessage(stringStream.str().c_str());
}

void albaVMERefSys::LogVTKMatrix4x4( vtkMatrix4x4 *mat, const char *logMessage )
{
  std::ostringstream stringStream;
  if (logMessage) stringStream << logMessage << std::endl;
  mat->Print(stringStream);
  albaLogMessage(stringStream.str().c_str());
}
