/*=========================================================================

 Program: MAF2
 Module: mafDataPipeCustomProber
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafDataPipeCustomProber_h
#define __mafDataPipeCustomProber_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDataPipeCustom.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class vtkMAFDistanceFilter;
class vtkPolyDataNormals;
class mafVME;

/** a data pipe which calculate new data depending on parameters given by the VME.
  This data pipe creates an internal vtkSource objects and links to it to 
  receive update events and calculate new data for the VME.
  @sa mafDataPipeCustom
*/
class MAF_EXPORT mafDataPipeCustomProber : public mafDataPipeCustom
{
public:
  mafTypeMacro(mafDataPipeCustomProber,mafDataPipe);

  enum PROBING_MODALITY
  {
    DENSITY_MODE = 0,
    DISTANCE_MODE,
    DISTANCE_MODE_SCALAR = 0,
    DISTANCE_MODE_VECTOR
  };

  /** Make a copy of itself.*/
  int DeepCopy(mafDataPipe *pipe);

  /** Set the surface to probe volume data*/
  void SetSurface(mafVME *surface);

  /** Return the probing surface node*/
  mafVME *GetSurface() {return m_Surface;};

  /** Set the volume to probe*/
  void SetVolume(mafVME *volume);

  /** Return the probed volume*/
  mafVME *GetVolume() {return m_Volume;};

  /** Set the mode to Density or Distance Map.*/
  void SetMode(int mode);

  /** Set the Density mode Map.*/
  void SetModeToDensity();

  /** Set the Distance mode Map.*/
  void SetModeToDistance();

  /** Get the mode.*/
  int GetMode();

  /** Set the threshold for Distance mode Map.*/
  void SetDistanceThreshold(float thr);

  /** Get the threshold of Distance mode Map.*/
  float GetDistanceThreshold();

  /** Set/Get the max distance for Distance mode Map.*/
  float GetMaxDistance();

  /** Set/Get the max distance for Distance mode Map.*/
  void SetMaxDistance(float max_dist);

  /** Set the Distance mode to produce scalar data or vector data.*/
  void SetDistanceMode(int mode);

  /** Set the Distance mode to produce scalar data.*/
  void SetDistanceModeToScalar() {this->SetDistanceMode(mafDataPipeCustomProber::DISTANCE_MODE_SCALAR);};

  /** Set the Distance mode to produce vector data.*/
  void SetDistanceModeToVector() {this->SetDistanceMode(mafDataPipeCustomProber::DISTANCE_MODE_VECTOR);};

  /** Get the distance mode type.*/
  int GetDistanceMode();

  /** Set/Get the High Density Threshold for Density mode Map.*/
  float GetHighDensity();

  /** Set/Get the High Density Threshold for Density mode Map.*/
  void SetHighDensity(float high_dens);

  /** Set/Get the Low Density Threshold for Density mode Map.*/
  float GetLowDensity();

  /** Set/Get the Low Density Threshold for Density mode Map.*/
  void SetLowDensity(float low_dens);

protected:
  mafDataPipeCustomProber();
  virtual ~mafDataPipeCustomProber();

  /** function called before of data pipe execution */
  virtual void PreExecute();

  /** function called to updated the data pipe output */
  virtual void Execute();

  mafVME *m_Surface;
  mafVME *m_Volume;

  vtkPolyDataNormals *m_Normals;
  vtkMAFDistanceFilter  *m_Prober;

  float  m_DistThreshold;
  float  m_MaxDistance;
  int		 m_DistanceModeType;
  int		 m_ProberMode;
  float  m_HighDensity;
  float  m_LowDensity;

private:
  mafDataPipeCustomProber(const mafDataPipeCustomProber&); // Not implemented
  void operator=(const mafDataPipeCustomProber&); // Not implemented  
};
#endif /* __mafDataPipeCustomProber_h */
