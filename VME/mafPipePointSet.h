/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePointSet.h,v $
  Language:  C++
  Date:      $Date: 2005-07-22 13:45:49 $
  Version:   $Revision: 1.1 $
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
class vtkSphereSource;
class vtkPolyDataNormals;
class vtkExtendedGlyph3D;
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

  vtkSphereSource			   *m_act_s;
	vtkPolyDataNormals     *m_normals;
  vtkExtendedGlyph3D     *m_act_g;
  vtkPolyDataMapper			 *m_act_m;
  vtkActor               *m_act_a;

  vtkOutlineCornerFilter *m_sel_ocf;
  vtkPolyDataMapper      *m_sel_m;
  vtkProperty            *m_sel_p;
  vtkActor               *m_sel_a;

protected:
  double m_Radius;

	/** Update the properties according to the vme's tags. */
	void										UpdateProperty(bool fromTag = false);
};

#endif // _mafPipePointSet_H_
