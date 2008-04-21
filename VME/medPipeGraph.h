/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeGraph.h,v $
  Language:  C++
  Date:      $Date: 2008-04-21 09:43:40 $
  Version:   $Revision: 1.15 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeGraph_H__
#define __medPipeGraph_H__

#include "mafPipe.h"
#include "mafEvent.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class medVMEAnalog;
class vtkActor;
class mmgCheckListBox;
class vtkLegendBoxActor;
class vtkXYPlotActor;
class vtkDoubleArray;
class vtkRectilinearGrid;

//----------------------------------------------------------------------------
// medPipeGraph :
//----------------------------------------------------------------------------
/** 
Visual pipe to visualize graphs of analog signals. */
class medPipeGraph : public mafPipe
{
public:
  mafTypeMacro(medPipeGraph,mafPipe);

  medPipeGraph();
  virtual     ~medPipeGraph ();

  void medPipeGraph::OnEvent(mafEventBase *maf_event); 

  virtual void Create(mafSceneNode *n);

  //Create plots of scalar data
  void UpdateGraph();

  //Change the name of the selected item in the legend box
  void ChangeItemName();

  //Change the title of the axis
  void ChangeAxisTitle();

  //Change signal color
  void ChangeSignalColor();
  
protected:
  mmgGui* CreateGui();

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

  mmgCheckListBox* m_CheckBox;
  vtkLegendBoxActor *m_LegendBox_Actor;

private:
  //create the legend
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

  std::vector<vtkRectilinearGrid*> m_vtkData;

  std::vector<vtkDoubleArray*> m_ScalarArray;
 
  vtkDoubleArray      *m_TimeArray;
  std::vector<mafTimeStamp> m_TimeVector;  

  medVMEAnalog   *m_EmgPlot;
  vtkXYPlotActor *m_PlotActor;

  vtkRectilinearGrid *m_TimeLine;
};  
#endif // __medPipeGraph_H__
