#
# Input(s) :
# $server_default_authentication_plugin
#

--echo # Restart the server with $server_default_authentication_plugin as default authnetication plugin
let $restart_file= $MYSQLTEST_VARDIR/tmp/mysqld.1.expect;
--exec echo "wait" > $restart_file
--shutdown_server
--source include/wait_until_disconnected.inc
--exec echo "restart:--default-authentication-plugin=$server_default_authentication_plugin" > $restart_file
--enable_reconnect
--source include/wait_until_connected_again.inc
--disable_reconnect


--echo # Create users
CREATE USER qa_test_1_user IDENTIFIED WITH qa_auth_interface AS 'qa_test_1_dest';
CREATE USER qa_test_1_dest IDENTIFIED BY 'dest_passwd';
GRANT ALL PRIVILEGES ON test_user_db.* TO qa_test_1_dest identified by 'dest_passwd';
GRANT PROXY ON qa_test_1_dest TO qa_test_1_user;

CREATE USER native@localhost IDENTIFIED WITH 'mysql_native_password' BY 'abcd';
CREATE USER sha256@localhost IDENTIFIED WITH 'sha256_password' BY 'abcd';

--echo # Connection tests

--exec $MYSQL $PLUGIN_AUTH_OPT --protocol=TCP --host=127.0.0.1 -P$MASTER_MYPORT -uqa_test_1_user -pqa_test_1_dest --ssl-mode=DISABLED --server-public-key-path=$MYSQL_TEST_DIR/std_data/rsa_public_key.pem -e "SELECT CURRENT_USER()" 2>&1
--exec $MYSQL $PLUGIN_AUTH_OPT --default-auth=mysql_native_password --protocol=TCP --host=127.0.0.1 -P$MASTER_MYPORT -uqa_test_1_user -pqa_test_1_dest --ssl-mode=DISABLED --server-public-key-path=$MYSQL_TEST_DIR/std_data/rsa_public_key.pem -e "SELECT CURRENT_USER()" 2>&1
--exec $MYSQL $PLUGIN_AUTH_OPT --default-auth=sha256_password --protocol=TCP --host=127.0.0.1 -P$MASTER_MYPORT -uqa_test_1_user -pqa_test_1_dest --ssl-mode=DISABLED --server-public-key-path=$MYSQL_TEST_DIR/std_data/rsa_public_key.pem -e "SELECT CURRENT_USER()" 2>&1
FLUSH PRIVILEGES;

--exec $MYSQL $PLUGIN_AUTH_OPT --protocol=TCP --host=127.0.0.1 -P$MASTER_MYPORT -uqa_test_1_user -pqa_test_1_dest --ssl-mode=DISABLED --server-public-key-path=$MYSQL_TEST_DIR/std_data/rsa_public_key.pem -e "SELECT CURRENT_USER()" 2>&1
FLUSH PRIVILEGES;
--exec $MYSQL $PLUGIN_AUTH_OPT --default-auth=mysql_native_password --protocol=TCP --host=127.0.0.1 -P$MASTER_MYPORT -uqa_test_1_user -pqa_test_1_dest --ssl-mode=DISABLED --server-public-key-path=$MYSQL_TEST_DIR/std_data/rsa_public_key.pem -e "SELECT CURRENT_USER()" 2>&1
FLUSH PRIVILEGES;
--exec $MYSQL $PLUGIN_AUTH_OPT --default-auth=sha256_password --protocol=TCP --host=127.0.0.1 -P$MASTER_MYPORT -uqa_test_1_user -pqa_test_1_dest --ssl-mode=DISABLED --server-public-key-path=$MYSQL_TEST_DIR/std_data/rsa_public_key.pem -e "SELECT CURRENT_USER()" 2>&1
FLUSH PRIVILEGES;


--exec $MYSQL $PLUGIN_AUTH_OPT --protocol=TCP --host=127.0.0.1 -P$MASTER_MYPORT -unative -pabcd --ssl-mode=DISABLED --server-public-key-path=$MYSQL_TEST_DIR/std_data/rsa_public_key.pem -e "SELECT CURRENT_USER()" 2>&1
--exec $MYSQL $PLUGIN_AUTH_OPT --default-auth=mysql_native_password --protocol=TCP --host=127.0.0.1 -P$MASTER_MYPORT -unative -pabcd --ssl-mode=DISABLED --server-public-key-path=$MYSQL_TEST_DIR/std_data/rsa_public_key.pem -e "SELECT CURRENT_USER()" 2>&1
--exec $MYSQL $PLUGIN_AUTH_OPT --default-auth=sha256_password --protocol=TCP --host=127.0.0.1 -P$MASTER_MYPORT -unative -pabcd --ssl-mode=DISABLED --server-public-key-path=$MYSQL_TEST_DIR/std_data/rsa_public_key.pem -e "SELECT CURRENT_USER()" 2>&1
FLUSH PRIVILEGES;


--exec $MYSQL $PLUGIN_AUTH_OPT --protocol=TCP --host=127.0.0.1 -P$MASTER_MYPORT -usha256 -pabcd --ssl-mode=DISABLED --server-public-key-path=$MYSQL_TEST_DIR/std_data/rsa_public_key.pem -e "SELECT CURRENT_USER()" 2>&1
--exec $MYSQL $PLUGIN_AUTH_OPT --default-auth=mysql_native_password --protocol=TCP --host=127.0.0.1 -P$MASTER_MYPORT -usha256 -pabcd --ssl-mode=DISABLED --server-public-key-path=$MYSQL_TEST_DIR/std_data/rsa_public_key.pem -e "SELECT CURRENT_USER()" 2>&1
--exec $MYSQL $PLUGIN_AUTH_OPT --default-auth=sha256_password --protocol=TCP --host=127.0.0.1 -P$MASTER_MYPORT -usha256 -pabcd --ssl-mode=DISABLED --server-public-key-path=$MYSQL_TEST_DIR/std_data/rsa_public_key.pem -e "SELECT CURRENT_USER()" 2>&1
FLUSH PRIVILEGES;

--echo # Change user tests

--connect(qa_test_1_conn, localhost, qa_test_1_user, qa_test_1_dest,,,,SSL)
--change_user sha256, abcd
SELECT CURRENT_USER();
--change_user
SELECT CURRENT_USER();

--connect(native_conn, localhost, native, abcd,,,,SSL)
--change_user qa_test_1_user, qa_test_1_dest
SELECT CURRENT_USER();
--change_user sha256, abcd
SELECT CURRENT_USER();
--change_user
SELECT CURRENT_USER();

--connect(sha256_conn, localhost, sha256, abcd,,,,SSL)
--change_user qa_test_1_user, qa_test_1_dest
SELECT CURRENT_USER();
--change_user
SELECT CURRENT_USER();

--connection default
--disconnect qa_test_1_conn
--disconnect native_conn
--disconnect sha256_conn

--echo # Drop users
DROP USER qa_test_1_user;
DROP USER qa_test_1_dest;
DROP USER native@localhost;
DROP USER sha256@localhost;

--source include/force_restart.inc
 