/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGenericAbstract.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:05:59 $
  Version:   $Revision: 1.25 $
  Authors:   Marco Petrone
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



#include "mafVMEGenericAbstract.h"
#include "mafGUI.h"

#include "mafDataVector.h"
#include "mafMatrixVector.h"
#include "mafMatrixInterpolator.h"
#include "mmuTimeSet.h"
#include "mafNodeIterator.h"
#include "mafVMEStorage.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEGenericAbstract)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEGenericAbstract::mafVMEGenericAbstract()
//-------------------------------------------------------------------------
{
	m_MatrixVector = new mafMatrixVector();
  m_DataVector   = NULL;
  SetMatrixPipe(mafMatrixInterpolator::New()); // matrix interpolator pipe  
}

//-------------------------------------------------------------------------
mafVMEGenericAbstract::~mafVMEGenericAbstract()
//-------------------------------------------------------------------------
{
  cppDEL(m_MatrixVector);
  cppDEL(m_DataVector);
  SetMatrixPipe(NULL); // destroy matrix pipe
}

//-------------------------------------------------------------------------
int mafVMEGenericAbstract::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEGenericAbstract *vme=(mafVMEGenericAbstract *)a;
    m_MatrixVector->DeepCopy(vme->GetMatrixVector());

    if (vme->GetDataVector())
    {
      if(m_DataVector == NULL)
        m_DataVector=vme->GetDataVector()->NewInstance(); // create a new instance of the same type
      m_DataVector->DeepCopy(vme->GetDataVector()); // copy data
      m_DataVector->SetListener(this);
    }
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEGenericAbstract::ShallowCopy(mafVME *a)
//-------------------------------------------------------------------------
{  
  if (Superclass::ShallowCopy(a)==MAF_OK)
  {
    mafVMEGenericAbstract *vme=(mafVMEGenericAbstract *)a;
    m_MatrixVector->DeepCopy(vme->GetMatrixVector());    
    
    // shallow copy data array
    if (m_DataVector)
    {
      m_DataVector->ShallowCopy(vme->GetDataVector());
    }
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMEGenericAbstract::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    mafVMEGenericAbstract *gvme=mafVMEGenericAbstract::SafeDownCast(vme);
    if (m_DataVector)
    {
      if (m_MatrixVector->Equals(gvme->GetMatrixVector()) && 
        m_DataVector->Equals(gvme->GetDataVector()))
        return true;
    }
    else
    {
      if (m_MatrixVector->Equals(gvme->GetMatrixVector()))
        return true;
    }
  }
  return false;
}

//-------------------------------------------------------------------------
void mafVMEGenericAbstract::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_MatrixVector->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool mafVMEGenericAbstract::IsAnimated()
//-------------------------------------------------------------------------
{
  if (m_DataVector)
  {
    return ((m_DataVector->GetNumberOfItems()>1)||(m_MatrixVector->GetNumberOfItems()>1));
  }
  else
  {
    return (m_MatrixVector->GetNumberOfItems()>1);
  }
}

//-------------------------------------------------------------------------
bool mafVMEGenericAbstract::IsDataAvailable()
//-------------------------------------------------------------------------
{
  if (m_DataVector)
  {
    mafTimeStamp t = this->GetTimeStamp();
    mafVMEItem *item = m_DataVector->GetItem(t);
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
    return item ? item->IsDataPresent() : true; // If item is NULL -> the data is generated by code
                                                // or is into its child as for the mafVMELandmarkCloud
  }
  else
    return Superclass::IsDataAvailable();
}

//-------------------------------------------------------------------------
void mafVMEGenericAbstract::GetDataTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  if (m_DataVector)
  {
    m_DataVector->GetTimeStamps(kframes);
  }
}

//-------------------------------------------------------------------------
void mafVMEGenericAbstract::GetMatrixTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  m_MatrixVector->GetTimeStamps(kframes);
}

//-------------------------------------------------------------------------
void mafVMEGenericAbstract::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();

  std::vector<mafTimeStamp> datatimestamps;
  std::vector<mafTimeStamp> matrixtimestamps;
  
  if (m_DataVector)
  {
    m_DataVector->GetTimeStamps(datatimestamps);
  }
  m_MatrixVector->GetTimeStamps(matrixtimestamps);

  mmuTimeSet::Merge(datatimestamps,matrixtimestamps,kframes);
}

//-------------------------------------------------------------------------
void mafVMEGenericAbstract::GetLocalTimeBounds(mafTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  tbounds[0]=0.0;
  tbounds[1]=0.0;
  if (m_DataVector)
  {
    m_DataVector->GetTimeBounds(tbounds);
  }

  mafTimeStamp tmp[2];
  m_MatrixVector->GetTimeBounds(tmp);

  tmp[0] = tmp[0] < 0 ? 0 : tmp[0];
  tmp[1] = tmp[1] < 0 ? 0 : tmp[1];

  if (tmp[0]<tbounds[0]||tbounds[0]<0)
    tbounds[0]=tmp[0];

  if (tmp[1]>tbounds[1]||tbounds[1]<0)
    tbounds[1]=tmp[1];
}

//-----------------------------------------------------------------------
int mafVMEGenericAbstract::ReparentTo(mafNode *parent)
//-----------------------------------------------------------------------
{
  if (CanReparentTo(parent) && !IsInTree(parent))
  {
    // When we re-parent to a different tree, or we simply
    // cut a tree, before traverse the sub tree to read data into memory
    // future release should read one item at once, write it
    // to disk and then release the data, or better simply copy the file
    // into the new place, this to be able to manage HUGE datasets.
    if (parent /*== NULL||*/ && this->GetRoot() != parent->GetRoot())
    {
      mafNodeIterator *iter=this->NewIterator();
      for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
      {
        if (mafVMEGenericAbstract *vme = mafVMEGenericAbstract::SafeDownCast(node))
        {
          mafDataVector *dvector = vme->GetDataVector();
          if(dvector)
          {
            for (int i = 0; i < dvector->GetNumberOfItems(); i++)
            {
              mafVMEItem *item = dvector->GetItemByIndex(i);
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
                  dvector->Modified(); // Alert the data vector that the item is changed.
                }
              }
              else
              {
                mafErrorMacro("found a NULL item in the node!!!!");
              }
            }
          }
        }
      }
      iter->Delete();
      if (m_DataVector)
      {
        m_DataVector->Modified();
      }
    }
    return Superclass::ReparentTo(parent);
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMEGenericAbstract::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  Superclass::InternalStore(parent);

  // sub-element for storing the data vector
  if (m_DataVector)
  {
    m_DataVector->SetCrypting(this->m_Crypting != 0);
    mafStorageElement *data_vector = parent->AppendChild("DataVector");
    if(m_DataVector->Store(data_vector) == MAF_ERROR)
      return MAF_ERROR;
  }

  // sub-element for storing the matrix vector
  mafStorageElement *matrix_vector = parent->AppendChild("MatrixVector");
  if(m_MatrixVector->Store(matrix_vector) == MAF_ERROR)
    return MAF_ERROR;

  return MAF_OK;
}

//-----------------------------------------------------------------------
int mafVMEGenericAbstract::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  int ret_val = MAF_OK;
  Superclass::InternalRestore(node);
  
  // restore Data Vector
  if (m_DataVector)
  {
    mafStorageElement *data_vector=node->FindNestedElement("DataVector");
    if (data_vector)
    {
      ret_val = m_DataVector->Restore(data_vector);
    }
  }
  // restore Matrix Vector  
  if (m_MatrixVector && ret_val == MAF_OK)
  {
    mafStorageElement *matrix_vector=node->FindNestedElement("MatrixVector");
    if (matrix_vector)
    {
      ret_val = m_MatrixVector->Restore(matrix_vector);
    }
  }
  return ret_val;
}
//-------------------------------------------------------------------------
mafGUI *mafVMEGenericAbstract::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
#ifdef MAF_USE_CRYPTO
  m_Gui->Bool(ID_VME_CRYPTING,"crypt",&m_Crypting);
#endif
	m_Gui->Divider();
  return m_Gui;
}

//-------------------------------------------------------------------------
void mafVMEGenericAbstract::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch (e->GetId())
    {
      case ID_VME_CRYPTING:
        SetCrypting(m_Crypting);
      break;
      //default:
        //Superclass::OnEvent(maf_event);
    }
  }
  else if (maf_event->GetChannel()==MCH_DOWN)
  {
    if (maf_event->GetId() == mafVMEStorage::MSF_FILENAME_CHANGED)
    {
      // force the data vector to save its data to file
      if(GetDataVector())
      {
        GetDataVector()->Modified();
      }
    } 
  }

  Superclass::OnEvent(maf_event);
}

//-----------------------------------------------------------------------
void mafVMEGenericAbstract::Print(std::ostream& os, const int tabs)// const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  os << indent << "Encryption: ";
  os << (m_Crypting != 0) ? "On" : "Off";
  os << "\n";

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
char** mafVMEGenericAbstract::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEGeneric.xpm"
  return mafVMEGeneric_xpm;
}
