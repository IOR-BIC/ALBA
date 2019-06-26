/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePointSet
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipePointSet_H__
#define __albaPipePointSet_H__

#include "albaPipe.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;

//----------------------------------------------------------------------------
// albaPipePointSet :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPipePointSet : public albaPipe
{
public:
  albaTypeMacro(albaPipePointSet,albaPipe);

               albaPipePointSet();
  virtual     ~albaPipePointSet ();

  virtual void Create(albaSceneNode *n);

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
#endif // __albaPipePointSet_H__
