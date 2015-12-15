/*=========================================================================

 Program: MAF2
 Module: mmaApplicationLayout
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mmaApplicationLayout_H__
#define __mmaApplicationLayout_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafAttribute.h"


#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,ViewLayoutInfo);
#endif
//----------------------------------------------------------------------------
// mmaApplicationLayout:
//----------------------------------------------------------------------------
/**  */  
class MAF_EXPORT mmaApplicationLayout : public mafAttribute
{
public:
	         mmaApplicationLayout();
  virtual ~mmaApplicationLayout();

  mafTypeMacro(mmaApplicationLayout, mafAttribute);

 
  /** Copy the contents of another Meter attribute into this one. */
  virtual void DeepCopy(const mafAttribute *a);

  /** Compare with another Meter attribute. */
  virtual bool Equals(const mafAttribute *a);

   /** Set a informations regarding the application dimension, position and maximize state. */
  void SetApplicationInfo(int maximized, int pos[2], int size[2]);
  
  /** Retrieve the stored frame settings.*/
  void GetApplicationInfo(int &maximized, int pos[2], int size[2]);

  /** Set the visibility for the corresponding interface element.*/
  void SetInterfaceElementVisibility(mafString panel_name, int visibility);

  /** Return the stored visibility for the Toolbar.*/
  int GetToolBarVisibility() {return m_ToolBarVisibility;};

  /** Return the stored visibility for the Logbar.*/
  int GetLogBarVisibility() {return m_LogBarVisibility;};

  /** Return the stored visibility for the Sidebar.*/
  int GetSideBarVisibility() {return m_SideBarVisibility;};

protected:
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  int m_AppMaximized;
  int m_AppPosition[2];
  int m_AppSize[2];

  int m_SideBarVisibility;
  int m_ToolBarVisibility;
  int m_LogBarVisibility;
};
#endif
