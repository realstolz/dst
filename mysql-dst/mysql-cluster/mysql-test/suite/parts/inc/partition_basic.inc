################################################################################
# inc/partition_basic.inc                                                      #
#                                                                              #
# Purpose:                                                                     #
#   Basic tests around create partitioned table with/without PRIMARY KEY and   #
#   /or UNIQUE INDEX                                                           #
#                                                                              #
#------------------------------------------------------------------------------#
# Original Author: mleich                                                      #
# Original Date: 2006-03-05                                                    #
# Change Author:                                                               #
# Change Date:                                                                 #
# Change:                                                                      #
################################################################################
--enable_abort_on_error

--echo
--echo #========================================================================
--echo #  Check partitioning methods on just created tables
--echo #  The tables should be defined without/with PRIMARY KEY and
--echo #  UNIQUE INDEXes.
--echo #  Every test round has to check
--echo #        PARTITION BY HASH/KEY/LIST/RANGE
--echo #        PARTITION BY RANGE/LIST ... SUBPARTITION BY HASH/KEY ...
--echo #========================================================================
--echo #------------------------------------------------------------------------
--echo #  1   Tables without PRIMARY KEY or UNIQUE INDEXes
--echo #------------------------------------------------------------------------
--echo #  1.1 The partitioning function contains one column.
let $unique= ;
--source suite/parts/inc/partition_methods1.inc
#
--echo #  1.2 The partitioning function contains two columns.
let $unique= ;
--source suite/parts/inc/partition_methods2.inc
#
--echo #------------------------------------------------------------------------
--echo #  2   Tables with PRIMARY KEY and/or UNIQUE INDEXes
--echo #        The partitioning function contains one column.
--echo #------------------------------------------------------------------------
if ($more_pk_ui_tests)
{
   if ($do_pk_tests)
   {
      --echo #  2.1 PRIMARY KEY consisting of one column
      let $unique= , PRIMARY KEY(f_int1);
      --source suite/parts/inc/partition_methods1.inc
   }
   --echo #  2.2 UNIQUE INDEX consisting of one column
   let $unique= , UNIQUE INDEX uidx1 (f_int1);
   --source suite/parts/inc/partition_methods1.inc
   #
   if ($do_pk_tests)
   {
      --echo #  2.3 PRIMARY KEY consisting of two columns
      let $unique= , PRIMARY KEY(f_int1,f_int2);
      --source suite/parts/inc/partition_methods1.inc
      let $unique= , PRIMARY KEY(f_int2,f_int1);
      --source suite/parts/inc/partition_methods1.inc
   }
   #
   --echo #  2.4 UNIQUE INDEX consisting of two columns
   let $unique= , UNIQUE INDEX uidx1 (f_int1,f_int2);
   --source suite/parts/inc/partition_methods1.inc
   let $unique= , UNIQUE INDEX uidx1 (f_int2,f_int1);
   --source suite/parts/inc/partition_methods1.inc
   #
}
--echo #  2.5 PRIMARY KEY + UNIQUE INDEX consisting of two columns
if ($do_pk_tests)
{
   let $unique= , UNIQUE INDEX uidx1 (f_int1,f_int2), PRIMARY KEY(f_int2,f_int1);
   --source suite/parts/inc/partition_methods1.inc
   let $unique= , UNIQUE INDEX uidx1 (f_int2,f_int1), PRIMARY KEY(f_int1,f_int2);
   --source suite/parts/inc/partition_methods1.inc
}
let $unique= , UNIQUE INDEX uidx1 (f_int1,f_int2), UNIQUE INDEX uidx2 (f_int2,f_int1);
--source suite/parts/inc/partition_methods1.inc

--echo #------------------------------------------------------------------------
--echo #  3   Tables with PRIMARY KEY and/or UNIQUE INDEXes
--echo #        The partitioning function contains two columns.
--echo #------------------------------------------------------------------------
#
if ($more_pk_ui_tests)
{
   if ($do_pk_tests)
   {
      --echo #  3.1 PRIMARY KEY consisting of two columns
      let $unique= , PRIMARY KEY(f_int1,f_int2);
      --source suite/parts/inc/partition_methods2.inc

      let $unique= , PRIMARY KEY(f_int2,f_int1);
      --source suite/parts/inc/partition_methods2.inc
   }
   #
   --echo #  3.2 UNIQUE INDEX consisting of two columns
   let $unique= , UNIQUE INDEX uidx1 (f_int1,f_int2);
   --source suite/parts/inc/partition_methods2.inc
   let $unique= , UNIQUE INDEX uidx1 (f_int2,f_int1);
   --source suite/parts/inc/partition_methods2.inc
}
#
--echo #  3.3 PRIMARY KEY and UNIQUE INDEX consisting of two columns
if ($do_pk_tests)
{
   let $unique= , UNIQUE INDEX uidx1 (f_int1,f_int2), PRIMARY KEY(f_int2,f_int1);
   --source suite/parts/inc/partition_methods2.inc
   let $unique= , UNIQUE INDEX uidx1 (f_int2,f_int1), PRIMARY KEY(f_int1,f_int2);
   --source suite/parts/inc/partition_methods2.inc
}
let $unique= , UNIQUE INDEX uidx1 (f_int1,f_int2), UNIQUE INDEX uidx2 (f_int2,f_int1);
--source suite/parts/inc/partition_methods2.inc
 