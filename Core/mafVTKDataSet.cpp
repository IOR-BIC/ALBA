/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVTKDataSet.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-02 00:31:48 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafDataSet.h"
#include "vtkObjectFactory.h"
#include "mflVME.h"
#include "mflBounds.h"

#include "vtkPointSet.h"

int mafDataSet::GlobalCompareDataFlag=0;

extern enum pippo {paperone=1, nonnapapera, gastone};

//-------------------------------------------------------------------------
mafDataSet* mafDataSet::New()
//-------------------------------------------------------------------------
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("mafDataSet");
  if(ret)
    {
    return (mafDataSet*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  mafDataSet *ptr=new mafDataSet;

  return ptr;
}


//-------------------------------------------------------------------------
mafDataSet::mafDataSet()
//-------------------------------------------------------------------------
{
  Data=NULL;
  ModifiedData=0;

  VME=NULL;

  ClientData=NULL;
	AutoDeleteClientData=0;
  //this->TagArray=NULL;
  TagArray=vtkTagArray::New();
  Id=-1;
  TimeStamp=0;

  Bounds=new mflBounds;
  Crypting = -1;
}

//-------------------------------------------------------------------------
mafDataSet::~mafDataSet()
//-------------------------------------------------------------------------
{ 
 
  this->TagArray->UnRegister(this);
  this->TagArray=NULL;

  if (this->Data)
  {
    this->Data->Delete();
    this->Data=NULL;
  }

  delete this->Bounds;
  this->Bounds=NULL;
}

//-------------------------------------------------------------------------
vtkDataSet *mafDataSet::GetData() 
//-------------------------------------------------------------------------
{
  this->UpdateData(); 
  return this->Data;
}

//-------------------------------------------------------------------------
int mafDataSet::GetCrypting()
//-------------------------------------------------------------------------
{
  RestoreNumericFromTag(GetTagArray(),"MFL_CRYPTING",(int)Crypting,-1,0);
  return Crypting;
}

//-------------------------------------------------------------------------
void mafDataSet::SetCrypting(int flag)
//-------------------------------------------------------------------------
{
  Crypting = flag;
  TagArray->SetTag(vtkTagItem("MFL_CRYPTING",Crypting));
  Modified();
}

//-------------------------------------------------------------------------
void mafDataSet::GetBounds(double bounds[6])
//-------------------------------------------------------------------------
{
  this->UpdateBounds();
  
  this->Bounds->CopyTo(bounds);
}

//-------------------------------------------------------------------------
void mafDataSet::GetBounds(mflBounds &bounds)
//-------------------------------------------------------------------------
{
  this->UpdateBounds();

  bounds.DeepCopy(this->Bounds);
}

//-------------------------------------------------------------------------
double *mafDataSet::GetBounds()
//-------------------------------------------------------------------------
{
  this->UpdateBounds();
  return this->Bounds->Bounds;
}

//-------------------------------------------------------------------------
void mafDataSet::UpdateBounds()
//-------------------------------------------------------------------------
{
  if (this->Data)
  {
    if (this->GetMTime()>this->Bounds->GetMTime()|| \
      this->Data->GetMTime()>this->Bounds->GetMTime() || \
      !this->Bounds->IsValid())
    {
      float fbounds[6];

      this->Data->Update();
      //this->Data->Modified();
      this->Data->GetBounds(fbounds);

      this->Bounds->DeepCopy(fbounds);

      this->GetTagArray()->SetTag(vtkTagItem("VTK_DATASET_BOUNDS",this->Bounds->Bounds,6));
    }
  }
  else
  {
    if (!this->Bounds->IsValid()&&this->Bounds->GetMTime()<this->GetMTime())
    {
      if (vtkTagItem *titem=this->GetTagArray()->GetTag("VTK_DATASET_BOUNDS"))
      {
        if ((titem->GetNumberOfComponents()==6)&&(titem->GetType()==vtkTagItem::MFL_NUMERIC_TAG))
        {
          for (int i=0;i<6;i++)
          {
            this->Bounds->Bounds[i]=titem->GetComponentAsDouble(i);
            this->Bounds->Modified();
          }
        }
      }

      if (!this->Bounds->IsValid())
      {
        // If no data is available and bounds are not updated and bounds
        // TAG is missing, UpdateData() and create the TAG:
        // this is necessary to make the regression test on TAGs not to fail.
        this->UpdateData();
        if (this->Data)
        {
          this->UpdateBounds();
        }
      }
    }
  }
}

//-------------------------------------------------------------------------
void mafDataSet::SetData(vtkDataSet *data)
//-------------------------------------------------------------------------
{
  if (this->Data!=data)
  {
    if (this->Data)
    {
      if (this->DataType==Data->GetClassName())
        this->DataType="";

      this->Data->UnRegister(this);
    }

    if (data)
    {
      data->Register(this);
      if (this->DataType.IsEmpty())
        this->SetDataType(data->GetClassName());

      float fbounds[6];
      data->Update();
      data->ComputeBounds();
      data->GetBounds(fbounds);

      this->Bounds->DeepCopy(fbounds);
      
      vtkTagItem *titem=this->GetTagArray()->GetTag("VTK_DATASET_BOUNDS");
      
      if (!titem)
      {
        vtkTagItem newitem("VTK_DATASET_BOUNDS",this->Bounds->Bounds,6);
        this->GetTagArray()->SetTag(newitem);
      }
      else
      {
        titem->SetComponents(this->Bounds->Bounds,6);
      }
    }
    else
    {
      this->DataType="";
      this->Bounds->Reset();
    }

    this->Data=data;

    this->UpdateTime.Modified();  //modified by Marco. 3-2-2004

    this->Modified();

    this->ModifiedDataOn();
  }
}

//-------------------------------------------------------------------------
vtkTagArray *mafDataSet::GetTagArray()
//-------------------------------------------------------------------------
{
  //if (!this->TagArray)
  //  this->TagArray=vtkTagArray::New();

  return this->TagArray;
}


//-------------------------------------------------------------------------
void mafDataSet::DeepCopy(mafDataSet *a)
//-------------------------------------------------------------------------
{
  this->SetData(NULL);
  if (a)
  {
    // copy Tags array content
    this->TagArray->DeepCopy(a->GetTagArray());

    a->UpdateData();

    if (a->GetData())
    {
      // create a data object of the same type of the input one
      vtkDataSet *newdata=vtkDataSet::SafeDownCast(a->GetData()->NewInstance());  //modified by Marco. 25-9-2003

      if (vtkPointSet *pset=vtkPointSet::SafeDownCast(newdata))
      {
        vtkPointSet *pset_orig=(vtkPointSet *)a->GetData();
        if (pset_orig->GetPoints())
          pset->SetPoints(vtkPoints::New(pset_orig->GetPoints()->GetDataType()));
       
      }

      newdata->DeepCopy(a->GetData());
     
      this->SetData(newdata); 
      newdata->Delete();
      
    }
  
    this->SetTimeStamp(a->GetTimeStamp());
    this->DataType=a->GetDataType();
    this->SetVME(a->GetVME());

    this->ModifiedDataOn();
  }
}



//-------------------------------------------------------------------------
void mafDataSet::ShallowCopy(mafDataSet *a)
//-------------------------------------------------------------------------
{
  if (a)
  {
    // make a copy of tags
    this->TagArray->DeepCopy(a->GetTagArray());

    this->SetTimeStamp(a->GetTimeStamp());
    this->DataType=a->GetDataType();
    this->SetVME(a->GetVME());

    // simply copy data pointer
    this->SetData(a->GetData());
    
  }  
}


//-------------------------------------------------------------------------
// Update the internally stored dataset.
void mafDataSet::UpdateData()
//-------------------------------------------------------------------------
{
  // If the data has been externally modified, and is not NULL ask the
  // pipeline to update.
  if (this->IsDataModified()&&this->Data)
  {
    this->Data->Update();
    //this->UpdateBounds();
    return;
  }
   
  // At present... if data is already present, simply return
  // otherwise make it be read from disk. Notice that when read
  // from this SetData() is called: Bounds are updated but we need 
  // to reset the ModifiedData flag.
  if (!this->Data)
  {
    if (this->VME)
    {
      this->VME->UpdateData(this);
      // Data has been generated internally
      this->ModifiedDataOff();
    }
  }
}

//-------------------------------------------------------------------------
void mafDataSet::DataHasBeenGenerated()
//-------------------------------------------------------------------------
{
  //this->DataReleased = 0;
  this->UpdateTime.Modified();
}

//-------------------------------------------------------------------------
bool mafDataSet::Equals(mafDataSet *item)
//-------------------------------------------------------------------------
{
  if (item==NULL || !(this->DataType.Equals(item->DataType)) || \
    this->TimeStamp!=item->TimeStamp)
  {
    return false;
  }

  this->UpdateBounds(); item->UpdateBounds();
  if (!this->Bounds->Equals(item->Bounds))
  {
    return false;
  }

  // must check the tags after updating the bounds, since UpdateBounds() could 
  // generate a new TAG for storing bounds.
  if (!this->GetTagArray()->Equals(item->GetTagArray()))
  {
    return false;
  }

  if (this->GlobalCompareDataFlag)
  {
    vtkDataSet *data1=this->GetData();
    vtkDataSet *data2=item->GetData();
    if (data1&&data2)
    {
      // We test for equivalency of data types. It could happen that the written 
      // dataset is inherited from a standard dataset type, thus when we read it 
      // back the data type is changed but data is the same. E.g. vtkImageData
      // are always written as vtkStructuredPoints by vtkDataSetWriter, thus when 
      // read back it becomes a vtkStructuredPoints with the same data.
      if (data1->SafeDownCast(data2)==NULL&&data2->SafeDownCast(data1)==NULL)
        return false;

      int pnt1=data1->GetNumberOfPoints();
      int pnt2=data2->GetNumberOfPoints();
      int cell1=data1->GetNumberOfCells();
      int cell2=data2->GetNumberOfCells();

      if (pnt1!=pnt2 || cell1!=cell2)
        return false;
      
      data1->ComputeBounds();
      data2->ComputeBounds();

      float bounds1[6],bounds2[6];
      data1->GetBounds(bounds1);
      data2->GetBounds(bounds2);

      if (bounds1[0]!=bounds2[0]||bounds1[1]!=bounds2[1]||bounds1[2]!=bounds2[2]|| \
        bounds1[3]!=bounds2[3]||bounds1[4]!=bounds2[4]||bounds1[5]!=bounds2[5])
      {
        return false;
      }

      float range1[2],range2[2];
      data1->GetScalarRange(range1);
      data2->GetScalarRange(range2);

      if (range1[0]!=range2[0]||range1[1]!=range2[1])
      {
        return false;
      }
    }
    else
    {
      if (data1!=data2)
      {
        return false;
      }
    }
  }

  return true;

}

//-------------------------------------------------------------------------
void mafDataSet::PrintSelf(ostream& os, vtkIndent indent)
//-------------------------------------------------------------------------
{
  os << indent << "mafDataSet Contents:\n";

  this->vtkObject::PrintSelf(os,indent);

  os << indent << "DataType: " << (this->GetData()?this->GetData()->GetClassName():"(null)");

  os << indent << "TimeStamp: "<< this->GetTimeStamp();

  os << indent << "Item Tags:\n";
  if (this->TagArray)
    this->TagArray->PrintSelf(os,indent.GetNextIndent());
}

