#
# Check if server has support for loading plugins
#
if (`SELECT @@have_dynamic_loading != 'YES'`) {
  --skip simple parser requires dynamic loading
}

#
# Check if the variable SIMPLE_PARSER is set
#
if (!$SIMPLE_PARSER) {
  --skip simple parser requires the environment variable \$SIMPLE_PARSER to be set (normally done by mtr)
}

#
# Check if --plugin-dir was setup for simple parser
#
if (`SELECT CONCAT('--plugin-dir=', REPLACE(@@plugin_dir, '\\\\', '/')) != '$SIMPLE_PARSER_OPT/'`) {
  --skip simple parser requires that --plugin-dir is set to the udf plugin dir (either the .opt file does not contain \$UDF_EXAMPLE_LIB_OPT or another plugin is in use)
}
 