select @@collation_connection;
select hex(weight_string('a' LEVEL 3));
select hex(weight_string('A' LEVEL 3));
select hex(weight_string('abc' LEVEL 3));
select hex(weight_string('abc' as char(2) LEVEL 3));
select hex(weight_string('abc' as char(3) LEVEL 3));
select hex(weight_string('Abc' as char(5) LEVEL 3));
select hex(weight_string('Abc' as char(5) LEVEL 3 REVERSE));
select hex(weight_string('Abc' as char(5) LEVEL 3 DESC));
select hex(weight_string('Abc' as char(5) LEVEL 3 DESC REVERSE));
 