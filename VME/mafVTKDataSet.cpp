/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVTKDataSet.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:16:33 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafVTKDataSet.h"
#include "vtkDataSet.h"

#ifdef VTK_USE_ANSI_STDLIB
#include <strstrea.h>"
#endif

//-------------------------------------------------------------------------
mafVTKDataSet::mafVTKDataSet()
//-------------------------------------------------------------------------
{
  m_VTKData = NULL;
}
//-------------------------------------------------------------------------
mafVTKDataSet::~mafVTKDataSet()
//-------------------------------------------------------------------------
{
  vtkDEL(m_VTKData);
}

//-------------------------------------------------------------------------
vtkDataSet *mafVTKDataSet::GetVTKData()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void mafVTKDataSet::SetData(vtkDataSet *data)
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
vtkDataSet *mafVTKDataSet::GetData()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
void mafVTKDataSet::DeepCopy(mafVMEItem *a)
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
void mafVTKDataSet::ShallowCopy(mafVMEItem *a)
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
bool mafVTKDataSet::Equals(mafVMEItem *item)
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
int mafVTKDataSet::InternalRestoreData()
//-------------------------------------------------------------------------
{
  mafStorage *storage=node->GetStorage();
  mafString filename;
  
  storage->ResolveInputURL(GetURL(),filename);
 
  if (filename.IsEmpty())
  {
    return MAF_ERROR;
  }

  mflString pathname=this->Parser->GetFileName();

  pathname.ExtractPathName();

  pathname.AppendPath(item->GetFileName());

  mflString datatype=item->GetDataType();

  std::string file_string;
  vtkDataSet *data;
  vtkDataReader *reader;

  if (item->GetCrypting())
  {
//    mflString tmp_filename=pathname+".tmp";
//    mflDefaultDecryptFile(pathname, tmp_filename);
    mflDefaultDecryptInMemory(pathname, file_string);
    
//    pathname=tmp_filename;
  }

  // Workarround for double read bug of the vtkDataSetReader class
  // Read immediatelly the data and destroy the reader to close the input file
  if (datatype=="vtkPolyData")
  {
    reader=vtkPolyDataReader::New();
    if (item->GetCrypting())
    {
      reader->ReadFromInputStringOn();
      reader->SetInputString(file_string.c_str(),file_string.size());
    }
    else
      reader->SetFileName(pathname.GetCStr());
    reader->Update();
    data=((vtkPolyDataReader *)reader)->GetOutput();
  }
  else if (datatype=="vtkStructuredPoints" || datatype=="vtkImageData")
  {
    reader=vtkStructuredPointsReader::New();
    reader->SetFileName(pathname.GetCStr());
    reader->Update();
    data=((vtkStructuredPointsReader *)reader)->GetOutput();
  }
  else if (datatype=="vtkStructuredGrid")
  {
    reader=vtkStructuredGridReader::New();
    reader->SetFileName(pathname.GetCStr());
    reader->Update();
    data=((vtkStructuredGridReader *)reader)->GetOutput();
  }
  else if (datatype=="vtkRectilinearGrid")
  {
    reader=vtkRectilinearGridReader::New();
    reader->SetFileName(pathname.GetCStr());
    reader->Update();
    data=((vtkRectilinearGridReader *)reader)->GetOutput();
  }
  else if (datatype=="vtkUnstructuredGrid")
  {
    reader=vtkUnstructuredGridReader::New();
    reader->SetFileName(pathname.GetCStr());
    reader->Update();
    data=((vtkUnstructuredGridReader *)reader)->GetOutput();
  }
  else
  {
    // using generic vtkdataset reader...
    vtkGenericWarningMacro("Unknown data type, using generic VTK dataset reader");
    reader=vtkDataSetReader::New();
    reader->SetFileName(pathname.GetCStr());
    reader->Update();
    data=((vtkDataSetReader *)reader)->GetOutput();
  }
 
  if (data==NULL)
  {
    vtkGenericWarningMacro("Cannot read data file "<<pathname.GetCStr());
    return -1;
  }
  else
  {
    item->SetData(data);
    data->SetSource(NULL);
  }

  reader->Delete();

  return 0;
}

//-------------------------------------------------------------------------
int mafVTKDataSet::InternalStoreData(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  
}

//-------------------------------------------------------------------------
bool mafVTKDataSet::IsDataPresent()
//-------------------------------------------------------------------------
{
  return (m_VTKData!=NULL);
}
//-------------------------------------------------------------------------
void mafVTKDataSet::Print(std::ostream& os, const int tabs)
//-------------------------------------------------------------------------
{
  Superclass::Print(os,tabs);

  mafIndent indent(tabs);

  os << indent << "mafVTKDataSet Contents:\n";

  Superclass::Print(os,indent);

  os << indent << "VTK DataSet dump:\n";
  if (m_VTKData)
  {
#ifdef VTK_USE_ANSI_STDLIB
    m_VTKData->PrintSelf(os,vtkIndent(tabs+1));
#else
    ostrstream sstr;
    m_VTKData->PrintSelf(sstr,vtkIndent(tabs+1));
    os<<sstr.str();
#endif
  }
}

//-------------------------------------------------------------------------
int mafVTKDataSet::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  Superclass::InternalStore(parent);
  

  return MAF_OK;
}

//-------------------------------------------------------------------------
int mafVTKDataSet::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  mafString crypting;
  mafObject *obj;
  if (node->RestoreText(m_URL,"URL")==MAF_OK &&
      node->RestoreInteger(m_Id,"Id")==MAF_OK &&
      node->RestoreText(m_DataType,"DataType")==MAF_OK &&
      node->RestoreText(crypting,"Crypting")==MAF_OK &&
      node->RestoreVectorN(m_Bounds->m_Bounds,6,"Bounds")==MAF_OK &&
      node->RestoreObject(obj,"TagArray")==MAF_OK)
  {
    m_Crypting = (crypting=="true"||crypting=="True"||crypting=="TRUE")?true:false;
    mafTagArray *tarray=mafTagArray::SafeDownCast(obj);

    assert(tarray);
    mafDEL(m_TagArray);
    m_TagArray=mafTagArray::SafeDownCast(tarray);

    assert(m_TagArray);
    m_TagArray->Register(this);

    return MAF_OK;
  }

  return MAF_ERROR;
}