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
#ifndef __otbGUIRectangle_h
#define __otbGUIRectangle_h

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Tooltip.H>
#include <FL/x.H>
#include <FL/Enumerations.H>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include "otbMacro.h"
#include "itkMacro.h"

// VC6/7 has troubel with min/max which are still defined in macros in Windows headers
#undef min
#undef max

namespace otb
{
class GUIRectangle: public Fl_Group
{
public:
  /** Standard typedefs */
  typedef GUIRectangle               Self;
  typedef Fl_Group                   Superclass;
  typedef std::vector<double>        VectorType;
  typedef std::vector<VectorType>    VectorVectorType;
  typedef std::vector<int>           IntVectorType;
  typedef std::vector<IntVectorType> VectorIntVectorType;

  virtual void draw()
  {
    Superclass::draw();

    if (m_PointList.size() == 4 && m_BoundingBox.size() == 4)
      {
      m_PrintedPointList.clear();
      m_PrintedExtentPointList.clear();
      m_PrintedBoundingBoxPointList.clear();

      int x = this->x();
      int y = this->y();
      int w = this->w();
      int h = this->h();

      VectorIntVectorType intVect = this->ConvertToInt(m_PointList);

      int maxSize =
        std::min(static_cast<int>(static_cast<double>(w) * 0.75), static_cast<int>(static_cast<double>(h) * 0.75));
      double geom = static_cast<double>(m_BoundingBox[2]) / static_cast<double>(m_BoundingBox[3]);

      int wfinal, hfinal;

      if (geom > 1.0)
        {
        wfinal  = maxSize;
        hfinal =  static_cast<int>(static_cast<double>(maxSize) / geom);
        }
      else
        {
        hfinal = maxSize;
        wfinal = static_cast<int>(static_cast<double>(maxSize) * geom);
        }

      double a = static_cast<double>(wfinal) / static_cast<double>(m_BoundingBox[2]);
      double b = static_cast<double>(hfinal) / static_cast<double>(m_BoundingBox[3]);
      double r = std::min(a, b);
      int    xfinal = x + static_cast<int>(static_cast<double>(w - wfinal) / 2);
      int    yfinal = y + static_cast<int>(static_cast<double>(h - hfinal) / 2);
      // Compute image ortho bounding box and print it
      intVect[0][0] = static_cast<int>(static_cast<double>(intVect[0][0] - m_BoundingBox[0]) * r) + xfinal;
      intVect[0][1] = static_cast<int>(static_cast<double>(intVect[0][1] - m_BoundingBox[1]) * r) + yfinal;
      intVect[1][0] = static_cast<int>(static_cast<double>(intVect[1][0] - m_BoundingBox[0]) * r) + xfinal;
      intVect[1][1] = static_cast<int>(static_cast<double>(intVect[1][1] - m_BoundingBox[1]) * r) + yfinal;
      intVect[2][0] = static_cast<int>(static_cast<double>(intVect[2][0] - m_BoundingBox[0]) * r) + xfinal;
      intVect[2][1] = static_cast<int>(static_cast<double>(intVect[2][1] - m_BoundingBox[1]) * r) + yfinal;
      intVect[3][0] = static_cast<int>(static_cast<double>(intVect[3][0] - m_BoundingBox[0]) * r) + xfinal;
      intVect[3][1] = static_cast<int>(static_cast<double>(intVect[3][1] - m_BoundingBox[1]) * r) + yfinal;

      if (m_PointList[0][1] > 0 && m_PointList[1][1] > 0 && m_PointList[2][1] > 0 && m_PointList[3][1] > 0)
        {
        intVect[0][1] = yfinal + hfinal - (intVect[0][1] - yfinal) - 1;
        intVect[1][1] = yfinal + hfinal - (intVect[1][1] - yfinal) - 1;
        intVect[2][1] = yfinal + hfinal - (intVect[2][1] - yfinal) - 1;
        intVect[3][1] = yfinal + hfinal - (intVect[3][1] - yfinal) - 1;
        }

      fl_rectf(xfinal, yfinal, wfinal, hfinal, FL_BLACK);

      // Construct printed boudingbox points list
      IntVectorType vect(2, 0);
      vect[0] = xfinal;
      vect[1] = yfinal;
      m_PrintedBoundingBoxPointList.push_back(vect);
      vect[0] += wfinal;
      m_PrintedBoundingBoxPointList.push_back(vect);
      vect[1] += hfinal;
      m_PrintedBoundingBoxPointList.push_back(vect);
      vect[0] -= wfinal;
      m_PrintedBoundingBoxPointList.push_back(vect);

      fl_color(FL_WHITE);
      fl_polygon(intVect[0][0],
                 intVect[0][1],
                 intVect[1][0],
                 intVect[1][1],
                 intVect[2][0],
                 intVect[2][1],
                 intVect[3][0],
                 intVect[3][1]);

      for (unsigned int i = 0; i < intVect.size(); i++)
        {
        vect.clear();
        for (unsigned int j = 0; j < intVect[i].size(); j++)
          {
          vect.push_back(intVect[i][j]);
          }
        m_PrintedPointList.push_back(vect);
        }

      if (m_ExtentList.size() != 0)
        {
        VectorIntVectorType intVectExtent = this->ConvertToInt(m_ExtentList);

        // Compute image ortho bouding box and print it
        intVectExtent[0][0] =
          static_cast<int>(static_cast<double>(intVectExtent[0][0] - m_BoundingBox[0]) * r) + xfinal;
        intVectExtent[0][1] =
          static_cast<int>(static_cast<double>(intVectExtent[0][1] - m_BoundingBox[1]) * r) + yfinal;
        intVectExtent[1][0] =
          static_cast<int>(static_cast<double>(intVectExtent[1][0] - m_BoundingBox[0]) * r) + xfinal;
        intVectExtent[1][1] =
          static_cast<int>(static_cast<double>(intVectExtent[1][1] - m_BoundingBox[1]) * r) + yfinal;
        intVectExtent[2][0] =
          static_cast<int>(static_cast<double>(intVectExtent[2][0] - m_BoundingBox[0]) * r) + xfinal;
        intVectExtent[2][1] =
          static_cast<int>(static_cast<double>(intVectExtent[2][1] - m_BoundingBox[1]) * r) + yfinal;
        intVectExtent[3][0] =
          static_cast<int>(static_cast<double>(intVectExtent[3][0] - m_BoundingBox[0]) * r) + xfinal;
        intVectExtent[3][1] =
          static_cast<int>(static_cast<double>(intVectExtent[3][1] - m_BoundingBox[1]) * r) + yfinal;

        if (m_PointList[0][1] > 0 && m_PointList[1][1] > 0 && m_PointList[2][1] > 0 && m_PointList[3][1] > 0)
          {
          intVectExtent[0][1] = yfinal + hfinal - (intVectExtent[0][1] - yfinal) - 1;
          intVectExtent[1][1] = yfinal + hfinal - (intVectExtent[1][1] - yfinal) - 1;
          intVectExtent[2][1] = yfinal + hfinal - (intVectExtent[2][1] - yfinal) - 1;
          intVectExtent[3][1] = yfinal + hfinal - (intVectExtent[3][1] - yfinal) - 1;
          }

        intVectExtent[0][0] = std::max(x, intVectExtent[0][0]);
        intVectExtent[0][1] = std::max(y, intVectExtent[0][1]);
        intVectExtent[0][0] = std::min(x + w, intVectExtent[0][0]);
        intVectExtent[0][1] = std::min(y + h, intVectExtent[0][1]);

        intVectExtent[1][0] = std::max(x, intVectExtent[1][0]);
        intVectExtent[1][1] = std::max(y, intVectExtent[1][1]);
        intVectExtent[1][0] = std::min(x + w, intVectExtent[1][0]);
        intVectExtent[1][1] = std::min(y + h, intVectExtent[1][1]);

        intVectExtent[2][0] = std::max(x, intVectExtent[2][0]);
        intVectExtent[2][1] = std::max(y, intVectExtent[2][1]);
        intVectExtent[2][0] = std::min(x + w, intVectExtent[2][0]);
        intVectExtent[2][1] = std::min(y + h, intVectExtent[2][1]);

        intVectExtent[3][0] = std::max(x, intVectExtent[3][0]);
        intVectExtent[3][1] = std::max(y, intVectExtent[3][1]);
        intVectExtent[3][0] = std::min(x + w, intVectExtent[3][0]);
        intVectExtent[3][1] = std::min(y + h, intVectExtent[3][1]);

        fl_color(FL_GREEN);
        fl_line(intVectExtent[0][0],
                intVectExtent[0][1],
                intVectExtent[1][0],
                intVectExtent[1][1],
                intVectExtent[2][0],
                intVectExtent[2][1]);
        fl_line(intVectExtent[2][0],
                intVectExtent[2][1],
                intVectExtent[3][0],
                intVectExtent[3][1],
                intVectExtent[0][0],
                intVectExtent[0][1]);

        for (unsigned int i = 0; i < intVectExtent.size(); i++)
          {
          vect.clear();
          for (unsigned int j = 0; j < intVectExtent[i].size(); j++)
            {
            vect.push_back(intVectExtent[i][j]);
            }
          m_PrintedExtentPointList.push_back(vect);
          }
        }

      this->PrintPoints();
      }
  }

// Point List accessors
  void SetPointList(VectorVectorType list)
  {
    m_PointList = list;
  }
  void AddPointToList(VectorType point)
  {
    m_PointList.push_back(point);
  }
  void ClearPointList()
  {
    m_PointList.clear();
  }
  VectorVectorType GetPointList()
  {
    return m_PointList;
  }
// Long/Lag list accessors
  void SetLongLatList(VectorVectorType list)
  {
    m_LongLatList = list;
  }
  void AddPointToLatLongList(VectorType point)
  {
    m_LongLatList.push_back(point);
  }
  void ClearLongLatList()
  {
    m_LongLatList.clear();
  }
  VectorVectorType GetLongLatList()
  {
    return m_LongLatList;
  }
// Bouding box point list accessors
  void SetBoundingBox(IntVectorType list)
  {
    m_BoundingBox = list;
  }
  void AddEltToBoundingBox(int coord)
  {
    m_BoundingBox.push_back(coord);
  }
  void ClearBoundingBoxt()
  {
    m_BoundingBox.clear();
  }
  IntVectorType GetBoundingBox()
  {
    return m_BoundingBox;
  }
// Extent point list accessors
  void SetExtentList(VectorVectorType list)
  {
    m_ExtentList = list;
  }
  void AddPointToExtentList(VectorType point)
  {
    m_ExtentList.push_back(point);
  }
  void ClearExtentList()
  {
    m_ExtentList.clear();
  }
  VectorVectorType GetExtentList()
  {
    return m_ExtentList;
  }
// Extent long/lat point list accessors
  void SetExtentLongLatList(VectorVectorType list)
  {
    m_LongLatExtentList = list;
  }
  void AddPointToExtentLongLatList(VectorType point)
  {
    m_LongLatExtentList.push_back(point);
  }
  void ClearExtentLongLatList()
  {
    m_LongLatExtentList.clear();
  }
  VectorVectorType GetExtentLongLatList()
  {
    return m_LongLatExtentList;
  }

  IntVectorType GetXBoundingBoxIdList()
  {
    return m_XBoundingBoxIdList;
  }
  IntVectorType GetYBoundingBoxIdList()
  {
    return m_YBoundingBoxIdList;
  }

// Convert a vector of double vector
  VectorIntVectorType ConvertToInt(VectorVectorType dVect)
  {
    VectorIntVectorType vectVect;
    IntVectorType       vect;
    for (unsigned int i = 0; i < dVect.size(); i++)
      {
      vect.clear();
      for (unsigned int j = 0; j < dVect[i].size(); j++)
        {
        vect.push_back(std::abs(static_cast<int>(dVect[i][j])));
        }
      vectVect.push_back(vect);
      }

    return vectVect;
  }

// Clear the area
  void Clear()
  {
    m_PointList.clear();
    m_LongLatList.clear();
    m_ExtentList.clear();
    m_LongLatExtentList.clear();
    m_PrintedPointList.clear();
    m_PrintedExtentPointList.clear();
    m_BoundingBox.clear();
    m_PrintedBoundingBoxPointList.clear();
    m_XBoundingBoxIdList.clear();
    m_YBoundingBoxIdList.clear();

    this->redraw();
  }

// Compute the point list bounding box
  void ComputeBoundingBox()
  {
    if (m_PointList.size() != 0)
      {
      VectorIntVectorType intVect = this->ConvertToInt(m_PointList);

      m_XBoundingBoxIdList.clear();
      m_YBoundingBoxIdList.clear();
      m_BoundingBox.clear();
      m_XBoundingBoxIdList.resize(2, 0);
      m_YBoundingBoxIdList.resize(2, 0);

      int x1 = intVect[0][0];
      int y1 = intVect[0][1];
      int w1 = intVect[0][0];
      int h1 = intVect[0][1];

      for (unsigned int i = 1; i < intVect.size(); i++)
        {
        // Origins
        if (x1 > intVect[i][0])
          {
          x1 = intVect[i][0];
          m_XBoundingBoxIdList[0] = i;
          }
        if (y1 > intVect[i][1])
          {
          y1 = intVect[i][1];
          m_YBoundingBoxIdList[0] = i;
          }

        // Sizes
        if (w1 < intVect[i][0])
          {
          w1 = intVect[i][0];
          m_XBoundingBoxIdList[1] = i;
          }
        if (h1 < intVect[i][1])
          {
          h1 = intVect[i][1];
          m_YBoundingBoxIdList[1] = i;
          }
        }

      w1 = std::abs(w1 - x1);
      h1 = std::abs(h1 - y1);

      m_BoundingBox.push_back(x1);
      m_BoundingBox.push_back(y1);
      m_BoundingBox.push_back(w1);
      m_BoundingBox.push_back(h1);
      }
  }

  /** Print little squares around interesting points */
  void PrintPoints()
  {
    // Extent image
    fl_color(FL_RED);
    if (m_PrintedPointList.size() != 0)
      {
      for (unsigned int i = 0; i < m_PrintedPointList.size(); i++)
        {
        fl_rect(m_PrintedPointList[i][0] - 5, m_PrintedPointList[i][1] - 5, 11, 11);
        }
      }
    fl_rect(m_PrintedPointList[0][0] - 6, m_PrintedPointList[0][1] - 6, 13, 13);
    fl_rect(m_PrintedPointList[0][0] - 4, m_PrintedPointList[0][1] - 4, 9, 9);

    // Bounding box
    fl_color(FL_BLUE);
    if (m_PrintedBoundingBoxPointList.size() != 0)
      {
      for (unsigned int i = 0; i < m_PrintedPointList.size(); i++)
        {
        fl_rect(m_PrintedBoundingBoxPointList[i][0] - 5, m_PrintedBoundingBoxPointList[i][1] - 5, 11, 11);
        }
      }
  }

// Create the insight tooltip once for all
  void CreateInsightTooltip()
  {
    m_Oss.str("");
    m_Oss << "UL: (" << m_ExtentList[0][0] << " , " << m_ExtentList[0][1] << ")" << std::endl;
    m_Oss << "       (" << m_LongLatExtentList[0][0] << " , " << m_LongLatExtentList[0][1] << ")" << std::endl;
    m_Oss << "UR: (" << m_ExtentList[1][0] << " , " << m_ExtentList[1][1] << ")" << std::endl;
    m_Oss << "       (" << m_LongLatExtentList[1][0] << " , " << m_LongLatExtentList[1][1] << ")" << std::endl;
    m_Oss << "LR: (" << m_ExtentList[2][0] << " , " << m_ExtentList[2][1] << ")" << std::endl;
    m_Oss << "       (" << m_LongLatExtentList[2][0] << " , " << m_LongLatExtentList[2][1] << ")" << std::endl;
    m_Oss << "LL: (" << m_ExtentList[3][0] << " , " << m_ExtentList[3][1] << ")" << std::endl;
    m_Oss << "       (" << m_LongLatExtentList[3][0] << " , " << m_LongLatExtentList[3][1] << ")" << std::endl;
  }

// Overload handle method to open tooltip over interesting points
  virtual int handle(int event)
  {
    itk::OStringStream oss;
    oss << std::setiosflags(std::ios_base::fixed);
    switch (event)
      {
      case FL_ENTER:
        {
        oss.str("");
        m_Tooltip->enter_area(this, 0, 0, 100, 100, oss.str().c_str());
        return 1;
        }
      case FL_LEAVE:
        {
        oss.str("");
        m_Tooltip->enter_area(this, 0, 0, 100, 100, oss.str().c_str());
        return 1;
        }
      case FL_MOVE:
        {
        if (m_PrintedPointList.size() != 0)
          {
          int x = Fl::event_x();
          int y = Fl::event_y();
          if (std::abs(x - m_PrintedPointList[0][0]) <= 5 && std::abs(y - m_PrintedPointList[0][1]) <= 5)
            {
            oss.str("");
            oss << "(" << m_PointList[0][0] << " , " << m_PointList[0][1] << ")" << std::endl;
            oss << "(" << m_LongLatList[0][0] << " , " << m_LongLatList[0][1] << ")";
            m_Tooltip->enter_area(this, 0, 0, 100, 100, oss.str().c_str());
            }
          else
            {
            if (std::abs(x - m_PrintedPointList[1][0]) <= 5 && std::abs(y - m_PrintedPointList[1][1]) <= 5)
              {
              oss.str("");
              oss << "(" << m_PointList[1][0] << " , " << m_PointList[1][1] << ")" << std::endl;
              oss << "(" << m_LongLatList[1][0] << " , " << m_LongLatList[1][1] << ")";
              m_Tooltip->enter_area(this, 0, 0, 100, 100, oss.str().c_str());
              }
            else
              {
              if (std::abs(x - m_PrintedPointList[2][0]) <= 5 && std::abs(y - m_PrintedPointList[2][1]) <= 5)
                {
                oss.str("");
                oss << "(" << m_PointList[2][0] << " , " << m_PointList[2][1] << ")" << std::endl;
                oss << "(" << m_LongLatList[2][0] << " , " << m_LongLatList[2][1] << ")";
                m_Tooltip->enter_area(this, 0, 0, 100, 100, oss.str().c_str());
                }
              else
                {
                if (std::abs(x - m_PrintedPointList[3][0]) <= 5 && std::abs(y - m_PrintedPointList[3][1]) <= 5)
                  {
                  oss.str("");
                  oss << "(" << m_PointList[3][0] << " , " << m_PointList[3][1] << ")" << std::endl;
                  oss << "(" << m_LongLatList[3][0] << " , " << m_LongLatList[3][1] << ")";
                  m_Tooltip->enter_area(this, 0, 0, 100, 100, oss.str().c_str());
                  }
                else
                  {
                  if (std::abs(x - m_PrintedBoundingBoxPointList[0][0]) <= 5 &&
                      std::abs(y - m_PrintedBoundingBoxPointList[0][1]) <= 5)
                    {
                    oss.str("");
                    oss << "(" << m_PointList[m_XBoundingBoxIdList[0]][0] << " , " <<
                    m_PointList[m_YBoundingBoxIdList[0]][1] << ")" << std::endl;
                    oss << "(" << m_LongLatList[m_XBoundingBoxIdList[0]][0] << " , " <<
                    m_LongLatList[m_YBoundingBoxIdList[0]][1] << ")";
                    m_Tooltip->enter_area(this, 0, 0, 100, 100, oss.str().c_str());
                    }
                  else
                    {
                    if (std::abs(x - m_PrintedBoundingBoxPointList[1][0]) <= 5 &&
                        std::abs(y - m_PrintedBoundingBoxPointList[1][1]) <= 5)
                      {
                      oss.str("");
                      oss << "(" << m_PointList[m_XBoundingBoxIdList[1]][0] << " , " <<
                      m_PointList[m_YBoundingBoxIdList[0]][1] << ")" << std::endl;
                      oss << "(" << m_LongLatList[m_XBoundingBoxIdList[1]][0] << " , " <<
                      m_LongLatList[m_YBoundingBoxIdList[0]][1] << ")";
                      m_Tooltip->enter_area(this, 0, 0, 100, 100, oss.str().c_str());
                      }
                    else
                      {
                      if (std::abs(x - m_PrintedBoundingBoxPointList[2][0]) <= 5 &&
                          std::abs(y - m_PrintedBoundingBoxPointList[2][1]) <= 5)
                        {
                        oss.str("");
                        oss << "(" << m_PointList[m_XBoundingBoxIdList[1]][0] << " , " <<
                        m_PointList[m_YBoundingBoxIdList[1]][1] << ")" << std::endl;
                        oss << "(" << m_LongLatList[m_XBoundingBoxIdList[1]][0] << " , " <<
                        m_LongLatList[m_YBoundingBoxIdList[1]][1] << ")";
                        m_Tooltip->enter_area(this, 0, 0, 100, 100, oss.str().c_str());
                        }
                      else
                        {
                        if (std::abs(x - m_PrintedBoundingBoxPointList[3][0]) <= 5 &&
                            std::abs(y - m_PrintedBoundingBoxPointList[3][1]) <= 5)
                          {
                          oss.str("");
                          oss << "(" << m_PointList[m_XBoundingBoxIdList[0]][0] << " , " <<
                          m_PointList[m_YBoundingBoxIdList[1]][1] << ")" << std::endl;
                          oss << "(" << m_LongLatList[m_XBoundingBoxIdList[0]][0] << " , " <<
                          m_LongLatList[m_YBoundingBoxIdList[1]][1] << ")";
                          m_Tooltip->enter_area(this, 0, 0, 100, 100, oss.str().c_str());
                          }
                        else
                          {
                          if (m_PrintedExtentPointList.size() == 4)
                            {
                            if (x >= m_PrintedExtentPointList[0][0] &&  x <= m_PrintedExtentPointList[1][0]
                                && y >= m_PrintedExtentPointList[0][1] &&  y <= m_PrintedExtentPointList[3][1])
                              {
                              m_Tooltip->enter_area(this, 0, 0, 100, 100, m_Oss.str().c_str());
                              }
                            else
                              {
                              oss.str("");
                              m_Tooltip->enter_area(this, 0, 0, 100, 100, oss.str().c_str());
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        return 1;
        }
      default:
        {
        oss.str("");
        m_Tooltip->enter_area(this, 0, 0, 100, 100, oss.str().c_str());
        return 1;
        }
      }
    return 0;
  }

  /** Constructor */
  GUIRectangle(int x, int y, int w, int h, const char* toto): Fl_Group(x, y, w, h, toto)
  {
    m_Tooltip = new Fl_Tooltip();
    m_Oss << std::setiosflags(std::ios_base::fixed);
  }

  /** Destructor */
  virtual ~GUIRectangle()
  {
    if (parent())
      {
      ((Fl_Group*) parent())->remove(*(Fl_Group*) this);
      }
    delete[] (m_Tooltip);
  }

private:
// Contain the four corner image points orthorectified carto coordinates
  VectorVectorType m_PointList;
// Contain the four corner image points orthorectified long/lat
  VectorVectorType m_LongLatList;
// Contain the four carto coordinates of the region to be extracted
  VectorVectorType m_ExtentList;
// Contain the four long/latcarto coordinates of the region to be extracted;
  VectorVectorType m_LongLatExtentList;
// Contain the bounding box carto coordinates of the orthorectified image.
  IntVectorType m_BoundingBox;
// Contains the drawn point coordinates (in the widget) that represent m_PointList
  VectorIntVectorType m_PrintedPointList;
// Contains the drawn point coordinates (in the widget) that represent m_ExtentList
  VectorIntVectorType m_PrintedExtentPointList;
// Contains the drawn point coordinates (in the widget) that represent m_BoundingBox
  VectorIntVectorType m_PrintedBoundingBoxPointList;
// Contains the index of m_PointList that gave the min and max X
  IntVectorType m_XBoundingBoxIdList;
// Contains the index of m_PointList that gave the min and max Y
  IntVectorType m_YBoundingBoxIdList;
  Fl_Tooltip *  m_Tooltip;
// Store the insight tooltip
  itk::OStringStream m_Oss;
};

} // End namespace otb

#endif
