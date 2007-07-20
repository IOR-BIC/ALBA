/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGeometryEditorPolylineGraph.h,v $
Language:  C++
Date:      $Date: 2007-07-20 14:14:33 $
Version:   $Revision: 1.5 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2007
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

#ifndef __medGeometryEditorPolylineGraph_H__
#define __medGeometryEditorPolylineGraph_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "vtkSystemIncludes.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class medVMEPolylineEditor;
class medVMEPolylineGraph;
class mafPolylineGraph;
class mmgGui;
class mmiPicker;
class mafInteractor;
class vtkSphereSource;
class vtkGlyph3D;
class vtkTubeFilter;
class vtkAppendPolyData;
class vtkPolyData;

#define UNDEFINED_POINT_ID -1
#define UNDEFINED_BRANCH_ID -1

class medGeometryEditorPolylineGraph: public mafObserver 
{
public:
	medGeometryEditorPolylineGraph(mafVME *input, mafObserver *listener = NULL, medVMEPolylineGraph *polyline=NULL);
	virtual ~medGeometryEditorPolylineGraph(); 

	/** Set the event receiver object*/
	void  SetListener(mafObserver *Listener) {m_Listener = Listener;};

	/** Events handling*/        
	virtual void OnEvent(mafEventBase *maf_event);

	/** Show On/Off VME Editor */
	void Show(bool show);

	/** Return the current GUI */
	mmgGui* GetGui();

	/** Add a new vertex at the selected branch */
	int AddNewVertex(double vertex[3],vtkIdType branch=UNDEFINED_BRANCH_ID);

	/** Return the result of the editing */
	vtkPolyData* GetOutput();

	/** Delete a branch and his child branch */
	void DeleteBranch(vtkIdType branchID);

	/** Move a point in the new position. If pointID isn't defined it use m_SelectedPoint */
	void MovePoint(double newPosition[3],int pointID=UNDEFINED_POINT_ID);

	/** Select the nearest point to position */
	void SelectPoint(double position[3]);

	/** Select Point with the ID */
	void SelectPoint(int pointID);
	
	/** Delete the nearest point to position */
	void DeletePoint(double position[3]);

	/** Delete pointID-th point */
	void DeletePoint(int pointID);

	/** Select the nearest branch to position */
	void SelectBranch(double position[3]);

	/** Attach a new branch to the selected point and add a point (defined by position) to the new branch */
	void AddBranch(double position[3]);

	/** Insert a point defined by position between two points selected before */
	void InsertPoint(double position[3]);

protected:

	void CreateGui();

	/** Create isa stuff */
	void CreateISA();

	void CreatePipe();

	int UpdateVMEEditorData(vtkPolyData *polydata);

	/** Menage if a VME is picked in the view */
	void VmePicked(mafEvent *e);

	/** Update VME Editor behavior and VME Input behavior */
	void BehaviorUpdate();

	/**
	Register the event receiver object*/
	mafObserver *m_Listener;

	mmiPicker					*m_Picker;
	mafInteractor			*m_OldBehavior;

	medVMEPolylineEditor			*m_VMEPolylineEditor;
	medVMEPolylineEditor			*m_VMEPolylineSelection;///<VME that show the selection
	mafPolylineGraph					*m_PolylineGraph;

	mmgGui	*m_Gui;

	mafVME	*m_InputVME;

	int m_CurrentBranch;///<Branch in witch we are working
	int m_SelectedPoint;
	int m_SelectedPointVTK;
	int m_SelectedBranch;

	int m_SelectedPoint1ForInserting;
	int m_SelectedPoint2ForInserting;

	int m_SelectedPoint1ForInsertingVTK;
	int m_SelectedPoint2ForInsertingVTK;
	//Gui Variables
	int m_PointTool;
	int m_BranchTool;
	int m_Action;

	//Pipe Variables
	vtkSphereSource		*m_Sphere;
	vtkGlyph3D				*m_Glyph;
	vtkTubeFilter			*m_Tube;
	vtkAppendPolyData	*m_AppendPolydata;
};
#endif
