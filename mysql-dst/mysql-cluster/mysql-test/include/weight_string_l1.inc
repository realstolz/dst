select @@collation_connection;
select hex(weight_string('a' LEVEL 1));
select hex(weight_string('A' LEVEL 1));
select hex(weight_string('abc' LEVEL 1));
select hex(weight_string('abc' as char(2) LEVEL 1));
select hex(weight_string('abc' as char(3) LEVEL 1));
select hex(weight_string('abc' as char(5) LEVEL 1));
select hex(weight_string('abc' as char(5) LEVEL 1 REVERSE));
select hex(weight_string('abc' as char(5) LEVEL 1 DESC));
select hex(weight_string('abc' as char(5) LEVEL 1 DESC REVERSE));
 