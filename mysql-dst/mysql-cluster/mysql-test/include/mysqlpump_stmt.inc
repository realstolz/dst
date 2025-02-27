# Wl7755


# Creating DB's and populating different types of data init for MYSQLPUMP testing.

Create database db1;
Create database db2;
CREATE DATABASE db3;
CREATE DATABASE db1_1gb;

USE db1;

CREATE TABLE dt1 (a INT);
CREATE TABLE dt2 (a INT, b VARCHAR(10), primary key(a));
CREATE TABLE dt3 (`a"b"` char(2));

INSERT INTO dt1 VALUES (289), (298), (234), (456), (789);
INSERT INTO dt2 VALUES (1, "on"), (2, "off"), (10, "pol"), (12, "meg");
INSERT INTO dt3 VALUES ("1\""), ("\"2");

CREATE TABLE categories(
   c_id int not null auto_increment primary key,
   c_name varchar(255) not null,
   c_description text,
   UNIQUE(c_id, c_name)
) ENGINE=InnoDB;

CREATE TABLE products(
   p_id int not null auto_increment primary key,
   p_name varchar(355) not null,
   p_price decimal,
   c_id int not null,
   FOREIGN KEY fk_categories(c_id)
   REFERENCES categories(c_id)
   ON UPDATE CASCADE
   ON DELETE RESTRICT
)ENGINE=InnoDB;

CREATE TABLE vendors(
    v_id int not null auto_increment primary key,
    v_name varchar(255)
)ENGINE=InnoDB;

ALTER TABLE products
ADD COLUMN v_id int not null AFTER c_id;

ALTER TABLE products
ADD FOREIGN KEY fk_vendor(v_id)
REFERENCES vendors(v_id)
ON DELETE NO ACTION
ON UPDATE CASCADE;

CREATE VIEW v1 AS SELECT * FROM dt2;
CREATE VIEW v2 AS SELECT * FROM dt1;
CREATE VIEW v3 AS SELECT v1.*,v2.a as X FROM v1,v2;
CREATE VIEW v11 AS SELECT * FROM v3;

CREATE TEMPORARY TABLE t14 (id INT);

CREATE TRIGGER trig1 BEFORE INSERT ON dt1 FOR EACH ROW SET NEW.a = 1;
CREATE TRIGGER trig2 BEFORE UPDATE ON dt1 FOR EACH ROW SET NEW.a = 999;

CREATE EVENT ev1 ON SCHEDULE AT '2030-01-01 00:00:00' DO SET @a=5;
CREATE EVENT ev2 ON SCHEDULE EVERY 5 HOUR DO SELECT 1;

DELIMITER |;
CREATE PROCEDURE db1t_cache()
BEGIN
  SET @A= 20;
  SELECT * FROM dt1;
  SELECT * FROM dt1;
  PREPARE x FROM 'SELECT 1';
  SELECT * FROM dt1;
  SELECT * FROM dt1;
END ;|
DELIMITER ;|

DELIMITER |;
CREATE FUNCTION db1_f1 () RETURNS int
BEGIN
SET NAMES 'big5';
RETURN (SELECT COUNT(*) FROM dt1);
END ;|
DELIMITER ;|

# db2
USE db2;

CREATE TABLE d2t1 (
   id INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
   fname VARCHAR(200) NOT NULL,
   lname  VARCHAR(60) ,
   phone VARCHAR(15) DEFAULT NULL,
   created TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
   eid tinyint NOT NULL DEFAULT 011,
  PRIMARY KEY (id),
  UNIQUE(id,fname)
)
ENGINE = InnoDB;

INSERT INTO d2t1 VALUES (1, "abc","","283682846",default,11), (2, "ed","adw\5\/fdfiw","wfdf%&$#^dif",default,26);
INSERT INTO d2t1 (fname) VALUES("wgdugw");
INSERT INTO d2t1 (fname,lname) VALUES("wgdugw","gdwsjdg");
INSERT INTO d2t1 (fname,phone) VALUES("wgdugw","+2937280802*");

ALTER TABLE d2t1 ADD INDEX d2t1_index4 (fname, eid ASC);
ALTER TABLE d2t1 ADD INDEX d2t1_index5 (eid, fname);


CREATE TABLE d2t2 (
   id INT PRIMARY KEY AUTO_INCREMENT,
   fname VARCHAR(200) NOT NULL,
   lname  VARCHAR(60) ,
   phone VARCHAR(15) DEFAULT NULL,
   created TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
   eid tinyint NOT NULL DEFAULT 011,
   KEY d2t2_index2 (fname,eid),
  KEY d2t2_index3 (Id,eid)
)
ENGINE = InnoDB;

INSERT INTO d2t2 VALUES (1, "abc","","283682846",default,13), (2, "ed","adw\5\/fdfiw","wfdf%&$#^dif",default,26);
INSERT INTO d2t2 (fname) VALUES("wgdugw");
INSERT INTO d2t2 (fname,lname) VALUES("wgdugw","gdwsjdg");
INSERT INTO d2t2 (fname,phone) VALUES("wgdugw","+2937280802*");

DELIMITER |;
CREATE PROCEDURE d2t_cache()
BEGIN
  SET @A= 20;
  SELECT * FROM d2t1;
  SELECT * FROM d2t1;
  PREPARE x FROM 'SELECT 1';
  SELECT * FROM d2t1;
  SELECT * FROM d2t1;
END ;|
DELIMITER ;|

DELIMITER |;
CREATE FUNCTION d2f1 () RETURNS int
BEGIN
SET NAMES 'big5';
RETURN (SELECT COUNT(*) FROM d2t1);
END ;|
DELIMITER ;|

--echo

# db3

USE db3;

CREATE TABLE d3t1 (a INT);
CREATE TABLE d3t2 (a INT, b VARCHAR(10), primary key(a));
CREATE TABLE d3t3 (`a"b"` char(2));
CREATE TABLE d3t4 (`x"z"` int,xyz varchar(20),notes text);

ALTER TABLE d3t2 ADD INDEX d3t1_index4 (b ASC);
ALTER TABLE d3t4 ADD INDEX d3t4_index5 (`x"z"`,xyz,notes(3));


INSERT INTO d3t1 VALUES (289), (298), (234), (456), (789);
INSERT INTO d3t2 VALUES (1, "on"), (2, "off"), (10, "pol"), (12, "meg");
INSERT INTO d3t3 VALUES ("1\""), ("\"2");
INSERT INTO d3t4 VALUES (1, "on","note"), (2, "off","note1"), (10, "pol","note3"), (12, "meg","note4");

CREATE TABLE parent (
    id INT NOT NULL,
    PRIMARY KEY (id)
) ENGINE=INNODB;

CREATE TABLE child (
    id INT,
    parent_id INT,
    INDEX par_ind (parent_id),
    FOREIGN KEY (parent_id)
        REFERENCES parent(id)
        ON DELETE CASCADE
) ENGINE=INNODB;

CREATE TABLE product (
    category INT NOT NULL, id INT NOT NULL,
    price DECIMAL,
    PRIMARY KEY(category, id)
)   ENGINE=INNODB;

CREATE TABLE customer (
    id INT NOT NULL,
    PRIMARY KEY (id)
)   ENGINE=INNODB;

CREATE TABLE product_order (
    no INT NOT NULL AUTO_INCREMENT,
    product_category INT NOT NULL,
    product_id INT NOT NULL,
    customer_id INT NOT NULL,
    PRIMARY KEY(no),
    INDEX (product_category, product_id),
    INDEX (customer_id),
    FOREIGN KEY (product_category, product_id)
      REFERENCES product(category, id)
      ON UPDATE CASCADE ON DELETE RESTRICT,
    FOREIGN KEY (customer_id)
      REFERENCES customer(id)
)ENGINE=INNODB;


CREATE VIEW v1 AS SELECT * FROM d3t2;
CREATE VIEW v2 AS SELECT * FROM d3t1;
CREATE VIEW v3 AS SELECT v1.*,v2.a as X FROM v1,v2;
CREATE VIEW v11 AS SELECT * FROM v3;

CREATE TEMPORARY TABLE t14 (id INT);

CREATE TRIGGER trig3 BEFORE INSERT ON d3t1 FOR EACH ROW SET NEW.a = 1;
CREATE TRIGGER trig4 BEFORE UPDATE ON d3t1 FOR EACH ROW SET NEW.a = 999;

CREATE EVENT ev3 ON SCHEDULE AT '2030-01-01 00:00:00' DO SET @a=5;
CREATE EVENT ev4 ON SCHEDULE EVERY 5 HOUR DO SELECT 1;

DELIMITER |;
CREATE PROCEDURE d3t_cache()
BEGIN
  SET @A= 20;
  SELECT * FROM d3t1;
  SELECT * FROM d3t1;
  PREPARE x FROM 'SELECT 1';
  SELECT * FROM d3t1;
  SELECT * FROM d3t1;
END ;|
DELIMITER ;|

DELIMITER |;
CREATE FUNCTION d3f1 () RETURNS int
BEGIN
SET NAMES 'big5';
RETURN (SELECT COUNT(*) FROM d3t1);
END ;|
DELIMITER ;|

use db1_1gb;

CREATE TABLE `db1_1gb`.`customer` (
`C_CUSTKEY` int(11) NOT NULL,
`C_NAME` varchar(25) NOT NULL,
`C_ADDRESS` varchar(40) NOT NULL,
`C_nationKEY` mediumint(9) NOT NULL,
`C_PHONE` char(15) NOT NULL,
`C_ACCTBAL` double(8,2) NOT NULL,
`C_MKTSEGMENT` char(10) NOT NULL,
`C_COMMENT` varchar(117) NOT NULL,
`C_SUMMARY` mediumblob NOT NULL,
`C_ISACTIVE` tinyint(1) DEFAULT NULL,
PRIMARY KEY (`C_CUSTKEY`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

ALTER TABLE `db1_1gb`.`customer` ADD FULLTEXT (`C_COMMENT`);

INSERT INTO `db1_1gb`.`customer` VALUES (1,"Customer#000000001","IVhzIApeRb ot,c,E",15,"25-989-741-2988",711.56,"BUILDING","to the even, regular platelets. regular, ironic epitaphs nag e","to the even, regular platelets. regular, ironic epitaphs nag e",1),(2,"Customer#000000002","XSTf4,NCwDVaWNe6tEgvwfmRchLXak",13,"23-768-687-3665",121.65,"AUTOMOBILE","l accounts. blithely ironic theodolites integrate boldly: caref","l accounts. blithely ironic theodolites integrate boldly: caref",1);

ALTER TABLE `db1_1gb`.`customer` ADD KEY `c_name_index` (`C_NAME`);
ALTER TABLE `db1_1gb`.`customer` ADD KEY `c_phone_index` (`C_PHONE`);

CREATE TABLE `db1_1gb`.`lineitem` (
`L_ORDERKEY` int(11) NOT NULL,
`L_partKEY` int(11) NOT NULL,
`L_SUPPKEY` int(11) NOT NULL,
`L_LINENUMBER` bigint(20) NOT NULL,
`L_QUANTITY` double NOT NULL,
`L_EXTENDEDPRICE` decimal(15,2) NOT NULL,
`L_DISCOUNT` float NOT NULL,
`L_TAX` decimal(15,2) NOT NULL,
`L_RETURNFLAG` char(1) DEFAULT NULL,
`L_LINESTATUS` tinytext NOT NULL,
`L_SHIPDATE` datetime NOT NULL,
`L_COMMITDATE` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
`L_RECEIPTDATE` date DEFAULT NULL,
`L_SHIPINSTRUCT` text NOT NULL,
`L_SHIPMODE` enum('ROAD','RAIL','SEA','AIR','TRUCK','MAIL','FOB','SHIP','REG AIR') DEFAULT NULL,
`L_COMMENT` longtext NOT NULL,
`L_TAGS` char(50) DEFAULT NULL,
PRIMARY KEY (`L_ORDERKEY`,`L_LINENUMBER`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

ALTER TABLE `db1_1gb`.`lineitem` ADD KEY `l_quantity_index` (`L_QUANTITY`);
ALTER TABLE `db1_1gb`.`lineitem` ADD KEY `l_discount_index` (`L_DISCOUNT`);
ALTER TABLE `db1_1gb`.`lineitem` ADD KEY `l_shipinstruct_index` (`L_SHIPINSTRUCT`(10));

INSERT INTO `db1_1gb`.`lineitem` VALUES (5987,23,2,1,1,923.02,0.01,0.04,"N","O","1996-09-13 00:00:00","1996-10-29 00:00:00","1996-09-21","DELIVER IN PERSON","REG AIR","refully final excuses haggle furiously ag","REG AIR"),(5987,176,5,2,20,21523.40,0.1,0.06,"N","O","1996-11-28 00:00:00","1996-09-17 00:00:00","1996-12-05","TAKE BACK RETURN","RAIL","ing excuses nag quickly always bold","RAIL");

CREATE TABLE `db1_1gb`.`nation` (
`N_nationKEY` int(11) NOT NULL,
`N_NAME` char(25) NOT NULL,
`N_regionKEY` int(11) NOT NULL,
`N_COMMENT` mediumtext,
PRIMARY KEY (`N_nationKEY`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

INSERT INTO `db1_1gb`.`nation` VALUES (0,"ALGERIA",0," haggle. carefully final deposits detect slyly agai"),(1,"ARGENTINA",1,"al foxes promise slyly according to the regular accounts. bold requests alon"),(2,"BRAZIL",1,"y alongside of the pending deposits. carefully special packages are about the ironic forges. slyly special "),(3,"CANADA",1,"eas hang ironic, silent packages. slyly regular packages are furiously over the tithes. fluffily bold");

CREATE TABLE `db1_1gb`.`orders` (
`O_ORDERKEY` int(11) NOT NULL,
`O_CUSTKEY` int(11) NOT NULL,
`O_ordersTATUS` char(20) NOT NULL,
`O_TOTALPRICE` decimal(15,2) NOT NULL,
`O_ORDERDATE` date NOT NULL,
`O_ORDERPRIORITY` char(15) NOT NULL,
`O_CLERK` char(15) NOT NULL,
`O_SHIPPRIORITY` tinyint(4) NOT NULL,
`O_COMMENT` blob NOT NULL,
`O_ORDERYEAR` year(4) NOT NULL,
`O_SUMMARY` longblob NOT NULL,
PRIMARY KEY (`O_ORDERKEY`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

INSERT INTO `db1_1gb`.`orders` VALUES (4001,115,"O",95929.46,"1997-05-15","3-MEDIUM","Clerk#000000878",0,"detect. asymptotes sleep furio",2013,"detect. asymptotes sleep furio"),(4002,104,"O",76518.11,"1997-04-08","5-LOW","Clerk#000000097",0," regular braids are. furiously even patterns agains",2013," regular braids are. furiously even patterns agains"),(4003,112,"F",17603.01,"1993-01-27","1-URGENT","Clerk#000000177",0," blithe theodolites are slyly. slyly silent accounts toward",2013," blithe theodolites are slyly. slyly silent accounts toward"),(4004,70,"F",220715.14,"1993-05-07","3-MEDIUM","Clerk#000000273",0,"accounts among the blithely regular sentiments ",2013,"accounts among the blithely regular sentiments ");

ALTER TABLE `db1_1gb`.`orders` ADD KEY `o_totalprice_index` (`O_TOTALPRICE`);
ALTER TABLE `db1_1gb`.`orders` ADD KEY `o_orderdate_index` (`O_ORDERDATE`);
ALTER TABLE `db1_1gb`.`orders` ADD KEY `o_comment_index` (`O_COMMENT`(10));

CREATE TABLE `db1_1gb`.`part` (
`P_partKEY` int(11) NOT NULL,
`P_NAME` varchar(55) NOT NULL,
`P_MFGR` char(25) NOT NULL,
`P_BRAND` binary(16) NOT NULL,
`P_TYPE` varbinary(128) NOT NULL,
`P_SIZE` smallint(6) NOT NULL,
`P_CONTAINER` char(10) NOT NULL,
`P_RETAILPRICE` decimal(15,2) NOT NULL,
`P_COMMENT` varchar(23) NOT NULL,
`P_SUMMARY` tinyblob NOT NULL,
PRIMARY KEY (`P_partKEY`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `db1_1gb`.`partsupp` (
`PS_partKEY` int(11) NOT NULL,
`PS_SUPPKEY` int(11) NOT NULL,
`PS_AVAILQTY` int(11) NOT NULL,
`PS_SUPPLYCOST` float NOT NULL,
`PS_COMMENT` varchar(199) NOT NULL,
PRIMARY KEY (`PS_partKEY`,`PS_SUPPKEY`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `db1_1gb`.`region` (
`R_regionKEY` int(11) NOT NULL,
`R_NAME` char(25) NOT NULL,
`R_COMMENT` longtext,
PRIMARY KEY (`R_regionKEY`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE `db1_1gb`.`supplier` (
`S_SUPPKEY` int(11) NOT NULL,
`S_NAME` char(25) NOT NULL,
`S_ADDRESS` varchar(40) NOT NULL,
`S_nationKEY` int(11) NOT NULL,
`S_PHONE` char(15) NOT NULL,
`S_ACCTBAL` decimal(15,2) NOT NULL,
`S_COMMENT` varchar(101) NOT NULL,
`S_SUMMARY` blob NOT NULL,
PRIMARY KEY (`S_SUPPKEY`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

 