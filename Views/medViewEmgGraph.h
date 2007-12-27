/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medViewEmgGraph.h,v $
  Language:  C++
  Date:      $Date: 2007-12-27 13:04:30 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medViewEmgGraph_H__
#define __medViewEmgGraph_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewVTK.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkXYPlotActor;
class vtkRectilinearGrid;
class vtkLegendBoxActor;

//----------------------------------------------------------------------------
// medViewEmgGraph :
//----------------------------------------------------------------------------
class medViewEmgGraph: public mafViewVTK
{
public:
  medViewEmgGraph(wxString label = "View Emg Graph");
  virtual ~medViewEmgGraph(); 

  mafTypeMacro(medViewEmgGraph, mafViewVTK);

  virtual mafView *Copy(mafObserver *Listener);

  /** Get data from datapipe. */
  std::vector<vtkRectilinearGrid*> GetDataPlot();

  void Create();

  /** Create the visual pipe for the node passed as argument. */
  void VmeCreatePipe(mafNode *vme);

  /** Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  void VmeDeletePipe(mafNode *vme);
  
  void OnEvent(mafEventBase *maf_event);
  
	/** Show/Hide VMEs into plugged views*/
  virtual void VmeShow(mafNode *node, bool show);

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  //virtual mmgGui  *CreateGui();

  enum PIPE_GRAPH_GUI_WIDGETS
  {
    ID_X_MIN = Superclass::ID_LAST, 
    ID_LEGEND,
    ID_LUT_CHOOSER,
    ID_LAST
  };

  /** Create the GUI on the bottom of the compounded view. */
  mmgGui* CreateGui();

  /** Create the legend box. */
  void CreateLegend();

  double			m_Xmin;
  double			m_Xmax;
  double			m_Ymin;
  double			m_Ymax;
  double m_OldColour[3];

  int				m_Xlabel;
  int				m_Ylabel;

  wxString		m_XTitle;
  wxString		m_YTitle;

  int m_Legend;
  bool m_InputPresent;

  vtkXYPlotActor *m_PlotActor;
  vtkLegendBoxActor *m_LegendBoxActor;
};
#endif
