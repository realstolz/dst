connection master;
INSERT INTO t1 VALUES(42,1,'Testing MySQL databases is a cool ', 'Must make it bug free for the customer',654321.4321,15.21,0,1965,"2005-11-14");
select id,hex(b1),vc,bc,d,f,total,y,t from t1 order by id;
sync_slave_with_master;
select id,hex(b1),vc,bc,d,f,total,y,t from t1 order by id;
connection master;
UPDATE t1 SET b1 = 0, t="2005-09-09" WHERE b1 = 1;
select id,hex(b1),vc,bc,d,f,total,y,t from t1 order by id;
sync_slave_with_master;
select id,hex(b1),vc,bc,d,f,total,y,t from t1 order by id;
connection master;
DELETE FROM t1 WHERE id = 42;
select id,hex(b1),vc,bc,d,f,total,y,t from t1 order by id;
sync_slave_with_master;
select id,hex(b1),vc,bc,d,f,total,y,t from t1 order by id;
connection master;
 