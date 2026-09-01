/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGenericAbstract
 Authors: Marco Petrone
 
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



#include "albaVMEGenericAbstract.h"
#include "albaGUI.h"

#include "albaDataPipe.h"
#include "albaDataVector.h"
#include "albaMatrixVector.h"
#include "albaMatrixInterpolator.h"
#include "mmuTimeSet.h"
#include "albaVMEIterator.h"
#include "albaVMEStorage.h"
#include "albaDataPipeInterpolatorVTK.h"
#include "albaVMEItemVTK.h"

#include <assert.h>

#include "vtkALBASmartPointer.h"
#include "vtkALBADataPipe.h"
#include "vtkDataSet.h"
#include "vtkDataSetWriter.h"
#include "vtkDataSetReader.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEGenericAbstract)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEGenericAbstract::albaVMEGenericAbstract()
//-------------------------------------------------------------------------
{
	m_MatrixVector = new albaMatrixVector();
  m_DataVector   = NULL;
	m_StoreDataVector = true;
  SetMatrixPipe(albaMatrixInterpolator::New()); // matrix interpolator pipe  
}

//-------------------------------------------------------------------------
albaVMEGenericAbstract::~albaVMEGenericAbstract()
//-------------------------------------------------------------------------
{
  cppDEL(m_MatrixVector);
  cppDEL(m_DataVector);
  SetMatrixPipe(NULL); // destroy matrix pipe
}

//-------------------------------------------------------------------------
int albaVMEGenericAbstract::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    albaVMEGenericAbstract *vme=(albaVMEGenericAbstract *)a;
    m_MatrixVector->DeepCopy(vme->GetMatrixVector());

    if (vme->GetDataVector())
    {
      if(m_DataVector == NULL)
        m_DataVector=vme->GetDataVector()->NewInstance(); // create a new instance of the same type
      m_DataVector->DeepCopy(vme->GetDataVector()); // copy data
      m_DataVector->SetListener(this);
    }
    return ALBA_OK;
  }
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
int albaVMEGenericAbstract::DeepCopyVmeLarge(albaVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    albaVMEGenericAbstract *vme=(albaVMEGenericAbstract *)a;
    m_MatrixVector->DeepCopy(vme->GetMatrixVector());

    if (vme->GetDataVector())
    {
      //////////////////////////////////////////////////////////////////////////
      albaDataPipeInterpolatorVTK::SafeDownCast(vme->GetDataPipe())->GetVTKDataPipe()->RemoveAllInputs();
      //////////////////////////////////////////////////////////////////////////

      if(m_DataVector == NULL)
        m_DataVector=vme->GetDataVector()->NewInstance(); // create a new instance of the same type

      m_DataVector->DeepCopyVmeLarge(vme->GetDataVector()); // copy data
      m_DataVector->SetListener(this);
    }
    return ALBA_OK;
  }
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
int albaVMEGenericAbstract::ShallowCopy(albaVME *a)
//-------------------------------------------------------------------------
{  
  if (Superclass::ShallowCopy(a)==ALBA_OK)
  {
    albaVMEGenericAbstract *vme=(albaVMEGenericAbstract *)a;
    m_MatrixVector->DeepCopy(vme->GetMatrixVector());    
    
    // shallow copy data array
    if (m_DataVector)
    {
      m_DataVector->ShallowCopy(vme->GetDataVector());
    }
    return ALBA_OK;
  }
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
bool albaVMEGenericAbstract::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    albaVMEGenericAbstract *gvme=albaVMEGenericAbstract::SafeDownCast(vme);
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
void albaVMEGenericAbstract::SetMatrix(const albaMatrix &mat)
//-------------------------------------------------------------------------
{
  m_MatrixVector->SetMatrix(mat);
	//Update AbsMatrix output
	GetOutput()->GetAbsMatrix();
  Modified();
}

//-------------------------------------------------------------------------
bool albaVMEGenericAbstract::IsAnimated()
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
bool albaVMEGenericAbstract::IsDataAvailable()
//-------------------------------------------------------------------------
{
  if (m_DataVector)
  {
    albaTimeStamp t = this->GetTimeStamp();
    albaVMEItem *item = m_DataVector->GetItem(t);
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
    return item ? item->IsDataPresent() : true; // If item is NULL -> the data is generated by code
                                                // or is into its child as for the albaVMELandmarkCloud
  }
  else
    return Superclass::IsDataAvailable();
}

//-------------------------------------------------------------------------
void albaVMEGenericAbstract::GetDataTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  if (m_DataVector)
  {
    m_DataVector->GetTimeStamps(kframes);
  }
}

//-------------------------------------------------------------------------
void albaVMEGenericAbstract::GetMatrixTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  m_MatrixVector->GetTimeStamps(kframes);
}

//-------------------------------------------------------------------------
void albaVMEGenericAbstract::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();

  std::vector<albaTimeStamp> datatimestamps;
  std::vector<albaTimeStamp> matrixtimestamps;
  
  if (m_DataVector)
  {
    m_DataVector->GetTimeStamps(datatimestamps);
  }
  m_MatrixVector->GetTimeStamps(matrixtimestamps);

  mmuTimeSet::Merge(datatimestamps,matrixtimestamps,kframes);
}

//-------------------------------------------------------------------------
void albaVMEGenericAbstract::GetLocalTimeBounds(albaTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  tbounds[0]=0.0;
  tbounds[1]=0.0;
  if (m_DataVector)
  {
    m_DataVector->GetTimeBounds(tbounds);
  }

  albaTimeStamp tmp[2];
  m_MatrixVector->GetTimeBounds(tmp);

  tmp[0] = tmp[0] < 0 ? 0 : tmp[0];
  tmp[1] = tmp[1] < 0 ? 0 : tmp[1];

  if (tmp[0]<tbounds[0]||tbounds[0]<0)
    tbounds[0]=tmp[0];

  if (tmp[1]>tbounds[1]||tbounds[1]<0)
    tbounds[1]=tmp[1];
}

//-----------------------------------------------------------------------
int albaVMEGenericAbstract::ReparentTo(albaVME *parent)
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
      albaVMEIterator *iter=this->NewIterator();
      for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
      {
        if (albaVMEGenericAbstract *vme = albaVMEGenericAbstract::SafeDownCast(node))
        {
          albaDataVector *dvector = vme->GetDataVector();
          if(dvector)
          {
            for (int i = 0; i < dvector->GetNumberOfItems(); i++)
            {
              albaVMEItem *item = dvector->GetItemByIndex(i);
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
                albaErrorMacro("found a NULL item in the node!!!!");
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
  return ALBA_ERROR;
}

//-----------------------------------------------------------------------
int albaVMEGenericAbstract::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{  
  Superclass::InternalStore(parent);

  // sub-element for storing the data vector
  if (m_StoreDataVector && m_DataVector)
  {
    m_DataVector->SetCrypting(this->m_Crypting != 0);
    albaStorageElement *data_vector = parent->AppendChild("DataVector");
    if(m_DataVector->Store(data_vector) == ALBA_ERROR)
      return ALBA_ERROR;
  }

  // sub-element for storing the matrix vector
  albaStorageElement *matrix_vector = parent->AppendChild("MatrixVector");
  if(m_MatrixVector->Store(matrix_vector) == ALBA_ERROR)
    return ALBA_ERROR;

  return ALBA_OK;
}

//-----------------------------------------------------------------------
int albaVMEGenericAbstract::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  int ret_val = ALBA_OK;
  Superclass::InternalRestore(node);
  
  // Restore Data Vector
	// Warning old LandmarkClouds have a data vector wich is required and must be loaded.
	// but LMC does not have to store DataVector so the m_StoreDataVector check is used
	// only on InternalStore
  if (m_DataVector)
  {
    albaStorageElement *data_vector=node->FindNestedElement("DataVector");
    if (data_vector)
    {
      ret_val = m_DataVector->Restore(data_vector);
    }
  }
  // restore Matrix Vector  
  if (m_MatrixVector && ret_val == ALBA_OK)
  {
    albaStorageElement *matrix_vector=node->FindNestedElement("MatrixVector");
    if (matrix_vector)
    {
      ret_val = m_MatrixVector->Restore(matrix_vector);
    }
  }
  return ret_val;
}
//-------------------------------------------------------------------------
albaGUI *albaVMEGenericAbstract::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
	m_Gui->Divider();

	return m_Gui;
}

//-------------------------------------------------------------------------
void albaVMEGenericAbstract::OnEvent(albaEventBase *alba_event)
//-------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch (e->GetId())
    {
      case ID_VME_CRYPTING:
        SetCrypting(m_Crypting);
      break;
      //default:
        //Superclass::OnEvent(alba_event);
    }
  }
  else if (alba_event->GetChannel()==MCH_DOWN)
  {
    if (alba_event->GetId() == albaVMEStorage::MSF_FILENAME_CHANGED)
    {
      // force the data vector to save its data to file
      if(GetDataVector())
      {
        GetDataVector()->Modified();
      }
    } 
  }

  Superclass::OnEvent(alba_event);
}

//-----------------------------------------------------------------------
void albaVMEGenericAbstract::Print(std::ostream& os, const int tabs)// const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);

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
char** albaVMEGenericAbstract::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "albaVMEGeneric.xpm"
  return albaVMEGeneric_xpm;
}
