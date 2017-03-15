/*=========================================================================

 Program: MAF2
 Module: mafVMELandmarkCloud
 Authors: Marco Petrone, Paolo Quadrani, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

const bool DEBUG_MODE = false;

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMELandmarkCloud.h"
#include "mmuIdFactory.h"
#include "mafDataVector.h"
#include "mafMatrixInterpolator.h"
#include "mafDataPipeInterpolator.h"
#include "mafTagArray.h"
#include "mafVMELandmark.h"
#include "mafMatrixVector.h"
#include "mafVMEItemVTK.h"
#include "mafVMEOutputLandmarkCloud.h"
#include "mafGUI.h"
#include "mmaMaterial.h"
#include "mafVMEIterator.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkBitArray.h"
#include "vtkPointData.h"
#include "vtkTransform.h"
#include "vtkDataSet.h"
#include "vtkDataSetWriter.h"

#include <vector>

#include <wx/busyinfo.h>
#include "mafProgressBarHelper.h"
#include "vtkMath.h"
//------------------------------------------------------------------------------
// local defines

#define MAF_LMC_ITEMS_NUMBER_TAG "MFL_VME_NUMBER_OF_LANDMARKS"
#define MAF_LMC_VERSION "MAF_LMC_VERSION"

//------------------------------------------------------------------------------
// Events

MAF_ID_IMP(mafVMELandmarkCloud::CLOUD_RADIUS_MODIFIED); // Event rised when the radius is changed with a SetRadius()
MAF_ID_IMP(mafVMELandmarkCloud::CLOUD_SPHERE_RES); // Event rised when the sphere resolution is changed with a SetSphereResolution()

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafVMELandmarkCloud);


//-------------------------------------------------------------------------
mafVMELandmarkCloud::mafVMELandmarkCloud()
{
  SetOutput(mafVMEOutputLandmarkCloud::New()); // create the output

	m_StoreDataVector = false;

  m_NumberOfLandmarks = 0;
  m_DefaultVisibility = 1;
  m_Radius            = 1.0;
  m_SphereResolution  = 15;

  m_SingleFile = 0;
	m_LanfmarkShowNumber = 0;
}

//-------------------------------------------------------------------------
mafVMELandmarkCloud::~mafVMELandmarkCloud()
{
	m_LMChildren.clear();
	m_LMChildrenShow.clear();
	m_LMIndexesMap.clear();
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::DeepCopy(mafVME *a)
//-------------------------------------------------------------------------
{ 
	if (Superclass::DeepCopy(a) == MAF_OK)
	{
		mafVMELandmarkCloud *lc = mafVMELandmarkCloud::SafeDownCast(a);
		m_Radius = lc->GetRadius();
		m_DefaultVisibility = lc->GetDefaultVisibility();
		m_SphereResolution = lc->GetSphereResolution();

		//Removing current Data Vector
		m_DataVector->RemoveAllItems();

		GetTagArray()->SetTag(mafTagItem(MAF_LMC_VERSION, 2));

    return MAF_OK;
  }  
  return MAF_ERROR;
}

//----------------------------------------------------------------------------
void mafVMELandmarkCloud::CopyLandmarks(mafVMELandmarkCloud *lmc)
{
	if (lmc == NULL)
		return;

	int lmNum = lmc->GetNumberOfLandmarks();
	for (int i = 0; i < lmNum; i++)
	{
		mafVMELandmark *lm;
		mafNEW(lm);
		lm->DeepCopy(lmc->GetLandmark(i));
		lm->ReparentTo(this);
		mafDEL(lm);
	}

}

//-------------------------------------------------------------------------
bool mafVMELandmarkCloud::Equals(mafVME *vme)
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    mafVMELandmarkCloud *lc = mafVMELandmarkCloud::SafeDownCast(vme);
    ret = (m_Radius == lc->GetRadius() && 
           m_NumberOfLandmarks == lc->GetNumberOfLandmarks() &&
           m_DefaultVisibility == lc->GetDefaultVisibility() &&
           m_SphereResolution  == lc->GetSphereResolution());
  }
  return ret;
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::InternalInitialize()
{


	const int closed_cloud_state = 1;
	const int open_cloud_state = 2;

  if (Superclass::InternalInitialize()==MAF_OK)
  {
    //Old versions of LC does not have MAF_LMC_VERSION tag
		//If we have and old LC it can be open or closed 
		//closed LC contains LMs as combination of vtk data (for position) and tags (for names)
		mafTagItem *tag;
		mafTagArray * tagArray = GetTagArray();

		if ((!tagArray->IsTagPresent(MAF_LMC_VERSION)) && (tag = tagArray->GetTag("MFL_VME_LANDMARK_CLOUD_STATE")) && (tag->GetValueAsDouble() == closed_cloud_state))
			CreateLMStructureFromDataVector();
		else if (!m_DataVector->GetItem(m_CurrentTime))
			NewPolyData(m_CurrentTime);

		//Setting state tag to Open Cloud in order to maintain backward compatibility
		GetTagArray()->SetTag(mafTagItem("MFL_VME_LANDMARK_CLOUD_STATE", open_cloud_state));

    GetMaterial();


    return MAF_OK;
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
{
  assert(data);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);

	//Remove all landmarks
	//SetNumberOfLandmarks(0);

  if (polydata != NULL && Superclass::SetData(polydata,t,mode)==MAF_OK)
  {
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
unsigned long mafVMELandmarkCloud::GetMTime()
{
  unsigned long mtime = Superclass::GetMTime();
  unsigned long mtimelm;
  for (int i = 0; i < GetNumberOfChildren(); i++)
  {
    mafVMELandmark *vme = mafVMELandmark::SafeDownCast(GetChild(i));
    if (vme)
    {
      mtimelm = vme->GetMTime();
      mtime = (mtimelm > mtime) ? mtimelm : mtime;
    }
  }
  return mtime;
}
//-------------------------------------------------------------------------
int mafVMELandmarkCloud::GetNumberOfLandmarks()
{
	return m_NumberOfLandmarks;
}


//-------------------------------------------------------------------------
int mafVMELandmarkCloud::SetNumberOfLandmarks(int num)
{  
  int oldnum = GetNumberOfLandmarks();

  if (num == oldnum)
    return MAF_OK;
  else if (num > oldnum)
  {
		//if(m_DataVector->GetNumberOfItems()==0)
		//	NewPolyData(m_CurrentTime);

    // add a new point and a new vertex to all items
    for (mafDataVector::Iterator it = m_DataVector->Begin(); it != m_DataVector->End(); it++)
    {
      mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
      assert(item);
      vtkPolyData *polydata = vtkPolyData::SafeDownCast(item->GetData());

      int idx = -1;
      // set the right number of points for each item
      // In case an item with a different number of points is added, 
      // this function is called to flat the number of points in all 
      // the time frames. See AddItem()
      int itemoldnum = polydata->GetNumberOfPoints();

      idx = AppendPoint(polydata,0,0,0,num-oldnum);
      if (idx != (num-1) && (itemoldnum != num))
      {
        mafErrorMacro("AppendLandmark: unexpected number of landmarks for timestamp t = "<<item->GetTimeStamp());
        return MAF_ERROR;
      }  
    }

  }
	else //(num < oldnum) 
	{
		// remove point from all items
		for (mafDataVector::Iterator it = m_DataVector->Begin(); it != m_DataVector->End(); it++)
		{
			mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
			assert(item);
			vtkPolyData *polydata = vtkPolyData::SafeDownCast(item->GetData());

			int itemoldnum = polydata->GetNumberOfPoints();
			for (int j = itemoldnum - 1; j >= num; j--)
			{
				if (RemovePoint(polydata, j) != MAF_OK)
				{
					mafErrorMacro("RemoveLandmark: unexpected number of landmarks for timestamp t=" << item->GetTimeStamp());
					return MAF_ERROR;
				}
			}
		}
	}

  m_NumberOfLandmarks = num;

  return MAF_OK;
}


//----------------------------------------------------------------------------
int mafVMELandmarkCloud::AppendLandmark(double x, double y, double z, const char *name)
{
	if (GetLandmarkIndex(name) >= 0)
	{
		mafErrorMacro("Cannot add new landmark \"" << name << "\": a landmark with the same name already exists!!!");
		return -1;
	}
	
	mafVMELandmark *newLM;
	mafNEW(newLM);
	newLM->SetName(name);
	newLM->SetPoint(x, y, z);
	newLM->ReparentTo(this);
	
	return m_NumberOfLandmarks-1;
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::SetLandmark(int idx, double x, double y, double z, mafTimeStamp t)
{
	if (idx < 0)
	{
		mafErrorMacro("SetLandmark: point index out of range");
		return MAF_ERROR;
	}

	mafVMELandmark *lm = GetLandmark(idx);
	if (!lm)
	{
		mafErrorMacro("Problems setting position of " << idx << "th landmark in the open cloud");
		return MAF_ERROR;
	}

	lm->SetPose(x, y, z, 0, 0, 0, t);

	return SetLandmarkToPolydata(idx, x, y, z, true, t);
}

//----------------------------------------------------------------------------
int mafVMELandmarkCloud::SetLandmarkToPolydata(int idx, double x, double y, double z,bool visibility, mafTimeStamp &t)
{
	t = t < 0 ? m_CurrentTime : t;

	// Retrieve polydata for time t or create a new one
	vtkPolyData *polydata = GetPolyData(t);
	if (!polydata)
		polydata = NewPolyData(t);

	Superclass::SetPoint(polydata, idx, x, y, z);
	
	return SetLandmarkVisibility(polydata, idx, visibility);
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::GetLandmarkIndex(const char *name)
{
  int numberOfLandmarks = GetNumberOfLandmarks();

  for (int i = 0; i < numberOfLandmarks; i++)
  {
    if (!strcmp(GetLandmarkName(i),name))
      return i;
  }

  return -1;
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::GetLandmark(int idx, double &x,double &y,double &z,mafTimeStamp t)
{
  return Superclass::GetPoint(idx,x,y,z,t);
}
//-------------------------------------------------------------------------
int mafVMELandmarkCloud::GetLandmark(int idx, double xyz[3],mafTimeStamp t)
{
  return Superclass::GetPoint(idx,xyz,t);
}

//----------------------------------------------------------------------------
int mafVMELandmarkCloud::GetLandmarkIndex(mafVMELandmark *lm)
{
	std::map<mafVMELandmark *, int>::iterator it = m_LMIndexesMap.find(lm);
	
	if (it != m_LMIndexesMap.end())
		return it->second;
	else
		return -1;
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::RemoveLandmark(int idx)
{
	int num_landmarks = GetNumberOfLandmarks();

	if (idx < 0 || idx >= num_landmarks)
	{
		mafErrorMacro("RemoveLandmark: landmark index out of range");
		return MAF_ERROR;
	}

	mafVMELandmark *lm = GetLandmark(idx);
	if (!lm)
	{
		mafErrorMacro("Problems removing of " << idx << "th landmark in the cloud");
		return MAF_ERROR;
	}

	//removing vme child
	RemoveChild(lm);
	   
  return MAF_OK;
}
//-------------------------------------------------------------------------
const char *mafVMELandmarkCloud::GetLandmarkName(int idx)
{
  mafVME *vme = GetLandmark(idx);
  return vme ? vme->GetName() : NULL;
}

//-------------------------------------------------------------------------
void mafVMELandmarkCloud::SetLandmarkName(int idx,const char *name)
{
	GetLandmark(idx)->SetName(name);
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::AppendPoint(vtkPolyData *polydata,double x,double y,double z,int num)
{
  int oldnum = polydata->GetNumberOfPoints();
  int idx = Superclass::AppendPoint(polydata,x,y,z,num);

  // test if the right number of points have been inserted
  if (idx-oldnum+1 == num)
  {
    // retrieve cell array
    vtkCellArray *cells = polydata->GetVerts();

    // retrieve scalars for visibility attribute
    vtkPointData* pdata=polydata->GetPointData();
    vtkBitArray *data=vtkBitArray::SafeDownCast(pdata->GetScalars());

    // if data structure is OK...
    if (cells && data)
    {
      for (int i = oldnum; i < oldnum+num; i++)
      {
        // set visibility scalar
        data->InsertNextValue(m_DefaultVisibility);
        // if visible by default add also the cell
        if (m_DefaultVisibility)
        {
          cells->InsertNextCell(1);
          cells->InsertCellPoint(i);
        }
      }
      cells->Modified();
      polydata->Modified();
    }
    else
    {
      mafErrorMacro("AppendPoint: Corrupted PolyData inside VME-LandmarkCloud: Cannot found Vertex cells or point scalars.");
    }
  }
  else
  {
    mafErrorMacro("AppendPoint: DEBUG, cannot add points to cloud...");
  }
  return idx;
}

//----------------------------------------------------------------------------
void mafVMELandmarkCloud::CreateLMStructureFromDataVector()
{
	mafTimeStamp currentTime = GetTimeStamp();
	mafTagArray *tarray = GetTagArray();
	double x, y, z;
	std::vector <mafVMELandmark*> landmarks;

	
	std::vector<std::string> tag_list;
	tarray->GetTagList(tag_list);
	
	//Creating landmarks from Tags --USED TO RESTORE OLD LMC--
	for (int i = 0; i < tag_list.size(); i++)
	{
		mafString tagName = tag_list[i].c_str();
		if (tagName.StartsWith("LM_NAME_"))
		{
			mafTagItem *tag = tarray->GetTag(tagName);
			const char *name = tag->GetValue();

			mafVMELandmark *newLM;
			mafNEW(newLM);
			newLM->SetName(name);
			landmarks.push_back(newLM);
		}
	}

	//if no tags were stored we read create landmarks from data vector
	if (landmarks.size() == 0)
	{
		mafDataVector::Iterator it = m_DataVector->Begin();
		mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
		int nPoints=item->GetData()->GetNumberOfPoints();
		for (int i = 0; i < nPoints; i++)
		{
			mafString name;
			name = "Landmark ";
			name << i;

			mafVMELandmark *newLM;
			mafNEW(newLM);
			newLM->SetName(name);
			landmarks.push_back(newLM);
		}
	}

	//Setting positions to landmarks
	for (mafDataVector::Iterator it = m_DataVector->Begin(); it != m_DataVector->End(); it++)
	{
		double xyz[3];
		mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
		
		assert(item);

		vtkPolyData *polydata = (vtkPolyData *)item->GetData();
		//polydata->Update();
		mafTimeStamp timeStamp = item->GetTimeStamp();
		
		if (!polydata)
		{
			mafErrorMacro("Open: problems retrieving polydata for timestamp: " << timeStamp);
			continue;
		}
		for(int i=0; i<landmarks.size();i++)
		{
			bool vis = GetLandmarkVisibility(polydata, i);
			GetPoint(polydata, i, xyz);

			landmarks[i]->SetPoint(xyz, timeStamp);
			landmarks[i]->SetLandmarkVisibility(vis, timeStamp);
		}
	}

	//Clean current data vector
	m_DataVector->RemoveAllItems();
	SetNumberOfLandmarks(0);

	//Reparent created landmark
	for (int i = 0; i < landmarks.size(); i++)
		landmarks[i]->ReparentTo(this);
	
	landmarks.clear();
}

//----------------------------------------------------------------------------
mafVMELandmark * mafVMELandmarkCloud::GetNearestLandmark(double pos[3], bool SerchOnlyVisible)
{
	int lmIndex = -1;
	int num = GetNumberOfLandmarks();
	double dist = VTK_DOUBLE_MAX;
	double lmPos[3];
	
	for (int idx = 0; idx < num; idx++)
	{
		mafVMELandmark *lm = GetLandmark(idx);

		if (lm && (!SerchOnlyVisible || m_LMChildrenShow[idx]))
		{
			lm->GetPoint(lmPos);
			double curDist=vtkMath::Distance2BetweenPoints(lmPos, pos);
			if (curDist < dist)
			{
				lmIndex = idx;
				dist = curDist;
			}
		}
	}

	if (lmIndex >= 0 && lmIndex < m_NumberOfLandmarks)
		return m_LMChildren[lmIndex];
	else
		return NULL;
}

//-------------------------------------------------------------------------
vtkPolyData *mafVMELandmarkCloud::NewPolyData(mafTimeStamp t)
{
  vtkPolyData *polydata = Superclass::NewPolyData(t);
  if (polydata)
  {
    // add cell array for visible points
    vtkMAFSmartPointer<vtkCellArray> cells;
    polydata->SetVerts(cells);

    // add scalars for visibility attribute
    vtkPointData* pdata = polydata->GetPointData();
    vtkMAFSmartPointer<vtkBitArray> data;
    pdata->SetScalars(data);
    AppendPoint(polydata,0,0,0,GetNumberOfLandmarks());
  }
  return polydata;
}

//-------------------------------------------------------------------------
void mafVMELandmarkCloud::SetRadius(double rad, bool force_update)
{
  if (mafEquals(rad, m_Radius) && !force_update)
    return;

  m_Radius = rad;
  Modified();
  InvokeEvent(mafVMELandmarkCloud::CLOUD_RADIUS_MODIFIED);
}

//-------------------------------------------------------------------------
double mafVMELandmarkCloud::GetRadius()
{
  return m_Radius;
}

//-------------------------------------------------------------------------
void mafVMELandmarkCloud::SetSphereResolution(int res, bool force_update)
{
  if (mafEquals(res, m_SphereResolution) && !force_update)
    return;

  m_SphereResolution = res;
  Modified();

  InvokeEvent(mafVMELandmarkCloud::CLOUD_SPHERE_RES);
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::GetSphereResolution()
{
  return m_SphereResolution;
}

//-------------------------------------------------------------------------
mafVMELandmark *mafVMELandmarkCloud::GetLandmark(const char *name)
{
  for (int i = 0; i < m_NumberOfLandmarks; i++)
  {
    mafVMELandmark *lm = m_LMChildren[i];
    if (lm && mafCString(lm->GetName()) == name)
      return lm;
  }
  
	mafLogMessage("Landmark %s not found", name);
  return NULL;
}
//-------------------------------------------------------------------------
mafVMELandmark *mafVMELandmarkCloud::GetLandmark(int idx)
{
	if (idx < 0 || idx >= m_NumberOfLandmarks)
		return NULL;
	else
		return m_LMChildren[idx];
}
//-------------------------------------------------------------------------
void mafVMELandmarkCloud::GetLandmarkPosition(int idx, double pos[3], mafTimeStamp t)
{
    GetPoint(idx,pos,t);
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::SetLandmarkVisibility(int idx,bool a,mafTimeStamp t)
//-------------------------------------------------------------------------
{

	//Setting visibility to the landmark
	mafVMELandmark *lm = GetLandmark(idx);
	if (!lm)
	{
		mafErrorMacro("SetVisibility: problems retrieving " << idx << "th child landamark for setting its visibility!");
		return MAF_ERROR;
	}
	lm->SetLandmarkVisibility(a, t);

	//Setting visibility to the cloud
  vtkPolyData *polydata = GetPolyData(t);
  if (polydata)
  {
    if (GetLandmarkVisibility(polydata,idx) == a)
      return MAF_OK;
  }
  else
  {
    polydata = NewPolyData(t);
		if (!polydata)
		{
			mafErrorMacro("SetVisibility: problems generarting Polydata at time" << t);
			return MAF_ERROR;
		}
    if ((a && m_DefaultVisibility) || (!a && !m_DefaultVisibility))
      return MAF_OK;
  }

  return SetLandmarkVisibility(polydata, idx, a);
}

//-------------------------------------------------------------------------
bool mafVMELandmarkCloud::GetLandmarkVisibility(int idx,mafTimeStamp t)
{
	//Getting Visibility from the LM
  if (mafVMELandmark *lm = GetLandmark(idx))
  {
    return lm->GetLandmarkVisibility(t);
  }
  else
  {
    mafErrorMacro("GetVisibility: problems retrieving "<<idx<<"th child landamark for getting its visibility!");
    return MAF_ERROR;
  }
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::SetLandmarkVisibility(vtkPolyData *polydata,int idx,bool a)
{
  if (idx >= polydata->GetNumberOfPoints())
  {
    mafErrorMacro("SetLandmarkVisibility: point index out of range");
    return MAF_ERROR;
  }
  vtkPointData* pdata = polydata->GetPointData();
  vtkBitArray *scalars = NULL;
  
  scalars = (vtkBitArray *)(pdata->GetScalars());
  if (scalars)
  {
    if ((scalars->GetValue(idx) != 0) == a)
      return MAF_OK;
  }
  else
  {
    mafErrorMacro("GetLandmarkVisibility: Corrupted PolyData inside VME-PointSet, cannot found visibility scalars");
  }

  vtkCellArray *cells = polydata->GetVerts();
  if (cells && scalars)
  {
    scalars->SetValue(idx,a);
    if (a)
    {
      cells->InsertNextCell(1);
      cells->InsertCellPoint(idx);
      cells->Modified();
    }
    else
    {
      //
      // Create a new CellArray without the cell of the invisible the point.
      //
      vtkMAFSmartPointer<vtkCellArray> newcells;
      cells->InitTraversal();        
      for (int i = 0; i < cells->GetNumberOfCells(); i++)
      {
        vtkIdType npts; 
        vtkIdType *pts;
        cells->GetNextCell(npts, pts);
        if (npts != 1)
        {
          mafErrorMacro("SetLandmarkVisibility: Corrupted polydata, found a cell with erong number of points: "<<npts );
        }
        else if (pts[0] != idx)
        {
          newcells->InsertNextCell(1);
          newcells->InsertCellPoint(pts[0]);
        }
      }
      polydata->SetVerts(newcells); // old cells should die at this point and cells to become invalid...
      // remove references to cells from given point to avoid unconsistencies...
      polydata->DeleteCells();
    }  
    polydata->Modified();
    return MAF_OK;
  }
  else
  {
    mafErrorMacro("SetLandmarkVisibility: Corrupted PolyData inside VME-LandmarkCloud, cannot found Vertx cells or scalars");
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMELandmarkCloud::GetLandmarkVisibility(vtkPolyData *polydata,int idx)
{
  vtkPointData* pdata = polydata->GetPointData();
  vtkBitArray *scalars = NULL;
  if (pdata)
  {
    scalars = vtkBitArray::SafeDownCast(pdata->GetScalars());
  }
  if (scalars)
  {
    return scalars->GetValue(idx) != 0;
  }
  else
  {
    mafErrorMacro("GetLandmarkVisibility: Corrupted PolyData inside VME-PointSet, cannot found visibility scalars");
  }
  // for old files, visibility is true by default...
  return true;
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::GetNumberOfVisibleLandmarks(mafTimeStamp t)
{
  int num = GetNumberOfLandmarks();
  int numvis = 0;
  for (int idx = 0; idx < num; idx++)
  {
    if (GetLandmarkVisibility(idx,t))
      numvis++;
  }
  return numvis;
}

//-------------------------------------------------------------------------
bool mafVMELandmarkCloud::IsRigid()
{
  return m_DataVector->GetNumberOfItems() == 1;
}
//-------------------------------------------------------------------------
void mafVMELandmarkCloud::Print(std::ostream &os, const int tabs)
{
  mafIndent indent(tabs);
  Superclass::Print(os,indent);
 
  os << indent << "Current Landmarks State:"<<std::endl;
  for (int idx = 0; idx < GetNumberOfLandmarks();idx++)
  {
    double x,y,z;
    GetLandmark(idx,x,y,z,m_CurrentTime);
    os << indent << "LM: \""<<GetLandmarkName(idx)<<"\" (" \
      <<x<<","<<y<<","<<z<<") Visibility=" \
      << GetLandmarkVisibility(idx,m_CurrentTime)<<std::endl;
  }
}
//-------------------------------------------------------------------------
mafGUI* mafVMELandmarkCloud::CreateGui()
{
  m_Gui = mafVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);

	m_Gui->Divider();
  GetRadius(); // Called to update m_Radius var from tag

  m_Gui->Double(ID_LM_RADIUS, "radius", &m_Radius, 0.0,MAXDOUBLE,-1);
  m_Gui->Integer(ID_LM_SPHERE_RESOLUTION, "Resolution", &m_SphereResolution, 0.0,MAXINT);
  
  m_Gui->Divider();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMELandmarkCloud::OnEvent(mafEventBase *maf_event)
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_LM_RADIUS:
        SetRadius(m_Radius, true);
      break;
      case ID_LM_SPHERE_RESOLUTION:
        SetSphereResolution(m_SphereResolution, true);
      break;
      default:
        mafVME::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
int mafVMELandmarkCloud::InternalStore(mafStorageElement *parent)
{
	GetTagArray()->SetTag(mafTagItem(MAF_LMC_VERSION, 2));

  if (Superclass::InternalStore(parent) == MAF_OK)
  {
    if (parent->StoreInteger("LM_SPHERE_RESOLUTION", m_SphereResolution) == MAF_OK &&
        parent->StoreDouble("LM_RADIUS", m_Radius) == MAF_OK)
    {
      return MAF_OK;
    }
  }
 
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
int mafVMELandmarkCloud::InternalRestore(mafStorageElement *node)
{
  if (Superclass::InternalRestore(node) == MAF_OK)
  {
    if (node->RestoreInteger("LM_SPHERE_RESOLUTION", m_SphereResolution) == MAF_OK &&
        node->RestoreDouble("LM_RADIUS", m_Radius) == MAF_OK)
    {
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *mafVMELandmarkCloud::GetMaterial()
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((mafVMEOutputPointSet *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}
//-------------------------------------------------------------------------
char** mafVMELandmarkCloud::GetIcon() 
{
#include "mafVMELandmarkCloud.xpm"
  return mafVMELandmarkCloud_xpm;
}


//-------------------------------------------------------------------------
bool mafVMELandmarkCloud::IsDataAvailable()
{
  if (m_DataVector)
  {
    mafTimeStamp t = this->GetTimeStamp();
    mafVMEItem *item = m_DataVector->GetItem(t);
    if (item)
      item->UpdateData();

    if (IsAnimated() && item == NULL)
    {
      mafTimeStamp tbounds[2];
      m_DataVector->GetTimeBounds(tbounds);
      if (t < tbounds[0])
      {
        item = m_DataVector->GetItemByIndex(0);
      }
      else
      {
        item = m_DataVector->GetItemByIndex(m_DataVector->GetNumberOfItems()-1);
      }
    }

    bool available = false;
    // If item is NULL -> the data is generated by code
    // or is into its child as for the mafVMELandmarkCloud
    available =  (item ? item->IsDataPresent() : true); 

    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "bool mafVMELandmarkCloud::IsDataAvailable()" << std::endl
        << "Data for VME: " << this->GetName() << " is "  << \
        (available ? " available" : " not available") << std::endl;
      mafLogMessage(stringStream.str().c_str());
    }
    return available;
  }
  else
    return mafVME::IsDataAvailable();
}

//----------------------------------------------------------------------------
int mafVMELandmarkCloud::AddChild(mafVME *node)
{
	if (Superclass::AddChild(node) == MAF_ERROR)
		return MAF_ERROR;

	if (mafVMELandmark *lm=mafVMELandmark::SafeDownCast(node))
	{
		double lmPos[3];
		m_LMChildren.push_back(lm);
		m_LMChildrenShow.push_back(false);
		m_LMIndexesMap[lm] = m_NumberOfLandmarks;
		SetNumberOfLandmarks(m_NumberOfLandmarks + 1);
		
		mafMatrixVector * lmMatrixVector = lm->GetMatrixVector();
		for (mafMatrixVector::Iterator it = lmMatrixVector->Begin(); it != lmMatrixVector->End();it++)
		{
			mafMatrix *mat = mafMatrix::SafeDownCast(it->second);
			mafTimeStamp timeStamp = mat->GetTimeStamp();

			double pos[3];
			lm->GetPoint(pos, timeStamp);
			
			//On reparent the node is not showed, so visibility is set to false.
			SetLandmarkToPolydata(m_NumberOfLandmarks - 1, pos[0], pos[1], pos[2], false, timeStamp);
		}
		GetOutput()->Update();
	}
}

//----------------------------------------------------------------------------
void mafVMELandmarkCloud::RemoveChild(mafVME *node)
{
	Superclass::RemoveChild(node);

	if (mafVMELandmark *lm = mafVMELandmark::SafeDownCast(node))
	{
		int pos=GetLandmarkIndex(lm);
		if (pos < 0)
		{
			mafLogMessage("Warning trying to remove of unattached LandMark");
			return;
		}

		//Reassign maps for children with higer index
		for (int i = pos + 1; i < m_NumberOfLandmarks; i++)
			m_LMIndexesMap[m_LMChildren[i]]--;

		m_LMChildren.erase(m_LMChildren.begin() + pos);
		m_LMChildrenShow.erase(m_LMChildrenShow.begin() + pos);
		m_LMIndexesMap.erase(lm);

		// remove the point to all data items
		for (mafDataVector::Iterator it = m_DataVector->Begin(); it != m_DataVector->End(); it++)
		{
			mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
			assert(item);
			vtkPolyData *polydata = vtkPolyData::SafeDownCast(item->GetData());
			RemovePoint(polydata, pos);
		}

		GetOutput()->Update();

		m_NumberOfLandmarks--;
	}
}

//----------------------------------------------------------------------------
void mafVMELandmarkCloud::ShowLandmark(mafVMELandmark *lm, bool show)
{
	int idx = GetLandmarkIndex(lm);
	if (idx < 0 || idx >= m_NumberOfLandmarks)
	{
		mafErrorMacro("Error: try to show a wrong landmark");
	}
	
	//No status change, return
	if (show == m_LMChildrenShow[idx])
		return;

	m_LMChildrenShow[idx] = show;

	//Show the landmark over the data vector
	for (mafDataVector::Iterator it = m_DataVector->Begin(); it != m_DataVector->End(); it++)
	{
		mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
		assert(item);
		vtkPolyData *polydata = vtkPolyData::SafeDownCast(item->GetData());
		
		//The landmark is visible if show is true and is visibility is true at the specific time.
		bool lmVisbility = show && lm->GetLandmarkVisibility(item->GetTimeStamp());
		
		SetLandmarkVisibility(polydata, idx, lmVisbility);
	}

	if (show)
		m_LanfmarkShowNumber++;
	else
		m_LanfmarkShowNumber--;

	GetOutput()->Update();
}

//----------------------------------------------------------------------------
void mafVMELandmarkCloud::ShowAllLandmarks(bool show)
{
	for (int i = 0; i < m_NumberOfLandmarks; i++)
	{
		ShowLandmark(m_LMChildren[i], show);
	}
}

//----------------------------------------------------------------------------
bool mafVMELandmarkCloud::IsLandmarkShow(mafVMELandmark *lm)
{
	int idx = GetLandmarkIndex(lm);
	if (idx < 0 || idx >= m_NumberOfLandmarks)
	{
		mafErrorMacro("Error: required show status for a wrong landmark");
		return false;
	}
	return m_LMChildrenShow[idx];
}

//----------------------------------------------------------------------------
int mafVMELandmarkCloud::GetLandmarkShowNumber()
{
	return m_LanfmarkShowNumber;
}

