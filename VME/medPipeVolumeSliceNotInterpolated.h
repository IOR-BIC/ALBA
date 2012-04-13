/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeSliceNotInterpolated.h,v $
  Language:  C++
  Date:      $Date: 2012-04-13 10:08:37 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Alberto Losi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeVolumeSliceNotInterpolated_H__
#define __medPipeVolumeSliceNotInterpolated_H__

#include "mafPipeSlice.h"

class vtkMEDVolumeSlicerNotInterpolated;
class mafGUI;
class vtkLookupTable;
class vtkImageData;
class vtkImageActor;
class vtkImageMapToColors;
class vtkImageShiftScale;
class mafGUIFloatSlider;

//----------------------------------------------------------------------------
class MED_EXPORT medPipeVolumeSliceNotInterpolated : public mafPipeSlice
//----------------------------------------------------------------------------
{
public:

  enum GUI_WIDGET_ID
  {
    ID_LUT,
    ID_AXIS,
    ID_SLICE,
    MINID,
  };

  /** RTTI Macro */
  mafTypeMacro(medPipeVolumeSliceNotInterpolated,mafPipeSlice);

  /** ctor */
  medPipeVolumeSliceNotInterpolated();

  /** dtor */
  ~medPipeVolumeSliceNotInterpolated();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase * event);

  /** The real setup must be performed here - not in the ctor */
  virtual void Create(mafSceneNode * node);

  /** Set slice with the specified parameters */
  void SetSlice(double origin[3], int sliceAxis);

protected:

  /** Create the slicer pipeline. */
  void CreateSlice();

  /** Create the GUI for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual mafGUI * CreateGui();

  
  int m_SliceAxis;                              //> Slicer direction
  double m_Bounds[6];                           //> Input volume bounds
  double m_CurrentSlice;                        //> Current slice coordinate
  mafGUIFloatSlider *m_SliceSlider;             //> Slice coordinate slider
  vtkLookupTable  *m_ColorLUT;                  //> Lookup table to display slice
  vtkLookupTable *m_VolumeLUT;                  //> Input volume lookup table
  vtkMEDVolumeSlicerNotInterpolated * m_Slicer; //> Not interpolated slicer
  vtkImageData *m_SlicerOutputImageData;        //> Image data from the slicer
  vtkImageData * m_SlicerImageDataToRender;     //> Image data to be rendered
  vtkImageMapToColors *m_ImageMapToColors;      //> Filter that map colors on image data
  vtkImageShiftScale *m_ImageShiftScale;        //> Filter that rescale image to be an unsigned char with scalar range from 0 to 255
  vtkImageActor *m_ImageActor;                  //> Image actor to display image

private:

  /** Rescale output lookup table to 0 255 scalar range */
  void RescaleLUT(vtkLookupTable *inputLUT,vtkLookupTable *outputLUT);

  /** Set slice with the current parameters */
  void SetSlice();

  /** Update slicer pipeline */
  void UpdateSlice();

  /** Set slice origin the current parameters */
  void SetOrigin();

  /** Update image actor in front renderer */
  void UpdateImageActor();

  /** Update intermediate image to be rendered */
  void UpdateImageToRender();

};

#endif //#ifndef __medPipeVolumeSliceNotInterpolated_H__