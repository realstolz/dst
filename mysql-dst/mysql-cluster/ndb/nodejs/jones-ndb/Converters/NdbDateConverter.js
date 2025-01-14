/*
 Copyright (c) 2013, 2016, Oracle and/or its affiliates. All rights
 reserved.
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2 of
 the License.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 02110-1301  USA
*/

"use strict";

var jones = require("database-jones"),
    MySQLTime = require(jones.common.MySQLTime),
    udebug = unified_debug.getLogger("NdbDateConverter.js");


exports.toDB = function(dateString) {
  var dbtime;
  if(typeof dateString === 'string') {
    dbtime = new MySQLTime().initializeFromDateString(dateString);
  } else if(dateString === null) {
    dbtime = null;
  }
  return dbtime;
};

exports.fromDB = function(dbTime) {
  if(dbTime === null) {
    return null;
  }
  if(typeof dbTime === 'object') {
    return MySQLTime.initializeFromNdb(dbTime).toDateString();
  }
};

 