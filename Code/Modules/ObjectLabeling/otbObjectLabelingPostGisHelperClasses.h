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
#ifndef __otbLabelingPostGisHelperClasses_h
#define __otbLabelingPostGisHelperClasses_h

#include "otbPostGISConnectionImplementation.h"
#include "otbPostGISTable.h"
#include "otbMacro.h"

namespace otb
{
struct SQLStringFunctor
{
  std::string operator()(std::string& in)
    {
    std::string out = in;
    std::replace(out.begin(), out.end(), ':', '_');
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
    }
};


typedef otb::PostGISConnectionImplementation BdConnection;
typedef otb::PostGISConnectionImplementation::Pointer BdConnectionPointer;
typedef otb::PostGISTable<BdConnection, double, 2> PostGISTableType;
typedef otb::PostGISTable<BdConnection, double, 2>::Pointer PostGISTableTypePointer;


class PostGISCreateTableWithAttributesAndClassTransactor
: public PostGISCreateTableTransactor
{
public:

  void SetAttributes(std::vector<std::string> attributes)
  {
    m_Attributes = attributes;
  }

  void operator()(pqxx::nontransaction &T)
  {
    // Drop old table if necessarry
    if(m_RemoveExistingTable)
      {
      std::stringstream dropCommand;
      dropCommand << "DROP TABLE IF EXISTS " << m_TableName;
      otbGenericMsgDebugMacro(<<"Drop Command : " << std::endl << dropCommand.str());
      m_Result = T.exec(dropCommand.str());
      }

    // Create new table
    std::stringstream createCommand;
    createCommand << "CREATE TABLE " << m_TableName
                  << " (id serial PRIMARY KEY, class_id INTEGER";
    std::vector<std::string>::const_iterator it;
    for (it =  m_Attributes.begin(); it !=  m_Attributes.end(); ++it)
      {
      createCommand << ", " << (*it) << " NUMERIC";
      }
    createCommand << ");";

    otbGenericMsgDebugMacro(<<"Create Command : " << std::endl << createCommand.str());
    m_Result = T.exec(createCommand.str());

    // Add geometry column
    std::stringstream addGeometryCommand;
    addGeometryCommand << "SELECT AddGeometryColumn( '"<< m_TableName <<
      "', 'the_geom', "<< m_SRID <<", 'GEOMETRY',"<< m_Dimension <<" );";
    m_Result = T.exec(addGeometryCommand.str());

    // Create GIST index
    std::stringstream addGISTIndexCommand;
    addGISTIndexCommand << "CREATE INDEX idx_" << m_TableName << "_the_geom ON " << m_TableName << " USING gist( the_geom );";
    otbGenericMsgDebugMacro(<<"Create Command " << addGISTIndexCommand.str());
    m_Result = T.exec(addGISTIndexCommand.str());
  }
private:
  std::vector<std::string> m_Attributes;
};


class ITK_EXPORT PostGISTableWithAttributesAndClass
: public otb::PostGISTable<BdConnection, double, 2>
{
public:
  typedef PostGISTableWithAttributesAndClass Self;
  typedef otb::PostGISTable<BdConnection, double, 2> Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard macros */
  itkNewMacro(Self);
  itkTypeMacro(PostGISTableWithAttributesAndClass,GISTable);
  itkStaticConstMacro(Dimension, unsigned int, 2);

  /** Some convenient typedefs */
  typedef Superclass::ConnectionType ConnectionType;
  typedef Superclass::ConnectionPointerType ConnectionPointerType;
  typedef Superclass::ConnectionPointerType PointType;
  typedef Superclass::LineType LineType;
  typedef Superclass::LineType LinePointerType;
  typedef Superclass::LineType PolygonType;
  typedef Superclass::PolygonPointerType PolygonPointerType;
  typedef Superclass::PolygonConstPointerType PolygonConstPointerType;
  typedef Superclass::PolygonListType PolygonListType;
  typedef Superclass::PolygonListPointerType PolygonListPointerType;
  typedef Superclass::PolygonListPointerType PolygonListConstPointerType;

  typedef ObjectLabelingModel::LabelObjectType LabelObjectType;
  typedef ObjectLabelingModel::LabelMapType LabelMapType;

public:

  const std::vector<std::string>& GetAttributes()
  {
    return m_Attributes;
  }
  void SetAttributes( const std::vector<std::string>& attributes )
  {
    m_Attributes = attributes;
    std::transform(m_Attributes.begin(), m_Attributes.end(), std::back_inserter(m_SQLAttributes), SQLStringFunctor());
  }

  // Create the table in the db. Attributes must be set already
  void CreateTable(bool dropExisting = true)
  {
    typedef PostGISCreateTableWithAttributesAndClassTransactor TransactorType;

    TransactorType myTransactor;
    myTransactor.SetAttributes(m_SQLAttributes);
    myTransactor.SetDimension( Dimension );
    myTransactor.SetTableName( this->GetTableName() );
    myTransactor.SetSRID( this->GetSrid() );
    myTransactor.SetRemoveExistingTable( dropExisting );

    this->GetConnection()->GetConnection()->perform( myTransactor );
  }

  void InsertLabelObject( std::stringstream& sqlCmd, const LabelObjectType* lo )
  {
    sqlCmd << "INSERT INTO "
           << "\"" << this->GetTableName() << "\" "
           << "( "
           << "\"the_geom\""
           << ", \"class_id\"";

    std::vector<std::string>::const_iterator it;
    for (it = m_SQLAttributes.begin(); it != m_SQLAttributes.end(); ++it)
      {
      sqlCmd << " ," << "\"" << *it << "\"";
      }

    sqlCmd << ") VALUES ";

    // Add geometry
    sqlCmd << "(GeometryFromText('";
    sqlCmd << "POLYGON( ( ";


    PolygonType::Pointer polygon = m_PolygonExtractor(lo);
    PolygonType::Pointer correctPolygon = m_CorrectFunctor(polygon);

    typedef PolygonType::VertexListConstIteratorType VertexIterator;
    VertexIterator itVertex = correctPolygon->GetVertexList()->Begin();

    while( itVertex != correctPolygon->GetVertexList()->End() )
      {
      sqlCmd << itVertex.Value()[0] << " " << itVertex.Value()[1];
      ++itVertex;

      if (itVertex != correctPolygon->GetVertexList()->End())
        {
        sqlCmd << ",";
        }
      }
    sqlCmd << ") )'";


    sqlCmd << " ," << this->GetSrid() << ") "; // end "GeometryFromText("

    sqlCmd << " ," << lo->GetClassLabel();
    //sqlCmd << " ," << lo->GetLabel();


    for (it = m_Attributes.begin(); it != m_Attributes.end(); ++it)
      {
      sqlCmd << " ," << lo->GetAttribute( (*it).c_str() );
      }

    sqlCmd << ");" << std::endl;

  }

  void InsertLabelMap( LabelMapType& labelMap )
  {
    typedef otb::PostGISFromStringTransactor TransactorType;
    TransactorType transactor;


    LabelMapType::LabelObjectContainerType::const_iterator it, beg, end;
    beg = labelMap.GetLabelObjectContainer().begin();
    end = labelMap.GetLabelObjectContainer().end();

    unsigned long nbLabelObjects = labelMap.GetNumberOfLabelObjects();
    unsigned long progressEach = nbLabelObjects / 20;
    unsigned long i = 0;

    otbGenericMsgDebugMacro( << "Inserting " << nbLabelObjects << " label objects..." );
    for( it = beg; it != end; ++it)
      {
      std::stringstream sqlCmd;
      this->InsertLabelObject( sqlCmd, it->second );
      transactor.SetTransactionString( sqlCmd.str() );
      this->GetConnection()->GetConnection()->perform( transactor );

      i++;
      if (i % progressEach == 0)
        {
        otbGenericMsgDebugMacro( <<  static_cast<int>( ::round(i * 100.0 / nbLabelObjects) )  << " % done" );
        }
      }
  }

protected:
  PostGISTableWithAttributesAndClass() {};
  virtual ~PostGISTableWithAttributesAndClass() {};

private:
  PostGISTableWithAttributesAndClass(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typedef otb::Functor::LabelObjectToPolygonFunctor<LabelObjectType,PolygonType> FunctorType;
  typedef otb::CorrectPolygonFunctor <PolygonType> CorrectFunctorType;

  FunctorType m_PolygonExtractor;
  CorrectFunctorType m_CorrectFunctor;

  std::vector<std::string> m_Attributes;
  std::vector<std::string> m_SQLAttributes; // m_Attributes with SQL regularized name
};

} // en namespace otb

#endif
