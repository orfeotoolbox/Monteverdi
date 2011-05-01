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
#ifndef __otbPoincareSphereWidget_h
#define __otbPoincareSphereWidget_h

#include <FL/gl.h>
#include <FL/Fl.H>
#include "FL/Fl_Gl_Window.H"
#include <FL/glut.H>
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkMacro.h"

namespace otb
{
/** \class PoincareSphereWidget
 *  \brief
 *
 */
class PoincareSphereWidget
      : public itk::Object, public Fl_Gl_Window
{
public:
  /** Standard class typedefs */
  typedef PoincareSphereWidget Self;
  typedef itk::Object Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Runtime information */
  itkTypeMacro(PoincareSphereWidget, Object);


  /** Initialize the widget */
  virtual void Init(int x, int y, int w, int h, const char * l);
  virtual int handle(int event);

  /*    itkSetMacro(PsiI, double);
      itkSetMacro(PsiR, double);
      itkSetMacro(KhiI, double);
      itkSetMacro(KhiR, double); */
  void SetPsiI(double val)
  {
    m_PsiI=2*val;
  }
  void SetKhiI(double val)
  {
    m_KhiI=2*val;
  }
  void SetPsiR(double val)
  {
    m_PsiR=2*val;
  }
  void SetKhiR(double val)
  {
    m_KhiR=2*val;
  }

protected:
  /** Constructor */
  PoincareSphereWidget();
  /** Destructor */
  ~PoincareSphereWidget();

  /** Draw the widget */
  virtual void draw(void);

  void InitGL();
  void ToggleLight();
  void DrawSphere();
  void DrawPlans();
  void DrawAxis();
  void DrawTorus();
  void DrawVectors();

  void RenderSphere(double radius, unsigned int thetaResolution, unsigned int khiResolution);
  void RenderCircle(double radius, unsigned int resolution);

private:
  PoincareSphereWidget(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  unsigned int m_Resolution;
  unsigned int m_GridSize;
  double       m_RotateAngle1;
  double       m_RotateAngle2;
  int          m_ClickedX;
  int          m_ClickedY;
  double       m_PsiI;
  double       m_KhiI;
  double       m_PsiR;
  double       m_KhiR;


};
} // end namespace otb
#endif
