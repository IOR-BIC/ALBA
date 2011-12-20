/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewSliceBlend.h,v $
Language:  C++
Date:      $Date: 2011-12-20 14:57:34 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 

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

#ifndef __medViewSliceBlend_H__
#define __medViewSliceBlend_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewVTK.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneNode;
class mafGUI;
class mafGUIFloatSlider;
class mafGUILutSlider;
class vtkActor2D;

//----------------------------------------------------------------------------
// medViewSliceBlend :
//----------------------------------------------------------------------------
/** 
medViewSliceBlend is a View that visualize two volume slice.
*/
class medViewSliceBlend: public mafViewVTK
{
public:
	medViewSliceBlend(wxString label = "Slice Blend", int camera_position = CAMERA_OS_Z	, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0);
	virtual ~medViewSliceBlend(); 

	mafTypeMacro(medViewSliceBlend, mafViewVTK);

	/*virtual*/ mafView*  Copy(mafObserver *Listener, bool lightCopyEnabled = false);
	virtual void			OnEvent(mafEventBase *maf_event);

	void Create();

	/** 
	Set the visualization status for the node (visible, not visible, mutex, ...) \sa mafSceneGraph mafView*/
	virtual int GetNodeStatus(mafNode *vme);

	/** IDs for the view GUI */
	enum VIEW_SLICE_WIDGET_ID
	{
		ID_OPACITY = Superclass::ID_LAST,
    ID_SLIDER,
		ID_LAST
	};

	/** 
	Create the visual pipe for the node passed as argument. 
	To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
	otherwise ask to vme which is its visual pipe. */
	virtual void VmeCreatePipe(mafNode *vme);

	/** 
	Delete vme's visual pipe. It is called when vme is removed from visualization.*/
	virtual void VmeDeletePipe(mafNode *vme);

	/** Change the range of the WindowLevel Lookup table.*/
	void SetLutRange(double low_val, double high_val);

	/** Set the slice coordinates.*/
	virtual void SetSliceLocalOrigin(double origin0[3],double origin1[3]);

	/** Return true if a there is a volume inside the view.*/
	bool VolumeIsVisible() {return m_CurrentVolume != NULL;};

	/** Create the background for the slice. */
	void BorderCreate(double col[3]);

	/** Delete the background of the slice. */
	void BorderDelete();

	/** Give an initial origin for the slice.*/
	void InitializeSlice(double slice1[3],double slice2[3]);

	virtual void VmeShow(mafNode *node, bool show);

	void VmeRemove(mafNode *vme);

	/** print a dump of this object */
	virtual void Print(std::ostream& os, const int tabs=0);// const;

	void SetNormal(double normal[3]);

	/** Function for the correction of slice pose in case of current volume pose matrix is different from identity */
	void MultiplyPointByInputVolumeABSMatrix(double *point);

	void SetSlice(int nSlice,double pos[3]);

protected:

	void UpdateSurfacesList(mafNode *node);

	virtual mafGUI *CreateGui();

	mafSceneNode    *m_CurrentVolume; ///< Current visualized volume
	vtkActor2D      *m_Border;
	double           m_Slice1[3];
	double           m_Slice2[3];
  double           m_Slice1Position;
  double           m_Slice2Position;
	bool             m_SliceInitialized; 

	double					m_Opacity;

	std::vector<mafSceneNode*> m_CurrentSurface;
};
#endif
