/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorageElement.cpp,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:39:29 $
  Version:   $Revision: 1.9 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafObject.h"
#include "mafObjectFactory.h"
#include "mafStorable.h"
#include "mafStorageElement.h"
#include "mafString.h"
#include <vector>
#include <assert.h>
#include <sstream>

//------------------------------------------------------------------------------
mafStorageElement::mafStorageElement(mafStorageElement *parent,mafStorage *storage)
//------------------------------------------------------------------------------
{
  assert(storage); // no NULL storage is allowed
  m_Storage = storage;
  m_Parent = parent;
  //m_Children = new std::vector<mafStorageElement>;
  m_Children = NULL;
}
//------------------------------------------------------------------------------
mafStorageElement::~mafStorageElement()
//------------------------------------------------------------------------------
{
  // remove pointers...
  m_Storage = NULL;
  m_Parent = NULL;

  if (m_Children)
  {
    // remove all child nodes
    for (unsigned int i=0;i<m_Children->size();i++)
    {
      delete (*m_Children)[i];
    }
  
    cppDEL(m_Children);
  }  
}

//------------------------------------------------------------------------------
mafStorageElement *mafStorageElement::FindNestedElement(const char *name)
//------------------------------------------------------------------------------
{
  mafString node_name(name); // no memory copy, thanks mafString :-)

  // force children list creation
  ChildrenVector &children=GetChildren();
  
  // to be rewritten as a map access
  for (unsigned int i=0;i<children.size();i++)
  {
    mafStorageElement *node=children[i];
    if (node_name==node->GetName())
      return node;
  }

  return NULL;
}

//------------------------------------------------------------------------------
bool mafStorageElement::GetNestedElementsByName(const char *name,std::vector<mafStorageElement *> &list)
//------------------------------------------------------------------------------
{
  mafString node_name(name); // no memory copy, thanks mafString :-)

  // force children list creation
  ChildrenVector &children=GetChildren();
  list.clear();
  
  // to be rewritten as a map access
  for (unsigned int i=0;i<children.size();i++)
  {
    mafStorageElement *node=children[i];
    if (node_name==node->GetName())
    {
      list.push_back(node);
    }
  }

  return list.size()>0;

}

//------------------------------------------------------------------------------
int mafStorageElement::StoreObjectVector(const std::vector<mafObject *> &vector,const char *name,const char *items_name)
//------------------------------------------------------------------------------
{
  assert(name);

  // create sub node for storing the vector
  mafStorageElement *vector_node = AppendChild(name);
  vector_node->SetAttribute("NumberOfItems",mafString(vector.size()));
  
  for (unsigned int i=0;i<vector.size();i++)
  {
    mafObject *object=vector[i];
    if (object)
    {
      if (vector_node->StoreObject(object,items_name)==NULL)
      {
        mafErrorMacro("Failed to store object of type \""<<object->GetTypeName()<<"\" in vector of objects");
        return MAF_ERROR;
      }
    }
    else
    {
      mafWarningMacro("NULL object in a vector being stored");
    }
  }

  return MAF_OK;  
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreObjectVector(std::vector<mafObject *> &vector,const char *name,const char *items_name)
//------------------------------------------------------------------------------
{
  assert(name);

  mafStorageElement *subnode=FindNestedElement(name);
  if (subnode)
  {
    ChildrenVector &items = subnode->GetChildren();
    mafString numItems_str;
    int numItems=-1;
    if (subnode->GetAttribute("NumberOfItems",numItems_str))
    {
      numItems=atof(numItems_str);  
    }
    else
    {
      mafWarningMacro("Warning while restoring <"<<GetName()<<"> element: cannot find \"NumberOfItems\" attribute..." );
    }

    int num=0;
    for (unsigned int i=0;i<items.size();i++)
    {
      mafStorageElement *item=items[i];
      if (mafString::Equals(items_name,item->GetName()))
      {
        mafObject *object=RestoreObject(item);
        if (object)
        {
          vector.push_back(object);
        }
        else
        {
          mafWarningMacro("Error while restoring <"<<GetName()<<"> element: cannot restore object");
          return MAF_ERROR;
        }
        num++;
      }    
    }

    // check if restored num of items is correct
    if (numItems>=0&&num!=numItems)
    {
      mafWarningMacro("Error while restoring <"<<GetName()<<"> element: wrong number of items in Objects vector");
      return MAF_ERROR;
    }

    return MAF_OK;
  }
  else
  {
    mafWarningMacro("Error while restoring <"<<GetName()<<"> element: cannot find nested element <"<<name<<">" );
  }
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
mafStorageElement *mafStorageElement::StoreObject(mafObject *object,const char *name)
//------------------------------------------------------------------------------
{
  assert(name);
  assert(object);
  
  try 
  {
    // items must be both a mafStorable object 
    mafStorable *storable=dynamic_cast<mafStorable *>(object);
    //mafStorable *storable=mafStorable::SafeDownCast(object);
    
    if (storable)
    {
      mafStorageElement *element=AppendChild(name);
      if (element)
      {
        element->SetAttribute("Type",object->GetTypeName());
      }

      if (storable->Store(element)==MAF_OK)
      {
        return element;
      }
      
      mafErrorMacro("Failed to store object of type \""<<object->GetTypeName()<<"\"");
    }
    else
    {
      mafWarningMacro("NULL object in a vector being stored");
    }
  } 
  catch (std::bad_cast) 
  { 
    mafErrorMacro("Cannot store object in vector because it's not a storable object");

  }
  return NULL;
}

//------------------------------------------------------------------------------
mafObject *mafStorageElement::RestoreObject(mafStorageElement *element)
//------------------------------------------------------------------------------
{
  mafString type_name;

  if (element->GetAttribute("Type",type_name)&&!type_name.IsEmpty())
  {
    mafObject *object=mafObjectFactory::CreateInstance(type_name);
    if (object)
    {
      try 
      {
        // items must be both a mafObject and mafStorable 
        mafStorable *restorable=dynamic_cast<mafStorable *>(object);
        
        if (restorable)
        {
          if (restorable->Restore(element)==MAF_OK)
          {
            // if restored correctly 
            return object;
          }
          else
          {
            mafErrorMacro("Problems restoring object of type "<<object->GetTypeName()<<" from element <"<<element->GetName()<<">");
          }
        }
        else
        {
          mafErrorMacro("Cannot restore object of type "<<object->GetTypeName()<<" from element <"<<element->GetName()<<"> since it's a not a restorable object");
        }
      }
      catch (std::bad_cast) 
      {
        mafErrorMacro("Cannot restore object of type "<<object->GetTypeName()<<" from element <"<<element->GetName()<<"> since it's a not a restorable object");
      }
      // release object memory
      object->Delete();     
    }    
  }
  else
  {

    mafErrorMacro("Cannot restore object from element <"<<element->GetName()<<"> since no 'Type' attribute is present");
  }

  return NULL;
}

//------------------------------------------------------------------------------
mafObject *mafStorageElement::RestoreObject(const char *name)
//------------------------------------------------------------------------------
{
  mafStorageElement *element=FindNestedElement(name);
  return (element)?RestoreObject(element):NULL;
}


//------------------------------------------------------------------------------
int mafStorageElement::RestoreObject(mafObject * &object,const char *name)
//------------------------------------------------------------------------------
{
  object=NULL;

  mafStorageElement *element=FindNestedElement(name);

  if (element)
  {
    object = RestoreObject(element);
  }
  
  return (object)?MAF_OK:MAF_ERROR;
}

//------------------------------------------------------------------------------
void mafStorageElement::StoreDouble(const double &value,const char *name)
//------------------------------------------------------------------------------
{
  StoreText(mafString(value),name);
}
//------------------------------------------------------------------------------
void mafStorageElement::StoreInteger(const int &value,const char *name)
//------------------------------------------------------------------------------
{
  StoreText(mafString(value),name);
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreDouble(double &value,const char *name)
//------------------------------------------------------------------------------
{
  mafString tmp;
  if (RestoreText(tmp,name)==MAF_OK)
  {
    value=atof(tmp);
    return MAF_OK;
  }
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafStorageElement::RestoreInteger(int &value,const char *name)
//------------------------------------------------------------------------------
{
  mafString tmp;
  if (RestoreText(tmp,name)==MAF_OK)
  {
    value=atof(tmp);
    return MAF_OK;
  }
  return MAF_ERROR;
}
