/*=========================================================================

 Program: MAF2
 Module: mafPipePointSet
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class MAF_EXPORT mafPipePointSet : public mafPipe
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
