select @@collation_connection;
select hex(weight_string('a' LEVEL 4));
select hex(weight_string('A' LEVEL 4));
select hex(weight_string('abc' LEVEL 4));
select hex(weight_string('abc' as char(2) LEVEL 4));
select hex(weight_string('abc' as char(3) LEVEL 4));
select hex(weight_string('abc' as char(5) LEVEL 4));
select hex(weight_string('abc' as char(5) LEVEL 4 REVERSE));
select hex(weight_string('abc' as char(5) LEVEL 4 DESC));
select hex(weight_string('abc' as char(5) LEVEL 4 DESC REVERSE));
 