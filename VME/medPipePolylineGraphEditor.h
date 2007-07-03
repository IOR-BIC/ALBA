/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medPipePolylineGraphEditor.h,v $
Language:  C++
Date:      $Date: 2007-07-03 09:59:44 $
Version:   $Revision: 1.1 $
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

#ifndef __medPipePolylineGraphEditor_H__
#define __medPipePolylineGraphEditor_H__

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

//----------------------------------------------------------------------------
// medPipePolylineGraphEditor :
//----------------------------------------------------------------------------
class medPipePolylineGraphEditor : public mafPipe
{
public:
	mafTypeMacro(medPipePolylineGraphEditor,mafPipe);

	medPipePolylineGraphEditor();
	virtual ~medPipePolylineGraphEditor();

	/** process events coming from gui */
	virtual void OnEvent(mafEventBase *maf_event);

	/** IDs for the GUI */
	enum PIPE_VOLUME_MIP_WIDGET_ID
	{
		ID_LUT_CHOOSER = Superclass::ID_LAST,
		ID_RESAMPLE_FACTOR,
		ID_LAST
	};

	virtual void Create(mafSceneNode *n);
	void Show(bool show); 
	virtual void Select(bool select);  

protected:
	virtual mmgGui  *CreateGui();

	vtkSphereSource		*m_Sphere;
	vtkGlyph3D				*m_Glyph;
	vtkTubeFilter			*m_Tube;
	vtkPolyDataMapper	*m_MapperSphere;
	vtkPolyDataMapper	*m_MapperTube;
	vtkAppendPolyData	*m_AppendPolydata;
	vtkActor					*m_ActorSphere;
	vtkActor					*m_ActorTube;

	double						m_SphereRadius;
	double						m_TubeRadius;

	vtkLookupTable		*m_LUT;
};
#endif // __medPipePolylineGraphEditor_H__
