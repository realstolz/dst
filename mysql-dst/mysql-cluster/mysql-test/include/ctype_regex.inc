#
# To test a desired collation, set session.collation_connection to
# this collation before including this file
#

--disable_warnings
drop table if exists t1;
--enable_warnings

#
# Create a table with two varchar(64) null-able column,
# using current values of
# @@character_set_connection and  @@collation_connection.
#

create table t1 as
select repeat(' ', 64) as s1, repeat(' ',64) as s2
union
select null, null;
--disable_query_log
let $default_engine = `select @@SESSION.default_storage_engine`;
--enable_query_log
--replace_result $default_engine default_engine
show create table t1;
delete from t1;

insert into t1 values('aaa','aaa');
insert into t1 values('aaa|qqq','qqq');
insert into t1 values('gheis','^[^a-dXYZ]+$');
insert into t1 values('aab','^aa?b');
insert into t1 values('Baaan','^Ba*n');
insert into t1 values('aaa','qqq|aaa');
insert into t1 values('qqq','qqq|aaa');

insert into t1 values('bbb','qqq|aaa');
insert into t1 values('bbb','qqq');
insert into t1 values('aaa','aba');

insert into t1 values(null,'abc');
insert into t1 values('def',null);
insert into t1 values(null,null);
insert into t1 values('ghi','ghi[');

select HIGH_PRIORITY s1 regexp s2 from t1;

drop table t1;
 