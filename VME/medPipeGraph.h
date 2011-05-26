/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeGraph.h,v $
  Language:  C++
  Date:      $Date: 2011-05-26 08:28:49 $
  Version:   $Revision: 1.19.2.4 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeGraph_H__
#define __medPipeGraph_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medDefines.h"
#include "mafPipe.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class medVMEAnalog;
class vtkActor;
class mafGUICheckListBox;
class vtkLegendBoxActor;
class vtkXYPlotActor;
class vtkDoubleArray;
class vtkRectilinearGrid;


/** 
class name medPipeGraph.
Visual pipe to visualize graphs of analog signals. 
*/
class MED_EXPORT medPipeGraph : public mafPipe
{
public:
  /** RTTI Macro */
  mafTypeMacro(medPipeGraph,mafPipe);
  /** Constructor. */
  medPipeGraph();
  /** Destructor. */
  virtual     ~medPipeGraph ();

  /** process events that comes from  other objects*/
  void medPipeGraph::OnEvent(mafEventBase *maf_event); 

  /** visual pipe creation */
  /*virtual*/ void Create(mafSceneNode *n);

  /** Create plots of scalar data*/
  void UpdateGraph();

  /**Change the name of the selected item in the legend box*/
  void ChangeItemName();

  /**Change the title of the axis*/
  void ChangeAxisTitle();

  /**Change signal color*/
  void ChangeSignalColor();

  /** Set if visualize or not a particular signal */
  void SetSignalToPlot(int index,bool plot);

  /** set title on x axis */
  void SetTitleX(mafString title);
  /** set title on Y axis */
  void SetTitleY(mafString title);
  /** set title  */
  void SetTitle(mafString title);
  
protected:
  /** creation of the gui */
  mafGUI* CreateGui();

  enum PIPE_GRAPH_GUI_WIDGETS
  {
    ID_X_MIN = Superclass::ID_LAST, 
    ID_DRAW,
    ID_CHECK_BOX,
    ID_LEGEND,
    ID_ITEM_NAME,
    ID_AXIS_NAME_X,
    ID_AXIS_NAME_Y,
    ID_RANGE_X,
    ID_RANGE_Y,
    ID_FIT_PLOT,
    ID_SIGNALS_COLOR,
    ID_LAST
  };

  mafGUICheckListBox* m_CheckBox;
  vtkLegendBoxActor *m_LegendBox_Actor;
  vtkLegendBoxActor *m_LegendBoxTimeLine_Actor;

private:
  /**create the legend*/
  void CreateLegend();

  double m_OldColour[3];
  double m_ColorRGB[3];
  wxColor m_SignalColor;

  double      m_DataMax;
  double      m_DataMin;
  double      m_TimeStampMax;
  double      m_TimesRange[2];
  double      m_DataManualRange[2];
  double      m_TimesManualRange[2];
  int				m_Xlabel;
  int				m_Ylabel;
  int       m_NumberOfSignals;
  int       m_TimeStamp;
  long      m_ItemId;

  int m_Legend;
  int m_FitPlot;

  mafString   m_ItemName;
  wxString		m_TitileX;
  wxString		m_TitileY;
  wxString    m_Title;

  std::vector<vtkRectilinearGrid*> m_VtkData;
  std::vector<vtkDoubleArray*> m_ScalarArray;
  std::vector<bool> m_CheckedVector;
 
  vtkDoubleArray      *m_TimeArray;
  std::vector<mafTimeStamp> m_TimeVector;  

  medVMEAnalog   *m_EmgPlot;
  vtkXYPlotActor *m_PlotActor;
  vtkXYPlotActor *m_PlotTimeLineActor;

  vtkRectilinearGrid *m_TimeLine;
};  
#endif // __medPipeGraph_H__
