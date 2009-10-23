/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbWidgetScroll_h
#define __otbWidgetScroll_h


#include <FL/Fl.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Tile.H>
#include <vector>
#include <iostream>
#include <iomanip>
#include "otbMacro.h"
#include "itkMacro.h"


namespace otb
{
/** \class WidgetScroll
 * \brief Imroved FLTK class widget. Allows to create a scroll area filled with widget. Widget type is the template of the class.
 *
 * This class is parameterized by the number of rows and columns of the area, the widget
 * area size (along X and Y) and the X and Y spacing between 2 widget. The user has to give widgets label. For that, use m_LabelList.
 * If the label list sie doesn't match the number of element in the area, nothing will be printed.
 *
 */

template<class TInternalWidget>
class WidgetScroll : public Fl_Group
{
public:
  typedef TInternalWidget                    InternalWidgetType;
  typedef WidgetScroll                       Self;
  typedef Fl_Group                           Superclass;
  typedef std::vector<const char*>           StringVectorType;
  typedef std::vector<InternalWidgetType *>  WidgetVectorType;


  /** *Accessors */
  void SetNumberOfColumns(int nbCol)
  {
    m_NumberOfColumns = nbCol;
  };
  void SetNumberOfRows   (int nbRows)
  {
    m_NumberOfRows = nbRows;
  };
  void SetRowSize        (int sizeRow)
  {
    m_RowSize = sizeRow;
  };
  void SetColumnSize     (int sizeColumn)
  {
    m_ColumnSize = sizeColumn;
  };
  void SetYWidgetSpacing (int sizey)
  {
    m_YWidgetSpacing = sizey;
  };
  void SetXWidgetSpacing (int sizex)
  {
    m_XWidgetSpacing = sizex;
  };
  int GetNumberOfColumns ()
  {
    return m_NumberOfColumns;
  };
  int GetNumberOfRows    ()
  {
    return m_NumberOfRows;
  };
  int GetRowSize         ()
  {
    return m_RowSize;
  };
  int GetColumnSize      ()
  {
    return m_ColumnSize;
  };
  int GetXWidgetSpacing  ()
  {
    return m_XWidgetSpacing;
  };
  int GetYWidgetSpacing  ()
  {
    return m_YWidgetSpacing;
  };

  void SetLabelList( StringVectorType list )
  {
    m_LabelList = list;
  };
  StringVectorType GetLabelList()
  {
    return m_LabelList;
  };
  void AddToLabelList( const char * val)
  {
    m_LabelList.push_back(val);
  };
  int ChangeLabelListValue( const char * val, int id)
  {
    int res = 1;
    if (m_LabelList.size()>id)
    {
      m_LabelList[id] = val;
      res = 0;
    }

    return res;
  };


  WidgetScroll(int X, int Y, int W, int H, const char*L=0) : Fl_Group(X,Y,W,H,L)
  {
    m_NumberOfRows = 0;
    m_NumberOfColumns = 0;
    m_RowSize = 1;
    m_ColumnSize = 1;
    m_XWidgetSpacing = 0;
    m_YWidgetSpacing = 0;
  };

  virtual ~WidgetScroll() {};


  void InitArea()
  {
    if (m_NumberOfColumns!=0 && m_NumberOfRows!=0)
    {
      if ( (m_NumberOfColumns*m_NumberOfRows)==m_LabelList.size() || m_LabelList.size()==0)
      {
        int cpt = 0;
        int xx = this->x() + m_XWidgetSpacing;
        int yy = this->y() + m_YWidgetSpacing;
        Fl_Scroll * scroll = new Fl_Scroll( this->x(), this->y(), this->w(), this->h() );
        Fl_Tile * tile = new Fl_Tile(xx, yy, m_NumberOfColumns*(m_ColumnSize+m_XWidgetSpacing), m_NumberOfRows*(m_RowSize+m_YWidgetSpacing) );
        // Create widgets
        for ( int r=0; r<m_NumberOfRows; r++ )
        {
          for ( int c=0; c<m_NumberOfColumns; c++ )
          {
            InternalWidgetType *in = new InternalWidgetType(xx, yy, m_ColumnSize, m_RowSize);
            if (m_LabelList.size()!=0)
              in->label(m_LabelList[cpt]);
            in->activate();
            in->visible();
            xx += m_ColumnSize+m_XWidgetSpacing;
            cpt++;
          }
          xx = this->x() + m_XWidgetSpacing;
          yy += m_RowSize + m_YWidgetSpacing;
        }
        tile->end();
        scroll->end();
        end();
      }
    }
  }



protected:

private:
  /** Number of rows. */
  int m_NumberOfRows;
  /** Number of columns. */
  int m_NumberOfColumns;
  /** Row size (y). */
  int m_RowSize;
  /** Column size (x)*/
  int m_ColumnSize;
  /** Spacing along X between 2 widgets*/
  int m_XWidgetSpacing;
  /** Spacing along Y between 2 widgets*/
  int m_YWidgetSpacing;
  /** Store the widget label list. */
  StringVectorType m_LabelList;
};

}// End namespace otb

#endif
