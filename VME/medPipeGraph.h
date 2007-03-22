/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeGraph.h,v $
  Language:  C++
  Date:      $Date: 2007-03-22 17:55:25 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeGraph_H__
#define __medPipeGraph_H__

#include "mafPipe.h"
#include "mafEvent.h"
#include "mafVMEScalar.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkPolyData;
class vtkSphereSource;
class mafVMELandmark;
class vtkAppendPolyData;
class mafMatrixVector;
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

  void medPipeGraph::UpdateGraph();
  
  //return data to plot
  std::vector<vtkRectilinearGrid*>  GetScalarData(mafSceneNode *n);

  std::vector<vtkDoubleArray*> medPipeGraph::GetScalarArray();

  vtkDoubleArray *medPipeGraph::GetTimeArray();

  void medPipeGraph::OnEvent(mafEventBase *maf_event); 
  
protected:

  mmgGui* medPipeGraph::CreateGui();

  enum PIPE_GRAPH_GUI_WIDGETS
  {
    ID_X_MIN = Superclass::ID_LAST, 

    ID_DRAW,
    ID_CHECK_BOX,
    ID_LEGEND,
    
    ID_LAST
  };

 mmgCheckListBox* m_CheckBox;
 int m_Legend;
 vtkLegendBoxActor *m_LegendBox_Actor;

private:

  void MinMax(double MinMax[2], std::vector<mafTimeStamp> vec);


  double m_OldColour[3];

  double			m_xmin;
  double			m_xmax;
  double			m_ymin;
  double			m_ymax;
  int				m_xlabel;
  int				m_ylabel;

  wxString		m_x_title;
  wxString		m_y_title;

  std::vector<vtkRectilinearGrid*> m_vtkData;

  std::vector<vtkDoubleArray*> scalar_array;
 
  vtkDoubleArray      *time_array;
  std::vector<mafTimeStamp> m_TimeVector;  


  mafVMEScalar *m_Emg_plot;
  vtkXYPlotActor *m_Actor1;
};  
#endif // __medPipeGraph_H__
