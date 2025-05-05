/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPolylineSpline
 Authors: Daniele Giunchi & Matteo Giacomoni, Nicola Vanella
 
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

#include "albaVMEPolylineSpline.h"
#include "albaTransform.h"
#include "albaStorageElement.h"
#include "albaIndent.h"
#include "albaDataPipeCustom.h"
#include "albaVMEPolyline.h"
#include "albaVMEOutputPolyline.h"
#include "albaGUI.h"
#include "mmaMaterial.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBADataPipe.h"
#include "vtkPolyData.h"
#include "vtkCardinalSpline.h"
#include "vtkPoints.h"
#include "vtkTransform.h"
#include "vtkCellArray.h"
#include "vtkTransformPolyDataFilter.h"

#include "vtkMath.h"

#include <assert.h>
#include "albaVMELandmarkCloud.h"
#include "albaVME.h"
#include "albaVMEPointSet.h"
#include "albaVMEOutput.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEPolylineSpline)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEPolylineSpline::albaVMEPolylineSpline()
{
  albaNEW(m_Transform);
  albaVMEOutputPolyline *output=albaVMEOutputPolyline::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  DependsOnLinkedNodeOn();

  // attach a datapipe which creates a bridge between VTK and ALBA
  albaDataPipeCustom *dpipe = albaDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
	
	m_Polyline = NULL;
	vtkNEW(m_Polyline);
  dpipe->SetInput(m_Polyline);

	m_PointsSplined = NULL;
	vtkNEW(m_PointsSplined);

	m_SplineCoefficient = 100;
  m_OrderByAxisMode   = AXIS_NONE;

	m_ClosePolyline = false;
  //m_MinimumSpacing = 0.;
}
//-------------------------------------------------------------------------
albaVMEPolylineSpline::~albaVMEPolylineSpline()
{
  vtkDEL(m_Polyline);
	vtkDEL(m_PointsSplined);
  albaDEL(m_Transform);
  SetOutput(NULL);
}

//-------------------------------------------------------------------------
int albaVMEPolylineSpline::DeepCopy(albaVME *a)
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    albaVMEPolylineSpline *splinePolyline = albaVMEPolylineSpline::SafeDownCast(a);
    albaVME *linked_node = splinePolyline->GetPolylineLink();
    if (linked_node)
    {
      this->SetPolylineLink(linked_node);
    }
    
    m_Transform->SetMatrix(splinePolyline->m_Transform->GetMatrix());

    albaDataPipeCustom *dpipe = albaDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_Polyline);
      m_Polyline->Update();
    }
    m_SplineCoefficient = splinePolyline->m_SplineCoefficient;
    m_OrderByAxisMode = splinePolyline->m_OrderByAxisMode;

    return ALBA_OK;
  }  
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
bool albaVMEPolylineSpline::Equals(albaVME *vme)
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((albaVMEPolylineSpline *)vme)->m_Transform->GetMatrix() && \
          GetPolylineLink() == ((albaVMEPolylineSpline *)vme)->GetPolylineLink() && \
          m_OrderByAxisMode == ((albaVMEPolylineSpline *)vme)->m_OrderByAxisMode &&
          m_SplineCoefficient == ((albaVMEPolylineSpline *)vme)->m_SplineCoefficient;

  }
  return ret;
}

//-------------------------------------------------------------------------
albaVMEOutputPolyline *albaVMEPolylineSpline::GetPolylineOutput()
{
  return (albaVMEOutputPolyline *)GetOutput();
}

//-------------------------------------------------------------------------
void albaVMEPolylineSpline::SetMatrix(const albaMatrix &mat)
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool albaVMEPolylineSpline::IsAnimated()
{
  return false;
}

//-------------------------------------------------------------------------
bool albaVMEPolylineSpline::IsDataAvailable()
{
  if(GetPolylineLink())
    return GetPolylineLink()->IsDataAvailable();
  else
    return false;
}

//-------------------------------------------------------------------------
void albaVMEPolylineSpline::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
{
  kframes.clear(); // no timestamps
}

//-----------------------------------------------------------------------
void albaVMEPolylineSpline::InternalUpdate() //Multi
{
	//wxBusyCursor wait;

	albaVMEPolyline *vmePL = albaVMEPolyline::SafeDownCast(GetPolylineLink());
	albaVMELandmarkCloud *vmeLC = albaVMELandmarkCloud::SafeDownCast(GetPolylineLink());

	if (vmePL == NULL && vmeLC == NULL)
	{
		return;
	}

	
	vtkALBASmartPointer<vtkPolyData> poly;

	if (vmePL) 
	{
		vmePL->Update();

		vtkPolyData *polyline = ((vtkPolyData *)vmePL->GetOutput()->GetVTKData());

		if (m_OrderByAxisMode) OrderPolylineByAxis(polyline, m_OrderByAxisMode);

		poly->DeepCopy(polyline);
	}
	else
	{
		vmeLC->Update();

		vtkALBASmartPointer<vtkPoints> in_points;
		vtkALBASmartPointer<vtkCellArray> in_cells;
		vtkIdType pointId[2];

		for (int i = 0; i < vmeLC->GetNumberOfChildren(); i++)
		{
			double point[3], rot[3];
			vmeLC->GetChild(i)->GetOutput()->GetPose(point, rot);
			in_points->InsertNextPoint(point);

			if (i > 0)
			{
				pointId[0] = i - 1;
				pointId[1] = i;
				in_cells->InsertNextCell(2, pointId);
			}
		}

		if (m_ClosePolyline)
		{
			double point[3], rot[3];
			vmeLC->GetChild(0)->GetOutput()->GetPose(point, rot);
			in_points->InsertNextPoint(point);

				pointId[0] = vmeLC->GetNumberOfChildren() - 1;
				pointId[1] = 0;
				in_cells->InsertNextCell(2, pointId);
		}

		in_points->Modified();

		poly->SetPoints(in_points);
		poly->SetLines(in_cells);
		poly->Modified();
		poly->Update();

		albaMatrix *vmeAbsPose = vmeLC->GetOutput()->GetAbsMatrix();

		if (vmeAbsPose)
			this->SetAbsMatrix(*vmeAbsPose, vmeLC->GetTimeStamp());
	}
	
  poly->Update();

  this->SplinePolyline(poly); // generate a "splined" polyline 

  this->OrderPolyline(poly); // create orderer sequence of points and cells

  m_Polyline->DeepCopy(poly);
	m_Polyline->Update();

	Modified();
}
//-----------------------------------------------------------------------
void albaVMEPolylineSpline::InternalPreUpdate()
{
}
//-----------------------------------------------------------------------
int albaVMEPolylineSpline::InternalStore(albaStorageElement *parent)
{  
	if (Superclass::InternalStore(parent) == ALBA_OK)
	{
		if (parent->StoreMatrix("Transform", &m_Transform->GetMatrix()) == ALBA_OK &&
			parent->StoreInteger("AxisReorder", m_OrderByAxisMode) == ALBA_OK &&
			parent->StoreInteger("ClosePolyline", m_ClosePolyline) == ALBA_OK)
			return ALBA_OK;
	}
  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMEPolylineSpline::InternalRestore(albaStorageElement *node)
{
  if (Superclass::InternalRestore(node)==ALBA_OK)
  {
    albaMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==ALBA_OK)
    {
			node->RestoreInteger("AxisReorder", m_OrderByAxisMode);
			node->RestoreInteger("ClosePolyline", m_ClosePolyline);
      m_Transform->SetMatrix(matrix);
			GetLogicManager()->CameraUpdate();
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}

//-----------------------------------------------------------------------
void albaVMEPolylineSpline::Print(std::ostream& os, const int tabs)
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);

  albaMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}

//-------------------------------------------------------------------------
char** albaVMEPolylineSpline::GetIcon() 
{
  #include "albaVMEPolylineSpline.xpm"
  return albaVMEPolylineSpline_xpm;
}

//-------------------------------------------------------------------------
void albaVMEPolylineSpline::SetPolylineLink(albaVME *n)
{
	SetMandatoryLink("PolylineSource", n);
}

//-------------------------------------------------------------------------
albaVME *albaVMEPolylineSpline::GetPolylineLink()
{
  return GetLink("PolylineSource");
}

//-------------------------------------------------------------------------
albaGUI* albaVMEPolylineSpline::CreateGui()
{
	albaID sub_id = -1;

  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();

	m_Gui->Integer(ID_NUMBER_NODES,_("Degree"),&m_SplineCoefficient);

	albaVME *polyline_vme = GetPolylineLink();
  m_PolylineLinkName = polyline_vme ? polyline_vme->GetName() : _("none");
  m_Gui->Button(ID_LINK_POLYLINE,&m_PolylineLinkName,_("Polyline / Landmark Cloud"), _("Select the Polyline or Landmark Cloud to create the Spline"));
	m_Gui->Bool(ID_CLOSE_POLYLINE, "Close Polyline", &m_ClosePolyline, 1);
	m_Gui->Update();
	//this->InternalUpdate();

  return m_Gui;
}

//-------------------------------------------------------------------------
void albaVMEPolylineSpline::OnEvent(albaEventBase *alba_event)
{
  // events to be sent up or down in the tree are simply forwarded
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_LINK_POLYLINE:
      {
        albaID button_id = e->GetId();
        albaString title = _("Choose vme");
        e->SetId(VME_CHOOSE);
        e->SetPointer(&albaVMEPolylineSpline::PolylineAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        albaVME *n = e->GetVme();
        if (n != NULL)
        {
					SetPolylineLink(n);
					m_PolylineLinkName = n->GetName();
					InternalUpdate();
          GetPolylineOutput()->Update();
					m_Gui->Update();
					GetLogicManager()->CameraUpdate();
        }
      }
      break;
			case ID_CLOSE_POLYLINE:
			case ID_NUMBER_NODES:
				{
					InternalUpdate();
          GetPolylineOutput()->Update();
					Update();
					GetLogicManager()->CameraUpdate();
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

//-------------------------------------------------------------------------
void albaVMEPolylineSpline::OrderPolyline(vtkPolyData *polyline)
{
  //cell 
  vtkALBASmartPointer<vtkCellArray> cellArray;
	vtkIdType pointId[2];

  for(int i = 0; i< polyline->GetNumberOfPoints();i++)
  {
    if (i > 0)
    {             
      pointId[0] = i - 1;
      pointId[1] = i;
      cellArray->InsertNextCell(2 , pointId);  
    }
  }

  polyline->SetLines(cellArray);
  polyline->Modified();
  polyline->Update();
}

//-------------------------------------------------------------------------
void albaVMEPolylineSpline::SplinePolyline(vtkPolyData *polyline)
{
  // ALGORITHM
  m_PointsSplined->Reset();

  //cleaned point list
  vtkALBASmartPointer<vtkPoints> pts;

  pts->DeepCopy(polyline->GetPoints());

  /*vtkALBASmartPointer<vtkCellArray> lineCells;
  lineCells->InsertNextCell(pts->GetNumberOfPoints());
  for (int i = 0; i < pts->GetNumberOfPoints(); i ++)
  lineCells->InsertCellPoint(i);      */ 


  vtkALBASmartPointer<vtkCardinalSpline> splineX;
  vtkALBASmartPointer<vtkCardinalSpline> splineY;
  vtkALBASmartPointer<vtkCardinalSpline> splineZ;

  for(int i=0 ; i<pts->GetNumberOfPoints(); i++)
  {
    //albaLogMessage(albaString::Format(_("old %d : %f %f %f"), i, pts->GetPoint(i)[0],pts->GetPoint(i)[1],pts->GetPoint(i)[2] ));
    splineX->AddPoint(i, pts->GetPoint(i)[0]);
    splineY->AddPoint(i, pts->GetPoint(i)[1]);
    splineZ->AddPoint(i, pts->GetPoint(i)[2]);
  }

  for(int i=0 ; i<(pts->GetNumberOfPoints() * m_SplineCoefficient); i++)
  {		 
    double t;
    t = ( pts->GetNumberOfPoints() - 1.0 ) / ( pts->GetNumberOfPoints()*m_SplineCoefficient - 1.0 ) * i;
    m_PointsSplined->InsertPoint(i , splineX->Evaluate(t), splineY->Evaluate(t), splineZ->Evaluate(t));

  }

  /*if(m_MinimumSpacing != 0.)
    OptimizeMinimumSpacingSpline();*/

  polyline->SetPoints(m_PointsSplined);
  polyline->Update();
}

/*/-------------------------------------------------------------------------
void albaVMEPolylineSpline::OptimizeMinimumSpacingSpline()
{
  vtkPoints *points = vtkPoints::New();

  double minimumSpacing2 = m_MinimumSpacing * m_MinimumSpacing;
  double previousPoint[3];
  for(int i = 0; i< m_PointsSplined->GetNumberOfPoints(); i++)
  {
    if(i == 0)
    {
      m_PointsSplined->GetPoint(i,previousPoint);
      points->InsertNextPoint(previousPoint);
    }
    else
    {
      double controlPoint[3];
      m_PointsSplined->GetPoint(i,controlPoint);
      double distance2 = vtkMath::Distance2BetweenPoints(controlPoint,previousPoint);
      if(minimumSpacing2 < distance2)
      {
        points->InsertNextPoint(controlPoint);
        m_PointsSplined->GetPoint(i,previousPoint);
      }
    }
    

  }

  m_PointsSplined->DeepCopy(points);
  vtkDEL(points);
}*/

//-------------------------------------------------------------------------
mmaMaterial *albaVMEPolylineSpline::GetMaterial()
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((albaVMEOutputPolyline *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}

//-------------------------------------------------------------------------
void albaVMEPolylineSpline::SetColor(double r, double g, double b)
{
	albaVMEPolyline *polyline_vme = (albaVMEPolyline*)GetPolylineLink();

	if (polyline_vme)
	{
		polyline_vme->GetMaterial()->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
		polyline_vme->GetMaterial()->m_Diffuse[0] = r;
		polyline_vme->GetMaterial()->m_Diffuse[1] = g;
		polyline_vme->GetMaterial()->m_Diffuse[2] = b;
		polyline_vme->GetMaterial()->UpdateProp();
	}
}

//-------------------------------------------------------------------------
void albaVMEPolylineSpline::OrderPolylineByAxis(vtkPolyData* polyline, int axis)
{
  vtkALBASmartPointer<vtkPolyData> poly;
  poly->DeepCopy(polyline);
  poly->Update();

  vtkALBASmartPointer<vtkPoints> points;
  points->DeepCopy(poly->GetPoints());

  vtkALBASmartPointer<vtkCellArray> cells;
  cells->DeepCopy(poly->GetLines());

  double firstPoint[3], lastPoint[3];

  points->GetPoint(0, firstPoint);
  points->GetPoint(points->GetNumberOfPoints()-1, lastPoint);

  vtkALBASmartPointer<vtkPoints> newPoints;
  vtkALBASmartPointer<vtkCellArray> newLines;

  if(axis == AXIS_X)
  {
    //control first and last point x coordinate
    if(firstPoint[0] > lastPoint[0])
    {
      int i;
      for(i = points->GetNumberOfPoints()-1; i >= 0; i--)
      {
        newPoints->InsertNextPoint(points->GetPoint(i));
      }

      int j;
			vtkIdType pointId[2];
      for(j = 0; j< newPoints->GetNumberOfPoints();j++)
      {
        if (j > 0)
        {             
          pointId[0] = j - 1;
          pointId[1] = j;
          newLines->InsertNextCell(2 , pointId);  
        }
      }
    }
    else
      return;
    
  }
  else if(axis == AXIS_Y)
  {
    //control first and last point y coordinate
    if(firstPoint[1] > lastPoint[1])
    {
      int i;
      for(i = points->GetNumberOfPoints()-1; i >= 0; i--)
      {
        newPoints->InsertNextPoint(points->GetPoint(i));
      }

      int j;
			vtkIdType pointId[2];
      for(j = 0; j< newPoints->GetNumberOfPoints();j++)
      {
        if (j > 0)
        {             
          pointId[0] = j - 1;
          pointId[1] = j;
          newLines->InsertNextCell(2 , pointId);  
        }
      }
    }
    else
      return;
  }
  else if(axis == AXIS_Z)
  {
    //control first and last point z coordinate
    if(firstPoint[2] > lastPoint[2])
    {
      int i;
      for(i = points->GetNumberOfPoints()-1; i >= 0; i--)
      {
        newPoints->InsertNextPoint(points->GetPoint(i));
      }

      int j;
			vtkIdType pointId[2];
      for(j = 0; j< newPoints->GetNumberOfPoints();j++)
      {
        if (j > 0)
        {             
          pointId[0] = j - 1;
          pointId[1] = j;
          newLines->InsertNextCell(2 , pointId);  
        }
      }
    }
    else
      return;
  }
  else
    return;

  poly->SetPoints(newPoints);
  poly->SetLines(newLines);
  poly->Update();

  polyline->DeepCopy(poly);
  polyline->Update();
}
