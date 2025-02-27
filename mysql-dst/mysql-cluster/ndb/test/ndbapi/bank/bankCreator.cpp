/*
   Copyright (c) 2003, 2016, Oracle and/or its affiliates. All rights reserved.

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


#include <ndb_global.h>
#include <NdbOut.hpp>

#include <NdbApi.hpp>
#include <NDBT.hpp> 
#include <NdbSleep.h>
#include <getarg.h>
#include "Bank.hpp"
 

int main(int argc, const char** argv){
  ndb_init();
  int _help = 0;
  const char * _database = "BANK";
  int disk = 0;
  int skip_create = 0;

  struct getargs args[] = {
    { "database", 'd', arg_string, &_database, "Database name", ""},
    { "disk", 0, arg_flag, &disk, "Use disk tables", "" },
    { "skip-create", 0, arg_flag, &skip_create, "Skip create", "" },
    { "usage", '?', arg_flag, &_help, "Print help", "" }
  };
  int num_args = sizeof(args) / sizeof(args[0]);
  int optind = 0;
  char desc[] = 
    "This program will create and load the tables for bank\n";
  
  if(getarg(args, num_args, argc, argv, &optind) ||  _help) {
    arg_printusage(args, num_args, argv[0], desc);
    return NDBT_ProgramExit(NDBT_WRONGARGS);
  }

  Ndb_cluster_connection con;
  if(con.connect(12, 5, 1) != 0)
  {
    return NDBT_ProgramExit(NDBT_FAILED);
  }

  Bank bank(con,_database);
  int overWriteExisting = true;
  bank.setSkipCreate(skip_create);
  if (bank.createAndLoadBank(overWriteExisting, disk) != NDBT_OK)
    return NDBT_ProgramExit(NDBT_FAILED);
  return NDBT_ProgramExit(NDBT_OK);

}



 