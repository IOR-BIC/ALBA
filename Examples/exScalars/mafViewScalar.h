/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewScalar.h,v $
  Language:  C++
  Date:      $Date: 2006-06-14 13:34:46 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafViewScalar_H__
#define __mafViewScalar_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewVTK.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class vtkDoubleArray;
class vtkPolyData;
class vtkLookupTable;
class vtkPolyDataMapper;

//----------------------------------------------------------------------------
// mafViewScalar :
//----------------------------------------------------------------------------
/** 
mafViewScalar is a View that got a RenderWindow and a SceneGraph
\sa mafSceneNode mafRWIBase mafRWI
*/
class mafViewScalar: public mafViewVTK
{
public:
  mafViewScalar(wxString label = "Scalar", int camera_position = CAMERA_PERSPECTIVE, bool show_axes = true, bool show_grid = false, bool show_ruler = false, int stereo = 0);
  virtual ~mafViewScalar(); 

  mafTypeMacro(mafViewScalar, mafViewVTK);

  virtual mafView*  Copy(mafObserver *Listener);
  virtual void			OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
  enum VIEW_SCALAR_WIDGET_ID
  {
    ID_ASCII_FILE = Superclass::ID_LAST,
    ID_DATA_ORDER,
    ID_READ_DATA,
    ID_SCALAR_LUT,
    ID_COMPONENT_ACTIVE,
    ID_LAST
  };

  enum SCALAR_DATA_ORDER
  {
    DATA_ORDER_ROW = 0,
    DATA_ORDER_COLUMN,
    DATA_ORDER_MATRIX
  };

protected:
  virtual mmgGui *CreateGui();

  /** Import ASCII data. */
  int ImportASCII();

  /** Visualize ASCII data. */
  void ViewData();

  void UpdateData();

  std::vector<std::string> m_Files;
  std::vector<vtkDoubleArray *> m_ScalarsArray;
  int m_DataOrder;
  int m_ComponentActivated;
  vtkPolyData *m_Line;
  vtkPolyDataMapper *m_Mapper;
  vtkLookupTable *m_Lut;
};
#endif
