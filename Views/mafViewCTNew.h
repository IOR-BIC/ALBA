/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafViewCTNew.h,v $
Language:  C++
Date:      $Date: 2007-03-15 10:06:05 $
Version:   $Revision: 1.8 $
Authors:   Daniele Giunchi, Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafViewCTNew_H__
#define __mafViewCTNew_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewCompound.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEVolume;
class mafViewVTK;
class vtkPolyData;
class vtkActor2D;
class vtkActor;
class vtkTextMapper;
class vtkPolyDataMapper;
class vtkProbeFilter;
class vtkPlaneSource;
class mafMatrix;

//----------------------------------------------------------------------------
// mafViewCTNew :
//----------------------------------------------------------------------------
/**
This view features two Rx views and one compound view made of six CT slices.*/
class mafViewCTNew : public mafViewCompound
{
public:
	mafViewCTNew(wxString label = "View CT");
	virtual ~mafViewCTNew();

	mafTypeMacro(mafViewCTNew, mafViewCompound);

	virtual mafView *Copy(mafObserver *Listener);
	virtual void OnEvent(mafEventBase *maf_event);

	/** Show/Hide VMEs into plugged sub-views */
	virtual void VmeShow(mafNode *node, bool show);

	/** Remove VME into plugged sub-views */
	virtual void VmeRemove(mafNode *node);

	/** Operations to be performed on vme selection */
	virtual void VmeSelect(mafNode *node, bool select);

	/** Create visual pipe and initialize them to build a RXCT view */
	void PackageView();

	/** IDs for the GUI */
	enum VIEW_RXCT_WIDGET_ID
	{
		ID_LAYOUT_WIDTH = Superclass::ID_LAST,
		ID_LAYOUT_HEIGHT,
		ID_LAYOUT_POSITION,
		ID_LAYOUT_NORMAL,
		ID_LAYOUT_THICKNESS,
		ID_LAYOUT_UPDATE,
		ID_LAST
	};

	enum CT_SUBVIEW_ID
	{
		CT1 = 0,
		CT2,
		CT3,
		CT4,
		CT5,
		CT6,
		CT7,
		CT8,
		CT9,
		CT10,
		CT_CHILD_VIEWS_NUMBER,
	};

	/** Create the GUI on the bottom of the compounded view. */
	virtual void CreateGuiView();

	void SetCurrentZ(double Z);
	void SetCurrentZ(int idview,double Z);

	void SetMatrix(int idview,mafMatrix *matrix);

	/**
	Function that compute and create actors with probed texture*/
	void ProbeVolume();

	void SetColorText(int idView,double *color);
	void SetColorText(double **color);

  /**
  Update normal and position in the idview ViewSlice*/
  void SetCTLookupTable(double range[2]);
  void SetCTLookupTable(double min, double max);

	virtual void CameraUpdate();
	virtual void CameraReset(mafNode *node = NULL);

protected:
	/**
	Internally used to create a new instance of the GUI. This function should be
	overridden by subclasses to create specialized GUIs. Each subclass should append
	its own widgets and define the enum of IDs for the widgets as an extension of
	the superclass enum. The last id value must be defined as "LAST_ID" to allow the
	subclass to continue the ID enumeration from it. For appending the widgets in the
	same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual mmgGui  *CreateGui();

	/**
	Redefine to arrange views to generate RXCT visualization.*/
	//virtual void LayoutSubViewCustom(int width, int height);


	/**
	Enable/disable view widgets.*/
	void EnableWidgets(bool enable = true);

	/**
	Update normal and position in the idview ViewSlice*/
	void UpdateSliceView(int idview);

	//double m_BorderColor[10][3];

	mafVMEVolume    *m_CurrentVolume; ///< Current visualized volume

	mafViewCompound *m_ViewCTCompound;

	// this member variables are used by side panel gui view
	int m_WidthSection;
	int m_HeightSection;

	std::vector<double *> m_Position;
	std::vector<double *> m_Normal;

	double m_CurrentZ;

	double m_Spacing[3];

	mmgGui  *m_GuiViews[3];

	// members relative to the projection
	double m_Thickness;
	int m_AdditionalProfileNumber;
	double m_ProfileDistance;

	double	m_MinLUTHistogram;
	double	m_MaxLUTHistogram;

  std::vector<vtkActor2D *>	        m_TextActor;
	std::vector<vtkActor *>					  m_Actor;
	std::vector<vtkTextMapper *>			m_Text;
	std::vector<vtkPolyDataMapper *>	m_Mapper;
	std::vector<vtkProbeFilter *>		  m_Prober;
	std::vector<vtkPlaneSource *>		  m_PlaneSec;
};
#endif