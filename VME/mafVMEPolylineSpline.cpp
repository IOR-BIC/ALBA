/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEPolylineSpline.cpp,v $
  Language:  C++
  Date:      $Date: 2007-11-21 14:47:33 $
  Version:   $Revision: 1.8 $
  Authors:   Daniele Giunchi & Matteo Giacomoni
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

#include "mafVMEPolylineSpline.h"
#include "mafTransform.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafDataPipeCustom.h"
#include "mafVMEPolyline.h"
#include "mafVMEOutputPolyline.h"
#include "mmgGui.h"
#include "mmaMaterial.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFDataPipe.h"
#include "vtkPolyData.h"
#include "vtkCardinalSpline.h"
#include "vtkPoints.h"
#include "vtkTransform.h"
#include "vtkCellArray.h"
#include "vtkTransformPolyDataFilter.h"


#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEPolylineSpline)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEPolylineSpline::mafVMEPolylineSpline()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  mafVMEOutputPolyline *output=mafVMEOutputPolyline::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  DependsOnLinkedNodeOn();

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
	
	m_Polyline = NULL;
	vtkNEW(m_Polyline);
  dpipe->SetInput(m_Polyline);

	m_PointsSplined = NULL;
	vtkNEW(m_PointsSplined);

	m_SplineCoefficient = 20;
  m_OrderByAxisMode   = AXIS_NONE;
}
//-------------------------------------------------------------------------
mafVMEPolylineSpline::~mafVMEPolylineSpline()
//-------------------------------------------------------------------------
{
  vtkDEL(m_Polyline);
	vtkDEL(m_PointsSplined);
  mafDEL(m_Transform);
  SetOutput(NULL);
}
//-------------------------------------------------------------------------
int mafVMEPolylineSpline::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEPolylineSpline *splinePolyline = mafVMEPolylineSpline::SafeDownCast(a);
    mafNode *linked_node = splinePolyline->GetPolylineLink();
    if (linked_node)
    {
      this->SetPolylineLink(linked_node);
    }
    
    m_Transform->SetMatrix(splinePolyline->m_Transform->GetMatrix());

    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_Polyline);
      m_Polyline->Update();
    }
    m_SplineCoefficient = splinePolyline->m_SplineCoefficient;
    m_OrderByAxisMode = splinePolyline->m_OrderByAxisMode;

    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMEPolylineSpline::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((mafVMEPolylineSpline *)vme)->m_Transform->GetMatrix() && \
          GetPolylineLink() == ((mafVMEPolylineSpline *)vme)->GetPolylineLink() && \
          m_OrderByAxisMode == ((mafVMEPolylineSpline *)vme)->m_OrderByAxisMode &&
          m_SplineCoefficient == ((mafVMEPolylineSpline *)vme)->m_SplineCoefficient;

  }
  return ret;
}


//-------------------------------------------------------------------------
mafVMEOutputPolyline *mafVMEPolylineSpline::GetPolylineOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputPolyline *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEPolylineSpline::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool mafVMEPolylineSpline::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
bool mafVMEPolylineSpline::IsDataAvailable()
//-------------------------------------------------------------------------
{
  return GetPolylineLink()->IsDataAvailable();
}

//-------------------------------------------------------------------------
void mafVMEPolylineSpline::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-----------------------------------------------------------------------
void mafVMEPolylineSpline::InternalUpdate() //Multi
//-----------------------------------------------------------------------
{
  //wxBusyCursor wait;
	
  mafVMEPolyline *vme = mafVMEPolyline::SafeDownCast(GetPolylineLink());
  

	if (vme == NULL) 
	{ 
		return;
	}
  vme->Update();

  vtkPolyData *polyline = ((vtkPolyData *)vme->GetOutput()->GetVTKData());

  if(m_OrderByAxisMode) OrderPolylineByAxis(polyline, m_OrderByAxisMode);

  ////transform
  ////this part set the transform for every polydata inside a node, using the pose of the node
  vtkMAFSmartPointer<vtkTransform> transform;
  transform->SetMatrix(vme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
  transform->Update();

  vtkMAFSmartPointer<vtkTransformPolyDataFilter> tpdf;
  tpdf->SetInput(polyline);
  tpdf->SetTransform(transform);
  tpdf->Update();

  ForwardUpEvent(&mafEvent(this,PROGRESSBAR_SHOW));

  vtkMAFSmartPointer<vtkPolyData> poly;
  poly->DeepCopy(polyline);
  poly->Update();

  this->SplinePolyline(poly); // generate a "splined" polyline 

  this->OrderPolyline(poly); // create orderer sequence of points and cells

	m_Polyline->DeepCopy(poly);
	m_Polyline->Update();

	ForwardUpEvent(&mafEvent(this,PROGRESSBAR_HIDE));

	Modified();
}
//-----------------------------------------------------------------------
void mafVMEPolylineSpline::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  /*mafVME *vol = mafVMEVolume::SafeDownCast(GetParent());
  if(vol)
  {
    vol->Update();
    if (vtkDataSet *vtkdata=vol->GetOutput()->GetVTKData())
    {
      m_PSlicer->Update();
      m_ISlicer->Update();
    }
  }*/
}
//-----------------------------------------------------------------------
int mafVMEPolylineSpline::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    if(parent->StoreMatrix("Transform",&m_Transform->GetMatrix())==MAF_OK && 
       parent->StoreInteger("AxisReorder", m_OrderByAxisMode) == MAF_OK
      )    
      return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMEPolylineSpline::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      node->RestoreInteger("AxisReorder",m_OrderByAxisMode);
      m_Transform->SetMatrix(matrix);
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}

//-----------------------------------------------------------------------
void mafVMEPolylineSpline::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
char** mafVMEPolylineSpline::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}
//-------------------------------------------------------------------------
void mafVMEPolylineSpline::SetPolylineLink(mafNode *n)
//-------------------------------------------------------------------------
{
	SetLink("PolylineSource", n);
}
//-------------------------------------------------------------------------
mafVME *mafVMEPolylineSpline::GetPolylineLink()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("PolylineSource"));
}
//-------------------------------------------------------------------------
mmgGui* mafVMEPolylineSpline::CreateGui()
//-------------------------------------------------------------------------
{

	mafID sub_id = -1;

  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();

	m_Gui->Integer(ID_NUMBER_NODES,_("Degree"),&m_SplineCoefficient);

	mafVME *polyline_vme = GetPolylineLink();
  m_PolylineLinkName = polyline_vme ? polyline_vme->GetName() : _("none");
  m_Gui->Button(ID_LINK_POLYLINE,&m_PolylineLinkName,_("Polyline"), _("Select the Polyline to create the Spline"));

	m_Gui->Update();
	//this->InternalUpdate();

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEPolylineSpline::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_LINK_POLYLINE:
      {
        mafID button_id = e->GetId();
        mafString title = _("Choose vme");
        e->SetId(VME_CHOOSE);
        e->SetArg((long)&mafVMEPolylineSpline::PolylineAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();
        if (n != NULL)
        {
					SetPolylineLink(n);
					m_PolylineLinkName = n->GetName();
					InternalUpdate();
          GetPolylineOutput()->Update();
					m_Gui->Update();
        }
      }
      break;
			case ID_NUMBER_NODES:
				{
					InternalUpdate();
          GetPolylineOutput()->Update();
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
//-------------------------------------------------------------------------
void mafVMEPolylineSpline::OrderPolyline(vtkPolyData *polyline)
//-------------------------------------------------------------------------
{
  //cell 
  vtkMAFSmartPointer<vtkCellArray> cellArray;
  int pointId[2];

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
void mafVMEPolylineSpline::SplinePolyline(vtkPolyData *polyline)
//-------------------------------------------------------------------------
{
  // ALGORITHM
  m_PointsSplined->Reset();

  //cleaned point list
  vtkMAFSmartPointer<vtkPoints> pts;

  pts->DeepCopy(polyline->GetPoints());

  /*vtkMAFSmartPointer<vtkCellArray> lineCells;
  lineCells->InsertNextCell(pts->GetNumberOfPoints());
  for (int i = 0; i < pts->GetNumberOfPoints(); i ++)
  lineCells->InsertCellPoint(i);      */ 


  vtkMAFSmartPointer<vtkCardinalSpline> splineX;
  vtkMAFSmartPointer<vtkCardinalSpline> splineY;
  vtkMAFSmartPointer<vtkCardinalSpline> splineZ;

  for(int i=0 ; i<pts->GetNumberOfPoints(); i++)
  {
    //mafLogMessage(wxString::Format(_("old %d : %f %f %f"), i, pts->GetPoint(i)[0],pts->GetPoint(i)[1],pts->GetPoint(i)[2] ));
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

  polyline->SetPoints(m_PointsSplined);
  polyline->Update();

}
//-------------------------------------------------------------------------
mmaMaterial *mafVMEPolylineSpline::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((mafVMEOutputPolyline *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}
//-------------------------------------------------------------------------
void mafVMEPolylineSpline::OrderPolylineByAxis(vtkPolyData* polyline, int axis)
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyData> poly;
  poly->DeepCopy(polyline);
  poly->Update();

  vtkMAFSmartPointer<vtkPoints> points;
  points->DeepCopy(poly->GetPoints());

  vtkMAFSmartPointer<vtkCellArray> cells;
  cells->DeepCopy(poly->GetLines());

  double firstPoint[3], lastPoint[3];

  points->GetPoint(0, firstPoint);
  points->GetPoint(points->GetNumberOfPoints()-1, lastPoint);

  vtkMAFSmartPointer<vtkPoints> newPoints;
  vtkMAFSmartPointer<vtkCellArray> newLines;

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
      int pointId[2];
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
      int pointId[2];
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
      int pointId[2];
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
