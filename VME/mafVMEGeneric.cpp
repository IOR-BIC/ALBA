/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGeneric.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 10:13:35 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafVMEGeneric.h"
#include "mafDataVector.h"
#include "mafMatrixVector.h"
#include "mmuTimeSet.h"
#include "mafNodeIterator.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEGeneric)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEGeneric::mafVMEGeneric()
//-------------------------------------------------------------------------
{
	cppNEW(m_MatrixVector);
  m_DataVector  = NULL;
}

//-------------------------------------------------------------------------
mafVMEGeneric::~mafVMEGeneric()
//-------------------------------------------------------------------------
{
  cppDEL(m_MatrixVector);
  cppDEL(m_DataVector);
}

//-------------------------------------------------------------------------
int mafVMEGeneric::DeepCopy(mafVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEGeneric *vme=(mafVMEGeneric *)a;
    m_MatrixVector->DeepCopy(vme->GetMatrixVector());

    m_DataVector=vme->GetDataVector()->NewInstance(); // create a new instance of the same type
    m_DataVector->DeepCopy(vme->GetDataVector()); // copy data

    return MAF_OK;
  }
  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEGeneric::ShallowCopy(mafVME *a)
//-------------------------------------------------------------------------
{  
  if (Superclass::ShallowCopy(a)==MAF_OK)
  {
    mafVMEGeneric *vme=(mafVMEGeneric *)a;
    
    m_MatrixVector->DeepCopy(vme->GetMatrixVector());    
    
    // shallow copy data array
    m_DataVector->ShallowCopy(vme->GetDataVector());

    return MAF_OK;
  }

  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMEGeneric::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    mafVMEGeneric *gvme=mafVMEGeneric::SafeDownCast(vme);
    if (m_MatrixVector->Equals(gvme->GetMatrixVector()) && 
        m_DataVector->Equals(gvme->GetDataVector()))
      return true;
  }
  return false;
}

//-------------------------------------------------------------------------
void mafVMEGeneric::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_MatrixVector->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool mafVMEGeneric::IsAnimated()
//-------------------------------------------------------------------------
{
  return ((m_DataVector->GetNumberOfItems()>1)||(m_MatrixVector->GetNumberOfItems()>1));
}

//-------------------------------------------------------------------------
void mafVMEGeneric::GetDataTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  m_DataVector->GetTimeStamps(kframes);
}

//-------------------------------------------------------------------------
void mafVMEGeneric::GetMatrixTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  m_MatrixVector->GetTimeStamps(kframes);
}

//-------------------------------------------------------------------------
void mafVMEGeneric::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();

  std::vector<mafTimeStamp> datatimestamps;
  std::vector<mafTimeStamp> matrixtimestamps;
  
  m_DataVector->GetTimeStamps(datatimestamps);
  m_MatrixVector->GetTimeStamps(matrixtimestamps);

  mmuTimeSet::Merge(datatimestamps,matrixtimestamps,kframes);
}

//-----------------------------------------------------------------------
mafVMEGeneric *mafVMEGeneric::ReparentTo(mafVMEGeneric *parent)
//-----------------------------------------------------------------------
{
  if (CanReparentTo(parent)&&!IsInTree(parent))
  {

    // When we reparent to a different tree, or we simply
    // cut a tree, pre travers the sub tree to read data into memory
    // future release should read one item at once, write it
    // to disk and then release the data, or better simply copy the file
    // into the new place, this to be able to manage HUGE datasets.
    if (parent==NULL||this->GetRoot()!=parent->GetRoot())
    {
      mafNodeIterator *iter=this->NewIterator();
      for (mafNode *node=iter->GetFirstNode();node;node=iter->GetNextNode())
      {
        if (mafVMEGeneric *vme=mafVMEGeneric::SafeDownCast(node))
        {
          mafDataVector *dvector=vme->GetDataVector();
          assert(dvector);
          for (int i=0;i<dvector->GetNumberOfItems();i++)
          {
            mafVMEItem *item=dvector->GetItem(i);
            assert(item);
            if (item)
            {
              // read the data from disk and if data is present 
              // set the Id to -1 to advise the reader to write
              // it again on disk. Also remove the old file name...
              item->UpdateData();
              if (item->IsDataPresent())
              {
                item->SetId(-1);
                item->SetURL("");
              }
            }
            else
            {
              mafErrorMacro("found a NULL item in the node!!!!");
            }
          }
        }
        
      }
      iter->Delete();
    }

    return this;
  }

  return NULL;
}

//-----------------------------------------------------------------------
int mafVMEGeneric::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  Superclass::InternalStore(parent);

  // sub-element for storing the data vector
  mafStorageElement *data_vector=parent->AppendChild("DataVector");
  if (m_DataVector->Store(data_vector))
    return MAF_ERROR;

  // sub-element for storing the matrix vector
  mafStorageElement *matrix_vector=parent->AppendChild("MatrixVector");
  if (m_MatrixVector->Store(matrix_vector))
    return MAF_ERROR;

  return MAF_OK;
}

//-----------------------------------------------------------------------
int mafVMEGeneric::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  Superclass::InternalRestore(node); 
  
  // restore Data Vector
  mafStorageElement *data_vector=node->FindNestedElement("DataVector");
  if (data_vector)
  {
    m_DataVector->Restore(data_vector);
  
    // restore Matrix Vector  
    mafStorageElement *matrix_vector=node->FindNestedElement("MatrixVector");
    if (matrix_vector)
    {
      m_MatrixVector->Restore(matrix_vector);

      return MAF_OK;
    }
  }

  return MAF_ERROR;
}

//-----------------------------------------------------------------------
void mafVMEGeneric::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  os << indent << "DataVector:"; 
  if (m_DataVector)
  {
    os << "\n";
    m_DataVector->Print(os,indent.GetNextIndent());
  }
  else
  {
    os << "(NULL)\n";
  }

  os << indent << "MatrixVector:"; 
  if (m_MatrixVector)
  {
    os << "\n";
    m_MatrixVector->Print(os,indent.GetNextIndent());
  }
  else
  {
    os << "(NULL)\n";
  }
}
//-------------------------------------------------------------------------
char** mafVMEGeneric::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEGeneric.xpm"
  return mafVMEGeneric_xpm;
}
