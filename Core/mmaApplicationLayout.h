/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmaApplicationLayout
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mmaApplicationLayout_H__
#define __mmaApplicationLayout_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaAttribute.h"


//----------------------------------------------------------------------------
// mmaApplicationLayout:
//----------------------------------------------------------------------------
/**  */  
class ALBA_EXPORT mmaApplicationLayout : public albaAttribute
{
public:
	         mmaApplicationLayout();
  virtual ~mmaApplicationLayout();

  albaTypeMacro(mmaApplicationLayout, albaAttribute);

 
  /** Copy the contents of another Meter attribute into this one. */
  virtual void DeepCopy(const albaAttribute *a);

  /** Compare with another Meter attribute. */
  virtual bool Equals(const albaAttribute *a);

   /** Set a informations regarding the application dimension, position and maximize state. */
  void SetApplicationInfo(int maximized, int pos[2], int size[2]);
  
  /** Retrieve the stored frame settings.*/
  void GetApplicationInfo(int &maximized, int pos[2], int size[2]);

  /** Set the visibility for the corresponding interface element.*/
  void SetInterfaceElementVisibility(albaString panel_name, int visibility);

  /** Return the stored visibility for the Toolbar.*/
  int GetToolBarVisibility() {return m_ToolBarVisibility;};

  /** Return the stored visibility for the Logbar.*/
  int GetLogBarVisibility() {return m_LogBarVisibility;};

  /** Return the stored visibility for the Sidebar.*/
  int GetSideBarVisibility() {return m_SideBarVisibility;};

protected:
  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  int m_AppMaximized;
  int m_AppPosition[2];
  int m_AppSize[2];

  int m_SideBarVisibility;
  int m_ToolBarVisibility;
  int m_LogBarVisibility;
};
#endif
