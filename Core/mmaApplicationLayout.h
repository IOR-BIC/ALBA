/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmaApplicationLayout.h,v $
  Language:  C++
  Date:      $Date: 2006-12-06 14:12:48 $
  Version:   $Revision: 1.5 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmaApplicationLayout_H__
#define __mmaApplicationLayout_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafAttribute.h"

#include <vector>

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mafView;

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
  
  struct ViewLayoutInfo 
  {
    mafString m_Label;
    int m_Id;
    int m_Mult;
    int m_Maximized;
    int m_Position[2];
    int m_Size[2];
    std::vector<int> m_VisibleVmes;
    double m_CameraParameters[9];
  };

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  /** Copy the contents of another Meter attribute into this one. */
  virtual void DeepCopy(const mafAttribute *a);

  /** Compare with another Meter attribute. */
  virtual bool Equals(const mafAttribute *a);

  /** Add the view to the view list to be saved into the layout.*/
  void AddView(mafView *v, bool vme_visibility = false);

  /** Remove the view from the view list to be saved into the layout.*/
  void RemoveView(mafView *v);

  /** Remove the view (given the id of the list vector) from the view list to be saved into the layout.*/
  void RemoveView(int view_id);

  /** Return the number of views added to the layout list.*/
  int GetNumberOfViewsInLayout() {return m_LayoutViewList.size();};

  /** Set a label to associate to the saved layout.*/
  void SetLayoutName(const char *name);

  const char *GetLayoutName() {return m_LayoutName.GetCStr();};

  /** Clear current layout.*/
  void ClearLayout();

  /** Set a informations regarding the application dimension, position and maximize state. */
  void SetApplicationInfo(int maximized, int pos[2], int size[2]);
  
  /** Retrieve the stored frame settings.*/
  void GetApplicationInfo(int &maximized, int pos[2], int size[2]);

  /** Set the visibility for the corresponding interface element.*/
  void SetInterfaceElementVisibility(mafString panel_name, int visibility);

  std::vector<ViewLayoutInfo>::iterator GetLayoutList() {return m_LayoutViewList.begin();};

  /** Return the stored visibility for the Toolbar.*/
  int GetToolBarVisibility() {return m_ToolBarVisibility;};

  /** Return the stored visibility for the Logbar.*/
  int GetLogBarVisibility() {return m_LogBarVisibility;};

  /** Return the stored visibility for the Sidebar.*/
  int GetSideBarVisibility() {return m_SideBarVisibility;};


  /** Set the value for vme visibility.*/
  void SetVisibilityVme(int value){m_VisibilityVme = value;};

  /** Return the value for vme visibility.*/
  int GetVisibilityVme(){return m_VisibilityVme;};

protected:
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  mafString m_LayoutName;
  int m_AppMaximized;
  int m_AppPosition[2];
  int m_AppSize[2];

  int m_SideBarVisibility;
  int m_ToolBarVisibility;
  int m_LogBarVisibility;
  int m_VisibilityVme;
  
  std::vector<ViewLayoutInfo> m_LayoutViewList;
};
#endif
