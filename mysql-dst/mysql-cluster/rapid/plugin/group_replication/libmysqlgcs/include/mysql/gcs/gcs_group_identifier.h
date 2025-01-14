/* Copyright (c) 2014, 2016, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#ifndef GCS_GROUP_IDENTIFIER_INCLUDED
#define GCS_GROUP_IDENTIFIER_INCLUDED

#include <string>

/**
  @class Gcs_group_identifier

  This represents the unique identification of a group. The group uniqueness is
  relevant since it is the destination of data. Thus, it cannot be ambiguous.
*/
class Gcs_group_identifier
{
public:
  /**
    Gcs_group_identifier constructor.

    @param[in] group_id the group identifier
  */

  explicit Gcs_group_identifier(const std::string &group_id);


  ~Gcs_group_identifier() {}


  /**
    @return the group identifier
  */

  const std::string &get_group_id() const;


  /**
    Redefinition of the operator less, to allow usage as key in maps.

    @param[in] other the Gcs_group_identifier to compare to

    @return the result of comparing internal representation with the
            compare operation from the string object
  */

  bool operator<(const Gcs_group_identifier &other) const;


  /**
    Redefinition of the operator equals, to allow usage in sets.

    @param[in] other the Gcs_group_identifier to compare to

    @return the result of comparing internal representation with the
            compare operation from the string object
  */

  bool operator==(const Gcs_group_identifier &other) const;


private:
  std::string group_id;
};

#endif // GCS_GROUP_IDENTIFIER_INCLUDED
 