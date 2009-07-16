/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medPipeSurfaceEditor.h,v $
Language:  C++
Date:      $Date: 2009-07-16 08:43:31 $
Version:   $Revision: 1.3.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#ifndef __medPipeSurfaceEditor_H__
#define __medPipeSurfaceEditor_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkSphereSource;
class vtkGlyph3D;
class vtkTubeFilter;
class vtkPolyDataMapper;
class vtkAppendPolyData;
class vtkActor;
class vtkLookupTable;
class vtkMAFFixedCutter;
class vtkPlane;

/**
  Class Name: medPipeSurfaceEditor.
  Default pipe of the medVMESurfaceEditor, that is a vme that permit the editing of topology.
*/
class medPipeSurfaceEditor : public mafPipe
{
public:
  /** RTTI Macro */
	mafTypeMacro(medPipeSurfaceEditor,mafPipe);

  /** constructor. */
	medPipeSurfaceEditor();
  /** destructor. */
	virtual ~medPipeSurfaceEditor();

	/** process events coming from gui */
	virtual void OnEvent(mafEventBase *maf_event);

  /** function that create the pipeline instancing vtk graphic pipe. */
	virtual void Create(mafSceneNode *n);
  /** Show can be used for hide or show elements (actually empty) */
	void Show(bool show); 
  /** During selection of vme, it can visualize graphic element (actually empty)*/
	virtual void Select(bool select);


protected:
  /** Creation of the gui that will be attached to visual prop panel.*/
	virtual mafGUI  *CreateGui();

	vtkPolyDataMapper	*m_Mapper;
	vtkActor					*m_Actor;

	vtkLookupTable		*m_LUT;
};
#endif // __medPipeSurfaceEditor_H__
