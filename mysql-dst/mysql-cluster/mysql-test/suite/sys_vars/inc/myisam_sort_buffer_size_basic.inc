############## mysql-test\t\myisam_sort_buffer_size_basic.test ###############
#                                                                             #
# Variable Name: myisam_sort_buffer_size                                     #
# Scope: GLOBAL | SESSION                                                     #
# Access Type: Dynamic                                                        #
# Data Type: numeric                                                        #
# Default Value:                                                              #
# Range:                                                                      #
#                                                                             #
#                                                                             #
# Creation Date: 2008-02-07                                                   #
# Author:  Rizwan                                                     #
#                                                                             #
# Description: Test Cases of Dynamic System Variable myisam_sort_buffer_size #
#              that checks the behavior of this variable in the following ways#
#              * Default Value                                                #
#              * Valid & Invalid values                                       #
#              * Scope & Access method                                        #
#              * Data Integrity                                               #
#                                                                             #
# Reference: http://dev.mysql.com/doc/refman/5.1/en/                          #
#  server-system-variables.html                                               #
#                                                                             #
###############################################################################

--source include/load_sysvars.inc

########################################################################
#           START OF myisam_sort_buffer_size   TESTS                    #
########################################################################


#############################################################
#                 Save initial value                        #
#############################################################

SET @start_global_value = @@global.myisam_sort_buffer_size ;
SELECT @start_global_value;
SET @start_session_value = @@session.myisam_sort_buffer_size ;
SELECT @start_session_value;


--echo '#--------------------FN_DYNVARS_005_01-------------------------#'
########################################################################
#     Display the DEFAULT value of myisam_block_size            #
########################################################################

SET @@global.myisam_sort_buffer_size   = 100;
SET @@global.myisam_sort_buffer_size   = DEFAULT;
SELECT @@global.myisam_sort_buffer_size  ;

SET @@session.myisam_sort_buffer_size   = 200;
SET @@session.myisam_sort_buffer_size   = DEFAULT;
SELECT @@session.myisam_sort_buffer_size  ;


--echo '#--------------------FN_DYNVARS_005_02-------------------------#'
########################################################################
#     Check the DEFAULT value of myisam_sort_buffer_size                #
########################################################################

SET @@global.myisam_sort_buffer_size   = DEFAULT;
SELECT @@global.myisam_sort_buffer_size   = 8388608;

SET @@session.myisam_sort_buffer_size   = DEFAULT;
SELECT @@session.myisam_sort_buffer_size   = 8388608;


--echo '#--------------------FN_DYNVARS_005_03-------------------------#'
##################################################################################
# Change the value of myisam_sort_buffer_size   to a valid value for GLOBAL Scope #
##################################################################################

SET @@global.myisam_sort_buffer_size   = 4;
SELECT @@global.myisam_sort_buffer_size  ;

SET @@global.myisam_sort_buffer_size   = 4294967295;
SELECT @@global.myisam_sort_buffer_size  ;

SET @@global.myisam_sort_buffer_size   = 655354;
SELECT @@global.myisam_sort_buffer_size  ;


--echo '#--------------------FN_DYNVARS_005_04-------------------------#'
###################################################################################
# Change the value of myisam_sort_buffer_size   to a valid value for SESSION Scope #
###################################################################################
 
SET @@session.myisam_sort_buffer_size   = 4;
SELECT @@session.myisam_sort_buffer_size  ;

SET @@session.myisam_sort_buffer_size   = 4294967295;
SELECT @@session.myisam_sort_buffer_size  ;

SET @@session.myisam_sort_buffer_size   = 655345;
SELECT @@session.myisam_sort_buffer_size  ;




--echo '#------------------FN_DYNVARS_005_05-----------------------#'
####################################################################
# Change the value of myisam_sort_buffer_size   to an invalid value #
####################################################################

SET @@global.myisam_sort_buffer_size   = 0;
SELECT @@global.myisam_sort_buffer_size  ;

SET @@global.myisam_sort_buffer_size   = -1024;
SELECT @@global.myisam_sort_buffer_size  ;

SET @@global.myisam_sort_buffer_size   = 429496729533;
SELECT @@global.myisam_sort_buffer_size  ;


--Error ER_WRONG_TYPE_FOR_VAR
SET @@global.myisam_sort_buffer_size   = 65530.34;
SELECT @@global.myisam_sort_buffer_size  ;

--Error ER_WRONG_TYPE_FOR_VAR
SET @@global.myisam_sort_buffer_size   = test;
SELECT @@global.myisam_sort_buffer_size  ;

--Error ER_WRONG_TYPE_FOR_VAR
SET @@global.myisam_sort_buffer_size   = "test";
SELECT @@global.myisam_sort_buffer_size  ;

--Error ER_WRONG_TYPE_FOR_VAR
SET @@global.myisam_sort_buffer_size   = 'test';
SELECT @@global.myisam_sort_buffer_size  ;

--Error ER_WRONG_TYPE_FOR_VAR
SET @@global.myisam_sort_buffer_size   = ON;
SELECT @@global.myisam_sort_buffer_size  ;


SET @@session.myisam_sort_buffer_size   = 0;
SELECT @@session.myisam_sort_buffer_size  ;

SET @@session.myisam_sort_buffer_size   = -2;
SELECT @@session.myisam_sort_buffer_size  ;

--Error ER_WRONG_TYPE_FOR_VAR
SET @@session.myisam_sort_buffer_size   = 65530.34;
SELECT @@session.myisam_sort_buffer_size  ;


--Error ER_WRONG_TYPE_FOR_VAR
SET @@session.myisam_sort_buffer_size   = test;
SELECT @@session.myisam_sort_buffer_size  ;

--Error ER_WRONG_TYPE_FOR_VAR
SET @@session.myisam_sort_buffer_size   = "test";
SELECT @@session.myisam_sort_buffer_size  ;


--echo '#------------------FN_DYNVARS_005_06-----------------------#'
####################################################################
#   Check if the value in GLOBAL Table matches value in variable   #
####################################################################

--disable_warnings
SELECT @@global.myisam_sort_buffer_size   = VARIABLE_VALUE 
FROM INFORMATION_SCHEMA.GLOBAL_VARIABLES 
WHERE VARIABLE_NAME='myisam_sort_buffer_size  ';
--enable_warnings

--echo '#------------------FN_DYNVARS_005_07-----------------------#'
####################################################################
#  Check if the value in SESSION Table matches value in variable   #
####################################################################

--disable_warnings
SELECT @@session.myisam_sort_buffer_size   = VARIABLE_VALUE 
FROM INFORMATION_SCHEMA.SESSION_VARIABLES 
WHERE VARIABLE_NAME='myisam_sort_buffer_size  ';
--enable_warnings

--echo '#------------------FN_DYNVARS_005_08-----------------------#'
####################################################################
#     Check if TRUE and FALSE values can be used on variable       #
####################################################################

SET @@global.myisam_sort_buffer_size   = TRUE;
SELECT @@global.myisam_sort_buffer_size  ;
SET @@global.myisam_sort_buffer_size   = FALSE;
SELECT @@global.myisam_sort_buffer_size  ;



--echo '#---------------------FN_DYNVARS_001_09----------------------#'
#################################################################################### 
#  Check if accessing variable with and without GLOBAL point to same variable   #
#################################################################################### 

SET @@global.myisam_sort_buffer_size   = 10;
SELECT @@myisam_sort_buffer_size   = @@global.myisam_sort_buffer_size  ;


--echo '#---------------------FN_DYNVARS_001_10----------------------#'
########################################################################################################
#    Check if accessing variable with SESSION,LOCAL and without SCOPE points to same session variable  #
########################################################################################################

SET @@myisam_sort_buffer_size   = 100;
SELECT @@myisam_sort_buffer_size   = @@local.myisam_sort_buffer_size  ;
SELECT @@local.myisam_sort_buffer_size   = @@session.myisam_sort_buffer_size  ;


--echo '#---------------------FN_DYNVARS_001_11----------------------#'
###################################################################################  
#   Check if myisam_sort_buffer_size   can be accessed with and without @@ sign    #
###################################################################################

SET myisam_sort_buffer_size   = 1;
SELECT @@myisam_sort_buffer_size  ;
--Error ER_UNKNOWN_TABLE

SELECT local.myisam_sort_buffer_size  ;

--Error ER_UNKNOWN_TABLE
SELECT session.myisam_sort_buffer_size  ;

--Error ER_BAD_FIELD_ERROR
SELECT myisam_sort_buffer_size   = @@session.myisam_sort_buffer_size  ;


####################################
#     Restore initial value        #
####################################

SET @@global.myisam_sort_buffer_size   = @start_global_value;
SELECT @@global.myisam_sort_buffer_size  ;
SET @@session.myisam_sort_buffer_size   = @start_session_value;
SELECT @@session.myisam_sort_buffer_size  ;


#############################################################
#                 END OF myisam_sort_buffer_size   TESTS     #
#############################################################

 