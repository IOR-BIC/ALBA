/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeGraph
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeGraph_H__
#define __albaPipeGraph_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"
#include "albaEvent.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class albaVMEAnalog;
class vtkActor;
class albaGUICheckListBox;
class vtkLegendBoxActor;
class vtkXYPlotActor;
class vtkDoubleArray;
class vtkRectilinearGrid;


/** 
class name albaPipeGraph.
Visual pipe to visualize graphs of analog signals. 
*/
class ALBA_EXPORT albaPipeGraph : public albaPipe
{
public:
  /** RTTI Macro */
  albaTypeMacro(albaPipeGraph,albaPipe);
  /** Constructor. */
  albaPipeGraph();
  /** Destructor. */
  virtual     ~albaPipeGraph ();

  /** process events that comes from  other objects*/
  void albaPipeGraph::OnEvent(albaEventBase *alba_event); 

  /** visual pipe creation */
  /*virtual*/ void Create(albaSceneNode *n);

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
  void SetTitleX(albaString title);
  /** set title on Y axis */
  void SetTitleY(albaString title);
  /** set title  */
  void SetTitle(albaString title);
  
protected:
  /** creation of the gui */
  albaGUI* CreateGui();

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

  albaGUICheckListBox* m_CheckBox;
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

  albaString   m_ItemName;
  wxString		m_TitileX;
  wxString		m_TitileY;
  wxString    m_Title;

  std::vector<vtkRectilinearGrid*> m_VtkData;
  std::vector<vtkDoubleArray*> m_ScalarArray;
  std::vector<bool> m_CheckedVector;
 
  vtkDoubleArray      *m_TimeArray;
  std::vector<albaTimeStamp> m_TimeVector;  

  albaVMEAnalog   *m_EmgPlot;
  vtkXYPlotActor *m_PlotActor;
  vtkXYPlotActor *m_PlotTimeLineActor;

  vtkRectilinearGrid *m_TimeLine;
};  
#endif // __albaPipeGraph_H__
