/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeGraph.h,v $
  Language:  C++
  Date:      $Date: 2007-09-28 08:38:56 $
  Version:   $Revision: 1.4 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeGraph_H__
#define __medPipeGraph_H__

#include "mafPipe.h"
#include "mafEvent.h"
#include "medVMEEmg.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

class vtkActor;
class mmgCheckListBox;
class vtkLegendBoxActor;
class vtkXYPlotActor;
class vtkDoubleArray;
class vtkRectilinearGrid;

//----------------------------------------------------------------------------
// medPipeGraph :
//----------------------------------------------------------------------------
class medPipeGraph : public mafPipe
{
public:
  mafTypeMacro(medPipeGraph,mafPipe);

  medPipeGraph();
  virtual     ~medPipeGraph ();

  virtual void Create(mafSceneNode *n);

  //Create plots of scalar data
  void medPipeGraph::UpdateGraph();

  //Change the name of the selected item in the legend box
  void medPipeGraph::ChangeItemName();
  
  void medPipeGraph::OnEvent(mafEventBase *maf_event); 
  
protected:

  mmgGui* medPipeGraph::CreateGui();

  enum PIPE_GRAPH_GUI_WIDGETS
  {
    ID_X_MIN = Superclass::ID_LAST, 

    ID_DRAW,
    ID_CHECK_BOX,
    ID_LEGEND,
    ID_ITEM_NAME,
    ID_LAST
  };

 mmgCheckListBox* m_CheckBox;
 int m_Legend;
 vtkLegendBoxActor *m_LegendBox_Actor;

private:

  void MinMax(double MinMax[2], std::vector<mafTimeStamp> vec);

  //create the legend
  void medPipeGraph::CreateLegend();

  double m_OldColour[3];

  double			m_Xmin;
  double			m_Xmax;
  double			m_Ymin;
  double			m_Ymax;
  int				m_Xlabel;
  int				m_Ylabel;
  long      m_ItemId;

  mafString   m_ItemName;
  wxString		m_X_title;
  wxString		m_Y_title;

  std::vector<vtkRectilinearGrid*> m_vtkData;

  std::vector<vtkDoubleArray*> scalar_Array;
 
  vtkDoubleArray      *time_Array;
  std::vector<mafTimeStamp> m_TimeVector;  

  mafVMEScalar *m_Emg_plot;
  vtkXYPlotActor *m_Actor1;
};  
#endif // __medPipeGraph_H__
