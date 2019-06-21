/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPointSet
 Authors: Marco Petrone, Paolo Quadrani
 
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

#include "albaVMEPointSet.h"
#include "albaDataVector.h"
#include "albaVMEOutputPointSet.h"
#include "albaDataPipe.h"
#include "mmaMaterial.h"
#include "albaVMEItemVTK.h"

#include "vtkALBASmartPointer.h"

#include "vtkPolyData.h"
#include "vtkCellArray.h"

//-------------------------------------------------------------------------
//albaCxxAbstractTypeMacro(albaVMEPointSet);
albaCxxTypeMacro(albaVMEPointSet);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEPointSet::albaVMEPointSet()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
albaVMEPointSet::~albaVMEPointSet()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
int albaVMEPointSet::InternalInitialize()
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
int albaVMEPointSet::GetNumberOfPoints(albaTimeStamp t)
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
void albaVMEPointSet::SetNumberOfPoints(int num,albaTimeStamp t)
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
int albaVMEPointSet::AppendPoint(vtkPolyData *polydata,double x,double y,double z,int num)
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
      albaErrorMacro("Corrupted PolyData inside VME-PointSet: Cannot found Points or Vertx cells");
    }
  }

  return idx;
}

//-------------------------------------------------------------------------
int albaVMEPointSet::AppendPoint(double x,double y,double z,albaTimeStamp t)
//-------------------------------------------------------------------------
{
  t=t<0?this->m_CurrentTime:t;

  // Retrieve polydata for time t or create a new one
  vtkPolyData *polydata=this->GetPolyData(t);
  if (!polydata)
    polydata=this->NewPolyData(t);

  if (!polydata)
  {
    albaErrorMacro("Currupted VME-item: wrong (not PolyData) or empty dataset in VME-PointSet"); 
    return -1;
  }

  return this->AppendPoint(polydata,x,y,z);
}

//-------------------------------------------------------------------------
// Retrieve a polydata, return NULL if does not exist
//-------------------------------------------------------------------------
vtkPolyData *albaVMEPointSet::GetPolyData(albaTimeStamp t)
//-------------------------------------------------------------------------
{
  albaVMEItemVTK *item = albaVMEItemVTK::SafeDownCast(m_DataVector->GetItem(t));
  if (item)
  {
    return vtkPolyData::SafeDownCast(item->GetData());
  }
  return NULL;
}
//-------------------------------------------------------------------------
vtkPolyData *albaVMEPointSet::NewPolyData(albaTimeStamp t)
//-------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPolyData> polydata;
  vtkALBASmartPointer<vtkPoints> points;
  polydata->SetPoints(points);
  this->SetDataByReference(polydata,t);
  return polydata;
}

//-------------------------------------------------------------------------
int albaVMEPointSet::SetPoint(int idx,double x,double y,double z,albaTimeStamp t)
//-------------------------------------------------------------------------
{
  if (idx<0)
  {
    albaErrorMacro("SetPoint: point index out of range");
    return ALBA_ERROR;
  }

  t = t < 0 ? this->m_CurrentTime : t;

  // Retrieve polydata for time t or create a new one
  vtkPolyData *polydata=this->GetPolyData(t);
  if (!polydata)
    polydata=this->NewPolyData(t);

  return this->SetPoint(polydata,idx,x,y,z); 
}


//-------------------------------------------------------------------------
int albaVMEPointSet::SetPoint(vtkPolyData *polydata,int idx,double x,double y,double z)
//-------------------------------------------------------------------------
{ 
  vtkPoints *points = polydata->GetPoints();

  if (!points)
  {
    albaErrorMacro("SetPoint: Corrupted PolyData inside VME-PointSet, Points' data not found");
    return ALBA_ERROR;
  }

  // Should I automatically increase the number of points?
  if (idx >=points->GetNumberOfPoints())
  {
    albaErrorMacro("SetPoint: point index out of range");
    return ALBA_ERROR;
    //this->SetNumberOfPoints(idx+1,t);
  }

  points->SetPoint(idx, x, y, z);
  points->Modified();

  polydata->Modified();

  return ALBA_OK;
}

//-------------------------------------------------------------------------
int albaVMEPointSet::GetPoint(int idx, double xyz[3],albaTimeStamp t)
//-------------------------------------------------------------------------
{
  t=t<0?this->m_CurrentTime:t;

  vtkPolyData *polydata;

  int ret = ALBA_ERROR;

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
int albaVMEPointSet::GetPoint(vtkPolyData *polydata,int idx,double xyz[3])
//-------------------------------------------------------------------------
{
  if (!polydata)
  {
    albaErrorMacro("Currupted VME-item: wrong (not PolyData) or empty dataset in VME-PointSet"); 
    return ALBA_ERROR;
  }

  vtkPoints *points = polydata->GetPoints();
  if (!points)
  {
    albaErrorMacro("Corrupted vtkPolyDaya in VME-item: Points data not found");
    return ALBA_ERROR;
  }

  if (idx>=0 && idx <points->GetNumberOfPoints())
  {
    points->GetPoint(idx, xyz);

    return ALBA_OK;
  }
  else
  {
    albaErrorMacro("GetPoint: point index out of range");
    return ALBA_ERROR;
  }
}

//-------------------------------------------------------------------------
int albaVMEPointSet::GetPoint(int idx, double &x,double &y,double &z,albaTimeStamp t)
//-------------------------------------------------------------------------
{
  double xyz[3];
  
  if (this->GetPoint(idx,xyz,t)==ALBA_ERROR)
    return ALBA_ERROR;

  x=xyz[0]; y=xyz[1]; z=xyz[2];

  return ALBA_OK;
}
//-------------------------------------------------------------------------
int albaVMEPointSet::RemovePoint(vtkPolyData *polydata,int idx)
//-------------------------------------------------------------------------
{
  if (!polydata)
  {
    albaErrorMacro("RemovePoint: Currupted VME-item, wrong (not PolyData) or empty dataset in VME-PointSet"); 
    return ALBA_ERROR;
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
      albaErrorMacro("RemovePoint: point index out of range");
      return ALBA_ERROR;
    }
  }
  else
  {
    albaErrorMacro("RemovePoint: Currupted Polydata inside VME-PointSet, cannot found Points or Vertx cells");
    return ALBA_ERROR;
  }

  return ALBA_OK;
}

//-------------------------------------------------------------------------
int albaVMEPointSet::RemovePoint(int idx,albaTimeStamp t)
//-------------------------------------------------------------------------
{
  if (idx<0)
  {
    albaErrorMacro("RemovePoint: point index out of range");
    return ALBA_ERROR;
  }

  vtkPolyData *polydata=this->GetPolyData(t);
   
  if (polydata)
  {
    return this->RemovePoint(polydata,idx);
  }

  albaErrorMacro("Currupted VME-item: wrong (not PolyData) or empty dataset in VME-PointSet"); 
  return ALBA_ERROR;
   
}
//-------------------------------------------------------------------------
char** albaVMEPointSet::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "albaVMEPointSet.xpm"
  return albaVMEPointSet_xpm;
}
//-------------------------------------------------------------------------
albaVMEOutput *albaVMEPointSet::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(albaVMEOutputPointSet::New()); // create the output
  }
  return m_Output;
}
//-------------------------------------------------------------------------
int albaVMEPointSet::SetData(vtkPolyData *data, albaTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  //return Superclass::SetData(data,t,mode);
  return albaVMEPointSet::SetData((vtkDataSet*)data,t,mode);
}
//-------------------------------------------------------------------------
int albaVMEPointSet::SetData(vtkDataSet *data, albaTimeStamp t, int mode)
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

  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *albaVMEPointSet::GetMaterial()
//-------------------------------------------------------------------------
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
