/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medDataPipeCustomSegmentationVolume.cpp,v $
Language:  C++
Date:      $Date: 2011-06-23 15:56:01 $
Version:   $Revision: 1.1.2.15 $
Authors:   Matteo Giacomoni, Gianluigi Crimi
==========================================================================
Copyright (c) 2010
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medDataPipeCustomSegmentationVolume.h"

#include "mafVME.h"
#include "medVMESegmentationVolume.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMath.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkShortArray.h"
#include "vtkDoubleArray.h"
#include "vtkImageCast.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredPoints.h"
#include "vtkUnsignedCharArray.h"

#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"

#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))

typedef  itk::Image< double, 3> RealImage;
typedef  itk::Image< unsigned char, 3> UCharImage;

//------------------------------------------------------------------------------
mafCxxTypeMacro(medDataPipeCustomSegmentationVolume)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
medDataPipeCustomSegmentationVolume::medDataPipeCustomSegmentationVolume()
//------------------------------------------------------------------------------
{
  m_Volume = NULL;
  m_ManualVolumeMask = NULL;
  m_RefinementVolumeMask = NULL;
  m_RG = NULL;
  vtkNEW(m_RG);
  m_SP = NULL;
  vtkNEW(m_SP);
  m_AutomaticRG = NULL;
  vtkNEW(m_AutomaticRG);
  m_AutomaticSP = NULL;
  vtkNEW(m_AutomaticSP);
  m_ManualRG = NULL;
  vtkNEW(m_ManualRG);
  m_ManualSP = NULL;
  vtkNEW(m_ManualSP);
  m_RefinementRG = NULL;
  vtkNEW(m_RefinementRG);
  m_RefinementSP = NULL;
  vtkNEW(m_RefinementSP);
  m_RegionGrowingRG = NULL;
  vtkNEW(m_RegionGrowingRG);
  m_RegionGrowingSP = NULL;
  vtkNEW(m_RegionGrowingSP);
  
  m_ChangedAutomaticData = false;
  m_ChangedManualData = false;
  m_ChangedRefinementData = false;
  m_ChangedRegionGrowingData = false;

  SetInput(NULL);
}

//------------------------------------------------------------------------------
medDataPipeCustomSegmentationVolume::~medDataPipeCustomSegmentationVolume()
//------------------------------------------------------------------------------
{
  vtkDEL(m_RG);
  vtkDEL(m_SP);
  vtkDEL(m_AutomaticRG);
  vtkDEL(m_AutomaticSP);
  vtkDEL(m_ManualRG);
  vtkDEL(m_ManualSP);
  vtkDEL(m_RefinementRG);
  vtkDEL(m_RefinementSP);
  vtkDEL(m_RegionGrowingRG);
  vtkDEL(m_RegionGrowingSP);
  //////////////////////////////////////////////////////////////////////////
  for (int i=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    delete []m_AutomaticSegmentationRanges[i];
  }
  m_AutomaticSegmentationRanges.clear();

  for (int i=0;i<m_RegionGrowingSeeds.size();i++)
  {
    delete []m_RegionGrowingSeeds[i];
  }
  m_RegionGrowingSeeds.clear();
}
//------------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::DeepCopy(mafDataPipe *pipe)
//------------------------------------------------------------------------------
{
  if (mafDataPipe::DeepCopy(pipe) == MAF_OK)
  {
    medDataPipeCustomSegmentationVolume *dpcp = medDataPipeCustomSegmentationVolume::SafeDownCast(pipe);
    if (dpcp != NULL)
    {
      m_Volume = ((medDataPipeCustomSegmentationVolume*)pipe)->GetVolume();
      m_ManualVolumeMask = ((medDataPipeCustomSegmentationVolume*)pipe)->GetManualVolumeMask();

      m_AutomaticSegmentationThresholdModality = ((medDataPipeCustomSegmentationVolume*)pipe)->GetAutomaticSegmentationThresholdModality();
      m_UseDoubleThreshold = ((medDataPipeCustomSegmentationVolume*)pipe)->GetDoubleThresholdModality();

      m_AutomaticSegmentationGlobalThreshold = ((medDataPipeCustomSegmentationVolume*)pipe)->GetAutomaticSegmentationGlobalThreshold();

      for (int i=0;i<((medDataPipeCustomSegmentationVolume*)pipe)->GetNumberOfRanges();i++)
      {
        int startSlice,endSlice;
        double threshold;
        ((medDataPipeCustomSegmentationVolume*)pipe)->GetRange(i,startSlice,endSlice,threshold);
        this->AddRange(startSlice,endSlice,threshold);
      }

      m_RegionGrowingUpperThreshold = ((medDataPipeCustomSegmentationVolume*)pipe)->GetRegionGrowingUpperThreshold();
      m_RegionGrowingLowerThreshold = ((medDataPipeCustomSegmentationVolume*)pipe)->GetRegionGrowingLowerThreshold();

      for (int i=0;i<((medDataPipeCustomSegmentationVolume*)pipe)->GetNumberOfSeeds();i++)
      {
        int seed[3];
        ((medDataPipeCustomSegmentationVolume*)pipe)->GetSeed(i,seed);
        this->AddSeed(seed);
      }

      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::SetAutomaticSegmentationThresholdModality(int modality)
//------------------------------------------------------------------------------
{
  m_AutomaticSegmentationThresholdModality = modality;
  m_ChangedAutomaticData = true;
  Modified();
}

//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::SetDoubleThresholdModality(int modality)
//------------------------------------------------------------------------------
{
 
  m_UseDoubleThreshold = modality;
  m_ChangedAutomaticData = true;
  Modified();
}

//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::ApplyManualSegmentation()
//------------------------------------------------------------------------------
{
  if (m_ManualVolumeMask == NULL)
  {
    return;
  }

  mafVME *vol = mafVME::SafeDownCast(m_Volume);
  vol->GetOutput()->Update();
  vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
  volumeData->Update();

  vtkDataSet *maskVolumeData = mafVME::SafeDownCast(m_ManualVolumeMask)->GetOutput()->GetVTKData();
  maskVolumeData->Update();

  if (maskVolumeData==NULL || maskVolumeData->GetNumberOfPoints() != volumeData->GetNumberOfPoints() || maskVolumeData->GetPointData()->GetScalars()==NULL)
  {
    mafLogMessage("Error! Wrong manual volume mask");
    return;
  }
  
  vtkDataArray *maskScalar = maskVolumeData->GetPointData()->GetScalars();
  vtkDataArray *regionGrowingScalar;
  
  if(maskVolumeData->IsA("vtkRectilinearGrid")) 
  {
    m_RegionGrowingRG->Update();
    if (m_RegionGrowingRG->GetPointData()->GetScalars() == NULL)
    {
      return;
    }
    if (m_RegionGrowingRG->GetPointData()->GetScalars()->GetNumberOfTuples() != maskScalar->GetNumberOfTuples())
    {
      return;
    }
    regionGrowingScalar = m_RegionGrowingRG->GetPointData()->GetScalars();
  }
  else if (maskVolumeData->IsA("vtkStructuredPoints"))
  {
    m_RegionGrowingSP->Update();
    if (m_RegionGrowingSP->GetPointData()->GetScalars() == NULL)
    {
      return;
    }
    if (m_RegionGrowingSP->GetPointData()->GetScalars()->GetNumberOfTuples() != maskScalar->GetNumberOfTuples())
    {
      return;
    }
    regionGrowingScalar = m_RegionGrowingSP->GetPointData()->GetScalars();
  }

  vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
  newScalars->SetName("SCALARS");
  newScalars->SetNumberOfTuples(maskScalar->GetNumberOfTuples());

  long progress = 0;

  mafEvent e(this,PROGRESSBAR_SHOW);
  this->GetVME()->ForwardUpEvent(&e);

  int step = ceil((double)maskScalar->GetNumberOfTuples()/100);
  double invStep = (double)1/step;
  for (int i=0;i<maskScalar->GetNumberOfTuples();i++)
  {
    if ((i%step) == 0)
    {
	    progress = (i*100/maskScalar->GetNumberOfTuples());
	    mafEvent eUpdate(this,PROGRESSBAR_SET_VALUE,progress);
	    this->GetVME()->ForwardUpEvent(&eUpdate);
    }

    if (maskScalar->GetTuple1(i) == 0)
    {
      double value = regionGrowingScalar->GetTuple1(i);
      newScalars->SetTuple1(i,value);
    }
    else if (maskScalar->GetTuple1(i) == 255)
    {
      double value = regionGrowingScalar->GetTuple1(i);
      newScalars->SetTuple1(i,255-value);
    }
  }

  if (volumeData->IsA("vtkStructuredPoints"))
  {
    vtkMAFSmartPointer<vtkStructuredPoints> newSP;
    newSP->CopyStructure(vtkStructuredPoints::SafeDownCast(volumeData));
    newSP->Update();
    newSP->GetPointData()->AddArray(newScalars);
    newSP->GetPointData()->SetActiveScalars("SCALARS");
    newSP->SetScalarTypeToUnsignedChar();
    newSP->Update();

    m_ManualSP->DeepCopy(newSP);
    m_ManualSP->Update();
    m_SP->DeepCopy(newSP);
    m_SP->Update();
  }
  else
  {
    vtkMAFSmartPointer<vtkRectilinearGrid> newRG;
    newRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(volumeData));
    newRG->Update();
    newRG->GetPointData()->AddArray(newScalars);
    newRG->GetPointData()->SetActiveScalars("SCALARS");
    newRG->Update();

    m_ManualRG->DeepCopy(newRG);
    m_ManualRG->Update();

    m_RG->DeepCopy(newRG);
    m_RG->Update();
  }

  mafEvent eHideProgress(this,PROGRESSBAR_HIDE);
  this->GetVME()->ForwardUpEvent(&eHideProgress);  

}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::ApplyAutomaticSegmentation()
//------------------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(m_Volume);
  vol->GetOutput()->Update();
  vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
  volumeData->Update();

  typedef itk::BinaryThresholdImageFilter<RealImage, RealImage> BinaryThresholdFilter;
  BinaryThresholdFilter::Pointer binaryThreshold = BinaryThresholdFilter::New();

  int volumeDimensions[3];
  if(volumeData->IsA("vtkRectilinearGrid")) {
    vtkRectilinearGrid *rectilinearGrid=vtkRectilinearGrid::SafeDownCast(volumeData);
    rectilinearGrid->GetDimensions(volumeDimensions);
  }
  else if (volumeData->IsA("vtkStructuredPoints")) {
    vtkStructuredPoints *imageData=vtkStructuredPoints::SafeDownCast(volumeData);
    imageData->GetDimensions(volumeDimensions);
  }

  long progress = 0;

  mafEvent e(this,PROGRESSBAR_SHOW);
  this->GetVME()->ForwardUpEvent(&e);

  vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
  newScalars->SetName("SCALARS");
  newScalars->SetNumberOfTuples(volumeDimensions[0]*volumeDimensions[1]*volumeDimensions[2]);

  int step = ceil((double)volumeDimensions[2]/100);
  double invStep = (double)1/step;

  for (int i=0;i<volumeDimensions[2];i++)
  {
    if (i%step == 0)
    {
	    progress = (i*100/volumeDimensions[2]);
	    mafEvent eUpdate(this,PROGRESSBAR_SET_VALUE,progress);
	    this->GetVME()->ForwardUpEvent(&eUpdate);
    }

    double localThreshold;
    double localUpperTheshold;
    bool inRange = false;
    if(m_AutomaticSegmentationThresholdModality == medVMESegmentationVolume::RANGE)
    {
      //Find the correct threshold for the slice
      int rangeIndex = -1;
      for (int j=0;j<m_AutomaticSegmentationRanges.size();j++)
      {
        if (i>=(m_AutomaticSegmentationRanges[j][0]) && i<=(m_AutomaticSegmentationRanges[j][1]))
        {
          rangeIndex = j;
          localThreshold = m_AutomaticSegmentationThresholds[j];
          localUpperTheshold = m_AutomaticSegmentationUpperThresholds[j];
          inRange = true;
        }
      }
    }
    else
    {
      inRange = true;
      localThreshold = m_AutomaticSegmentationGlobalThreshold;
      localUpperTheshold = m_AutomaticSegmentationGlobalUpperThreshold;
    }
    //////////////////////////////////////////////////////////////////////////

    int numberOfPoints = volumeDimensions[0] * volumeDimensions[1];
    if(!inRange)
    // range not specified
    {
      
      for (int k=0;k<numberOfPoints;k++)
      {
        char value = 0;
        newScalars->SetTuple1(k + i*numberOfPoints,value);
      }
    }

    else
    {
      for (int k=0;k<(numberOfPoints);k++)
      {
        vtkDataArray *inputScalars = volumeData->GetPointData()->GetScalars();       
        double value = inputScalars->GetTuple1(k + i*numberOfPoints);

        if((!m_UseDoubleThreshold && value >= localThreshold) || (value>= localThreshold && value <= localUpperTheshold))
          newScalars->SetTuple1(k + i*numberOfPoints,255);
        else    
          newScalars->SetTuple1(k + i*numberOfPoints,0);
      }
    }
  }


  if (volumeData->IsA("vtkStructuredPoints"))
  {
    vtkMAFSmartPointer<vtkStructuredPoints> newSP;
    newSP->CopyStructure(vtkStructuredPoints::SafeDownCast(volumeData));
    newSP->Update();
    newSP->GetPointData()->AddArray(newScalars);
    newSP->GetPointData()->SetActiveScalars("SCALARS");
    //newSP->SetScalarTypeToShort();
    newSP->Update();

    m_AutomaticSP->DeepCopy(newSP);
    m_AutomaticSP->Update();
    m_SP->DeepCopy(newSP); 
    m_SP->Update();
  }
  else
  {
    vtkMAFSmartPointer<vtkRectilinearGrid> newRG;
    newRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(volumeData));
    newRG->Update();
    newRG->GetPointData()->AddArray(newScalars);
    newRG->GetPointData()->SetActiveScalars("SCALARS");
    newRG->Update();

    double sr[2];
    int n;
    vtkDataArray *x = newRG->GetXCoordinates();
    x->GetRange(sr);
    n = x->GetNumberOfTuples();

    m_AutomaticRG->DeepCopy(newRG);
    m_AutomaticRG->Update();
    m_RG->DeepCopy(newRG);
    m_RG->Update();
  }

  mafEvent eHideProgress(this,PROGRESSBAR_HIDE);
  this->GetVME()->ForwardUpEvent(&eHideProgress);


}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::ApplyRefinementSegmentation()
//------------------------------------------------------------------------------
{
  if (m_RefinementVolumeMask == NULL)
  {
    return;
  }

  mafVME *vol = mafVME::SafeDownCast(m_Volume);
  vol->GetOutput()->Update();
  vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
  volumeData->Update();

  vtkDataSet *maskVolumeData = mafVME::SafeDownCast(m_RefinementVolumeMask)->GetOutput()->GetVTKData();
  maskVolumeData->Update();

  if (maskVolumeData==NULL || maskVolumeData->GetNumberOfPoints() != volumeData->GetNumberOfPoints() || maskVolumeData->GetPointData()->GetScalars()==NULL)
  {
    mafLogMessage("Error! Wrong refinement volume mask");
    return;
  }

  vtkDataArray *maskScalar = maskVolumeData->GetPointData()->GetScalars();
  vtkDataArray *manualScalar;

  if(maskVolumeData->IsA("vtkRectilinearGrid")) 
  {
    if (m_ManualRG == NULL)
    {
      return;
    }

    m_ManualRG->Update();
    if (m_ManualRG->GetPointData()->GetScalars() == NULL)
    {
      return;
    }
    if (m_ManualRG->GetPointData()->GetScalars()->GetNumberOfTuples() != maskScalar->GetNumberOfTuples())
    {
      return;
    }
    manualScalar = m_ManualRG->GetPointData()->GetScalars();
  }
  else if (maskVolumeData->IsA("vtkStructuredPoints"))
  {
    if (m_ManualSP == NULL)
    {
      return;
    }

    m_ManualSP->Update();
    if (m_ManualSP->GetPointData()->GetScalars() == NULL)
    {
      return;
    }
    if (m_ManualSP->GetPointData()->GetScalars()->GetNumberOfTuples() != maskScalar->GetNumberOfTuples())
    {
      return;
    }
    manualScalar = m_ManualSP->GetPointData()->GetScalars();
  }

  vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
  newScalars->SetName("SCALARS");
  newScalars->SetNumberOfTuples(maskScalar->GetNumberOfTuples());

  long progress = 0;

  mafEvent e(this,PROGRESSBAR_SHOW);
  this->GetVME()->ForwardUpEvent(&e);

  int step = ceil((double)maskScalar->GetNumberOfTuples()/100);
  double invStep = (double)1/step;
  for (int i=0;i<maskScalar->GetNumberOfTuples();i++)
  {
    if ((i%step) == 0)
    {
      progress = (i*100/maskScalar->GetNumberOfTuples());
      mafEvent eUpdate(this,PROGRESSBAR_SET_VALUE,progress);
      this->GetVME()->ForwardUpEvent(&eUpdate);
    }

    if (maskScalar->GetTuple1(i) == 0)
    {
      double value = manualScalar->GetTuple1(i);
      newScalars->SetTuple1(i,value);
    }
    if (maskScalar->GetTuple1(i) == 255)
    {
      double value = manualScalar->GetTuple1(i);
      newScalars->SetTuple1(i,255-value);
    }
  }

  if (volumeData->IsA("vtkStructuredPoints"))
  {
    vtkMAFSmartPointer<vtkStructuredPoints> newSP;
    newSP->CopyStructure(vtkStructuredPoints::SafeDownCast(volumeData));
    newSP->Update();
    newSP->GetPointData()->AddArray(newScalars);
    newSP->GetPointData()->SetActiveScalars("SCALARS");
    newSP->SetScalarTypeToUnsignedChar();
    newSP->Update();

    m_RefinementSP->DeepCopy(newSP);
    m_RefinementSP->Update();
    m_SP->DeepCopy(newSP);
    m_SP->Update();
  }
  else
  {
    vtkMAFSmartPointer<vtkRectilinearGrid> newRG;
    newRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(volumeData));
    newRG->Update();
    newRG->GetPointData()->AddArray(newScalars);
    newRG->GetPointData()->SetActiveScalars("SCALARS");
    newRG->Update();

    m_RefinementRG->DeepCopy(newRG);
    m_RefinementRG->Update();
    m_RG->DeepCopy(newRG);
    m_RG->Update();
  }

  mafEvent eHideProgress(this,PROGRESSBAR_HIDE);
  this->GetVME()->ForwardUpEvent(&eHideProgress);  
}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::ApplyRegionGrowingSegmentation()
//------------------------------------------------------------------------------
{
  mafEvent e(this,PROGRESSBAR_SHOW);
  this->GetVME()->ForwardUpEvent(&e);

  typedef itk::ConnectedThresholdImageFilter<RealImage, RealImage> ITKConnectedThresholdFilter;
  ITKConnectedThresholdFilter::Pointer connectedThreshold = ITKConnectedThresholdFilter::New();

  mafVME *vol = mafVME::SafeDownCast(m_Volume);
  vol->GetOutput()->Update();
  vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
  volumeData->Update();

  if (m_RegionGrowingSeeds.size()==0)
  {
    if (volumeData->IsA("vtkStructuredPoints"))
    {
      m_RegionGrowingSP->DeepCopy(m_AutomaticSP);
      m_RegionGrowingSP->Update();
      m_SP->DeepCopy(m_RegionGrowingSP);
      m_SP->Update();
    }
    else if (volumeData->IsA("vtkRectilinearGrid"))
    {
      m_RegionGrowingRG->DeepCopy(m_AutomaticRG);
      m_RegionGrowingRG->Update();
      m_RG->DeepCopy(m_RegionGrowingRG);
      m_RG->Update();
    }

    mafEvent eHideProgress(this,PROGRESSBAR_HIDE);
    this->GetVME()->ForwardUpEvent(&eHideProgress);  
    return;
  }

  mafEvent eUpdate(this,PROGRESSBAR_SET_VALUE,(long)5);
  this->GetVME()->ForwardUpEvent(&eUpdate);

  vtkMAFSmartPointer<vtkStructuredPoints> spInputOfRegionGrowing;
  vtkDataSet *automaticData;
  if (volumeData->IsA("vtkStructuredPoints"))
  {
    spInputOfRegionGrowing->DeepCopy(vtkStructuredPoints::SafeDownCast(volumeData));
    spInputOfRegionGrowing->Update();

    automaticData = m_AutomaticSP;
  }
  else if (volumeData->IsA("vtkRectilinearGrid"))
  {
    int dim[3];
    vtkRectilinearGrid::SafeDownCast(volumeData)->GetDimensions(dim);
    vtkMAFSmartPointer<vtkDoubleArray> oldScalars;
    oldScalars->SetNumberOfTuples(volumeData->GetNumberOfPoints());
    oldScalars->SetName("SCALARS");
    for (int k=0;k<volumeData->GetNumberOfPoints();k++)
    {
      oldScalars->SetTuple1(k,volumeData->GetPointData()->GetScalars()->GetTuple1(k));
    }

    spInputOfRegionGrowing->SetDimensions(dim);
    spInputOfRegionGrowing->SetSpacing(1.0,1.0,1.0);
    spInputOfRegionGrowing->GetPointData()->AddArray(oldScalars);
    spInputOfRegionGrowing->GetPointData()->SetActiveScalars("SCALARS");
    spInputOfRegionGrowing->Update();

    automaticData = m_AutomaticRG;
    
  }

  eUpdate.SetArg(10);
  this->GetVME()->ForwardUpEvent(&eUpdate);

  vtkMAFSmartPointer<vtkImageCast> vtkImageToFloat;
  vtkImageToFloat->SetOutputScalarTypeToDouble();
  vtkImageToFloat->SetInput(spInputOfRegionGrowing);
  vtkImageToFloat->Modified();
  vtkImageToFloat->Update();

  typedef itk::VTKImageToImageFilter< RealImage > ConvertervtkTOitk;
  ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
  vtkTOitk->SetInput( vtkImageToFloat->GetOutput() );
  vtkTOitk->Update();

  connectedThreshold->SetLower(m_RegionGrowingLowerThreshold);
  connectedThreshold->SetUpper(m_RegionGrowingUpperThreshold);
  connectedThreshold->SetReplaceValue(255);

  RealImage::IndexType seed;
  for (int iSeeds=0;iSeeds<m_RegionGrowingSeeds.size();iSeeds++)
  {
	  for (int i=0;i<3;i++)
	  {
	    seed[i]=m_RegionGrowingSeeds[iSeeds][i];
	  }
    connectedThreshold->AddSeed(seed);
  }

  connectedThreshold->SetInput( ((RealImage*)vtkTOitk->GetOutput()) );

  eUpdate.SetArg(15);
  this->GetVME()->ForwardUpEvent(&eUpdate);

  try
  {
    connectedThreshold->Update();
  }
  catch ( itk::ExceptionObject &err )
  {
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
  }

  typedef itk::ImageToVTKImageFilter< RealImage > ConverteritkTOvtk;
  ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
  itkTOvtk->SetInput( connectedThreshold->GetOutput() );
  itkTOvtk->Update();

  vtkStructuredPoints *spOutputRegionGrowing = ((vtkStructuredPoints*)itkTOvtk->GetOutput());
  spOutputRegionGrowing->Update();

  eUpdate.SetArg(65);
  this->GetVME()->ForwardUpEvent(&eUpdate);


  //////////////////////////////////////////////////////////////////////////
  //Perform the OR operation between region growing and automatic output
  //////////////////////////////////////////////////////////////////////////


  int volumeDimensions[3];
  if(volumeData->IsA("vtkRectilinearGrid")) {
    vtkRectilinearGrid *rectilinearGrid=vtkRectilinearGrid::SafeDownCast(volumeData);
    rectilinearGrid->GetDimensions(volumeDimensions);
  }
  else if (volumeData->IsA("vtkStructuredPoints")) {
    vtkStructuredPoints *imageData=vtkStructuredPoints::SafeDownCast(volumeData);
    imageData->GetDimensions(volumeDimensions);
  }

  vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
  newScalars->SetNumberOfTuples(volumeData->GetNumberOfPoints());
  newScalars->SetName("SCALARS");
  newScalars->SetNumberOfTuples(volumeDimensions[0]*volumeDimensions[1]*volumeDimensions[2]);

  int numberOfPoints = volumeDimensions[0] * volumeDimensions[1];
  
  for (int i=0;i<volumeDimensions[2];i++)
  {

    for (int k=0;k<numberOfPoints;k++)
    {
      int index = k+i*(volumeDimensions[0]*volumeDimensions[1]);
      double automaticValue = automaticData->GetPointData()->GetScalars()->GetTuple1(index);
      double regionGrowingValue = spOutputRegionGrowing->GetPointData()->GetScalars()->GetTuple1(index);

      if(i>m_RegionGrowingEndSlice || i<m_RegionGrowingStartSlice)
      {
        newScalars->SetTuple1(k+i*(volumeDimensions[0]*volumeDimensions[1]),automaticValue);
      }
      else
      {
        if (automaticValue == 255 || regionGrowingValue == 255)
        {
          newScalars->SetTuple1(index,255);
        }
        else
        {
          newScalars->SetTuple1(index,0);
        }

      }
      
    }
  }




 /* for (int i=0;i<volumeData->GetNumberOfPoints();i++)
  {
    if (automaticData->GetPointData()->GetScalars()->GetTuple1(i) == 255 || spOutputRegionGrowing->GetPointData()->GetScalars()->GetTuple1(i) == 255)
    {
      newScalars->SetTuple1(i,255);
    }
    else
    {
      newScalars->SetTuple1(i,0);
    }
  }*/
  //////////////////////////////////////////////////////////////////////////

  eUpdate.SetArg(85);
  this->GetVME()->ForwardUpEvent(&eUpdate);

  if (volumeData->IsA("vtkStructuredPoints"))
  {
    m_RegionGrowingSP->CopyStructure(vtkStructuredPoints::SafeDownCast(volumeData));
    m_RegionGrowingSP->GetPointData()->AddArray(newScalars);
    m_RegionGrowingSP->GetPointData()->SetActiveScalars("SCALARS");
    m_RegionGrowingSP->SetScalarTypeToUnsignedChar();
    m_RegionGrowingSP->Update();
    
    m_SP->DeepCopy(m_RegionGrowingSP);
    m_SP->Update();
  }
  else if (volumeData->IsA("vtkRectilinearGrid"))
  {
    m_RegionGrowingRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(volumeData));
    m_RegionGrowingRG->GetPointData()->AddArray(newScalars);
    m_RegionGrowingRG->GetPointData()->SetActiveScalars("SCALARS");
    m_RegionGrowingRG->Update();

    m_RG->DeepCopy(m_RegionGrowingRG);
    m_RG->Update();
  }

  eUpdate.SetArg(100);
  this->GetVME()->ForwardUpEvent(&eUpdate);

  mafEvent eHideProgress(this,PROGRESSBAR_HIDE);
  this->GetVME()->ForwardUpEvent(&eHideProgress);  

}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::PreExecute()
//------------------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(m_Volume);

  if(vol)
  {
    vol->GetOutput()->Update();
    vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
    if(volumeData)
    {
      if (m_ChangedAutomaticData && CheckNumberOfThresholds())
      {
        ApplyAutomaticSegmentation();
      }
      if ((m_ChangedAutomaticData && CheckNumberOfThresholds()) || m_ChangedRegionGrowingData)
      {
        ApplyRegionGrowingSegmentation();
      }
      if ((m_ChangedAutomaticData && CheckNumberOfThresholds()) || m_ChangedRegionGrowingData || m_ChangedManualData)
      {
        ApplyManualSegmentation();
      }
      if ((m_ChangedAutomaticData && CheckNumberOfThresholds()) || m_ChangedRegionGrowingData || m_ChangedManualData || m_ChangedRefinementData)
      {
        ApplyRefinementSegmentation();
      }

      m_ChangedAutomaticData = false;
      m_ChangedManualData = false;
      m_ChangedRefinementData = false;
      m_ChangedRegionGrowingData = false;
    }
  }
  else
  {
    SetInput(NULL);
  }
}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::Execute()
//------------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::AddRange(int startSlice,int endSlice,double threshold, double upperThreshold)
//----------------------------------------------------------------------------
{
  if (!this->GetVME()->IsDataAvailable())
  {
    return MAF_ERROR;
  }

  if (AutomaticCheckRange(startSlice,endSlice) == MAF_ERROR)
  {
    return MAF_ERROR;
  }
  
  int *range = new int[2];
  range[0] = startSlice;
  range[1] = endSlice;
  m_AutomaticSegmentationRanges.push_back(range);
  m_AutomaticSegmentationThresholds.push_back(threshold);
  m_AutomaticSegmentationUpperThresholds.push_back(upperThreshold);

  m_ChangedAutomaticData = true;
  Modified();
  return MAF_OK;
}
//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold)
//----------------------------------------------------------------------------
{
  if (index<0 || index+1>m_AutomaticSegmentationRanges.size())
  {
    return MAF_ERROR;
  }

  startSlice = m_AutomaticSegmentationRanges[index][0];
  endSlice = m_AutomaticSegmentationRanges[index][1];
  threshold = m_AutomaticSegmentationThresholds[index];

  return MAF_OK;
}

//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold, double &upperThreshold)
//----------------------------------------------------------------------------
{
  if (index<0 || index+1>m_AutomaticSegmentationRanges.size())
  {
    return MAF_ERROR;
  }

  startSlice = m_AutomaticSegmentationRanges[index][0];
  endSlice = m_AutomaticSegmentationRanges[index][1];
  threshold = m_AutomaticSegmentationThresholds[index];
  upperThreshold = m_AutomaticSegmentationUpperThresholds[index];

  return MAF_OK;
}


//------------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::SetVolume(mafNode *volume)
//------------------------------------------------------------------------------
{
  m_Volume = volume;
  
  if (m_Volume == NULL || !mafVME::SafeDownCast(m_Volume)->GetOutput()->GetVTKData() || 
    ( !mafVME::SafeDownCast(m_Volume)->GetOutput()->GetVTKData()->IsA("vtkStructuredPoints")
    && !mafVME::SafeDownCast(m_Volume)->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid") )
    )
  {
    return MAF_ERROR;
  }

  if (mafVME::SafeDownCast(m_Volume)->GetOutput()->GetVTKData() && mafVME::SafeDownCast(m_Volume)->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))
  {
    SetInput(m_RG);
    Modified();
  }
  else if (mafVME::SafeDownCast(m_Volume)->GetOutput()->GetVTKData() && mafVME::SafeDownCast(m_Volume)->GetOutput()->GetVTKData()->IsA("vtkStructuredPoints"))
  {
    SetInput(m_SP);
    Modified();
  }

  m_ChangedAutomaticData = true;

  return MAF_OK;
}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::SetManualVolumeMask(mafNode *volume)
//------------------------------------------------------------------------------
{
  m_ManualVolumeMask = volume;
  
  m_ChangedManualData = true;
  Modified();
}
//------------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::SetRefinementVolumeMask(mafNode *volume)
//------------------------------------------------------------------------------
{
  m_RefinementVolumeMask = volume;

  m_ChangedRefinementData = true;
  Modified();
}
//------------------------------------------------------------------------------
vtkDataSet *medDataPipeCustomSegmentationVolume::GetAutomaticOutput()
//------------------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(m_Volume);
  if(vol)
  {
    vol->GetOutput()->Update();
    vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
    if(volumeData)
    {
      volumeData->Update();
      if(volumeData->IsA("vtkRectilinearGrid"))
      {
        return m_AutomaticRG;
      }
      else if (volumeData->IsA("vtkImageData"))
      {
        return m_AutomaticSP;
      }
    }
  }

  return NULL;
}
//------------------------------------------------------------------------------
vtkDataSet *medDataPipeCustomSegmentationVolume::GetManualOutput()
//------------------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(m_Volume);
  if(vol)
  {
    vol->GetOutput()->Update();
    vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
    if(volumeData)
    {
      volumeData->Update();
      if(volumeData->IsA("vtkRectilinearGrid"))
      {
        return m_ManualRG;
      }
      else if (volumeData->IsA("vtkImageData"))
      {
        return m_ManualSP;
      }
    }
  }

  return NULL;
}
//------------------------------------------------------------------------------
vtkDataSet *medDataPipeCustomSegmentationVolume::GetRefinementOutput()
//------------------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(m_Volume);
  if(vol)
  {
    vol->GetOutput()->Update();
    vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
    if(volumeData)
    {
      volumeData->Update();
      if(volumeData->IsA("vtkRectilinearGrid"))
      {
        return m_RefinementRG;
      }
      else if (volumeData->IsA("vtkImageData"))
      {
        return m_RefinementSP;
      }
    }
  }

  return NULL;
}
//------------------------------------------------------------------------------
vtkDataSet *medDataPipeCustomSegmentationVolume::GetRegionGrowingOutput()
//------------------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(m_Volume);
  if(vol)
  {
    vol->GetOutput()->Update();
    vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
    if(volumeData)
    {
      volumeData->Update();
      if(volumeData->IsA("vtkRectilinearGrid"))
      {
        return m_RegionGrowingRG;
      }
      else if (volumeData->IsA("vtkImageData"))
      {
        return m_RegionGrowingSP;
      }
    }
  }

  return NULL;
}
//----------------------------------------------------------------------------
bool medDataPipeCustomSegmentationVolume::CheckNumberOfThresholds()
//----------------------------------------------------------------------------
{
  //////////////////////////////////////////////////////////////////////////
  //Check that all slices have a threshold

  if (m_AutomaticSegmentationThresholdModality == medVMESegmentationVolume::GLOBAL)
  {
    return true;
  }

  mafVME *vol = mafVME::SafeDownCast(m_Volume);

  if(vol)
  {
    vol->GetOutput()->Update();
    vtkDataSet *volumeData = vol->GetOutput()->GetVTKData();
    if(volumeData)
    {
      volumeData->Update();

      int volumeDimensions[3];
      if(volumeData->IsA("vtkRectilinearGrid")) {
        vtkRectilinearGrid *rectilinearGrid=vtkRectilinearGrid::SafeDownCast(volumeData);
        rectilinearGrid->GetDimensions(volumeDimensions);
      }
      else if (volumeData->IsA("vtkStructuredPoints")) {
        vtkStructuredPoints *sp=vtkStructuredPoints::SafeDownCast(volumeData);
        sp->GetDimensions(volumeDimensions);
      }

      for (int i=0;i<volumeDimensions[2];i++)
      {
        bool found = false;
        for (int j=0;j<m_AutomaticSegmentationRanges.size();j++)
        {
          if (i>=(m_AutomaticSegmentationRanges[j][0]) && i<=(m_AutomaticSegmentationRanges[j][1]))
          {
            found = true;
            break;
          }
        }
        if (!found)
        {
          mafLogMessage("Slice %d° hasn't a threshold",i+1);
          return false;
        }
      }

      return true;
    }
  }
  else
  {
    return false;
  }
  //////////////////////////////////////////////////////////////////////////
}
//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::UpdateRange(int index,int startSlice, int endSlice, double threshold, double upperThreshold)
//----------------------------------------------------------------------------
{
  if (m_AutomaticSegmentationRanges.size()==0 || index<0 || index+1>m_AutomaticSegmentationRanges.size() || AutomaticCheckRange(startSlice,endSlice,index) == MAF_ERROR)
  {
    return MAF_ERROR;
  }
  m_AutomaticSegmentationRanges[index][0] = startSlice;
  m_AutomaticSegmentationRanges[index][1] = endSlice;
  m_AutomaticSegmentationThresholds[index] = threshold;
  m_AutomaticSegmentationUpperThresholds[index] = upperThreshold;


  m_ChangedAutomaticData = true;
  Modified();
  return MAF_OK;
}
//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::RemoveAllRanges()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    delete []m_AutomaticSegmentationRanges[i];
  }
  m_AutomaticSegmentationRanges.clear();
  m_AutomaticSegmentationThresholds.clear();
  m_AutomaticSegmentationUpperThresholds.clear();

  m_ChangedAutomaticData = true;
  Modified();

  return MAF_OK;
}
//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::RemoveAllSeeds()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_RegionGrowingSeeds.size();i++)
  {
    delete []m_RegionGrowingSeeds[i];
  }
  m_RegionGrowingSeeds.clear();

  m_ChangedRegionGrowingData = true;
  Modified();

  return MAF_OK;
}
//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::DeleteSeed(int index)
//----------------------------------------------------------------------------
{
  if (m_RegionGrowingSeeds.size()==0 || index<0 || index+1>(m_RegionGrowingSeeds.size()))
  {
    return MAF_ERROR;
  }

  for (int i=0,j=0;i<m_RegionGrowingSeeds.size();i++)
  {
    if (i != index)
    {
      m_RegionGrowingSeeds[j][0] = m_RegionGrowingSeeds[i][0];
      m_RegionGrowingSeeds[j][1] = m_RegionGrowingSeeds[i][1];
      m_RegionGrowingSeeds[j][2] = m_RegionGrowingSeeds[i][2];
      j++;
    }
  }

  delete []m_RegionGrowingSeeds[m_RegionGrowingSeeds.size()-1];
  m_RegionGrowingSeeds.pop_back();

  m_ChangedRegionGrowingData = true;
  Modified();
  return MAF_OK;
}
//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::DeleteRange(int index)
//----------------------------------------------------------------------------
{
  if (m_AutomaticSegmentationRanges.size()==0 || index<0 || index+1>m_AutomaticSegmentationRanges.size())
  {
    return MAF_ERROR;
  }

  for (int i=0,j=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    if (i != index)
    {
      m_AutomaticSegmentationRanges[j][0] = m_AutomaticSegmentationRanges[i][0];
      m_AutomaticSegmentationRanges[j][1] = m_AutomaticSegmentationRanges[i][1];
      m_AutomaticSegmentationThresholds[j] = m_AutomaticSegmentationThresholds[i];
      m_AutomaticSegmentationUpperThresholds[j] = m_AutomaticSegmentationUpperThresholds[i];
      j++;
    }
  }

  delete []m_AutomaticSegmentationRanges[m_AutomaticSegmentationRanges.size()-1];
  m_AutomaticSegmentationRanges.pop_back();
  m_AutomaticSegmentationThresholds.pop_back();
  m_AutomaticSegmentationUpperThresholds.pop_back();


  m_ChangedAutomaticData = true;
  Modified();
  return MAF_OK;
}
//------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::SetAutomaticSegmentationGlobalThreshold(double threshold, double upperThreshold)
//------------------------------------------------------------------------
{
  m_AutomaticSegmentationGlobalThreshold = threshold;
  m_AutomaticSegmentationGlobalUpperThreshold = upperThreshold;
  m_ChangedAutomaticData = true;
  Modified();
}
//------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::AddSeed(int seed[3])
//------------------------------------------------------------------------
{
  int *newSeed = new int[3];
  newSeed[0] = seed[0];
  newSeed[1] = seed[1];
  newSeed[2] = seed[2];

  m_RegionGrowingSeeds.push_back(newSeed);

  m_ChangedRegionGrowingData = true;
  Modified();

  return MAF_OK;
}
//----------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::GetSeed(int index,int seed[3])
//----------------------------------------------------------------------------
{
 
  if (index<0 || index+1>m_RegionGrowingSeeds.size())
  {
    return MAF_ERROR;
  }
  seed[0] = m_RegionGrowingSeeds[index][0];
  seed[1] = m_RegionGrowingSeeds[index][1];
  seed[2] = m_RegionGrowingSeeds[index][2];

  return MAF_OK;
}
//------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::SetRegionGrowingLowerThreshold(double value)
//------------------------------------------------------------------------
{
  m_RegionGrowingLowerThreshold = value;

  m_ChangedRegionGrowingData = true;
  Modified();
}
//------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::SetRegionGrowingUpperThreshold(double value)
//------------------------------------------------------------------------
{
  m_RegionGrowingUpperThreshold = value;

  m_ChangedRegionGrowingData = true;
  Modified();
}
//------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::AutomaticCheckRange(int startSlice,int endSlice,int indexToExclude)
//------------------------------------------------------------------------
{
  int minInt,maxInt;
  minInt = startSlice;
  maxInt = endSlice;

  if (minInt > maxInt)
  {
    mafLogMessage("Lower slice index is higher than upper slice index");
    return MAF_ERROR;
  }

  //Check if any range overlaps with the actual
  for (int i=0;i<m_AutomaticSegmentationRanges.size();i++)
  {
    if (indexToExclude != i)
    {
	    if (m_AutomaticSegmentationRanges[i][1] == maxInt || m_AutomaticSegmentationRanges[i][0] == minInt)
	    {
	      mafLogMessage("Slice already inserted in the automatic segmentation");
	      return MAF_ERROR;
	    }
	
	    if (m_AutomaticSegmentationRanges[i][1] >= minInt && m_AutomaticSegmentationRanges[i][1] <= maxInt)
	    {
	      mafLogMessage("Overlaps of slices range");
	      return MAF_ERROR;
	    }
	
	    if (m_AutomaticSegmentationRanges[i][0] >= minInt && m_AutomaticSegmentationRanges[i][0] <= maxInt)
	    {
	      mafLogMessage("Overlaps of slices range");
	      return MAF_ERROR;
	    }
    }
  }

  return MAF_OK;
}

//------------------------------------------------------------------------
void medDataPipeCustomSegmentationVolume::SetRegionGrowingSliceRange(int startSlice, int endSlice)
//------------------------------------------------------------------------
{
  m_RegionGrowingStartSlice = startSlice;
  m_RegionGrowingEndSlice = endSlice;
}

//------------------------------------------------------------------------
int medDataPipeCustomSegmentationVolume::GetRegionGrowingSliceRange(int &startSlice, int &endSlice)
//------------------------------------------------------------------------
{
  startSlice = m_RegionGrowingStartSlice;
  endSlice = m_RegionGrowingEndSlice;

  return MAF_OK;
}