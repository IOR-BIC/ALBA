/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePointSet.h,v $
  Language:  C++
  Date:      $Date: 2006-03-02 22:00:51 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden - Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipePointSet_H__
#define __mafPipePointSet_H__

#include "mafPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;

//----------------------------------------------------------------------------
// mafPipePointSet :
//----------------------------------------------------------------------------
class mafPipePointSet : public mafPipe
{
public:
  mafTypeMacro(mafPipePointSet,mafPipe);

               mafPipePointSet();
  virtual     ~mafPipePointSet ();

  virtual void Create(mafSceneNode *n);

	/** Change the visibility of the bounding box actor representing the selection for the vme. */
  virtual void Select     (bool select); 

  vtkPolyDataMapper			 *m_PointSetMapper;
  vtkActor               *m_PointSetActor;

  vtkOutlineCornerFilter *m_OutlineFilter;
  vtkPolyDataMapper      *m_OutlineMapper;
  vtkProperty            *m_OutlineProperty;
  vtkActor               *m_OutlineActor;

protected:
	/** Update the properties according to the vme's tags. */
	void										UpdateProperty(bool fromTag = false);
};
#endif // __mafPipePointSet_H__
