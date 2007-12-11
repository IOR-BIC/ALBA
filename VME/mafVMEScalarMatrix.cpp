/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEScalarMatrix.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:22:56 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafVMEScalarMatrix.h"
#include "mmgGui.h"

#include "mafStorageElement.h"
#include "mafVME.h"
#include "mafVMEOutputScalarMatrix.h"

#include "mafScalarMatrixInterpolator.h"
#include "mafDataVector.h"
#include "mafVMEItemScalarMatrix.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEScalarMatrix)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEScalarMatrix::mafVMEScalarMatrix()
//-------------------------------------------------------------------------
{
  m_DataVector = mafDataVector::New();
  m_DataVector->SetItemTypeName(mafVMEItemScalarMatrix::GetStaticTypeName());  
  m_DataVector->SetListener(this);
  SetDataPipe(mafScalarMatrixInterpolator::New()); // interpolator data pipe

  m_ScalarArrayOrientationInMatrix = ROWS;
  m_Xtype = USE_TIME;
  m_Ytype = USE_PROGRESS_NUMBER;
  m_Ztype = USE_SCALAR;
  m_XID = 0;
  m_YID = 0;
  m_ZID = 0;
  m_ActiveScalar = -1;
}

//-------------------------------------------------------------------------
mafVMEScalarMatrix::~mafVMEScalarMatrix()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEScalarMatrix *scalar = mafVMEScalarMatrix::SafeDownCast(a);
    this->SetTypeForXCoordinates(scalar->GetTypeForXCoordinates());
    this->SetTypeForYCoordinates(scalar->GetTypeForYCoordinates());
    this->SetTypeForZCoordinates(scalar->GetTypeForZCoordinates());
    this->SetScalarArrayOrientation(scalar->GetScalarArrayOrientation());
    this->SetScalarIdForXCoordinate(scalar->GetScalarIdForXCoordinate());
    this->SetScalarIdForYCoordinate(scalar->GetScalarIdForYCoordinate());
    this->SetScalarIdForZCoordinate(scalar->GetScalarIdForZCoordinate());
    this->SetActiveScalarOnGeometry(scalar->GetActiveScalarOnGeometry());
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMEScalarMatrix::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    mafVMEScalarMatrix *scalar = mafVMEScalarMatrix::SafeDownCast(vme);
    ret = this->m_Xtype == scalar->GetTypeForXCoordinates() && \
          this->m_Ytype == scalar->GetTypeForYCoordinates() && \
          this->m_Ztype == scalar->GetTypeForZCoordinates() && \
          this->m_XID == scalar->GetScalarIdForXCoordinate() && \
          this->m_YID == scalar->GetScalarIdForYCoordinate() && \
          this->m_ZID == scalar->GetScalarIdForZCoordinate() && \
          this->m_ActiveScalar == scalar->GetActiveScalarOnGeometry() && \
          this->m_ScalarArrayOrientationInMatrix == scalar->GetScalarArrayOrientation();
  }
  return ret;
}
//-------------------------------------------------------------------------
mafVMEOutput *mafVMEScalarMatrix::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputScalarMatrix::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int mafVMEScalarMatrix::SetData(vnl_matrix<double> &data, mafTimeStamp t)
//-------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemScalarMatrix> item;
  item->SetData(data);
  item->SetTimeStamp(t);
  GetDataVector()->InsertItem(item);

  return MAF_OK;
}
//-------------------------------------------------------------------------
mmgGui* mafVMEScalarMatrix::CreateGui()
//-------------------------------------------------------------------------
{
  wxString type_array[3] = {_("time"), _("progress number"), _("scalar ID")};
  
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  m_Gui->Label(_("Parameters used for"));
  m_Gui->Label(_("VTK representation"));
  m_Gui->Divider(2);
  m_Gui->Label(_("x coord"));
  m_Gui->Combo(ID_TYPE_FOR_X,_("type"),&m_Xtype,3,type_array);
  m_Gui->Integer(ID_SCALAR_FOR_X,_("scalar id"),&m_XID,0,MAXINT,_("Scalar ID associated to the X coordinate \nof the VTK representation"));
  m_Gui->Label(_("y coord"));
  m_Gui->Combo(ID_TYPE_FOR_Y,_("type"),&m_Ytype,3,type_array);
  m_Gui->Integer(ID_SCALAR_FOR_Y,_("scalar id"),&m_YID,0,MAXINT,_("Scalar ID associated to the Y coordinate \nof the VTK representation"));
  m_Gui->Label(_("z coord"));
  m_Gui->Combo(ID_TYPE_FOR_Z,_("type"),&m_Ztype,3,type_array);
  m_Gui->Integer(ID_SCALAR_FOR_Z,_("scalar id"),&m_ZID,0,MAXINT,_("Scalar ID associated to the Z coordinate \nof the VTK representation"));
  m_Gui->Divider();
  m_Gui->Label(_("scalar for geometry."));
  m_Gui->Integer(ID_ACTIVE_SCALAR,_("scalat id"),&m_ActiveScalar,-1,MAXINT,_("Active scalar to use with geometry;\n -1 means no scalar active."));
  m_Gui->Divider();

  EnableWidgets();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::EnableWidgets()
//-------------------------------------------------------------------------
{
  m_Gui->Enable(ID_SCALAR_FOR_X, m_Xtype == USE_SCALAR);
  m_Gui->Enable(ID_SCALAR_FOR_Y, m_Ytype == USE_SCALAR);
  m_Gui->Enable(ID_SCALAR_FOR_Z, m_Ztype == USE_SCALAR);
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_SCALAR_FOR_X:
      case ID_SCALAR_FOR_Y:
      case ID_SCALAR_FOR_Z:
      case ID_ACTIVE_SCALAR:
      break;
      case ID_TYPE_FOR_X:
      case ID_TYPE_FOR_Y:
      case ID_TYPE_FOR_Z:
        EnableWidgets();
      break;
      default:
        mafNode::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-----------------------------------------------------------------------
int mafVMEScalarMatrix::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    if (parent->StoreInteger("XID",m_XID)==MAF_OK &&
        parent->StoreInteger("YID",m_YID)==MAF_OK &&
        parent->StoreInteger("ZID",m_ZID)==MAF_OK &&
        parent->StoreInteger("Xtype",m_Xtype)==MAF_OK &&
        parent->StoreInteger("Ytype",m_Ytype)==MAF_OK &&
        parent->StoreInteger("Ztype",m_Ztype)==MAF_OK &&
        parent->StoreInteger("ActiveScalar",m_ActiveScalar)==MAF_OK &&
        parent->StoreInteger("ScalarArrayOrientationInMatrix",m_ScalarArrayOrientationInMatrix)==MAF_OK)
      return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMEScalarMatrix::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    if (node->RestoreInteger("XID",m_XID) == MAF_OK &&
        node->RestoreInteger("YID",m_YID) == MAF_OK &&
        node->RestoreInteger("ZID",m_ZID) == MAF_OK &&
        node->RestoreInteger("Xtype",m_Xtype) == MAF_OK &&
        node->RestoreInteger("Ytype",m_Ytype) == MAF_OK &&
        node->RestoreInteger("Ztype",m_Ztype) == MAF_OK &&
        node->RestoreInteger("ActiveScalar",m_ActiveScalar)==MAF_OK &&
        node->RestoreInteger("ScalarArrayOrientationInMatrix",m_ScalarArrayOrientationInMatrix) == MAF_OK)
    {
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetScalarArrayOrientation(int o)
//-------------------------------------------------------------------------
{
  m_ScalarArrayOrientationInMatrix = o;
  m_ScalarArrayOrientationInMatrix = m_ScalarArrayOrientationInMatrix < ROWS ? ROWS : m_ScalarArrayOrientationInMatrix;
  m_ScalarArrayOrientationInMatrix = m_ScalarArrayOrientationInMatrix > COLUMNS ? COLUMNS : m_ScalarArrayOrientationInMatrix;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetActiveScalarOnGeometry(int scalar)
//-------------------------------------------------------------------------
{
  if (scalar == m_ActiveScalar)
  {
    return;
  }
  m_ActiveScalar = scalar < -1 ? -1 : scalar;
  Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::GetActiveScalarOnGeometry()
//-------------------------------------------------------------------------
{
  return m_ActiveScalar;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetScalarIdForXCoordinate(int id)
//-------------------------------------------------------------------------
{
  if (m_XID == id)
  {
    return;
  }
  m_XID = id;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::GetScalarIdForXCoordinate()
//-------------------------------------------------------------------------
{
  mafVMEOutputScalarMatrix *output = GetScalarOutput();
  vnl_matrix<double> data = output->GetScalarData();
  if (!data.empty())
  {
    if (m_ScalarArrayOrientationInMatrix == COLUMNS)
    {
      m_XID = m_XID > data.columns() ? data.columns() : m_XID;
    }
    else
    {
      m_XID = m_XID > data.rows() ? data.rows() : m_XID;
    }
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
  return m_XID;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetTypeForXCoordinates(int t)
//-------------------------------------------------------------------------
{
  if (m_Xtype == t)
  {
    return;
  }
  m_Xtype = t;
  m_Xtype = m_Xtype < USE_TIME ? USE_TIME : m_Xtype;
  m_Xtype = m_Xtype > USE_SCALAR ? USE_SCALAR : m_Xtype;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetScalarIdForYCoordinate(int id)
//-------------------------------------------------------------------------
{
  if (m_YID == id)
  {
    return;
  }
  m_YID = id;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::GetScalarIdForYCoordinate()
//-------------------------------------------------------------------------
{
  mafVMEOutputScalarMatrix *output = GetScalarOutput();
  vnl_matrix<double> data = output->GetScalarData();
  if (!data.empty())
  {
    if (m_ScalarArrayOrientationInMatrix == COLUMNS)
    {
      m_YID = m_YID > data.columns() ? data.columns() : m_YID;
    }
    else
    {
      m_YID = m_YID > data.rows() ? data.rows() : m_YID;
    }
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
  return m_YID;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetTypeForYCoordinates(int t)
//-------------------------------------------------------------------------
{
  if (m_Ytype == t)
  {
    return;
  }
  m_Ytype = t;
  m_Ytype = m_Ytype < USE_TIME ? USE_TIME : m_Ytype;
  m_Ytype = m_Ytype > USE_SCALAR ? USE_SCALAR : m_Ytype;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetScalarIdForZCoordinate(int id)
//-------------------------------------------------------------------------
{
  if (m_ZID == id)
  {
    return;
  }
  m_ZID = id;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::GetScalarIdForZCoordinate()
//-------------------------------------------------------------------------
{
  mafVMEOutputScalarMatrix *output = GetScalarOutput();
  vnl_matrix<double> data = output->GetScalarData();
  if (!data.empty())
  {
    if (m_ScalarArrayOrientationInMatrix == COLUMNS)
    {
      m_ZID = m_ZID > data.columns() ? data.columns() : m_ZID;
    }
    else
    {
      m_ZID = m_ZID > data.rows() ? data.rows() : m_ZID;
    }
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
  return m_ZID;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetTypeForZCoordinates(int t)
//-------------------------------------------------------------------------
{
  if (m_Ztype == t)
  {
    return;
  }
  m_Ztype = t;
  m_Ztype = m_Ztype < USE_TIME ? USE_TIME : m_Ztype;
  m_Ztype = m_Ztype > USE_SCALAR ? USE_SCALAR : m_Ztype;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-----------------------------------------------------------------------
void mafVMEScalarMatrix::InternalPreUpdate()
//-----------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
}
//-----------------------------------------------------------------------
void mafVMEScalarMatrix::SetTimeStamp(mafTimeStamp t)
//-----------------------------------------------------------------------
{
  t = t < 0 ? 0 : t;
  bool update_vtk_data = t != m_CurrentTime;
  if (update_vtk_data)
  {
    Superclass::SetTimeStamp(t);

#ifdef MAF_USE_VTK
    GetScalarOutput()->UpdateVTKRepresentation();
#endif
  }
}
//-----------------------------------------------------------------------
void mafVMEScalarMatrix::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  os << indent << _("Scalar Array orientation in matrix: ");
  os << (m_ScalarArrayOrientationInMatrix == 0) ? _("Rows") : _("Columns");
  os << _("\n");

  mafString type_scalar[3] = {_("Time"), _("Progress numbers"), _("Scalars")};

  os << indent << _("Scalar type for X coordinate: ");
  os << type_scalar[m_Xtype];
  os << _("\n");

  if (m_Xtype == USE_SCALAR)
  {
    os << indent << _("Scalar ID for X coordinate: ");
    os << m_XID;
    os << _("\n");
  }

  os << indent << _("Scalar type for Y coordinate: ");
  os << type_scalar[m_Ytype];
  os << _("\n");

  if (m_Ytype == USE_SCALAR)
  {
    os << indent << _("Scalar ID for Y coordinate: ");
    os << m_YID;
    os << _("\n");
  }

  os << indent << _("Scalar type for Z coordinate: ");
  os << type_scalar[m_Ztype];
  os << _("\n");

  if (m_Ztype == USE_SCALAR)
  {
    os << indent << _("Scalar ID for Z coordinate: ");
    os << m_ZID;
    os << _("\n");
  }
}
//-------------------------------------------------------------------------
char** mafVMEScalarMatrix::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEScalar.xpm"
  return mafVMEScalar_xpm;
}
