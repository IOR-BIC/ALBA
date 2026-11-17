/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMELandmarkCloud
 Authors: Marco Petrone, Paolo Quadrani, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

const bool DEBUG_MODE = false;

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaVMELandmarkCloud.h"
#include "mmuIdFactory.h"
#include "albaDataVector.h"
#include "albaDataPipe.h"
#include "albaTagArray.h"
#include "albaVMELandmark.h"
#include "albaMatrixVector.h"
#include "albaVMEItemVTK.h"
#include "albaVMEOutputLandmarkCloud.h"
#include "albaGUI.h"
#include "mmaMaterial.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkBitArray.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkCellArrayIterator.h"

#include <vector>

#include "vtkMath.h"
//------------------------------------------------------------------------------
// local defines

#define ALBA_LMC_VERSION "ALBA_LMC_VERSION"

//------------------------------------------------------------------------------
// Events

ALBA_ID_IMP(albaVMELandmarkCloud::CLOUD_RADIUS_MODIFIED); // Event rised when the radius is changed with a SetRadius()
ALBA_ID_IMP(albaVMELandmarkCloud::CLOUD_SPHERE_RES); // Event rised when the sphere resolution is changed with a SetSphereResolution()

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaVMELandmarkCloud);


//-------------------------------------------------------------------------
albaVMELandmarkCloud::albaVMELandmarkCloud()
{
  SetOutput(albaVMEOutputLandmarkCloud::New()); // create the output

	m_StoreDataVector = false;

  m_NumberOfLandmarks = 0;
  m_DefaultVisibility = 1;
  m_Radius            = 1.0;
  m_Pickable          = true;
  m_SphereResolution  = 15;

	m_LandmarkShowNumber = 0;
}

//-------------------------------------------------------------------------
albaVMELandmarkCloud::~albaVMELandmarkCloud()
{
	m_LMChildren.clear();
	m_LMChildrenShow.clear();
	m_LMIndexesMap.clear();
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{ 
	if (Superclass::DeepCopy(a) == ALBA_OK)
	{
		albaVMELandmarkCloud *lc = albaVMELandmarkCloud::SafeDownCast(a);
		m_Radius = lc->GetRadius();
		m_DefaultVisibility = lc->GetDefaultVisibility();
		m_SphereResolution = lc->GetSphereResolution();

		//Removing current Data Vector
		m_DataVector->RemoveAllItems();

		GetTagArray()->SetTag(albaTagItem(ALBA_LMC_VERSION, 2));

    return ALBA_OK;
  }  
  return ALBA_ERROR;
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloud::CopyLandmarks(albaVMELandmarkCloud *lmc)
{
	if (lmc == NULL)
		return;

	int lmNum = lmc->GetNumberOfLandmarks();
	for (int i = 0; i < lmNum; i++)
	{
		albaVMELandmark *lm;
		albaNEW(lm);
		lm->DeepCopy(lmc->GetLandmark(i));
		lm->ReparentTo(this);
		albaDEL(lm);
	}

}

//-------------------------------------------------------------------------
bool albaVMELandmarkCloud::Equals(albaVME *vme)
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    albaVMELandmarkCloud *lc = albaVMELandmarkCloud::SafeDownCast(vme);
    ret = (m_Radius == lc->GetRadius() && 
           m_NumberOfLandmarks == lc->GetNumberOfLandmarks() &&
           m_DefaultVisibility == lc->GetDefaultVisibility() &&
           m_SphereResolution  == lc->GetSphereResolution());
  }
  return ret;
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::InternalInitialize()
{


	const int closed_cloud_state = 1;
	const int open_cloud_state = 2;

  if (Superclass::InternalInitialize()==ALBA_OK)
  {
    // force material allocation
    GetMaterial();

    //Old versions of LC does not have ALBA_LMC_VERSION tag
		//If we have and old LC it can be open or closed 
		//closed LC contains LMs as combination of vtk data (for position) and tags (for names)
		albaTagItem *tag;
		albaTagArray * tagArray = GetTagArray();

		if ((!tagArray->IsTagPresent(ALBA_LMC_VERSION)) && (tag = tagArray->GetTag("MFL_VME_LANDMARK_CLOUD_STATE")) && (tag->GetValueAsDouble() == closed_cloud_state))
			CreateLMStructureFromDataVector();
		else if (!m_DataVector->GetItem(m_CurrentTime))
			NewPolyData(m_CurrentTime);

		//Setting state tag to Open Cloud in order to maintain backward compatibility
		GetTagArray()->SetTag(albaTagItem("MFL_VME_LANDMARK_CLOUD_STATE", open_cloud_state));



    return ALBA_OK;
  }
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::SetData(vtkDataSet *data, albaTimeStamp t, int mode)
{
  assert(data);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);

  // only polydata made of points are accepted
  if (polydata != NULL && polydata->GetPolys()->GetNumberOfCells() == 0 && polydata->GetStrips()->GetNumberOfCells() == 0 && polydata->GetLines()->GetNumberOfCells() == 0)
    return Superclass::SetData(data,t,mode);

  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
vtkMTimeType albaVMELandmarkCloud::GetMTime()
{
	vtkMTimeType mtime = Superclass::GetMTime();
	vtkMTimeType mtimelm;
  for (int i = 0; i < GetNumberOfChildren(); i++)
  {
    albaVMELandmark *vme = albaVMELandmark::SafeDownCast(GetChild(i));
    if (vme)
    {
      mtimelm = vme->GetMTime();
      mtime = (mtimelm > mtime) ? mtimelm : mtime;
    }
  }
  return mtime;
}
//-------------------------------------------------------------------------
int albaVMELandmarkCloud::GetNumberOfLandmarks()
{
	return m_NumberOfLandmarks;
}


//-------------------------------------------------------------------------
int albaVMELandmarkCloud::SetNumberOfLandmarks(int num)
{  
	int oldnum = GetNumberOfLandmarks();

	if (num == oldnum)
		return ALBA_OK;

	// all the time stamps must have the same number of points
	std::vector<vtkPolyData *> polydataList = GetAllPolyData();
	for (int i = 0; i < polydataList.size(); i++)
	{
		vtkPolyData *polydata = polydataList[i];
		int itemoldnum = polydata->GetNumberOfPoints();

		if (num > oldnum)
		{
			// add the new points, with their visibility, in the origin
			int idx = AppendPoint(polydata, 0, 0, 0, num - oldnum);
			if (idx != (num - 1) && (itemoldnum != num))
			{
				albaErrorMacro("SetNumberOfLandmarks: unexpected number of points in the cloud data");
				return ALBA_ERROR;
			}
		}
		else
		{
			// remove the last points
			for (int j = itemoldnum - 1; j >= num; j--)
			{
				if (RemovePoint(polydata, j) != ALBA_OK)
				{
					albaErrorMacro("SetNumberOfLandmarks: unexpected number of points in the cloud data");
					return ALBA_ERROR;
				}
			}
		}
	}

	m_NumberOfLandmarks = num;

	return ALBA_OK;
}


//----------------------------------------------------------------------------
int albaVMELandmarkCloud::AppendLandmark(double x, double y, double z, const char *name)
{
	albaVMELandmark *lm;

	int lmIndex = GetLandmarkIndex(name);
	if (lmIndex >= 0)
	{
		albaLogMessage("Warning cannot add new landmark \"%s\": a landmark with the same name already exists!!", name);
		lm = GetLandmark(lmIndex);
	}
	else
	{
		albaNEW(lm);
		lm->SetName(name);
		lm->ReparentTo(this);
		lmIndex = m_NumberOfLandmarks - 1;
	}

	SetLandmark(lmIndex, x, y, z);

	return lmIndex;

}

//----------------------------------------------------------------------------
int albaVMELandmarkCloud::AppendAbsoluteLandmark(double x, double y, double z, const char *name)
{
	double lm[4] = { x,y,z,1 }, abslm[4];

	albaMatrix mtr = *GetOutput()->GetAbsMatrix();
	mtr.Invert();
	mtr.MultiplyPoint(lm, abslm);

	return AppendLandmark(abslm, name);
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::SetLandmark(int idx, double x, double y, double z, albaTimeStamp t)
{
	if (idx < 0)
	{
		albaErrorMacro("SetLandmark: point index out of range");
		return ALBA_ERROR;
	}

	albaVMELandmark *lm = GetLandmark(idx);
	if (!lm)
	{
		albaErrorMacro("Problems setting position of " << idx << "th landmark in the open cloud");
		return ALBA_ERROR;
	}
	/*albaMatrix * itemMtr = GetMatrixVector()->GetItem(t);
	if (itemMtr) {
		albaMatrix identity, mtr;
		mtr.DeepCopy(itemMtr);
		if (!identity.Equals(&mtr))
		{
			double in[4] = { x,y,z,1 };
			double out[4];
			mtr.Invert();
			mtr.MultiplyPoint(in, out);
			x = out[0];
			y = out[1];
			z = out[2];
		}
	}*/
	lm->SetPose(x, y, z, 0, 0, 0, t);

	return SetLandmarkToPolydata(idx, x, y, z, m_LMChildrenShow[idx], t);
}

//----------------------------------------------------------------------------
int albaVMELandmarkCloud::SetLandmarkToPolydata(int idx, double x, double y, double z,bool visibility, albaTimeStamp &t)
{
	t = t < 0 ? m_CurrentTime : t;

	// Retrieve polydata for time t or create a new one
	vtkPolyData *polydata = GetPolyData(t);
	if (!polydata)
		polydata = NewPolyData(t);

	SetPoint(polydata, idx, x, y, z);
	
	return SetLandmarkVisibility(polydata, idx, visibility);
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::GetLandmarkIndex(const char *name)
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
int albaVMELandmarkCloud::GetLandmark(int idx, double &x,double &y,double &z,albaTimeStamp t)
{
	albaVMELandmark *lm=GetLandmark(idx);

	if (lm == NULL)
		return ALBA_ERROR;

	return lm->GetPoint(x, y, z, t);
}
//-------------------------------------------------------------------------
int albaVMELandmarkCloud::GetLandmark(int idx, double xyz[3],albaTimeStamp t)
{
	return GetLandmark(idx, xyz[0], xyz[1], xyz[2], t);
}

//----------------------------------------------------------------------------
int albaVMELandmarkCloud::GetLandmarkIndex(albaVMELandmark *lm)
{
	std::map<albaVMELandmark *, int>::iterator it = m_LMIndexesMap.find(lm);
	
	if (it != m_LMIndexesMap.end())
		return it->second;
	else
		return -1;
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::RemoveLandmark(int idx)
{
	int num_landmarks = GetNumberOfLandmarks();

	if (idx < 0 || idx >= num_landmarks)
	{
		albaErrorMacro("RemoveLandmark: landmark index out of range");
		return ALBA_ERROR;
	}

	albaVMELandmark *lm = GetLandmark(idx);
	if (!lm)
	{
		albaErrorMacro("Problems removing of " << idx << "th landmark in the cloud");
		return ALBA_ERROR;
	}

	//removing vme child
	RemoveChild(lm);
	   
  return ALBA_OK;
}
//-------------------------------------------------------------------------
const char *albaVMELandmarkCloud::GetLandmarkName(int idx)
{
  albaVME *vme = GetLandmark(idx);
  return vme ? vme->GetName() : NULL;
}

//-------------------------------------------------------------------------
void albaVMELandmarkCloud::SetLandmarkName(int idx,const char *name)
{
	GetLandmark(idx)->SetName(name);
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::AppendPoint(vtkPolyData *polydata,double x,double y,double z,int num)
{
  int oldnum = polydata->GetNumberOfPoints();
  int idx = -1;

  if (num > 0)
  {
    vtkPoints *points = polydata->GetPoints();
    if (points)
    {
      for (int i = 0; i < num; i++)
        idx = points->InsertNextPoint(x,y,z);

      points->Modified();
      polydata->Modified();
    }
    else
      albaErrorMacro("Corrupted PolyData inside VME-LandmarkCloud: Cannot found Points or Vertx cells");
  }

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
      albaErrorMacro("AppendPoint: Corrupted PolyData inside VME-LandmarkCloud: Cannot found Vertex cells or point scalars.");
    }
  }
  else
  {
    albaErrorMacro("AppendPoint: DEBUG, cannot add points to cloud...");
  }
  return idx;
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloud::CreateLMStructureFromDataVector()
{
	albaTimeStamp currentTime = GetTimeStamp();
	albaTagArray *tarray = GetTagArray();
	double x, y, z;
	std::vector <albaVMELandmark*> landmarks;

	
	std::vector<std::string> tag_list;
	tarray->GetTagList(tag_list);
	
	//Creating landmarks from Tags --USED TO RESTORE OLD LMC--
	for (int i = 0; i < tag_list.size(); i++)
	{
		albaString tagName = tag_list[i].c_str();
		if (tagName.StartsWith("LM_NAME_"))
		{
			albaTagItem *tag = tarray->GetTag(tagName);
			const char *name = tag->GetValue();

			albaVMELandmark *newLM;
			albaNEW(newLM);
			newLM->SetName(name);
			landmarks.push_back(newLM);
		}
	}

	//if no tags were stored we read create landmarks from data vector
	if (landmarks.size() == 0)
	{
		albaDataVector::Iterator it = m_DataVector->Begin();
		albaVMEItemVTK *item = albaVMEItemVTK::SafeDownCast(it->second);
		int nPoints=item->GetData()->GetNumberOfPoints();
		for (int i = 0; i < nPoints; i++)
		{
			albaString name;
			name = "Landmark ";
			name << i;

			albaVMELandmark *newLM;
			albaNEW(newLM);
			newLM->SetName(name);
			landmarks.push_back(newLM);
		}
	}

	//Setting positions to landmarks
	for (albaDataVector::Iterator it = m_DataVector->Begin(); it != m_DataVector->End(); it++)
	{
		double xyz[3];
		albaVMEItemVTK *item = albaVMEItemVTK::SafeDownCast(it->second);
		
		assert(item);

		vtkPolyData *polydata = (vtkPolyData *)item->GetData();
		//polydata->Update();
		albaTimeStamp timeStamp = item->GetTimeStamp();
		
		if (!polydata)
		{
			albaErrorMacro("Open: problems retrieving polydata for timestamp: " << timeStamp);
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
albaVMELandmark * albaVMELandmarkCloud::GetNearestLandmark(double pos[3], bool SerchOnlyVisible)
{
	int lmIndex = -1;
	int num = GetNumberOfLandmarks();
	double dist = VTK_DOUBLE_MAX;
	double lmPos[3];
	
	for (int idx = 0; idx < num; idx++)
	{
		albaVMELandmark *lm = GetLandmark(idx);

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
vtkPolyData *albaVMELandmarkCloud::NewPolyData(albaTimeStamp t)
{
	// the polydata is stored by reference in the data vector, which keeps it alive
	vtkALBASmartPointer<vtkPolyData> polydata;
	vtkALBASmartPointer<vtkPoints> points;
	polydata->SetPoints(points);
	SetDataByReference(polydata,t);

	// cell array for the visible points
	vtkALBASmartPointer<vtkCellArray> cells;
	polydata->SetVerts(cells);

	// scalars for the visibility attribute
	vtkALBASmartPointer<vtkBitArray> visibility;
	polydata->GetPointData()->SetScalars(visibility);

	// all the time stamps have one point for each landmark
	if (GetNumberOfLandmarks() > 0)
		AppendPoint(polydata,0,0,0,GetNumberOfLandmarks());

	return polydata;
}

//-------------------------------------------------------------------------
void albaVMELandmarkCloud::SetRadius(double rad, bool force_update)
{
  if (albaEquals(rad, m_Radius) && !force_update)
    return;

  m_Radius = rad;
  Modified();
  InvokeEvent(this,albaVMELandmarkCloud::CLOUD_RADIUS_MODIFIED);
}

//-------------------------------------------------------------------------
double albaVMELandmarkCloud::GetRadius()
{
  return m_Radius;
}

//-------------------------------------------------------------------------
void albaVMELandmarkCloud::SetSphereResolution(int res, bool force_update)
{
  if (albaEquals(res, m_SphereResolution) && !force_update)
    return;

  m_SphereResolution = res;
  Modified();

  InvokeEvent(this,albaVMELandmarkCloud::CLOUD_SPHERE_RES);
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::GetSphereResolution()
{
  return m_SphereResolution;
}

//-------------------------------------------------------------------------
albaVMELandmark *albaVMELandmarkCloud::GetLandmark(const char *name)
{
  for (int i = 0; i < m_NumberOfLandmarks; i++)
  {
    albaVMELandmark *lm = m_LMChildren[i];
    if (lm && albaCString(lm->GetName()) == name)
      return lm;
  }
  
	albaLogMessage("Landmark %s not found", name);
  return NULL;
}
//-------------------------------------------------------------------------
albaVMELandmark *albaVMELandmarkCloud::GetLandmark(int idx)
{
	if (idx < 0 || idx >= m_NumberOfLandmarks)
		return NULL;
	else
		return m_LMChildren[idx];
}
//-------------------------------------------------------------------------
void albaVMELandmarkCloud::GetLandmarkPosition(int idx, double pos[3], albaTimeStamp t)
{
    GetPoint(idx,pos,t);
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::SetLandmarkVisibility(int idx,bool a,albaTimeStamp t)
//-------------------------------------------------------------------------
{

	//Setting visibility to the landmark
	albaVMELandmark *lm = GetLandmark(idx);
	if (!lm)
	{
		albaErrorMacro("SetVisibility: problems retrieving " << idx << "th child landamark for setting its visibility!");
		return ALBA_ERROR;
	}
	lm->SetLandmarkVisibility(a, t);

	//Setting visibility to the cloud
  vtkPolyData *polydata = GetPolyData(t);
  if (polydata)
  {
    if (GetLandmarkVisibility(polydata,idx) == a)
      return ALBA_OK;
  }
  else
  {
    polydata = NewPolyData(t);
		if (!polydata)
		{
			albaErrorMacro("SetVisibility: problems generarting Polydata at time" << t);
			return ALBA_ERROR;
		}
    if ((a && m_DefaultVisibility) || (!a && !m_DefaultVisibility))
      return ALBA_OK;
  }

  return SetLandmarkVisibility(polydata, idx, a);
}

//-------------------------------------------------------------------------
bool albaVMELandmarkCloud::GetLandmarkVisibility(int idx,albaTimeStamp t)
{
	//Getting Visibility from the LM
  if (albaVMELandmark *lm = GetLandmark(idx))
  {
    return lm->GetLandmarkVisibility(t);
  }
  else
  {
    albaErrorMacro("GetVisibility: problems retrieving "<<idx<<"th child landamark for getting its visibility!");
    return false;
  }
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::SetLandmarkVisibility(vtkPolyData *polydata,int idx,bool a)
{
  if (idx >= polydata->GetNumberOfPoints())
  {
    albaErrorMacro("SetLandmarkVisibility: point index out of range");
    return ALBA_ERROR;
  }
  vtkPointData* pdata = polydata->GetPointData();
  vtkBitArray *scalars = NULL;
  
  scalars = (vtkBitArray *)(pdata->GetScalars());
  if (scalars)
  {
    if ((scalars->GetValue(idx) != 0) == a)
      return ALBA_OK;
  }
  else
  {
    albaErrorMacro("GetLandmarkVisibility: Corrupted PolyData inside VME-PointSet, cannot found visibility scalars");
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
      vtkALBASmartPointer<vtkCellArray> newcells;

      vtkSmartPointer<vtkCellArrayIterator> cellIterator = cells->NewIterator();
      for (cellIterator->GoToFirstCell(); !cellIterator->IsDoneWithTraversal(); cellIterator->GoToNextCell())
      {
        vtkIdType npts;
        const vtkIdType* pts;
        cellIterator->GetCurrentCell(npts, pts);

        if (npts != 1)
        {
          albaErrorMacro("SetLandmarkVisibility: Corrupted polydata, found a cell with wrong number of points: " << npts);
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
    return ALBA_OK;
  }
  else
  {
    albaErrorMacro("SetLandmarkVisibility: Corrupted PolyData inside VME-LandmarkCloud, cannot found Vertx cells or scalars");
  }
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
bool albaVMELandmarkCloud::GetLandmarkVisibility(vtkPolyData *polydata,int idx)
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
    albaErrorMacro("GetLandmarkVisibility: Corrupted PolyData inside VME-PointSet, cannot found visibility scalars");
  }
  // for old files, visibility is true by default...
  return true;
}

//-------------------------------------------------------------------------
bool albaVMELandmarkCloud::IsRigid()
{
  return m_DataVector->GetNumberOfItems() == 1;
}
//-------------------------------------------------------------------------
void albaVMELandmarkCloud::Print(std::ostream &os, const int tabs)
{
  albaIndent indent(tabs);
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
albaGUI* albaVMELandmarkCloud::CreateGui()
{
  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);

	m_Gui->Divider();

  m_Gui->Double(ID_LM_RADIUS, "Radius", &m_Radius, 0.0,MAXDOUBLE,-1);
  m_Gui->Integer(ID_LM_SPHERE_RESOLUTION, "Resolution", &m_SphereResolution, 0.0,MAXINT);
  
  m_Gui->Divider();
  return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMELandmarkCloud::OnEvent(albaEventBase *alba_event)
{
  // events to be sent up or down in the tree are simply forwarded
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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
        albaVME::OnEvent(alba_event);
    }
  }
  else
  {
    Superclass::OnEvent(alba_event);
  }
}
//-------------------------------------------------------------------------
int albaVMELandmarkCloud::InternalStore(albaStorageElement *parent)
{
	GetTagArray()->SetTag(albaTagItem(ALBA_LMC_VERSION, 2));

  if (Superclass::InternalStore(parent) == ALBA_OK)
  {
    if (parent->StoreInteger("LM_SPHERE_RESOLUTION", m_SphereResolution) == ALBA_OK &&
        parent->StoreDouble("LM_RADIUS", m_Radius) == ALBA_OK)
    {
      return ALBA_OK;
    }
  }
 
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
int albaVMELandmarkCloud::InternalRestore(albaStorageElement *node)
{
  if (Superclass::InternalRestore(node) == ALBA_OK)
  {
    if (node->RestoreInteger("LM_SPHERE_RESOLUTION", m_SphereResolution) == ALBA_OK &&
        node->RestoreDouble("LM_RADIUS", m_Radius) == ALBA_OK)
    {
      return ALBA_OK;
    }
  }
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *albaVMELandmarkCloud::GetMaterial()
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((albaVMEOutputPointSet *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}
//-------------------------------------------------------------------------
char** albaVMELandmarkCloud::GetIcon() 
{
#include "albaVMELandmarkCloud.xpm"
  return albaVMELandmarkCloud_xpm;
}


//-------------------------------------------------------------------------
bool albaVMELandmarkCloud::IsDataAvailable()
{
  if (m_DataVector)
  {
    albaTimeStamp t = this->GetTimeStamp();
    albaVMEItem *item = m_DataVector->GetItem(t);
    if (item)
      item->UpdateData();

    if (IsAnimated() && item == NULL)
    {
      albaTimeStamp tbounds[2];
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
    // or is into its child as for the albaVMELandmarkCloud
    available =  (item ? item->IsDataPresent() : true); 

    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "bool albaVMELandmarkCloud::IsDataAvailable()" << std::endl
        << "Data for VME: " << this->GetName() << " is "  << \
        (available ? " available" : " not available") << std::endl;
      albaLogMessage(stringStream.str().c_str());
    }
    return available;
  }
  else
    return albaVME::IsDataAvailable();
}

//----------------------------------------------------------------------------
int albaVMELandmarkCloud::AddChild(albaVME *node)
{
	if (Superclass::AddChild(node) == ALBA_ERROR)
		return ALBA_ERROR;

	if (albaVMELandmark *lm=albaVMELandmark::SafeDownCast(node))
	{
		m_LMChildren.push_back(lm);
		m_LMChildrenShow.push_back(false);
		m_LMIndexesMap[lm] = m_NumberOfLandmarks;
		SetNumberOfLandmarks(m_NumberOfLandmarks + 1);
		
		albaMatrixVector * lmMatrixVector = lm->GetMatrixVector();
		for (albaMatrixVector::Iterator it = lmMatrixVector->Begin(); it != lmMatrixVector->End();it++)
		{
			albaMatrix *mat = albaMatrix::SafeDownCast(it->second);
			albaTimeStamp timeStamp = mat->GetTimeStamp();

			//use the landmark position at this time stamp, not the current one
			double pos[3] = { mat->GetElement(0, 3), mat->GetElement(1, 3), mat->GetElement(2, 3) };

			//On reparent the node is not showed, so visibility is set to false.
			SetLandmarkToPolydata(m_NumberOfLandmarks - 1, pos[0], pos[1], pos[2], false, timeStamp);
		}
		GetOutput()->Update();
	}

	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloud::RemoveChild(albaVME *node)
{
	Superclass::RemoveChild(node);

	if (albaVMELandmark *lm = albaVMELandmark::SafeDownCast(node))
	{
		int pos=GetLandmarkIndex(lm);
		if (pos < 0)
		{
			albaLogMessage("Warning trying to remove of unattached LandMark");
			return;
		}

		//Reassign maps for children with higer index
		for (int i = pos + 1; i < m_NumberOfLandmarks; i++)
			m_LMIndexesMap[m_LMChildren[i]]--;

		if (m_LMChildrenShow[pos])
			m_LandmarkShowNumber--;

		m_LMChildren.erase(m_LMChildren.begin() + pos);
		m_LMChildrenShow.erase(m_LMChildrenShow.begin() + pos);
		m_LMIndexesMap.erase(lm);

		// remove the point from all the time stamps
		std::vector<vtkPolyData *> polydataList = GetAllPolyData();
		for (int i = 0; i < polydataList.size(); i++)
			RemovePoint(polydataList[i], pos);

		GetOutput()->Update();

		m_NumberOfLandmarks--;
	}
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloud::ShowLandmark(albaVMELandmark *lm, bool show)
{
	int idx = GetLandmarkIndex(lm);
	if (idx < 0 || idx >= m_NumberOfLandmarks)
	{
		albaErrorMacro("Error: try to show a wrong landmark");
		return;
	}
	
	//No status change, return
	if (show == m_LMChildrenShow[idx])
		return;

	m_LMChildrenShow[idx] = show;

	//Show the landmark over the data vector
	for (albaDataVector::Iterator it = m_DataVector->Begin(); it != m_DataVector->End(); it++)
	{
		albaVMEItemVTK *item = albaVMEItemVTK::SafeDownCast(it->second);
		assert(item);
		vtkPolyData *polydata = vtkPolyData::SafeDownCast(item->GetData());
		
		//The landmark is visible if show is true and is visibility is true at the specific time.
		bool lmVisbility = show && lm->GetLandmarkVisibility(item->GetTimeStamp());
		
		SetLandmarkVisibility(polydata, idx, lmVisbility);
	}

	if (show)
		m_LandmarkShowNumber++;
	else
		m_LandmarkShowNumber--;

	GetOutput()->Update();
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloud::ShowAllLandmarks(bool show)
{
	for (int i = 0; i < m_NumberOfLandmarks; i++)
	{
		ShowLandmark(m_LMChildren[i], show);
	}
}

//----------------------------------------------------------------------------
bool albaVMELandmarkCloud::IsLandmarkShow(albaVMELandmark *lm)
{
	int idx = GetLandmarkIndex(lm);
	if (idx < 0 || idx >= m_NumberOfLandmarks)
	{
		albaErrorMacro("Error: required show status for a wrong landmark");
		return false;
	}
	return m_LMChildrenShow[idx];
}

//----------------------------------------------------------------------------
int albaVMELandmarkCloud::GetLandmarkShowNumber()
{
	return m_LandmarkShowNumber;
}


//-------------------------------------------------------------------------
albaVMEOutput *albaVMELandmarkCloud::GetOutput()
{
	// allocate the right type of output on demand
	if (m_Output == NULL)
		SetOutput(albaVMEOutputLandmarkCloud::New()); // create the output

	return m_Output;
}

//-------------------------------------------------------------------------
vtkPolyData *albaVMELandmarkCloud::GetPolyData(albaTimeStamp t)
{
	albaVMEItemVTK *item = albaVMEItemVTK::SafeDownCast(m_DataVector->GetItem(t));
	if (item)
		return vtkPolyData::SafeDownCast(item->GetData());

	return NULL;
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::GetNumberOfPoints(albaTimeStamp t)
{
	t = t < 0 ? m_CurrentTime : t;

	vtkPolyData *polydata = GetPolyData(t);
	if (polydata)
		return polydata->GetNumberOfPoints();

	return 0;
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::SetPoint(vtkPolyData *polydata, int idx, double x, double y, double z)
{
	vtkPoints *points = polydata->GetPoints();
	if (!points)
	{
		albaErrorMacro("SetPoint: Corrupted PolyData inside VME-LandmarkCloud, Points' data not found");
		return ALBA_ERROR;
	}

	if (idx >= points->GetNumberOfPoints())
	{
		albaErrorMacro("SetPoint: point index out of range");
		return ALBA_ERROR;
	}

	points->SetPoint(idx, x, y, z);
	points->Modified();
	polydata->Modified();

	return ALBA_OK;
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::GetPoint(int idx, double xyz[3], albaTimeStamp t)
{
	t = t < 0 ? m_CurrentTime : t;

	int ret = ALBA_ERROR;

	if (t != m_CurrentTime)
	{
		// force the pipe to the specified timestamp
		GetDataPipe()->SetTimeStamp(t);
		GetDataPipe()->Update();
	}

	vtkPolyData *polydata = (vtkPolyData *)GetPointSetOutput()->GetPointSetData();
	if (polydata)
		ret = GetPoint(polydata, idx, xyz);

	// reset the pipe timestamp and its output
	if (t != m_CurrentTime)
	{
		GetDataPipe()->SetTimeStamp(m_CurrentTime);
		GetDataPipe()->Update();
	}

	return ret;
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::GetPoint(int idx, double &x, double &y, double &z, albaTimeStamp t)
{
	double xyz[3];

	if (GetPoint(idx, xyz, t) == ALBA_ERROR)
		return ALBA_ERROR;

	x = xyz[0];
	y = xyz[1];
	z = xyz[2];

	return ALBA_OK;
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::GetPoint(vtkPolyData *polydata, int idx, double xyz[3])
{
	if (!polydata)
	{
		albaErrorMacro("Currupted VME-item: wrong (not PolyData) or empty dataset in VME-LandmarkCloud");
		return ALBA_ERROR;
	}

	vtkPoints *points = polydata->GetPoints();
	if (!points)
	{
		albaErrorMacro("Corrupted vtkPolyDaya in VME-item: Points data not found");
		return ALBA_ERROR;
	}

	if (idx >= 0 && idx < points->GetNumberOfPoints())
	{
		points->GetPoint(idx, xyz);
		return ALBA_OK;
	}

	albaErrorMacro("GetPoint: point index out of range");
	return ALBA_ERROR;
}

//-------------------------------------------------------------------------
int albaVMELandmarkCloud::RemovePoint(vtkPolyData *polydata, int idx)
{
	if (!polydata)
	{
		albaErrorMacro("RemovePoint: Currupted VME-item, wrong (not PolyData) or empty dataset in VME-LandmarkCloud");
		return ALBA_ERROR;
	}

	vtkPoints *points = polydata->GetPoints();
	vtkCellArray *cells = polydata->GetVerts();

	if (!points || !cells)
	{
		albaErrorMacro("RemovePoint: Currupted Polydata inside VME-LandmarkCloud, cannot found Points or Vertx cells");
		return ALBA_ERROR;
	}

	if (idx >= points->GetNumberOfPoints())
	{
		albaErrorMacro("RemovePoint: point index out of range");
		return ALBA_ERROR;
	}

	// Squeeze the array
	vtkDataArray *pointdata = points->GetData();
	double *pnttuple = new double[pointdata->GetNumberOfComponents()];

	for (int i = idx; i < pointdata->GetNumberOfTuples() - 1; i++)
	{
		pointdata->GetTuple(i + 1, pnttuple);
		pointdata->SetTuple(i, pnttuple);
	}
	delete[] pnttuple;

	pointdata->Resize(pointdata->GetNumberOfTuples() - 1);

	// remove also the visibility of the point
	vtkDataArray *visibility = polydata->GetPointData()->GetScalars();
	if (visibility && idx < visibility->GetNumberOfTuples())
	{
		// vtkBitArray does not implement RemoveTuple: squeeze the array
		for (int i = idx; i < visibility->GetNumberOfTuples() - 1; i++)
			visibility->SetTuple1(i, visibility->GetTuple1(i + 1));

		visibility->Resize(visibility->GetNumberOfTuples() - 1);
	}

	// rebuild the vertex cells without the removed point, the ids of the following points are decreased
	vtkALBASmartPointer<vtkCellArray> newcells;
	vtkSmartPointer<vtkCellArrayIterator> cellIterator = cells->NewIterator();
	for (cellIterator->GoToFirstCell(); !cellIterator->IsDoneWithTraversal(); cellIterator->GoToNextCell())
	{
		vtkIdType npts;
		const vtkIdType* pts;
		cellIterator->GetCurrentCell(npts, pts);

		if (npts == 1 && pts[0] != idx)
		{
			newcells->InsertNextCell(1);
			newcells->InsertCellPoint(pts[0] > idx ? pts[0] - 1 : pts[0]);
		}
	}
	polydata->SetVerts(newcells);
	polydata->DeleteCells();

	points->Modified();
	polydata->Modified();

	return ALBA_OK;
}

//-------------------------------------------------------------------------
std::vector<vtkPolyData *> albaVMELandmarkCloud::GetAllPolyData()
{
	std::vector<vtkPolyData *> polydataList;

	for (albaDataVector::Iterator it = m_DataVector->Begin(); it != m_DataVector->End(); it++)
	{
		albaVMEItemVTK *item = albaVMEItemVTK::SafeDownCast(it->second);
		vtkPolyData *polydata = item ? vtkPolyData::SafeDownCast(item->GetData()) : NULL;
		if (polydata)
			polydataList.push_back(polydata);
	}

	return polydataList;
}
