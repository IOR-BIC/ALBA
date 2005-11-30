/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeIsosurface.h,v $
Language:  C++
Date:      $Date: 2005-11-30 14:49:56 $
Version:   $Revision: 1.2 $
Authors:   Alexander Savenko  -  Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipeIsosurface_H_
#define _mafPipeIsosurface_H_

#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkContourVolumeMapper;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkOutlineCornerFilter;
class mafVME;
class mmgFloatSlider;
class mafEventBase;

//----------------------------------------------------------------------------
// mafPipeIsosurface :
//----------------------------------------------------------------------------
class mafPipeIsosurface : public mafPipe 
{
public:
  mafTypeMacro(mafPipeIsosurface, mafPipe);

            mafPipeIsosurface();
  virtual  ~mafPipeIsosurface();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n);
  virtual void Select(bool select);

  /** Set the contour value for contour mapper, return false if contour mapper is NULL, otherwise return true. */
	bool   SetContourValue(float value);
  
	/** return the contour value. */
	float  GetContourValue();

  /** IDs for the GUI */
  enum PIPE_ISOSURFACE_WIDGET_ID
  {
    ID_CONTOUR_VALUE = Superclass::ID_LAST,
    ID_ISOSURFACE_OPACITY,
    ID_LAST
  };

protected:
  virtual mmgGui  *CreateGui();

  vtkPolyData              *m_Isosurface;
  vtkContourVolumeMapper   *m_ContourMapper; 
  vtkPolyDataMapper        *m_IsosurfaceMapper;
  vtkActor                 *m_IsosurfaceActor;

  vtkOutlineCornerFilter   *m_OutlineBox;
  vtkPolyDataMapper        *m_OutlineMapper;
  vtkActor                 *m_OutlineActor;

  mmgFloatSlider  *m_ContourSlider;
  double m_ContourValue;
  double m_Opacity;
};  
#endif // _mafPipeIsosurface_H_
