/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMELandmarkCloud.cpp,v $
  Language:  C++
  Date:      $Date: 2005-10-10 15:16:03 $
  Version:   $Revision: 1.13 $
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

#include "mafVMELandmarkCloud.h"
#include "mmuIdFactory.h"
#include "mafDataVector.h"
#include "mafMatrixInterpolator.h"
#include "mafDataInterpolator.h"
#include "mafTagArray.h"
#include "mafVMELandmark.h"
#include "mafMatrixVector.h"
#include "mafVMEItemVTK.h"
#include "mafEventSource.h"
#include "mafVMEOutputLandmarkCloud.h"
#include "mmgGui.h"
#include "mmaMaterial.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkBitArray.h"
#include "vtkPointData.h"
#include "vtkTransform.h"

#include <vector>

//------------------------------------------------------------------------------
// local defines
//------------------------------------------------------------------------------
#define MAF_LMC_ITEMS_NUMBER_TAG "MFL_VME_NUMBER_OF_LANDMARKS"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafVMELandmarkCloud::CLOUDE_OPEN_CLOSE);   // Event rised by Open and Close functions 
MAF_ID_IMP(mafVMELandmarkCloud::CLOUDE_RADIUS_MODIFIED); // Event rised when the radius is changed with a SetRadius()
MAF_ID_IMP(mafVMELandmarkCloud::CLOUDE_SPHERE_RES); // Event rised when the sphere resolution is changed with a SetSphereResolution()

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafVMELandmarkCloud);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafVMELandmarkCloud::mafVMELandmarkCloud()
//-------------------------------------------------------------------------
{
  SetOutput(mafVMEOutputLandmarkCloud::New()); // create the output

  m_NumberOfLandmarks = -1;
  m_State             = UNSET_CLOUD;
  m_DefaultVisibility = 1;
  m_Radius            = -1;
  m_SphereResolution  = -1;
}

//-------------------------------------------------------------------------
mafVMELandmarkCloud::~mafVMELandmarkCloud()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMELandmarkCloud *lc = mafVMELandmarkCloud::SafeDownCast(a);
    m_Radius            = lc->GetRadius();
    m_NumberOfLandmarks = lc->GetNumberOfLandmarks();
    m_State             = lc->IsOpen()? mafVMELandmarkCloud::OPEN_CLOUD : mafVMELandmarkCloud::CLOSED_CLOUD;
    m_DefaultVisibility = lc->GetDefaultVisibility();
    m_SphereResolution  = lc->GetSphereResolution();
    lc->SetState(m_State);

    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMELandmarkCloud::Equals(mafVME *vme)
//-------------------------------------------------------------------------
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
int mafVMELandmarkCloud::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);

  if (Superclass::SetData(polydata,t,mode)==MAF_OK)
  {
    int oldnum = GetNumberOfLandmarks();
    if (polydata->GetNumberOfPoints() != oldnum)
      SetNumberOfLandmarks(oldnum > polydata->GetNumberOfPoints() ? oldnum : polydata->GetNumberOfPoints());
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
int mafVMELandmarkCloud::GetNumberOfLandmarks()
//-------------------------------------------------------------------------
{
  if (GetState() == CLOSED_CLOUD)
  {
    if (m_NumberOfLandmarks < 0)
    {
      mafTagItem *item = GetTagArray()->GetTag(MAF_LMC_ITEMS_NUMBER_TAG);
      if (item)
      {
        m_NumberOfLandmarks = item->GetValueAsDouble();
      }
      else
      {
        // try counting the number of landmarks (this is for compatibility with 
        // old MSF files
        mafTagArray *tarray=GetTagArray();
        m_NumberOfLandmarks=0;
        std::vector<std::string> tag_list;
        tarray->GetTagList(tag_list);
        for (int i = 0; i < tag_list.size(); i++)
        {
          mafString name = tag_list[i].c_str();
          if (name.StartsWith("LM_NAME_"))
          {
            m_NumberOfLandmarks++;
          }
        }
      }
    }
  }
  else
  {
    int num = 0;
    for (int i = 0; i < GetNumberOfChildren(); i++)
    {
      mafVMELandmark *vme = mafVMELandmark::SafeDownCast(GetChild(i));
      if (vme)
        num++;
    }
    return num;
  }
  return m_NumberOfLandmarks;
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::SetNumberOfLandmarks(int num)
//-------------------------------------------------------------------------
{  
  int oldnum = GetNumberOfLandmarks();

  if (num == oldnum)
    return MAF_OK;

  if (num > oldnum)
  {
    if (GetState() == OPEN_CLOUD)
    {
      // add default names to new landmarks
      for (int n = oldnum; n < num; n++)
      {
        mafSmartPointer<mafVMELandmark> lm;
        Superclass::AddChild(lm);

        mafString name;
        name = "LM_NAME_";
        name << n;
        SetLandmarkName(n, name);
      }
    }
    else
    {
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

        idx = AppendPoint(polydata,0,0,0,num-itemoldnum);
        if (idx != (num-1) && (itemoldnum != num))
        {
          mafErrorMacro("AppendLandmark: unexpected number of landmarks for timestamp t = "<<item->GetTimeStamp());
          return MAF_ERROR;
        }  
      }

      // add default names to new landmarks
      for (int n = oldnum; n < num; n++)
      {
        const char *oldname = GetLandmarkName(n);
        // it could be the name already exist...
        if (oldname == NULL)
        {
          mafString name;
          name = "LM_NAME_";
          name << n;
          SetLandmarkName(n,name);
        }
      }
    }
  }
  else
  {
    if (GetState() == OPEN_CLOUD)
    {
      for (int n = oldnum-1; n >= num; n--)
      {
        // if the cloud is open remove the extra children
        if (mafVMELandmark *lm = GetLandmark(n))
        {
          RemoveChild(lm);
        }
        else
        {
          mafErrorMacro("RemoveLandmark: cannot find "<<n<<"th child landmark to be removed");
          return MAF_ERROR;
        }
      }
    }
    else
    {
      // remove point from all items
      for (mafDataVector::Iterator it = m_DataVector->Begin(); it != m_DataVector->End(); it++)
      {
        mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
        assert(item);
        vtkPolyData *polydata=vtkPolyData::SafeDownCast(item->GetData());
        
        int itemoldnum = polydata->GetNumberOfPoints();
        for (int j = itemoldnum-1; j >= num; j--)
        {
          if (RemovePoint(polydata,j) != MAF_OK)
          {
            mafErrorMacro("RemoveLandmark: unexpected number of landmarks for timestamp t="<<item->GetTimeStamp());
            return MAF_ERROR;
          }
        }
      }
      for (int n = oldnum-1; n >= num; n--)
      {
        // remove also the Landmark name from tag array
        RemoveLandmarkName(n);
      }
    }
  }
  m_NumberOfLandmarks = num;
  GetTagArray()->SetTag(mafTagItem(MAF_LMC_ITEMS_NUMBER_TAG,num));

  return MAF_OK;
}
//-------------------------------------------------------------------------
// this is a protected function to be used only internally
void mafVMELandmarkCloud::RemoveLandmarkName(int idx)
//-------------------------------------------------------------------------
{ 
  mafString tagname="LM_NAME_";
  tagname << idx;
  GetTagArray()->DeleteTag(tagname);
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::AppendLandmark(const char *name)
//-------------------------------------------------------------------------
{
  if (FindLandmarkIndex(name) >= 0)
  {
    mafErrorMacro("Cannot add new landmark \""<<name<<"\": a landmark with the same name already exists!!!");
    return -1;
  }

  int old = GetNumberOfLandmarks();
  int ret = SetNumberOfLandmarks(old+1);

  if (ret!=MAF_OK)
    return -1;

  SetLandmarkName(old,name);

  return old;
}
/*
//-------------------------------------------------------------------------
int mafVMELandmarkCloud::SetLandmarkForTimeFrame(int idx,double x,double y,double z,unsigned long tid,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  int num = GetNumberOfLandmarks();
  if (idx >= num)
  {
    mafErrorMacro("SetLandmarkForTimeFrame: Landmark index out of range");
    return MAF_ERROR;
  }
  if (GetState() == CLOSED_CLOUD)
  { 
    vtkPolyData   *polydata = NULL;
    mafVMEItemVTK *item;
    item = GetItem(tid);
    if (item)
    { 
      if (item->GetTimeStamp() == t)
      {
        polydata = (vtkPolyData *)item->GetData();
      }
      else
      {
        mafErrorMacro("SetLandmarkForTimeFrame: time stamp for item "<<tid<<" is not matching the provided t="<<t);
        return MAF_ERROR;
      }
    }
    else
    {
      polydata = NewPolyData(t);
    }
    Superclass::SetPoint(polydata,idx,x,y,z);
    return SetLandmarkVisibility(polydata,idx,true);
  }
  else
  {
    mafErrorMacro("SetLandmarkForTimeFrame can work only on closed clouds");
    return MAF_ERROR;
  }
}*/
//-------------------------------------------------------------------------
int mafVMELandmarkCloud::SetLandmark(mafVMELandmark *lm)
//-------------------------------------------------------------------------
{
  const char *lm_name = lm->GetName();
  double pos[3], rot[3];
  lm->GetOutput()->GetPose(pos,rot);
  return AppendLandmark(pos[0],pos[1],pos[2],lm_name);
}
//-------------------------------------------------------------------------
int mafVMELandmarkCloud::SetLandmark(int idx,double x,double y,double z,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  if (idx < 0)
  {
    mafErrorMacro("SetLandmark: point index out of range");
    return MAF_ERROR;
  }
  
  if (GetState()==CLOSED_CLOUD)
  {  
    t = t < 0 ? m_CurrentTime : t;

    // Retrieve polydata for time t or create a new one
    vtkPolyData *polydata=GetPolyData(t);
    if (!polydata)
      polydata = NewPolyData(t);

    Superclass::SetPoint(polydata,idx,x,y,z);
    return SetLandmarkVisibility(polydata,idx,true);
  }
  else
  {
    if (mafVMELandmark *lm = GetLandmark(idx))
    {
      lm->SetPose(x,y,z,0,0,0,t);
      return MAF_OK;
    }
    else
    {
      mafErrorMacro("Problems setting position of "<<idx<<"th landmark in the open cloud");
      return MAF_ERROR;
    }
  }
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::FindLandmarkIndex(const char *name)
//-------------------------------------------------------------------------
{
  mafString lmname;
  for (int i = 0; i < GetNumberOfLandmarks(); i++)
  {
    lmname = GetLandmarkName(i);
    if (lmname == name)
      return i;
  }
  return -1;
}
//-------------------------------------------------------------------------
int mafVMELandmarkCloud::GetLandmark(int idx, double &x,double &y,double &z,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  if (GetState() == CLOSED_CLOUD)
    return Superclass::GetPoint(idx,x,y,z,t);

  if (mafVMELandmark *lm = GetLandmark(idx))
    return lm->GetPoint(x,y,z,t);

  return MAF_ERROR;
}
//-------------------------------------------------------------------------
int mafVMELandmarkCloud::GetLandmark(int idx, double xyz[3],mafTimeStamp t)
//-------------------------------------------------------------------------
{
  if (GetState() == CLOSED_CLOUD)
    return Superclass::GetPoint(idx,xyz,t);

  if (mafVMELandmark *lm = GetLandmark(idx))
    return lm->GetPoint(xyz,t);

  return MAF_ERROR;
}
//-------------------------------------------------------------------------
int mafVMELandmarkCloud::RemoveLandmark(int idx)
//-------------------------------------------------------------------------
{
  int num_landmarks = GetNumberOfLandmarks();
  if (GetState()==CLOSED_CLOUD)
  {
    if (idx < 0|| idx >= num_landmarks)
    {
      mafErrorMacro("RemoveLandmark: landmark index out of range");
      return MAF_ERROR;
    }
    // remove the point to all items
    for (mafDataVector::Iterator it = m_DataVector->Begin(); it != m_DataVector->End(); it++)
    {
      mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
      assert(item);
      vtkPolyData *polydata = vtkPolyData::SafeDownCast(item->GetData());
      if (RemovePoint(polydata,idx) != MAF_OK)
        return MAF_ERROR;
    }
    RemoveLandmarkName(idx);
  }
  else
  {
    mafVMELandmark *lm = GetLandmark(idx);
    if (lm)
      RemoveChild(lm);
    else
      return MAF_ERROR;
  } 
  //update internal structure to avoid dummy landmark
  m_NumberOfLandmarks = num_landmarks - 1; //modified by Vladik Aranov 25-03-2005
  GetTagArray()->SetTag(mafTagItem(MAF_LMC_ITEMS_NUMBER_TAG, m_NumberOfLandmarks));
  return MAF_OK;
}
//-------------------------------------------------------------------------
const char *mafVMELandmarkCloud::GetLandmarkName(int idx)
//-------------------------------------------------------------------------
{
  if (GetState() == CLOSED_CLOUD)
  {
    mafString tag = "LM_NAME_";
    tag << idx;
    mafTagItem *item = GetTagArray()->GetTag(tag.GetCStr());
    return item ? item->GetValue() : NULL;
  }
  else
  {
  	mafVME *vme = GetLandmark(idx);
    return vme ? vme->GetName() : NULL;
  }
}

//-------------------------------------------------------------------------
void mafVMELandmarkCloud::SetLandmarkName(int idx,const char *name)
//-------------------------------------------------------------------------
{
  if (GetState() == CLOSED_CLOUD)
  {
    mafString tag = "LM_NAME_";
    tag << idx;
    GetTagArray()->SetTag(mafTagItem(tag.GetCStr(),name));
  }
  else
  {
    GetLandmark(idx)->SetName(name);
  }
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::AppendPoint(vtkPolyData *polydata,double x,double y,double z,int num)
//-------------------------------------------------------------------------
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

//-------------------------------------------------------------------------
vtkPolyData *mafVMELandmarkCloud::NewPolyData(mafTimeStamp t)
//-------------------------------------------------------------------------
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
void mafVMELandmarkCloud::SetRadius(double rad)
//-------------------------------------------------------------------------
{
  if (rad == m_Radius)
    return;

  mafTagItem item;
  m_Radius = rad;
  Modified();
  item.SetName("LM_RADIUS");
  item.SetValue(rad);
  GetTagArray()->SetTag(item);
  GetEventSource()->InvokeEvent(this, mafVMELandmarkCloud::CLOUDE_RADIUS_MODIFIED);
}

//-------------------------------------------------------------------------
double mafVMELandmarkCloud::GetRadius()
//-------------------------------------------------------------------------
{
  return mafRestoreNumericFromTag(GetTagArray(),"LM_RADIUS",m_Radius,-1.0,1.0);
}

//-------------------------------------------------------------------------
void mafVMELandmarkCloud::SetSphereResolution(int res)
//-------------------------------------------------------------------------
{
  if (res==m_SphereResolution)
    return;

  mafTagItem item;

  m_SphereResolution = res;
  Modified();

  item.SetName("LM_SPHERE_RESOLUTION");
  item.SetValue(res);
  GetTagArray()->SetTag(item);
  GetEventSource()->InvokeEvent(this, mafVMELandmarkCloud::CLOUDE_SPHERE_RES);
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::GetSphereResolution()
//-------------------------------------------------------------------------
{
  return mafRestoreNumericFromTag(GetTagArray(),"LM_SPHERE_RESOLUTION",m_SphereResolution,-1,15);
}

//-------------------------------------------------------------------------
mafVMELandmark *mafVMELandmarkCloud::GetLandmark(const char *name)
//-------------------------------------------------------------------------
{
  if (GetState() == OPEN_CLOUD)
  {
    for (int i=0;i<GetNumberOfChildren();i++)
    {
      mafVMELandmark *vme = mafVMELandmark::SafeDownCast(GetChild(i));
      if (vme && mafCString(vme->GetName()) == name)
       return vme;
    }
  }
  else
  {
    mafErrorMacro("GetLandmark by name: this function works only when cloud is Closed!");
  }
  return NULL;
}

//-------------------------------------------------------------------------
mafVMELandmark *mafVMELandmarkCloud::GetLandmark(int idx)
//-------------------------------------------------------------------------
{
  if (GetState() == OPEN_CLOUD)
  {
	  int num = 0;
	  for (int i = 0; i < GetNumberOfChildren(); i++)
	  {
		  mafVMELandmark *vme = mafVMELandmark::SafeDownCast(GetChild(i));
		  if (vme)
		  {
			  if (num == idx)
			  {
				  return vme;
			  }
			  num++;
		  }
	  }
  }
  else
  {
    mafErrorMacro("GetLandmark by index: this function works only when cloud is Open!");
  }
	return NULL;
}

//-------------------------------------------------------------------------
void mafVMELandmarkCloud::Close()
//-------------------------------------------------------------------------
{
  int num = GetNumberOfLandmarks();
  // change the state to closed to disable extra features
  SetState(CLOSED_CLOUD);
  m_NumberOfLandmarks = 0;
  
  // pre-set the number of landmarks.
  SetNumberOfLandmarks(num);

  // Notice that the number of Landmarks children and the number of landmarks points
  // in the cloud should already be the same due to synchronization features enabled
  // when the cloud is open. The only thing out of sync are point positions... this
  // could change in the future having also position to be synchronous.
  int idx = 0;
  std::vector<mafVMELandmark *> landmarks;
	for (int c = 0; c < GetNumberOfChildren();c++)
  {
    if (mafVMELandmark *lm = mafVMELandmark::SafeDownCast(GetChild(c)))
    {
      SetLandmarkName(idx,lm->GetName());

      mafDataVector::Iterator item_id = m_DataVector->Begin();
      for (mafMatrixVector::Iterator it = lm->GetMatrixVector()->Begin(); it != lm->GetMatrixVector()->End();)
      {
        double xyz[3];
        bool vis;
        mafMatrix *mat = mafMatrix::SafeDownCast(it->second);
        // extract the position
        xyz[0] = mat->GetElements()[0][3];
        xyz[1] = mat->GetElements()[1][3];
        xyz[2] = mat->GetElements()[2][3];

        if (mat->GetElements()[0][0]!=mat->GetElements()[1][1]||mat->GetElements()[0][0]!=mat->GetElements()[2][2]) // DEBUG Test
          mafErrorMacro("Close: corrupted visibility information for landmark "<<lm->GetName()<<" a time "<<mat->GetTimeStamp());

        vis = mat->GetElements()[0][0] != 0;
        
        mafVMEItemVTK *item = NULL;
        if(item_id != m_DataVector->End())
          item = mafVMEItemVTK::SafeDownCast(item_id->second);
        vtkPolyData *polydata;

        // All this stuff is to cope with possible open landmark clouds having different
        // timestamps for the different landmarks.
        // We take advantage of the fact items are ordered arrays, thus all items with 
        // id < item_id have lower timestamps. This is true both for MatrixVector and 
        // VMEItemArray.

        // if current item has timestamp< mat->TimeStamp get next one
        if (item && (item->GetTimeStamp() < mat->GetTimeStamp()))
        {
          // if this is the first pose matrix, set the position in all the 
          // previous items to this one.
          if (idx == 0)
          {
            polydata = (vtkPolyData *)item->GetData(); // we found an item with the given timestamp
            if (polydata)
            {
              Superclass::SetPoint(polydata,idx,xyz[0],xyz[1],xyz[2]);
              SetLandmarkVisibility(polydata,idx,vis);
            }
            else
            {
              mafErrorMacro("Close: corrupted data structure, NULL polydata for timestamp: "<<item->GetTimeStamp());
            }
          }
          item_id++; 
          continue;
        }

        // if current item has timestamp > mat->TimeStamp we need a new item for that timestamp
        if (!item || item->GetTimeStamp() > mat->GetTimeStamp())
        {
          // Here I have to add code for setting position of landmarks by default to 
          // the one in the previous timestamp
          mafVMEItemVTK *previous_item = NULL;
          if (item_id != m_DataVector->Begin())
          {
            item_id--;
            previous_item = mafVMEItemVTK::SafeDownCast(item_id->second);
            assert(previous_item);
            item_id++;
          }
          vtkPolyData *previous_polydata = previous_item ? (vtkPolyData*)previous_item->GetData() : NULL;
          polydata = NewPolyData(mat->GetTimeStamp());

          if (previous_polydata && polydata)
          {
            polydata->DeepCopy(previous_polydata);
          }
          item_id = m_DataVector->Last();
        }
        else if (item->GetTimeStamp() == mat->GetTimeStamp())
        { 
          polydata = (vtkPolyData *)item->GetData(); // we found an item with the given timestamp
        }
        if (polydata)
        {
          Superclass::SetPoint(polydata,idx,xyz[0],xyz[1],xyz[2]);
          SetLandmarkVisibility(polydata,idx,vis);
        }
        else
        {
          mafErrorMacro("Close: problems retrieving or allocating polydata for timestamp: "<<item->GetTimeStamp());
        }
        item_id++; // increase item index
        it++; // increase matrix index
      }
      idx++;

      landmarks.push_back(lm); 
    }
  }
  
  // remove all child landmarks
  for (int i=0;i<landmarks.size();i++)
  {
    RemoveChild(landmarks[i]);
  }
    
  Modified();
  GetEventSource()->InvokeEvent(this, mafVMELandmarkCloud::CLOUDE_OPEN_CLOSE);
}

//-------------------------------------------------------------------------
void mafVMELandmarkCloud::Open()
//-------------------------------------------------------------------------
{
  int i,numlm = GetNumberOfLandmarks();
  for (i = 0; i < numlm; i++)
	{
    // add a new LM child with the same name of the corresponding LM
    mafSmartPointer<mafVMELandmark> lm;
    lm->SetName(GetLandmarkName(i));
    
    // force node adding
    m_State = OPEN_CLOUD;
    Superclass::AddChild(lm);
    m_State = CLOSED_CLOUD;
    mafTimeStamp ct = GetTimeStamp();
    lm->SetTimeStamp(ct);
    
		for (mafDataVector::Iterator it = m_DataVector->Begin(); it != m_DataVector->End() ; it++)
		{
			double xyz[3];
      mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
      assert(item);
      if (vtkPolyData *polydata = (vtkPolyData *)item->GetData())
      {
        bool vis = GetLandmarkVisibility(polydata,i);
        mafMatrix mat;
			  GetPoint(polydata,i,xyz);
        mat.GetElements()[0][3] = xyz[0];
        mat.GetElements()[1][3] = xyz[1];
        mat.GetElements()[2][3] = xyz[2];		  

        // In case non visible set the scale factor to 0.
        if (!vis)
        {
          mat.GetElements()[0][0]=0;
          mat.GetElements()[1][1]=0;
          mat.GetElements()[2][2]=0;
        }
        mat.SetTimeStamp(item->GetTimeStamp());
        lm->GetMatrixVector()->AppendKeyMatrix(mat);
      }
      else
      {
        mafErrorMacro("Open: problems retrieving polydata for timestamp: "<<item->GetTimeStamp());
      }
		}
	}
  // remove all items and tags...
  m_DataVector->RemoveAllItems();
  for (i = 0; i < numlm; i++)
    RemoveLandmarkName(i);

  // change the state to open to enable extra features
  SetState(OPEN_CLOUD);
  Modified();
  GetEventSource()->InvokeEvent(this, mafVMELandmarkCloud::CLOUDE_OPEN_CLOSE);
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::SetLandmarkVisibility(int idx,bool a,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  if (GetState() == CLOSED_CLOUD)
  {
    vtkPolyData *polydata = GetPolyData(t);
    if (polydata)
    {
      if (GetLandmarkVisibility(polydata,idx) == a)
        return MAF_OK;
    }
    else
    {
      polydata = NewPolyData(t);
      if ((a && m_DefaultVisibility) || (!a && !m_DefaultVisibility))
        return MAF_OK;
    }
    if (polydata)
    {
      return SetLandmarkVisibility(polydata, idx, a);
    }
    else
    {
      return MAF_ERROR;
    }
  }
  else
  {
    if (mafVMELandmark *lm = GetLandmark(idx))
    {
      return lm->SetLandmarkVisibility(a,t);
    }
    else
    {
      mafErrorMacro("SetVisibility: problems retrieving "<<idx<<"th child landamark for setting its visibility!");
      return MAF_ERROR;
    }
  }
}

//-------------------------------------------------------------------------
bool mafVMELandmarkCloud::GetLandmarkVisibility(int idx,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  if (GetState() == CLOSED_CLOUD)
  {
    bool ret = true; // visibility is true by default
    vtkPolyData *polydata;

    // extract scalars for visibility
    if (t != m_CurrentTime)
    {
      // set time to the argument value
      GetDataPipe()->SetTimeStamp(t);
    }
    polydata = GetPointSetOutput()->GetPointSetData();
    if (polydata)
    {
      polydata->Update();
      ret = GetLandmarkVisibility(polydata, idx);
    }
    else
    {
      mafErrorMacro("GetLandmarkVisibility: problems retrieving polydata for time stamp"<<t);
    }
    // restore current time
    if (t != m_CurrentTime)
    {
      GetDataPipe()->SetTimeStamp(m_CurrentTime);
      GetDataPipe()->Update();
    }
    return ret;
  }
  else
  {
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
}

//-------------------------------------------------------------------------
int mafVMELandmarkCloud::SetLandmarkVisibility(vtkPolyData *polydata,int idx,bool a)
//-------------------------------------------------------------------------
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
//-------------------------------------------------------------------------
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
//-------------------------------------------------------------------------
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
int mafVMELandmarkCloud::GetState()
//-------------------------------------------------------------------------
{
  if (m_State == UNSET_CLOUD)
  {
    if (mafTagItem *tag = GetTagArray()->GetTag("MFL_VME_LANDMARK_CLOUD_STATE"))
    {
      m_State = tag->GetValueAsDouble();
    }
    else
    {
      m_State = CLOSED_CLOUD;
    }
  }
  return m_State;
}

//-------------------------------------------------------------------------
void mafVMELandmarkCloud::SetState(int state)
//-------------------------------------------------------------------------
{
  m_State = state;
  GetTagArray()->SetTag(mafTagItem("MFL_VME_LANDMARK_CLOUD_STATE", m_State));
  Modified();
}

//-------------------------------------------------------------------------
bool mafVMELandmarkCloud::IsRigid()
//-------------------------------------------------------------------------
{
  return m_DataVector->GetNumberOfItems() == 1;
}
//-------------------------------------------------------------------------
void mafVMELandmarkCloud::Print(std::ostream &os, const int tabs)
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);
  Superclass::Print(os,indent);
  os << indent << "Cloud m_State: ";
  switch (GetState()) 
  {
    case CLOSED_CLOUD: os <<"CLOSED"; break;
    case OPEN_CLOUD: os <<"OPEN"; break;
    case UNSET_CLOUD: os <<"UNSET"; break;
  };

  os << std::endl;

  os << indent << "Current Landmarks m_State:"<<std::endl;
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
mmgGui* mafVMELandmarkCloud::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  GetRadius(); // Called to update m_Radius var from tag
  m_Gui->Double(ID_LM_RADIUS, "radius", &m_Radius, 0.0,MAXDOUBLE,-1);

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMELandmarkCloud::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_LM_RADIUS:
        SetRadius(e->GetDouble());
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
mmaMaterial *mafVMELandmarkCloud::GetMaterial()
//-------------------------------------------------------------------------
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
