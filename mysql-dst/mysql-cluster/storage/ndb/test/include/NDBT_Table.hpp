/*
   Copyright (c) 2003, 2010, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef NDBT_TABLE_HPP
#define NDBT_TABLE_HPP

#include <ndb_global.h>

#include <NdbApi.hpp>
#include <NdbOut.hpp>

class NDBT_Attribute : public NdbDictionary::Column {
public:
  NDBT_Attribute(const char* _name,
		 NdbDictionary::Column::Type _type,
		 int _length = 1,
		 bool _pk = false, 
		 bool _nullable = false,
		 CHARSET_INFO *cs= 0,
		 NdbDictionary::Column::StorageType storage = NdbDictionary::Column::StorageTypeMemory,
                 bool dynamic = false,
                 const void* defaultVal = NULL,
                 Uint32 defaultValBytes = 0):
    NdbDictionary::Column(_name)
  {
    require(_name != 0);
    
    setType(_type);
    setLength(_length);
    setNullable(_nullable);
    setPrimaryKey(_pk);
    if (cs)
    {
      setCharset(cs);
    }
    setStorageType(storage);
    setDynamic(dynamic);
    setDefaultValue(defaultVal, defaultValBytes);
  }
};

class NDBT_Table : public NdbDictionary::Table {
  /**
   * Print meta information about table 
   * (information on how it is strored, what the attributes look like etc.)
   */
public: 
  
  NDBT_Table(const char* name, 
	     int noOfAttributes,
	     const NdbDictionary::Column attributes[])
    : NdbDictionary::Table(name)
  {
    require(name != 0);
    
    //setStoredTable(stored);
    for(int i = 0; i<noOfAttributes; i++)
      addColumn(attributes[i]);

    // validate() might cause initialization order problem with charset
    NdbError error;
    int ret = aggregate(error);
    (void)ret;
    require(ret == 0);
  }

  NDBT_Table(const char* name, 
	     int noOfAttributes,
	     NdbDictionary::Column* attributePtrs[])
    : NdbDictionary::Table(name)
  {
    require(name != 0);
    
    //setStoredTable(stored);
    for(int i = 0; i<noOfAttributes; i++)
      addColumn(*attributePtrs[i]);
    
    // validate() might cause initialization order problem with charset
    NdbError error;
    int ret = aggregate(error);
    (void)ret;
    require(ret == 0);
  }
  
  static const NdbDictionary::Table * discoverTableFromDb(Ndb* ndb,
							  const char * name);
};

inline
const NdbDictionary::Table * 
NDBT_Table::discoverTableFromDb(Ndb* ndb, const char * name){
  return ndb->getDictionary()->getTable(name);
}

#endif
 