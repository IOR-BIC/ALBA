/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpExtrusionHoles.h,v $
Language:  C++
Date:      $Date: 2008-07-25 10:32:50 $
Version:   $Revision: 1.3 $
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
#ifndef __medOpExtrusionHoles_H__
#define __medOpExtrusionHoles_H__

#include "mafOp.h"
#include "vtkAppendPolyData.h"
#include "vtkFeatureEdges.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafGUIDialog;
class mafGUIButton;
class mafRWI;
class mmiSelectPoint;
class mafVMESurface;
class vtkHoleConnectivity;
class vtkPolyDataMapper;
class vtkActor;
class vtkSphereSource;
class vtkGlyph3D;
class vtkAppendPolyData;
class vtkPlaneSource;
class vtkMEDExtrudeToCircle;

//----------------------------------------------------------------------------
// medOpExtrusionHoles :
//----------------------------------------------------------------------------
/** 
Perform extrusion of holes in a surface.
CTRL modifier must be used in order to select a hole in render window while
performing mouse dragging.
*/
class medOpExtrusionHoles: public mafOp
{
public:
	medOpExtrusionHoles(const wxString &label = "Extrude Holes");
	~medOpExtrusionHoles(); 

	virtual void OnEvent(mafEventBase *maf_event);

	mafTypeMacro(medOpExtrusionHoles, mafOp);

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(mafNode *node);

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Makes the undo for the operation. */
	/*virtual*/ void OpUndo();

	/** Highlight the hole selected with red color */
	void SelectHole(int pointID);

	/** Perform extrusion and visualize the result */
	void Extrude();

	/** Save the result polydata of extrusion in VME data */
	void SaveExtrusion();

	void ExtractFreeEdge();

	void SetExtrusionFactor(double value){m_ExtrusionFactor = value;};
	double GetExtrusionFactor(){return m_ExtrusionFactor;};

	int GetExtractFreeEdgesNumeberOfPoints(){if(m_ExtractFreeEdges)return m_ExtractFreeEdges->GetOutput()->GetNumberOfPoints(); else return 0;};

	vtkPolyData* GetExtrutedSurface(){return m_ResultPolydata;};

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);

	void CreateOpDialog();
	void DeleteOpDialog();
	void CreatePolydataPipeline();

	mafGUIDialog							*m_Dialog;
	mafGUIButton							*m_ButtonOk;
	mafRWI								*m_Rwi;
	mmiSelectPoint				*m_Picker;

	vtkPolyData						*m_ResultPolydata;
	vtkPolyData						*m_OriginalPolydata;
	mafVMESurface					*m_ResultSurface;
	vtkAppendPolyData			*m_ResultAfterExtrusion;

	double								m_SphereRadius;
	double								m_ExtrusionFactor;

	vtkPolyDataMapper			*m_SurfaceMapper;
	vtkActor							*m_SurfaceActor;
	vtkPolyDataMapper			*m_HolesMapper;
	vtkActor							*m_HolesActor;
	vtkPolyDataMapper			*m_SelectedHoleMapper;
	vtkActor							*m_SelectedHoleActor;

	vtkFeatureEdges						*m_ExtractFreeEdges;
	vtkHoleConnectivity				*m_ExtractHole;
	vtkMEDExtrudeToCircle	    *m_ExtrusionFilter;

	vtkSphereSource				*m_Sphere;
	vtkGlyph3D						*m_Glyph;

	vtkPlaneSource *m_Plane;

	double m_MaxBounds;

  int m_ReverseExtrusion;
  int m_NumVertices;
};
#endif
