/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePointSet.h,v $
  Language:  C++
  Date:      $Date: 2005-08-31 09:14:26 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden - Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipePointSet_H_
#define _mafPipePointSet_H_

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
#endif // _mafPipePointSet_H_
