#
# Test TRIM() function with Japanese characters sjis encoding
#

--source include/have_sjis.inc
--character_set sjis
SET NAMES sjis;
SET character_set_database = sjis;

--disable_warnings
DROP TABLE IF EXISTS `Ｔ１`;
DROP TABLE IF EXISTS `Ｔ２`;
DROP TABLE IF EXISTS `Ｔ３`;
--enable_warnings
--replace_result $engine <engine_to_be_tested>
eval CREATE TABLE `Ｔ１` (`Ｃ１` CHAR(12), INDEX(`Ｃ１`)) DEFAULT CHARSET = sjis ENGINE = $engine;
--replace_result $engine <engine_to_be_tested>
eval CREATE TABLE `Ｔ２` (`Ｃ１` CHAR(12), INDEX(`Ｃ１`)) DEFAULT CHARSET = sjis ENGINE = $engine;
--replace_result $engine <engine_to_be_tested>
eval CREATE TABLE `Ｔ３` (`Ｃ１` CHAR(12), INDEX(`Ｃ１`)) DEFAULT CHARSET = sjis ENGINE = $engine;

# Nothing to trim
INSERT INTO `Ｔ１` VALUES ('ｱｲｳｴｵ');
# Test data for TRIM(TRAILING [remstr] FROM str)
INSERT INTO `Ｔ１` VALUES ('ｱｲｳｴｵｶ');
INSERT INTO `Ｔ１` VALUES ('ｱｲｳｴｵｶｶ');
INSERT INTO `Ｔ１` VALUES ('ｱｲｳｴｵｶｶｶ');
# Test data for TRIM(LEADING [remstr] FROM str)
INSERT INTO `Ｔ１` VALUES ('ｶｱｲｳｴｵ');
INSERT INTO `Ｔ１` VALUES ('ｶｶｱｲｳｴｵ');
INSERT INTO `Ｔ１` VALUES ('ｶｶｶｱｲｳｴｵ');
# Test data for TRIM(BOTH [remstr] FROM str)
INSERT INTO `Ｔ１` VALUES ('ｶｶｶｱｲｳｴｵｶｶｶ');
# Test data for TRIM without [remstr] (remove spaces) 
INSERT INTO `Ｔ１` VALUES ('   ｱｲｳｴｵ   ');
INSERT INTO `Ｔ２` VALUES ('あいうえお');
INSERT INTO `Ｔ２` VALUES ('あいうえおか');
INSERT INTO `Ｔ２` VALUES ('あいうえおかか');
INSERT INTO `Ｔ２` VALUES ('あいうえおかかか');
INSERT INTO `Ｔ２` VALUES ('かあいうえお');
INSERT INTO `Ｔ２` VALUES ('かかあいうえお');
INSERT INTO `Ｔ２` VALUES ('かかかあいうえお');
INSERT INTO `Ｔ２` VALUES ('かかかあいうえおかかか');
INSERT INTO `Ｔ２` VALUES ('   あいうえお   ');
INSERT INTO `Ｔ３` VALUES ('ソ十表予構');
INSERT INTO `Ｔ３` VALUES ('ソ十表予構暴');
INSERT INTO `Ｔ３` VALUES ('ソ十表予構暴暴');
INSERT INTO `Ｔ３` VALUES ('ソ十表予構暴暴暴');
INSERT INTO `Ｔ３` VALUES ('暴ソ十表予構');
INSERT INTO `Ｔ３` VALUES ('暴暴ソ十表予構');
INSERT INTO `Ｔ３` VALUES ('暴暴暴ソ十表予構');
INSERT INTO `Ｔ３` VALUES ('暴暴暴ソ十表予構暴暴暴');
INSERT INTO `Ｔ３` VALUES ('   ソ十表予構   ');

SELECT `Ｃ１`,TRIM(TRAILING 'ｶ' FROM `Ｃ１`) FROM `Ｔ１`;
SELECT `Ｃ１`,TRIM(LEADING 'ｶ' FROM `Ｃ１`) FROM `Ｔ１`;
SELECT `Ｃ１`,TRIM(BOTH 'ｶ' FROM `Ｃ１`) FROM `Ｔ１`;
SELECT `Ｃ１`,TRIM(`Ｃ１`) FROM `Ｔ１`;
SELECT `Ｃ１`,TRIM(TRAILING 'か' FROM `Ｃ１`) FROM `Ｔ２`;
SELECT `Ｃ１`,TRIM(LEADING 'か' FROM `Ｃ１`) FROM `Ｔ２`;
SELECT `Ｃ１`,TRIM(BOTH 'か' FROM `Ｃ１`) FROM `Ｔ２`;
SELECT `Ｃ１`,TRIM(`Ｃ１`) FROM `Ｔ２`;
SELECT `Ｃ１`,TRIM(TRAILING '暴' FROM `Ｃ１`) FROM `Ｔ３`;
SELECT `Ｃ１`,TRIM(LEADING '暴' FROM `Ｃ１`) FROM `Ｔ３`;
SELECT `Ｃ１`,TRIM(BOTH '暴' FROM `Ｃ１`) FROM `Ｔ３`;
SELECT `Ｃ１`,TRIM(`Ｃ１`) FROM `Ｔ３`;

DROP TABLE `Ｔ１`;
DROP TABLE `Ｔ２`;
DROP TABLE `Ｔ３`;
 