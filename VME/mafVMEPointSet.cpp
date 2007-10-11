/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEPointSet.cpp,v $
  Language:  C++
  Date:      $Date: 2007-10-11 11:44:48 $
  Version:   $Revision: 1.8 $
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

#include "mafVMEPointSet.h"
#include "mafDataVector.h"
#include "mafVMEOutputPointSet.h"
#include "mafDataPipe.h"
#include "mmaMaterial.h"
#include "mafVMEItemVTK.h"

#include "vtkMAFSmartPointer.h"

#include "vtkPolyData.h"
#include "vtkCellArray.h"

//-------------------------------------------------------------------------
//mafCxxAbstractTypeMacro(mafVMEPointSet);
mafCxxTypeMacro(mafVMEPointSet);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEPointSet::mafVMEPointSet()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEPointSet::~mafVMEPointSet()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
int mafVMEPointSet::InternalInitialize()
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
int mafVMEPointSet::GetNumberOfPoints(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t=t<0?this->m_CurrentTime:t;

  vtkPolyData *polydata=this->GetPolyData(t);
  
  if (polydata)
  {
    return polydata->GetNumberOfPoints();
  }

  return 0;
}

//-------------------------------------------------------------------------
void mafVMEPointSet::SetNumberOfPoints(int num,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t=t<0?this->m_CurrentTime:t;

  vtkPolyData *polydata=this->GetPolyData(t);
  if (!polydata)
    polydata=this->NewPolyData(t);

  int oldnum=polydata->GetNumberOfPoints();

  if (num==oldnum)
    return;

  if (num>oldnum)
  {
    // add new points and cells
    this->AppendPoint(polydata,0,0,0,num-oldnum);
  }
  else
  {
    // remove points and cells
    int n=oldnum-num;

    for (int j=0;j<n;j++)
    {
      this->RemovePoint(polydata,oldnum-j-1);
    }
  }
}

//-------------------------------------------------------------------------
int mafVMEPointSet::AppendPoint(vtkPolyData *polydata,double x,double y,double z,int num)
//-------------------------------------------------------------------------
{
  int idx=-1;

  if (num>0)
  {

    vtkPoints *points=polydata->GetPoints();

    if (points)
    {
      for (int i=0;i<num;i++)
      {
        idx=points->InsertNextPoint(x,y,z);  
      }

      points->Modified();
      polydata->Modified();
    }
    else
    {
      mafErrorMacro("Corrupted PolyData inside VME-PointSet: Cannot found Points or Vertx cells");
    }
  }

  return idx;
}

//-------------------------------------------------------------------------
int mafVMEPointSet::AppendPoint(double x,double y,double z,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t=t<0?this->m_CurrentTime:t;

  // Retrieve polydata for time t or create a new one
  vtkPolyData *polydata=this->GetPolyData(t);
  if (!polydata)
    polydata=this->NewPolyData(t);

  if (!polydata)
  {
    mafErrorMacro("Currupted VME-item: wrong (not PolyData) or empty dataset in VME-PointSet"); 
    return -1;
  }

  return this->AppendPoint(polydata,x,y,z);
}

//-------------------------------------------------------------------------
// Retrieve a polydata, return NULL if does not exist
//-------------------------------------------------------------------------
vtkPolyData *mafVMEPointSet::GetPolyData(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(m_DataVector->GetItem(t));
  if (item)
  {
    return vtkPolyData::SafeDownCast(item->GetData());
  }
  return NULL;
}
//-------------------------------------------------------------------------
vtkPolyData *mafVMEPointSet::NewPolyData(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyData> polydata;
  vtkMAFSmartPointer<vtkPoints> points;
  polydata->SetPoints(points);
  this->SetDataByReference(polydata,t);
  return polydata;
}

//-------------------------------------------------------------------------
int mafVMEPointSet::SetPoint(int idx,double x,double y,double z,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  if (idx<0)
  {
    mafErrorMacro("SetPoint: point index out of range");
    return MAF_ERROR;
  }

  t = t < 0 ? this->m_CurrentTime : t;

  // Retrieve polydata for time t or create a new one
  vtkPolyData *polydata=this->GetPolyData(t);
  if (!polydata)
    polydata=this->NewPolyData(t);

  return this->SetPoint(polydata,idx,x,y,z); 
}


//-------------------------------------------------------------------------
int mafVMEPointSet::SetPoint(vtkPolyData *polydata,int idx,double x,double y,double z)
//-------------------------------------------------------------------------
{ 
  vtkPoints *points = polydata->GetPoints();

  if (!points)
  {
    mafErrorMacro("SetPoint: Corrupted PolyData inside VME-PointSet, Points' data not found");
    return MAF_ERROR;
  }

  // Should I automatically increase the number of points?
  if (idx >=points->GetNumberOfPoints())
  {
    mafErrorMacro("SetPoint: point index out of range");
    return MAF_ERROR;
    //this->SetNumberOfPoints(idx+1,t);
  }

  points->SetPoint(idx, x, y, z);
  points->Modified();

  polydata->Modified();

  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafVMEPointSet::GetPoint(int idx, double xyz[3],mafTimeStamp t)
//-------------------------------------------------------------------------
{
  t=t<0?this->m_CurrentTime:t;

  vtkPolyData *polydata;

  int ret = MAF_ERROR;

  if (t!=this->m_CurrentTime)
  {
    // force the pipe to the specified timestamp
    this->GetDataPipe()->SetTimeStamp(t);
    this->GetDataPipe()->Update();
  }

  polydata=(vtkPolyData *)this->GetPointSetOutput()->GetPointSetData();

  if (polydata)
  {
    polydata->Update();
    ret=this->GetPoint(polydata,idx,xyz);
  }
    
  // reset the pipe timestamp and its output
  if (t!=this->m_CurrentTime)
  {
    this->GetDataPipe()->SetTimeStamp(this->m_CurrentTime);
    this->GetDataPipe()->Update();
  }

  return ret;
}

//-------------------------------------------------------------------------
int mafVMEPointSet::GetPoint(vtkPolyData *polydata,int idx,double xyz[3])
//-------------------------------------------------------------------------
{
  if (!polydata)
  {
    mafErrorMacro("Currupted VME-item: wrong (not PolyData) or empty dataset in VME-PointSet"); 
    return MAF_ERROR;
  }

  vtkPoints *points = polydata->GetPoints();
  if (!points)
  {
    mafErrorMacro("Corrupted vtkPolyDaya in VME-item: Points data not found");
    return MAF_ERROR;
  }

  if (idx>=0 && idx <points->GetNumberOfPoints())
  {
    points->GetPoint(idx, xyz);

    return MAF_OK;
  }
  else
  {
    mafErrorMacro("GetPoint: point index out of range");
    return MAF_ERROR;
  }
}

//-------------------------------------------------------------------------
int mafVMEPointSet::GetPoint(int idx, double &x,double &y,double &z,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  double xyz[3];
  
  if (this->GetPoint(idx,xyz,t)==MAF_ERROR)
    return MAF_ERROR;

  x=xyz[0]; y=xyz[1]; z=xyz[2];

  return MAF_OK;
}
//-------------------------------------------------------------------------
int mafVMEPointSet::RemovePoint(vtkPolyData *polydata,int idx)
//-------------------------------------------------------------------------
{
  if (!polydata)
  {
    mafErrorMacro("RemovePoint: Currupted VME-item, wrong (not PolyData) or empty dataset in VME-PointSet"); 
    return MAF_ERROR;
  }

  vtkPoints *points=polydata->GetPoints();
  vtkCellArray *cells = polydata->GetVerts();

  if (points&&cells)
  {
    if (idx <points->GetNumberOfPoints())
    {
      // Squeeze the array 
      vtkDataArray *pointdata=points->GetData();
      vtkIdTypeArray *celldata=cells->GetData();
      
      double *pnttuple=new double[pointdata->GetNumberOfComponents()];

      for (int i=idx;i<pointdata->GetNumberOfTuples()-1;i++)
      {
        pointdata->GetTuple(i+1,pnttuple);
        pointdata->SetTuple(i,pnttuple);
      }
      delete[] pnttuple;

      pointdata->Resize(pointdata->GetNumberOfTuples()-1);

      points->Modified();
      polydata->Modified();
    }
    else
    {
      mafErrorMacro("RemovePoint: point index out of range");
      return MAF_ERROR;
    }
  }
  else
  {
    mafErrorMacro("RemovePoint: Currupted Polydata inside VME-PointSet, cannot found Points or Vertx cells");
    return MAF_ERROR;
  }

  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafVMEPointSet::RemovePoint(int idx,mafTimeStamp t)
//-------------------------------------------------------------------------
{
  if (idx<0)
  {
    mafErrorMacro("RemovePoint: point index out of range");
    return MAF_ERROR;
  }

  vtkPolyData *polydata=this->GetPolyData(t);
   
  if (polydata)
  {
    return this->RemovePoint(polydata,idx);
  }

  mafErrorMacro("Currupted VME-item: wrong (not PolyData) or empty dataset in VME-PointSet"); 
  return MAF_ERROR;
   
}
//-------------------------------------------------------------------------
char** mafVMEPointSet::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEPointSet.xpm"
  return mafVMEPointSet_xpm;
}
//-------------------------------------------------------------------------
mafVMEOutput *mafVMEPointSet::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputPointSet::New()); // create the output
  }
  return m_Output;
}
//-------------------------------------------------------------------------
int mafVMEPointSet::SetData(vtkPolyData *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  return Superclass::SetData(data,t,mode);
}
//-------------------------------------------------------------------------
int mafVMEPointSet::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);
  
  if (polydata)
    polydata->Update();

  if (polydata&&polydata->GetPolys()->GetNumberOfCells()==0&& \
    polydata->GetStrips()->GetNumberOfCells()==0&&polydata->GetLines()->GetNumberOfCells()==0)
  {
    return Superclass::SetData(data,t,mode);
  }

  mafErrorMacro("Trying to set the wrong type of fata inside a VME Image :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *mafVMEPointSet::GetMaterial()
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
