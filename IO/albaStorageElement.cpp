/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaStorageElement
 Authors: Marco Petrone m.petrone@cineca.it
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaObject.h"
#include "albaObjectFactory.h"
#include "albaStorable.h"
#include "albaStorageElement.h"
#include "albaStorage.h"
#include "albaString.h"
#include <vector>
#include <assert.h>

//------------------------------------------------------------------------------
albaStorageElement::albaStorageElement(albaStorageElement *parent,albaStorage *storage)
//------------------------------------------------------------------------------
{
  assert(storage); // no NULL storage is allowed
  m_Storage = storage;
  m_Parent = parent;
  //m_Children = new std::vector<albaStorageElement>;
  m_Children = NULL;
}
//------------------------------------------------------------------------------
albaStorageElement::~albaStorageElement()
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
albaStorageElement *albaStorageElement::FindNestedElement(const char *name)
//------------------------------------------------------------------------------
{
  albaString node_name(name); // no memory copy, thanks albaString :-)

  // force children list creation
  ChildrenVector &children=GetChildren();
  
  // to be rewritten as a map access
  for (unsigned int i=0;i<children.size();i++)
  {
    albaStorageElement *node=children[i];
    if (node_name==node->GetName())
      return node;
  }

  return NULL;
}

//------------------------------------------------------------------------------
bool albaStorageElement::GetNestedElementsByName(const char *name,std::vector<albaStorageElement *> &list)
//------------------------------------------------------------------------------
{
  albaString node_name(name); // no memory copy, thanks albaString :-)

  // force children list creation
  ChildrenVector &children=GetChildren();
  list.clear();
  
  // to be rewritten as a map access
  for (unsigned int i=0;i<children.size();i++)
  {
    albaStorageElement *node=children[i];
    if (node_name==node->GetName())
    {
      list.push_back(node);
    }
  }

  return list.size()>0;

}

//------------------------------------------------------------------------------
int albaStorageElement::StoreObjectVector(const char *name,const std::vector<albaObject *> &vector,const char *items_name)
//------------------------------------------------------------------------------
{
  assert(name);

  // create sub node for storing the vector
  albaStorageElement *vector_node = AppendChild(name);
  vector_node->SetAttribute("NumberOfItems",albaString(vector.size()));
  
  for (unsigned int i=0;i<vector.size();i++)
  {
    albaObject *object=vector[i];
    if (object)
    {
      if (vector_node->StoreObject(items_name,object)==NULL)
      {
        albaErrorMacro("Failed to store object of type \""<<object->GetTypeName()<<"\" in vector of objects");
        return ALBA_ERROR;
      }
    }
    else
    {
      albaWarningMacro("NULL object in a vector being stored");
    }
  }

  return ALBA_OK;  
}

//------------------------------------------------------------------------------
int albaStorageElement::RestoreObjectVector(const char *name,std::vector<albaObject *> &vector,const char *items_name)
//------------------------------------------------------------------------------
{
  assert(name);

  albaStorageElement *subnode=FindNestedElement(name);
  if (subnode)
  {
    return RestoreObjectVector(subnode,vector,items_name);
  }
  
  albaWarningMacro("Error while restoring <"<<GetName()<<"> element: cannot find nested element <"<<name<<">" );
  
  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaStorageElement::RestoreObjectVector(albaStorageElement *subnode,std::vector<albaObject *> &vector,const char *items_name)
//------------------------------------------------------------------------------
{
  assert(subnode);

  ChildrenVector &items = subnode->GetChildren();

  albaID numItems=-1;
  if (!subnode->GetAttributeAsInteger("NumberOfItems",numItems))
  {
    albaWarningMacro("Warning while restoring vector of objects from element <"<<GetName()<<">: cannot find \"NumberOfItems\" attribute..." );
  }

  int num=0;
  bool fail=false;
  for (unsigned int i=0;i<items.size();i++)
  {
    albaStorageElement *item=items[i];
    if (albaString::Equals(items_name,item->GetName()))
    {
      albaObject *object=item->RestoreObject();
      if (object)
      {
        vector.push_back(object);
      }
      else
      {
        fail=true;
        albaString type_name;
        item->GetAttribute("Type",type_name);
        albaWarningMacro("Error while restoring vector of objects from element <"<<GetName()<<">: cannot restore object from element <"<<item->GetName()<<">, object's Type=\""<<type_name.GetCStr()<<"\".");
        // try continue restoring other objects
        GetStorage()->SetErrorCode(albaStorage::IO_WRONG_OBJECT_TYPE);
      }
      num++;
    }    
  }

  if (fail)
    return ALBA_ERROR;

  // check if restored num of items is correct
  if (numItems>=0&&num!=numItems)
  {
    albaWarningMacro("Error while restoring <"<<GetName()<<"> element: wrong number of items in Objects vector");
    return ALBA_ERROR;
  }

  return ALBA_OK;
}

//------------------------------------------------------------------------------
albaStorageElement *albaStorageElement::StoreObject(const char *name,albaStorable *storable, const char *type_name)
//------------------------------------------------------------------------------
{
  assert(storable);
  if (storable->IsStorable())
  {
    albaStorageElement *element=AppendChild(name);
    if (element)
    {
      element->SetAttribute("Type",type_name);

      if (storable->Store(element)==ALBA_OK)
      {
        return element;
      }
    }
    albaErrorMacro("Failed to store object of type \""<<type_name<<"\"");
  }
  
  return NULL;
}
//------------------------------------------------------------------------------
albaStorageElement *albaStorageElement::StoreObject(const char *name,albaObject *object)
//------------------------------------------------------------------------------
{
  assert(name);
  assert(object);
  
  try 
  {
    // items must be both a albaStorable object 
    albaStorable *storable=dynamic_cast<albaStorable *>(object);
    //albaStorable *storable=albaStorable::SafeDownCast(object);
    
    if (storable)
    {
      return StoreObject(name,storable,object->GetTypeName());
    }
    else
    {
      albaWarningMacro("NULL object in a vector being stored");
    }
  } 
  catch (std::bad_cast) 
  { 
    albaErrorMacro("Cannot store the given object because it's not a storable object");

  }
  return NULL;
}
//------------------------------------------------------------------------------
albaString albaStorageElement::UpgradeAttribute(const char *attribute)
//------------------------------------------------------------------------------
{
  albaString att_name;
  albaString new_att_name = "";
  GetAttribute(attribute,att_name);
  if (att_name.Equals("albaVMEItemScalar"))
  {
    new_att_name = "albaVMEItemScalarMatrix";
    SetAttribute(attribute, new_att_name);
    return new_att_name;
  }
  if (att_name.FindFirst("albaVME") != -1)
  {
    if (att_name.Equals("albaVMEScalar"))
    {
      new_att_name = "albaVMEScalarMatrix";
      SetAttribute(attribute, new_att_name);
    }
    else
    {
      new_att_name = att_name;
    }
    albaStorageElement *data_vector = FindNestedElement("DataVector");
    albaString item_type;
    if (data_vector && data_vector->GetAttribute("ItemTypeName", item_type))
    {
      if (item_type.Equals("albaVMEItemScalar"))
      {
        data_vector->SetAttribute("ItemTypeName", "albaVMEItemScalarMatrix");
      }
    }
    return new_att_name;
  }
  
  return att_name;
}
//------------------------------------------------------------------------------
albaObject *albaStorageElement::RestoreObject()
//------------------------------------------------------------------------------
{
  albaString type_name;

  if (GetAttribute("Type",type_name)&&!type_name.IsEmpty())
  {
    if (m_Storage->NeedsUpgrade())
    {
      type_name = UpgradeAttribute("Type");
    }
    
    albaObject *object=albaObjectFactory::CreateInstance(type_name);
    if (object)
    {
      try 
      {
        // items must be both a albaObject and albaStorable 
        albaStorable *restorable=dynamic_cast<albaStorable *>(object);
        
        if (restorable)
        {
          if (restorable->Restore(this)==ALBA_OK)
          {
            // if restored correctly 
            return object;
          }
          else
          {
            albaErrorMacro("Problems restoring object of type "<<object->GetTypeName()<<" from element <"<<GetName()<<">");
          }
        }
        else
        {
          albaErrorMacro("Cannot restore object of type "<<object->GetTypeName()<<" from element <"<<GetName()<<"> since it's a not a restorable object");
        }
      }
      catch (std::bad_cast) 
      {
        albaErrorMacro("Cannot restore object of type "<<object->GetTypeName()<<" from element <"<<GetName()<<"> since it's a not a restorable object");
      }
      // release object memory
      object->Delete();     
    }    
    else
    {
      albaErrorMacro("Cannot restore object of type \""<<type_name.GetCStr()<<"\" from element <"<<GetName()<<"> since this object type is unknown.");
    }
  }
  else
  {

    albaErrorMacro("Cannot restore object from element <"<<GetName()<<"> since no 'Type' attribute is present");
  }

  return NULL;
}

//------------------------------------------------------------------------------
albaObject *albaStorageElement::RestoreObject(const char *name)
//------------------------------------------------------------------------------
{
  albaStorageElement *element=FindNestedElement(name);
  return (element)?element->RestoreObject():NULL;
}


//------------------------------------------------------------------------------
int albaStorageElement::RestoreObject(const char *name,albaStorable * object)
//------------------------------------------------------------------------------
{
  albaStorageElement *element=FindNestedElement(name);

  if (element)
  {
    return object->Restore(element);
  }
  
  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaStorageElement::StoreDouble(const char *name,const double &value)
//------------------------------------------------------------------------------
{
  return StoreText(name,albaString(value));
}
//------------------------------------------------------------------------------
int albaStorageElement::StoreInteger(const char *name,const int &value)
//------------------------------------------------------------------------------
{
  return StoreText(name,albaString(value));
}

//------------------------------------------------------------------------------
int albaStorageElement::RestoreDouble(const char *name,double &value)
//------------------------------------------------------------------------------
{
  albaString tmp;

  if (RestoreText(name,tmp)==ALBA_OK)
  {
    value=atof(tmp);
    return ALBA_OK;
  }
  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaStorageElement::RestoreDouble(double &value)
//------------------------------------------------------------------------------
{
  albaString tmp;
  if (RestoreText(tmp)==ALBA_OK)
  {
    value=atof(tmp);
    return ALBA_OK;
  }

  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaStorageElement::RestoreInteger(int &value)
//------------------------------------------------------------------------------
{
  albaString tmp;
  if (RestoreText(tmp)==ALBA_OK)
  {
    value=atof(tmp);
    return ALBA_OK;
  }

  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaStorageElement::RestoreInteger(const char *name,int &value)
//------------------------------------------------------------------------------
{
  albaString tmp;
  if (RestoreText(name,tmp)==ALBA_OK)
  {
    value=atof(tmp);
    return ALBA_OK;
  }

  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaStorageElement::RestoreText(char *&buffer)
//------------------------------------------------------------------------------
{
  albaString tmp;
  if (!RestoreText(tmp))
  {
    buffer=tmp.Duplicate();
    return ALBA_OK;
  }

  return ALBA_ERROR;
}


//------------------------------------------------------------------------------
int albaStorageElement::RestoreMatrix(const char *name,albaMatrix *matrix)
//------------------------------------------------------------------------------
{
  albaStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreMatrix(matrix);
  }

  albaWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaStorageElement::RestoreVectorN(const char *name,double *comps,unsigned int num)
//------------------------------------------------------------------------------
{
  albaStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num);
  }

  albaWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return ALBA_ERROR;
}
//------------------------------------------------------------------------------
int albaStorageElement::RestoreVectorN(const char *name,int *comps,unsigned int num)
//------------------------------------------------------------------------------
{
  albaStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num);
  }

  albaWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return ALBA_ERROR;
}
//------------------------------------------------------------------------------
int albaStorageElement::RestoreVectorN(const char *name,std::vector<double> &comps,unsigned int num)
//------------------------------------------------------------------------------
{
  albaStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num);
  }

  albaWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return ALBA_ERROR;
}
//------------------------------------------------------------------------------
int albaStorageElement::RestoreVectorN(const char *name,std::vector<int> &comps,unsigned int num)
//------------------------------------------------------------------------------
{
  albaStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num);
  }

  albaWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return ALBA_ERROR;
}
//------------------------------------------------------------------------------
int albaStorageElement::RestoreVectorN(const char *name,std::vector<albaString> &comps,unsigned int num,const char *tag)
//------------------------------------------------------------------------------
{
  albaStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreVectorN(comps,num,tag);
  }

  albaWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaStorageElement::RestoreText(const char *name,char *&buffer)
//------------------------------------------------------------------------------
{
  albaStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreText(buffer);
  }
 
  albaWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );
  
  return ALBA_ERROR;
}
//------------------------------------------------------------------------------
int albaStorageElement::RestoreText(const char *name,albaString &buffer)
//------------------------------------------------------------------------------
{
  albaStorageElement *elem=FindNestedElement(name);
  if (elem)
  {
    return elem->RestoreText(buffer);
  }

  albaWarningMacro("Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );

  return ALBA_ERROR;
}
//------------------------------------------------------------------------------
bool albaStorageElement::GetAttributeAsDouble(const char *name,double &value)
//------------------------------------------------------------------------------
{
  albaString tmp;
  if (GetAttribute(name,tmp))
  {
    value=atof(tmp);
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
bool albaStorageElement::GetAttributeAsInteger(const char *name,albaID &value)
//------------------------------------------------------------------------------
{
  albaString tmp;
  if (GetAttribute(name,tmp))
  {
    value=atof(tmp);
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
void albaStorageElement::SetAttribute(const char *name,const albaID value)
//------------------------------------------------------------------------------
{
  SetAttribute(name,albaString(value));
}
//------------------------------------------------------------------------------
void albaStorageElement::SetAttribute(const char *name,const double value)
//------------------------------------------------------------------------------
{
  SetAttribute(name,albaString(value));
}