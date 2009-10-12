/*=========================================================================

Program:   ORFEO Toolbox
Language:  C++
Date:      $Date$
Version:   $Revision$


Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbMonteverdiController_h
#define __otbMonteverdiController_h

#include "otbMonteverdiControllerInterface.h"
#include "otbMonteverdiViewGUI.h"



namespace otb
{
class ITK_EXPORT MonteverdiController
      : public MonteverdiControllerInterface
{
public:
  /** Standard class typedefs */
  typedef MonteverdiController          Self;
  typedef MonteverdiControllerInterface Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  typedef MonteverdiViewGUI                 ViewType;
  typedef ViewType::Pointer                 ViewPointerType;


  /** Standard type macros */
  itkTypeMacro(MonteverdiController,Superclass);
  itkNewMacro(Self);

  itkSetObjectMacro(View,ViewType);

  /** User actions */
  virtual void CreateModuleByKey(const char * modulekey);

  /** Start module by instance id */
  virtual void StartModuleByInstanceId(const std::string& id);

  /** Add a module connection */
  virtual void AddModuleConnection(const std::string& src, const std::string& outKey, const std::string & dest, const std::string & inKey);

  /** Change module InstanceId (ie. tree label) */
  virtual int ChangeInstanceId( const std::string & oldInstanceId,  const std::string & newInstanceId );

  /** Starts caching */
  virtual void StartCaching(const std::string & id, const std::string & key);

protected:
  /** Constructor */
  MonteverdiController(){};
  /** Destructor */
  virtual ~MonteverdiController(){};

private:
  MonteverdiController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  ViewPointerType m_View;




};
} //end namespace otb

#endif
