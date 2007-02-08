/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEPolylineSpline.cpp,v $
  Language:  C++
  Date:      $Date: 2007-02-08 16:43:40 $
  Version:   $Revision: 1.1 $
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

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
	
	m_Polyline = NULL;
	vtkNEW(m_Polyline);
  dpipe->SetInput(m_Polyline);

	m_PointsSplined = NULL;
	vtkNEW(m_PointsSplined);

	m_SplineCoefficient = 10;

	m_VMEPolylineAccept = new mafVMEPolylineAccept();

	DependsOnLinkedNodeOn();
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
  /*if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEPolylineSpline *vme_slicer=mafVMEPolylineSpline::SafeDownCast(a);
    m_Transform->SetMatrix(vme_slicer->m_Transform->GetMatrix());
    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetDependOnAbsPose(true);
      dpipe->SetInput(m_BackTransform->GetOutput());
      dpipe->SetNthInput(1,m_PSlicer->GetTexture());
    }
    GetMaterial()->SetMaterialTexture(m_PSlicer->GetTexture());
    return MAF_OK;
  }
  return MAF_ERROR;*/
	return MAF_OK;
}

//-------------------------------------------------------------------------
bool mafVMEPolylineSpline::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  /*if (Superclass::Equals(vme))
  {
    return m_Transform->GetMatrix()==((mafVMEPolylineSpline *)vme)->m_Transform->GetMatrix();
  }*/
  return false;
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
  return GetParent()->IsDataAvailable();
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
  wxBusyCursor wait;
	
  mafVMEPolyline *vme = mafVMEPolyline::SafeDownCast(GetPolylineLink());
	if (vme == NULL) 
	{ 
		return;
	}
  vme->Update();

  ////transform
  ////this part set the transform for every polydata inside a node, using the pose of the node
  vtkMAFSmartPointer<vtkTransform> transform;
  transform->SetMatrix(vme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
  transform->Update();

  vtkMAFSmartPointer<vtkTransformPolyDataFilter> tpdf;
  tpdf->SetInput(((vtkPolyData *)vme->GetOutput()->GetVTKData()));
  tpdf->SetTransform(transform);
  tpdf->Update();

  ForwardUpEvent(&mafEvent(this,PROGRESSBAR_SHOW));

  // ALGORITHM

   m_PointsSplined->Reset();

	 //cleaned point list
	 vtkMAFSmartPointer<vtkPoints> pts;
   
	 pts->DeepCopy(((vtkPolyData*)vme->GetOutput()->GetVTKData())->GetPoints());

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

	 vtkMAFSmartPointer<vtkCellArray> cellArray;
	 int pointId[2];

	 for(int i = 0; i< m_PointsSplined->GetNumberOfPoints();i++)
   {
     if (i > 0)
     {             
       pointId[0] = i - 1;
       pointId[1] = i;
       cellArray->InsertNextCell(2 , pointId);  
     }
   }

	 vtkMAFSmartPointer<vtkPolyData> poly;
	 poly->SetPoints(m_PointsSplined);
	 poly->SetLines(cellArray);
	 poly->Update();

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
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
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
        e->SetArg((long)m_VMEPolylineAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();
        if (n != NULL)
        {
					SetPolylineLink(n);
					m_PolylineLinkName = n->GetName();
					InternalUpdate();
					m_Gui->Update();
        }
      }
      break;
			case ID_NUMBER_NODES:
				{
					InternalUpdate();
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