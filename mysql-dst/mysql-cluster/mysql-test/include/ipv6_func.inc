eval SET @nip= inet_aton('$IPv6');
CREATE TABLE t1 (c1 varbinary(16), c2 varchar(16), c3 binary(16), c4 char(16), c5 long, c6 int);
INSERT INTO t1 VALUES (@nip,@nip,@nip,@nip,@nip,@nip);
SELECT inet_ntoa(c1) FROM t1;
SELECT inet_ntoa(CAST(c1 AS UNSIGNED)) FROM t1;
SELECT inet_ntoa(c2) FROM t1;
SELECT inet_ntoa(CAST(c2 AS UNSIGNED)) FROM t1;
SELECT inet_ntoa(c3) FROM t1;
SELECT inet_ntoa(CAST(c3 AS UNSIGNED)) FROM t1;
SELECT inet_ntoa(c4) FROM t1;
SELECT inet_ntoa(CAST(c4 AS UNSIGNED)) FROM t1;
SELECT inet_ntoa(c5) FROM t1;
SELECT inet_ntoa(CAST(c5 AS UNSIGNED)) FROM t1;
SELECT inet_ntoa(c6) FROM t1;
DROP TABLE IF EXISTS t1;
SELECT @nip;
SELECT inet_ntoa(@nip);

eval SET @nip6= inet6_aton('$IPv6');
CREATE TABLE t1 (c1 varbinary(16), c2 varchar(16), c3 binary(16), c4 char(16), c5 long, c6 int);
INSERT IGNORE INTO t1 VALUES (@nip6,@nip6,@nip6,@nip6,@nip6,@nip6);
SELECT inet6_ntoa(c1) FROM t1;
SELECT inet6_ntoa(CAST(c1 AS BINARY(16))) FROM t1;
SELECT inet6_ntoa(c2) FROM t1;
SELECT inet6_ntoa(CAST(c2 AS BINARY(16))) FROM t1;
SELECT inet6_ntoa(c3) FROM t1;
SELECT inet6_ntoa(CAST(c3 AS BINARY(16))) FROM t1;
SELECT inet6_ntoa(c4) FROM t1;
SELECT inet6_ntoa(CAST(c4 AS BINARY(16))) FROM t1;
SELECT inet6_ntoa(c5) FROM t1;
SELECT inet6_ntoa(CAST(c5 AS BINARY(16))) FROM t1;
SELECT inet6_ntoa(c6) FROM t1;
SELECT inet6_ntoa(CAST(c6 AS BINARY(16))) FROM t1;
DROP TABLE IF EXISTS t1;
SELECT inet6_ntoa(@nip6);

 