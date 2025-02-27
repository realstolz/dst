#
# Test of different EXPLAINs

--disable_warnings
drop table if exists t1;
--enable_warnings
create table t1 (id int not null, str char(10), unique(str));
eval explain $FORMAT select * from t1;
insert into t1 values (1, null),(2, null),(3, "foo"),(4, "bar");
select * from t1 where str is null;
select * from t1 where str="foo";
eval explain $FORMAT select * from t1 where str is null;
eval explain $FORMAT select * from t1 where str="foo";
eval explain $FORMAT select * from t1 ignore key (str) where str="foo";
eval explain $FORMAT select * from t1 use key (str,str) where str="foo";

#The following should give errors
--error 1176
eval explain $FORMAT select * from t1 use key (str,str,foo) where str="foo";
--error 1176
eval explain $FORMAT select * from t1 ignore key (str,str,foo) where str="foo";
drop table t1;

--eval explain $FORMAT select 1

create table t1 (a int not null);
eval explain $FORMAT select count(*) from t1;
insert into t1 values(1);
eval explain $FORMAT select count(*) from t1;
insert into t1 values(1);
eval explain $FORMAT select count(*) from t1;
drop table t1;

#
# Bug #3403 Wrong encoding in EXPLAIN SELECT output
#
set names koi8r;
create table ��� (���0 int, ���1 int, key ���0 (���0), key ���01 (���0,���1));
insert into ��� (���0) values (1);
insert into ��� (���0) values (2);
eval explain $FORMAT select ���0 from ��� where ���0=1;
drop table ���;
set names latin1;

# End of 4.1 tests


#
# Bug#15463: EXPLAIN SELECT..INTO hangs the client (QB, command line)
#
select 3 into @v1;
eval explain $FORMAT select 3 into @v1;

#
# Bug#22331: Wrong WHERE in EXPLAIN EXTENDED when all expressions were
#            optimized away.
#
create table t1(f1 int, f2 int);
insert into t1 values (1,1);
create view v1 as select * from t1 where f1=1;
explain extended select * from v1 where f2=1;
explain extended select * from t1 where 0;
explain extended select * from t1 where 1;
explain extended select * from t1 having 0;
explain extended select * from t1 having 1;
drop view v1;
drop table t1;

#
# Bug #32241: memory corruption due to large index map in 'Range checked for 
#             each record'
#

CREATE TABLE t1(c INT);
INSERT INTO t1 VALUES (),();

CREATE TABLE t2 (b INT,
KEY(b),KEY(b),KEY(b),KEY(b),KEY(b),
KEY(b),KEY(b),KEY(b),KEY(b),KEY(b),
KEY(b),KEY(b),KEY(b),KEY(b),KEY(b),
KEY(b),KEY(b),KEY(b),KEY(b),KEY(b),
KEY(b),KEY(b),KEY(b),KEY(b),KEY(b),
KEY(b),KEY(b),KEY(b),KEY(b),KEY(b),
KEY(b),KEY(b),KEY(b),KEY(b),KEY(b),
KEY(b),KEY(b),KEY(b),KEY(b),KEY(b));

INSERT INTO t2 VALUES (),(),();

# We only need to make sure that there is no buffer overrun and the index map
# is displayed correctly
#--replace_column 1 X 2 X 3 X 5 X 6 X 7 X 8 X 9 X 10 X
eval EXPLAIN $FORMAT SELECT 1 FROM (SELECT 1 FROM t2,t1 WHERE b < c GROUP BY 1 LIMIT 1) AS d2;
DROP TABLE t2;
DROP TABLE t1;

#
# Bug #34773: query with explain extended and derived table / other table
# crashes server
#

CREATE TABLE t1(a INT);
CREATE TABLE t2(a INT);
INSERT INTO t1 VALUES (1),(2);
INSERT INTO t2 VALUES (1),(2);

EXPLAIN EXTENDED SELECT 1
 FROM (SELECT COUNT(DISTINCT t1.a) FROM t1,t2 GROUP BY t1.a) AS s1;

EXPLAIN EXTENDED SELECT 1
 FROM (SELECT COUNT(DISTINCT t1.a) FROM t1,t2 GROUP BY t1.a) AS s1;

prepare s1 from 
'EXPLAIN EXTENDED SELECT 1  
 FROM (SELECT COUNT(DISTINCT t1.a) FROM t1,t2 GROUP BY t1.a) AS s1';
execute s1;

prepare s1 from 
'EXPLAIN EXTENDED SELECT 1  
 FROM (SELECT COUNT(DISTINCT t1.a) FROM t1,t2 GROUP BY t1.a) AS s1';
execute s1;
execute s1;

DROP TABLE t1,t2;


#
# Bug #43354: Use key hint can crash server in explain extended query
#

CREATE TABLE t1 (a INT PRIMARY KEY);

--error ER_KEY_DOES_NOT_EXITS
EXPLAIN EXTENDED SELECT COUNT(a) FROM t1 USE KEY(a);

DROP TABLE t1;

#
# Bug#45989 memory leak after explain encounters an error in the query
#
CREATE TABLE t1(a LONGTEXT);
INSERT INTO t1 VALUES (repeat('a',@@global.max_allowed_packet));
INSERT INTO t1 VALUES (repeat('b',@@global.max_allowed_packet));
--error ER_BAD_FIELD_ERROR
eval EXPLAIN $FORMAT SELECT DISTINCT 1 FROM t1,
        (SELECT DISTINCTROW a AS away FROM t1 GROUP BY a WITH ROLLUP) as d1
         WHERE t1.a = d1.a;
DROP TABLE t1;

--echo #
--echo # Bug#48295:
--echo # explain extended crash with subquery and ONLY_FULL_GROUP_BY sql_mode
--echo #

CREATE TABLE t1 (f1 INT);

SELECT @@session.sql_mode INTO @old_sql_mode;
SET SESSION sql_mode='ONLY_FULL_GROUP_BY';

# EXPLAIN EXTENDED (with subselect). used to crash.
# This is actually a valid query for this sql_mode,
# but it was transformed in such a way that it failed, see
# Bug#12329653 - EXPLAIN, UNION, PREPARED STATEMENT, CRASH, SQL_FULL_GROUP_BY
EXPLAIN EXTENDED SELECT 1 FROM t1
                          WHERE f1 > ALL( SELECT t.f1 FROM t1,t1 AS t );

SET SESSION sql_mode=@old_sql_mode;

DROP TABLE t1;

--echo End of 5.0 tests.

--echo #
--echo # Bug#37870: Usage of uninitialized value caused failed assertion.
--echo #
set @opt_sw_save=  @@optimizer_switch;

--disable_query_log
if (`select locate('semijoin', @@optimizer_switch) > 0`) 
{
  set optimizer_switch='semijoin=off';
}
--enable_query_log
create table t1 (dt datetime not null, t time not null);
create table t2 (dt datetime not null);
insert into t1 values ('2001-01-01 1:1:1', '1:1:1'),
('2001-01-01 1:1:1', '1:1:1');
insert into t2 values ('2001-01-01 1:1:1'), ('2001-01-01 1:1:1');
flush tables;
eval EXPLAIN $FORMAT SELECT OUTR.dt FROM t1 AS OUTR WHERE OUTR.dt IN (SELECT INNR.dt FROM t2 AS INNR WHERE OUTR.dt IS NULL );
flush tables;
SELECT OUTR.dt FROM t1 AS OUTR WHERE OUTR.dt IN (SELECT INNR.dt FROM t2 AS INNR WHERE OUTR.dt IS NULL );
flush tables;
eval EXPLAIN $FORMAT SELECT OUTR.dt FROM t1 AS OUTR WHERE OUTR.dt IN ( SELECT INNR.dt FROM t2 AS INNR WHERE OUTR.t < '2005-11-13 7:41:31' ); 
flush tables;
SELECT OUTR.dt FROM t1 AS OUTR WHERE OUTR.dt IN ( SELECT INNR.dt FROM t2 AS INNR WHERE OUTR.t < '2005-11-13 7:41:31' );
drop tables t1, t2;
set optimizer_switch= @opt_sw_save;

--echo #
--echo # Bug#47669: Query showed by EXPLAIN EXTENDED gives different result from original query
--echo #

CREATE TABLE t1 (c int);
INSERT INTO t1 VALUES (NULL);
CREATE TABLE t2 (d int);
INSERT INTO t2 VALUES (NULL), (0);
EXPLAIN EXTENDED SELECT (SELECT 1 FROM t2 WHERE d = c) FROM t1;
DROP TABLE t1, t2;

--echo #
--echo # Bug#30302: Tables that were optimized away are printed in the
--echo #            EXPLAIN EXTENDED warning.
--echo #
create table t1(f1 int);
create table t2(f2 int);
insert into t1 values(1);
insert into t2 values(1),(2);
explain extended select * from t1 where f1=1;
explain extended select * from t1 join t2 on f1=f2 where f1=1;
drop table t1,t2;

--echo #
--echo # Bug #48419: another explain crash..
--echo #
CREATE TABLE t1 (a INT);
CREATE TABLE t2 (b BLOB, KEY b(b(100)));
INSERT INTO t2 VALUES ('1'), ('2'), ('3');

FLUSH TABLES;

eval EXPLAIN $FORMAT SELECT 1 FROM t1 WHERE a = (SELECT 1 FROM t1 t JOIN t2 WHERE b <= 1 AND t.a);

DROP TABLE t1, t2;

--echo #
--echo # Bug #48573: difference of index selection between rpm binary and
--echo #             .tar.gz, windows vs linux..
--echo #

CREATE TABLE t1(c1 INT, c2 INT, c4 INT, c5 INT, KEY(c2, c5), KEY(c2, c4, c5));
INSERT INTO t1 VALUES(4, 1, 1, 1);
INSERT INTO t1 VALUES(3, 1, 1, 1);
INSERT INTO t1 VALUES(2, 1, 1, 1);
INSERT INTO t1 VALUES(1, 1, 1, 1);

eval EXPLAIN $FORMAT SELECT c1 FROM t1 WHERE c2 = 1 AND c4 = 1 AND c5 = 1;

DROP TABLE t1;

--echo #
--echo # Bug#56814 Explain + subselect + fulltext crashes server
--echo #

CREATE TABLE t1(f1 VARCHAR(6) NOT NULL,
FULLTEXT KEY(f1),UNIQUE(f1));
INSERT INTO t1 VALUES ('test');

eval EXPLAIN $FORMAT SELECT 1 FROM t1 WHERE 1 > ALL((SELECT 1 FROM t1 JOIN t1 a ON (MATCH(t1.f1) AGAINST ("")) WHERE t1.f1 GROUP BY t1.f1));

eval PREPARE stmt FROM 'EXPLAIN $FORMAT SELECT 1 FROM t1 WHERE 1 > ALL((SELECT 1 FROM t1 RIGHT OUTER JOIN t1 a ON (MATCH(t1.f1) AGAINST ("")) WHERE t1.f1 GROUP BY t1.f1))';

EXECUTE stmt;
EXECUTE stmt;

DEALLOCATE PREPARE stmt;

eval PREPARE stmt FROM 'EXPLAIN $FORMAT SELECT 1 FROM t1 WHERE 1 > ALL((SELECT 1 FROM t1 JOIN t1 a ON (MATCH(t1.f1) AGAINST ("")) WHERE t1.f1 GROUP BY t1.f1))';

EXECUTE stmt;
EXECUTE stmt;

DEALLOCATE PREPARE stmt;

DROP TABLE t1;

--echo End of 5.1 tests.

--echo #
--echo # Bug#46860: 
--echo # Crash/segfault using EXPLAIN EXTENDED on query using UNION in subquery.
--echo #
--disable_warnings
drop table if exists t1;
--enable_warnings
create table `t1` (`a` int);
--error ER_NON_UNIQ_ERROR
explain extended select 1 from `t1`, `t1` as `t2`
where `t1`.`a`  > all ( (select `a` from `t1` ) union (select `a`) );
drop table t1;

--echo # 
--echo # BUG#30597: Change EXPLAIN output to include extrema of 
--echo #            UNION components
--echo # 

eval EXPLAIN $FORMAT SELECT 1
 UNION ALL SELECT 1 UNION ALL SELECT 1 UNION ALL SELECT 1 UNION ALL SELECT 1
 UNION ALL SELECT 1 UNION ALL SELECT 1 UNION ALL SELECT 1 UNION ALL SELECT 1
 UNION ALL SELECT 1 UNION ALL SELECT 1 UNION ALL SELECT 1 UNION ALL SELECT 1
 UNION ALL SELECT 1 UNION ALL SELECT 1 UNION ALL SELECT 1 UNION ALL SELECT 1
 UNION ALL SELECT 1 UNION ALL SELECT 1 UNION ALL SELECT 1 UNION ALL SELECT 1
 UNION ALL SELECT 1 UNION ALL SELECT 1 UNION ALL SELECT 1 UNION ALL SELECT 1
 ;

--echo # End BUG#30597

--echo #
--echo # BUG#53562: EXPLAIN statement should hint when 
--echo #            index is not used due to type conversion
--echo #

CREATE TABLE t1 (url char(1) PRIMARY KEY);
INSERT INTO t1 VALUES ('1'),('2'),('3'),('4'),('5');       

--echo
--echo # Normally, lookup access on primary key is done
EXPLAIN EXTENDED SELECT * FROM t1 WHERE url='1';
--echo
--echo # Test that index can't be used for lookup due to type conversion
--echo # (comparing char and int)
SELECT * FROM t1 WHERE url=1;
EXPLAIN EXTENDED SELECT * FROM t1 WHERE url=1;
--echo
--echo # Test that index can't be used for lookup due to collation mismatch
SELECT * FROM t1 WHERE url='1' collate latin1_german2_ci;
EXPLAIN EXTENDED SELECT * FROM t1 WHERE url='1' collate latin1_german2_ci;

--echo
--echo # Normally, range access on primary key is done
EXPLAIN EXTENDED SELECT * FROM t1 WHERE url>'3';
--echo
--echo # Test that range access on index can't be done due to type conversion
--echo # (comparing char and int)
SELECT * FROM t1 WHERE url>3;
EXPLAIN EXTENDED SELECT * FROM t1 WHERE url>3;
--echo
--echo # Test that range access on index can't be done due to collation mismatch
SELECT * FROM t1 WHERE url>'3' collate latin1_german2_ci;
EXPLAIN EXTENDED SELECT * FROM t1 WHERE url>'3' collate latin1_german2_ci;

--echo
DROP TABLE t1;

--echo # End BUG#53562

--echo #
--echo # Bug#11829785 EXPLAIN EXTENDED CRASH WITH RIGHT OUTER JOIN, SUBQUERIES
--echo #

CREATE TABLE t1(a INT);

INSERT INTO t1 VALUES (0), (0);

PREPARE s FROM 
'EXPLAIN EXTENDED
SELECT SUBSTRING(1, (SELECT 1 FROM t1 a1 RIGHT OUTER JOIN t1 ON 0)) AS d
FROM t1 WHERE 0 > ANY (SELECT @a FROM t1)';

--error ER_SUBQUERY_NO_1_ROW
EXECUTE s;

DEALLOCATE PREPARE s;
DROP TABLE t1;

--echo #
--echo # WL#4897: Add EXPLAIN INSERT/UPDATE/DELETE
--echo # 
--echo # Coverage tests after code refactoring
--echo #

CREATE TABLE t1 (a INT);
INSERT INTO t1 VALUES (1),(2),(3);
CREATE TABLE t2 (a INT);
INSERT INTO t2 VALUES (3),(4),(5);

# LIMIT <offset> is for SELECT, not for EXPLAIN OUTPUT:
--echo # EXPLAIN must return 3 rows:
eval EXPLAIN $FORMAT SELECT SQL_CALC_FOUND_ROWS * FROM t1 UNION SELECT * FROM t2 LIMIT 2,2;

DROP TABLE t1, t2;

--echo # End WL#4897

--echo End of 6.0 tests.
 