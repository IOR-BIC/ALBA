/*=========================================================================
Program:   Alba
Module:    mafViewSliceBlend.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __mafViewSliceBlend_H__
#define __mafViewSliceBlend_H__

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
class mafGUILutSlider;
class vtkActor2D;

//----------------------------------------------------------------------------
// mafViewSliceBlend :
//----------------------------------------------------------------------------
/** 
mafViewSliceBlend is a View that visualize two volume slice.
*/
class mafViewSliceBlend: public mafViewVTK
{
public:
	mafViewSliceBlend(wxString label = "Slice Blend", int camera_position = CAMERA_OS_Z	, bool show_axes = false, bool show_grid = false, int stereo = 0);
	virtual ~mafViewSliceBlend(); 

	mafTypeMacro(mafViewSliceBlend, mafViewVTK);

	/*virtual*/ mafView* Copy(mafObserver *Listener, bool lightCopyEnabled = false);
	virtual void OnEvent(mafEventBase *maf_event);

	void Create();

	/** 
	Set the visualization status for the vme (visible, not visible, mutex, ...) \sa mafSceneGraph mafView*/
	virtual int GetNodeStatus(mafVME *vme);

	/** IDs for the view GUI */
	enum VIEW_SLICE_WIDGET_ID
	{
		ID_OPACITY = Superclass::ID_LAST,
    ID_SLIDER,
		ID_LAST
	};

	/** 
	Create the visual pipe for the vme passed as argument. 
	To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
	otherwise ask to vme which is its visual pipe. */
	virtual void VmeCreatePipe(mafVME *vme);

	/** 
	Delete vme's visual pipe. It is called when vme is removed from visualization.*/
	virtual void VmeDeletePipe(mafVME *vme);

	/** Change the range of the WindowLevel Lookup table.*/
	void SetLutRange(double low_val, double high_val);

	void GetLutRange(double minMax[2]);

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

	virtual void VmeShow(mafVME *vme, bool show);

	void VmeRemove(mafVME *vme);

	/** print a dump of this object */
	virtual void Print(std::ostream& os, const int tabs=0);// const;

	void SetNormal(double normal[3]);

	/** Function for the correction of slice pose in case of current volume pose matrix is different from identity */
	void MultiplyPointByInputVolumeABSMatrix(double *point);

	void SetOpacity(double opacity);
	void SetSlice(int nSlice, double pos[3]);

	double* GetSlice(int nSlice);

protected:

	void UpdateSurfacesList(mafVME *vme);

	virtual mafGUI *CreateGui();

	mafSceneNode    *m_CurrentVolume; ///< Current visualized volume
	vtkActor2D      *m_Border;
	double           m_Slice1[3];
	double           m_Slice2[3];
  double           m_Slice1Position;
  double           m_Slice2Position;
	bool             m_SliceInitialized; 

	double					 m_Opacity;

	std::vector<mafSceneNode*> m_CurrentSurface;
};
#endif
