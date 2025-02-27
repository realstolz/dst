/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_MYSQL_EXPORT_HOME_PB2_BUILD_SB_3_30900853_1538998080_7_DIST_GPL_SQL_SQL_YACC_H_INCLUDED
# define YY_MYSQL_EXPORT_HOME_PB2_BUILD_SB_3_30900853_1538998080_7_DIST_GPL_SQL_SQL_YACC_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int MYSQLdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ABORT_SYM = 258,
    ACCESSIBLE_SYM = 259,
    ACCOUNT_SYM = 260,
    ACTION = 261,
    ADD = 262,
    ADDDATE_SYM = 263,
    AFTER_SYM = 264,
    AGAINST = 265,
    AGGREGATE_SYM = 266,
    ALGORITHM_SYM = 267,
    ALL = 268,
    ALTER = 269,
    ALWAYS_SYM = 270,
    ANALYSE_SYM = 271,
    ANALYZE_SYM = 272,
    AND_AND_SYM = 273,
    AND_SYM = 274,
    ANY_SYM = 275,
    AS = 276,
    ASC = 277,
    ASCII_SYM = 278,
    ASENSITIVE_SYM = 279,
    AT_SYM = 280,
    AUTOEXTEND_SIZE_SYM = 281,
    AUTO_INC = 282,
    AVG_ROW_LENGTH = 283,
    AVG_SYM = 284,
    BACKUP_SYM = 285,
    BEFORE_SYM = 286,
    BEGIN_SYM = 287,
    BETWEEN_SYM = 288,
    BIGINT = 289,
    BINARY = 290,
    BINLOG_SYM = 291,
    BIN_NUM = 292,
    BIT_AND = 293,
    BIT_OR = 294,
    BIT_SYM = 295,
    BIT_XOR = 296,
    BLOB_SYM = 297,
    BLOCK_SYM = 298,
    BOOLEAN_SYM = 299,
    BOOL_SYM = 300,
    BOTH = 301,
    BTREE_SYM = 302,
    BY = 303,
    BYTE_SYM = 304,
    CACHE_SYM = 305,
    CALL_SYM = 306,
    CASCADE = 307,
    CASCADED = 308,
    CASE_SYM = 309,
    CAST_SYM = 310,
    CATALOG_NAME_SYM = 311,
    CHAIN_SYM = 312,
    CHANGE = 313,
    CHANGED = 314,
    CHANNEL_SYM = 315,
    CHARSET = 316,
    CHAR_SYM = 317,
    CHECKSUM_SYM = 318,
    CHECK_SYM = 319,
    CIPHER_SYM = 320,
    CLASS_ORIGIN_SYM = 321,
    CLIENT_SYM = 322,
    CLOSE_SYM = 323,
    COALESCE = 324,
    CODE_SYM = 325,
    COLLATE_SYM = 326,
    COLLATION_SYM = 327,
    COLUMNS = 328,
    COLUMN_SYM = 329,
    COLUMN_FORMAT_SYM = 330,
    COLUMN_NAME_SYM = 331,
    COMMENT_SYM = 332,
    COMMITTED_SYM = 333,
    COMMIT_SYM = 334,
    COMPACT_SYM = 335,
    COMPLETION_SYM = 336,
    COMPRESSED_SYM = 337,
    COMPRESSION_SYM = 338,
    ENCRYPTION_SYM = 339,
    CONCURRENT = 340,
    CONDITION_SYM = 341,
    CONNECTION_SYM = 342,
    CONSISTENT_SYM = 343,
    CONSTRAINT = 344,
    CONSTRAINT_CATALOG_SYM = 345,
    CONSTRAINT_NAME_SYM = 346,
    CONSTRAINT_SCHEMA_SYM = 347,
    CONTAINS_SYM = 348,
    CONTEXT_SYM = 349,
    CONTINUE_SYM = 350,
    CONVERT_SYM = 351,
    COUNT_SYM = 352,
    CPU_SYM = 353,
    CREATE = 354,
    CROSS = 355,
    CUBE_SYM = 356,
    CURDATE = 357,
    CURRENT_SYM = 358,
    CURRENT_USER = 359,
    CURSOR_SYM = 360,
    CURSOR_NAME_SYM = 361,
    CURTIME = 362,
    DATABASE = 363,
    DATABASES = 364,
    DATAFILE_SYM = 365,
    DATA_SYM = 366,
    DATETIME = 367,
    DATE_ADD_INTERVAL = 368,
    DATE_SUB_INTERVAL = 369,
    DATE_SYM = 370,
    DAY_HOUR_SYM = 371,
    DAY_MICROSECOND_SYM = 372,
    DAY_MINUTE_SYM = 373,
    DAY_SECOND_SYM = 374,
    DAY_SYM = 375,
    DEALLOCATE_SYM = 376,
    DECIMAL_NUM = 377,
    DECIMAL_SYM = 378,
    DECLARE_SYM = 379,
    DEFAULT = 380,
    DEFAULT_AUTH_SYM = 381,
    DEFINER_SYM = 382,
    DELAYED_SYM = 383,
    DELAY_KEY_WRITE_SYM = 384,
    DELETE_SYM = 385,
    DESC = 386,
    DESCRIBE = 387,
    DES_KEY_FILE = 388,
    DETERMINISTIC_SYM = 389,
    DIAGNOSTICS_SYM = 390,
    DIRECTORY_SYM = 391,
    DISABLE_SYM = 392,
    DISCARD = 393,
    DISK_SYM = 394,
    DISTINCT = 395,
    DIV_SYM = 396,
    DOUBLE_SYM = 397,
    DO_SYM = 398,
    DROP = 399,
    DUAL_SYM = 400,
    DUMPFILE = 401,
    DUPLICATE_SYM = 402,
    DYNAMIC_SYM = 403,
    EACH_SYM = 404,
    ELSE = 405,
    ELSEIF_SYM = 406,
    ENABLE_SYM = 407,
    ENCLOSED = 408,
    END = 409,
    ENDS_SYM = 410,
    END_OF_INPUT = 411,
    ENGINES_SYM = 412,
    ENGINE_SYM = 413,
    ENUM = 414,
    EQ = 415,
    EQUAL_SYM = 416,
    ERROR_SYM = 417,
    ERRORS = 418,
    ESCAPED = 419,
    ESCAPE_SYM = 420,
    EVENTS_SYM = 421,
    EVENT_SYM = 422,
    EVERY_SYM = 423,
    EXCHANGE_SYM = 424,
    EXECUTE_SYM = 425,
    EXISTS = 426,
    EXIT_SYM = 427,
    EXPANSION_SYM = 428,
    EXPIRE_SYM = 429,
    EXPORT_SYM = 430,
    EXTENDED_SYM = 431,
    EXTENT_SIZE_SYM = 432,
    EXTRACT_SYM = 433,
    FALSE_SYM = 434,
    FAST_SYM = 435,
    FAULTS_SYM = 436,
    FETCH_SYM = 437,
    FILE_SYM = 438,
    FILE_BLOCK_SIZE_SYM = 439,
    FILTER_SYM = 440,
    FIRST_SYM = 441,
    FIXED_SYM = 442,
    FLOAT_NUM = 443,
    FLOAT_SYM = 444,
    FLUSH_SYM = 445,
    FOLLOWS_SYM = 446,
    FORCE_SYM = 447,
    FOREIGN = 448,
    FOR_SYM = 449,
    FORMAT_SYM = 450,
    FOUND_SYM = 451,
    FROM = 452,
    FULL = 453,
    FULLTEXT_SYM = 454,
    FUNCTION_SYM = 455,
    GE = 456,
    GENERAL = 457,
    GENERATED = 458,
    GROUP_REPLICATION = 459,
    GEOMETRYCOLLECTION = 460,
    GEOMETRY_SYM = 461,
    GET_FORMAT = 462,
    GET_SYM = 463,
    GLOBAL_SYM = 464,
    GRANT = 465,
    GRANTS = 466,
    GROUP_SYM = 467,
    GROUP_CONCAT_SYM = 468,
    GT_SYM = 469,
    HANDLER_SYM = 470,
    HASH_SYM = 471,
    HAVING = 472,
    HELP_SYM = 473,
    HEX_NUM = 474,
    HIGH_PRIORITY = 475,
    HOST_SYM = 476,
    HOSTS_SYM = 477,
    HOUR_MICROSECOND_SYM = 478,
    HOUR_MINUTE_SYM = 479,
    HOUR_SECOND_SYM = 480,
    HOUR_SYM = 481,
    IDENT = 482,
    IDENTIFIED_SYM = 483,
    IDENT_QUOTED = 484,
    IF = 485,
    IGNORE_SYM = 486,
    IGNORE_SERVER_IDS_SYM = 487,
    IMPORT = 488,
    INDEXES = 489,
    INDEX_SYM = 490,
    INFILE = 491,
    INITIAL_SIZE_SYM = 492,
    INNER_SYM = 493,
    INOUT_SYM = 494,
    INSENSITIVE_SYM = 495,
    INSERT = 496,
    INSERT_METHOD = 497,
    INSTANCE_SYM = 498,
    INSTALL_SYM = 499,
    INTERVAL_SYM = 500,
    INTO = 501,
    INT_SYM = 502,
    INVOKER_SYM = 503,
    IN_SYM = 504,
    IO_AFTER_GTIDS = 505,
    IO_BEFORE_GTIDS = 506,
    IO_SYM = 507,
    IPC_SYM = 508,
    IS = 509,
    ISOLATION = 510,
    ISSUER_SYM = 511,
    ITERATE_SYM = 512,
    JOIN_SYM = 513,
    JSON_SEPARATOR_SYM = 514,
    JSON_UNQUOTED_SEPARATOR_SYM = 515,
    JSON_SYM = 516,
    KEYS = 517,
    KEY_BLOCK_SIZE = 518,
    KEY_SYM = 519,
    KILL_SYM = 520,
    LANGUAGE_SYM = 521,
    LAST_SYM = 522,
    LE = 523,
    LEADING = 524,
    LEAVES = 525,
    LEAVE_SYM = 526,
    LEFT = 527,
    LESS_SYM = 528,
    LEVEL_SYM = 529,
    LEX_HOSTNAME = 530,
    LIKE = 531,
    LIMIT = 532,
    LINEAR_SYM = 533,
    LINES = 534,
    LINESTRING = 535,
    LIST_SYM = 536,
    LOAD = 537,
    LOCAL_SYM = 538,
    LOCATOR_SYM = 539,
    LOCKS_SYM = 540,
    LOCK_SYM = 541,
    LOGFILE_SYM = 542,
    LOGS_SYM = 543,
    LONGBLOB = 544,
    LONGTEXT = 545,
    LONG_NUM = 546,
    LONG_SYM = 547,
    LOOP_SYM = 548,
    LOW_PRIORITY = 549,
    LT = 550,
    MASTER_AUTO_POSITION_SYM = 551,
    MASTER_BIND_SYM = 552,
    MASTER_CONNECT_RETRY_SYM = 553,
    MASTER_DELAY_SYM = 554,
    MASTER_HOST_SYM = 555,
    MASTER_LOG_FILE_SYM = 556,
    MASTER_LOG_POS_SYM = 557,
    MASTER_PASSWORD_SYM = 558,
    MASTER_PORT_SYM = 559,
    MASTER_RETRY_COUNT_SYM = 560,
    MASTER_SERVER_ID_SYM = 561,
    MASTER_SSL_CAPATH_SYM = 562,
    MASTER_TLS_VERSION_SYM = 563,
    MASTER_SSL_CA_SYM = 564,
    MASTER_SSL_CERT_SYM = 565,
    MASTER_SSL_CIPHER_SYM = 566,
    MASTER_SSL_CRL_SYM = 567,
    MASTER_SSL_CRLPATH_SYM = 568,
    MASTER_SSL_KEY_SYM = 569,
    MASTER_SSL_SYM = 570,
    MASTER_SSL_VERIFY_SERVER_CERT_SYM = 571,
    MASTER_SYM = 572,
    MASTER_USER_SYM = 573,
    MASTER_HEARTBEAT_PERIOD_SYM = 574,
    MATCH = 575,
    MAX_CONNECTIONS_PER_HOUR = 576,
    MAX_QUERIES_PER_HOUR = 577,
    MAX_ROWS = 578,
    MAX_SIZE_SYM = 579,
    MAX_SYM = 580,
    MAX_UPDATES_PER_HOUR = 581,
    MAX_USER_CONNECTIONS_SYM = 582,
    MAX_VALUE_SYM = 583,
    MEDIUMBLOB = 584,
    MEDIUMINT = 585,
    MEDIUMTEXT = 586,
    MEDIUM_SYM = 587,
    MEMORY_SYM = 588,
    MERGE_SYM = 589,
    MESSAGE_TEXT_SYM = 590,
    MICROSECOND_SYM = 591,
    MIGRATE_SYM = 592,
    MINUTE_MICROSECOND_SYM = 593,
    MINUTE_SECOND_SYM = 594,
    MINUTE_SYM = 595,
    MIN_ROWS = 596,
    MIN_SYM = 597,
    MODE_SYM = 598,
    MODIFIES_SYM = 599,
    MODIFY_SYM = 600,
    MOD_SYM = 601,
    MONTH_SYM = 602,
    MULTILINESTRING = 603,
    MULTIPOINT = 604,
    MULTIPOLYGON = 605,
    MUTEX_SYM = 606,
    MYSQL_ERRNO_SYM = 607,
    NAMES_SYM = 608,
    NAME_SYM = 609,
    NATIONAL_SYM = 610,
    NATURAL = 611,
    NCHAR_STRING = 612,
    NCHAR_SYM = 613,
    NDBCLUSTER_SYM = 614,
    NE = 615,
    NEG = 616,
    NEVER_SYM = 617,
    NEW_SYM = 618,
    NEXT_SYM = 619,
    NODEGROUP_SYM = 620,
    NONE_SYM = 621,
    NOT2_SYM = 622,
    NOT_SYM = 623,
    NOW_SYM = 624,
    NO_SYM = 625,
    NO_WAIT_SYM = 626,
    NO_WRITE_TO_BINLOG = 627,
    NULL_SYM = 628,
    NUM = 629,
    NUMBER_SYM = 630,
    NUMERIC_SYM = 631,
    NVARCHAR_SYM = 632,
    OFFSET_SYM = 633,
    ON = 634,
    ONE_SYM = 635,
    ONLY_SYM = 636,
    OPEN_SYM = 637,
    OPTIMIZE = 638,
    OPTIMIZER_COSTS_SYM = 639,
    OPTIONS_SYM = 640,
    OPTION = 641,
    OPTIONALLY = 642,
    OR2_SYM = 643,
    ORDER_SYM = 644,
    OR_OR_SYM = 645,
    OR_SYM = 646,
    OUTER = 647,
    OUTFILE = 648,
    OUT_SYM = 649,
    OWNER_SYM = 650,
    PACK_KEYS_SYM = 651,
    PAGE_SYM = 652,
    PARAM_MARKER = 653,
    PARSER_SYM = 654,
    PARSE_GCOL_EXPR_SYM = 655,
    PARTIAL = 656,
    PARTITION_SYM = 657,
    PARTITIONS_SYM = 658,
    PARTITIONING_SYM = 659,
    PASSWORD = 660,
    PHASE_SYM = 661,
    PLUGIN_DIR_SYM = 662,
    PLUGIN_SYM = 663,
    PLUGINS_SYM = 664,
    POINT_SYM = 665,
    POLYGON = 666,
    PORT_SYM = 667,
    POSITION_SYM = 668,
    PRECEDES_SYM = 669,
    PRECISION = 670,
    PREPARE_SYM = 671,
    PRESERVE_SYM = 672,
    PREV_SYM = 673,
    PRIMARY_SYM = 674,
    PRIVILEGES = 675,
    PROCEDURE_SYM = 676,
    PROCESS = 677,
    PROCESSLIST_SYM = 678,
    PROFILE_SYM = 679,
    PROFILES_SYM = 680,
    PROXY_SYM = 681,
    PURGE = 682,
    QUARTER_SYM = 683,
    QUERY_SYM = 684,
    QUICK = 685,
    RANGE_SYM = 686,
    READS_SYM = 687,
    READ_ONLY_SYM = 688,
    READ_SYM = 689,
    READ_WRITE_SYM = 690,
    REAL = 691,
    REBUILD_SYM = 692,
    RECOVER_SYM = 693,
    REDOFILE_SYM = 694,
    REDO_BUFFER_SIZE_SYM = 695,
    REDUNDANT_SYM = 696,
    REFERENCES = 697,
    REGEXP = 698,
    RELAY = 699,
    RELAYLOG_SYM = 700,
    RELAY_LOG_FILE_SYM = 701,
    RELAY_LOG_POS_SYM = 702,
    RELAY_THREAD = 703,
    RELEASE_SYM = 704,
    RELOAD = 705,
    REMOVE_SYM = 706,
    RENAME = 707,
    REORGANIZE_SYM = 708,
    REPAIR = 709,
    REPEATABLE_SYM = 710,
    REPEAT_SYM = 711,
    REPLACE = 712,
    REPLICATION = 713,
    REPLICATE_DO_DB = 714,
    REPLICATE_IGNORE_DB = 715,
    REPLICATE_DO_TABLE = 716,
    REPLICATE_IGNORE_TABLE = 717,
    REPLICATE_WILD_DO_TABLE = 718,
    REPLICATE_WILD_IGNORE_TABLE = 719,
    REPLICATE_REWRITE_DB = 720,
    REQUIRE_SYM = 721,
    RESET_SYM = 722,
    RESIGNAL_SYM = 723,
    RESOURCES = 724,
    RESTORE_SYM = 725,
    RESTRICT = 726,
    RESUME_SYM = 727,
    RETURNED_SQLSTATE_SYM = 728,
    RETURNS_SYM = 729,
    RETURN_SYM = 730,
    REVERSE_SYM = 731,
    REVOKE = 732,
    RIGHT = 733,
    ROLLBACK_SYM = 734,
    ROLLUP_SYM = 735,
    ROTATE_SYM = 736,
    ROUTINE_SYM = 737,
    ROWS_SYM = 738,
    ROW_FORMAT_SYM = 739,
    ROW_SYM = 740,
    ROW_COUNT_SYM = 741,
    RTREE_SYM = 742,
    SAVEPOINT_SYM = 743,
    SCHEDULE_SYM = 744,
    SCHEMA_NAME_SYM = 745,
    SECOND_MICROSECOND_SYM = 746,
    SECOND_SYM = 747,
    SECURITY_SYM = 748,
    SELECT_SYM = 749,
    SENSITIVE_SYM = 750,
    SEPARATOR_SYM = 751,
    SERIALIZABLE_SYM = 752,
    SERIAL_SYM = 753,
    SESSION_SYM = 754,
    SERVER_SYM = 755,
    SERVER_OPTIONS = 756,
    SET = 757,
    SET_VAR = 758,
    SHARE_SYM = 759,
    SHIFT_LEFT = 760,
    SHIFT_RIGHT = 761,
    SHOW = 762,
    SHUTDOWN = 763,
    SIGNAL_SYM = 764,
    SIGNED_SYM = 765,
    SIMPLE_SYM = 766,
    SLAVE = 767,
    SLOW = 768,
    SMALLINT = 769,
    SNAPSHOT_SYM = 770,
    SOCKET_SYM = 771,
    SONAME_SYM = 772,
    SOUNDS_SYM = 773,
    SOURCE_SYM = 774,
    SPATIAL_SYM = 775,
    SPECIFIC_SYM = 776,
    SQLEXCEPTION_SYM = 777,
    SQLSTATE_SYM = 778,
    SQLWARNING_SYM = 779,
    SQL_AFTER_GTIDS = 780,
    SQL_AFTER_MTS_GAPS = 781,
    SQL_BEFORE_GTIDS = 782,
    SQL_BIG_RESULT = 783,
    SQL_BUFFER_RESULT = 784,
    SQL_CACHE_SYM = 785,
    SQL_CALC_FOUND_ROWS = 786,
    SQL_NO_CACHE_SYM = 787,
    SQL_SMALL_RESULT = 788,
    SQL_SYM = 789,
    SQL_THREAD = 790,
    SSL_SYM = 791,
    STACKED_SYM = 792,
    STARTING = 793,
    STARTS_SYM = 794,
    START_SYM = 795,
    STATS_AUTO_RECALC_SYM = 796,
    STATS_PERSISTENT_SYM = 797,
    STATS_SAMPLE_PAGES_SYM = 798,
    STATUS_SYM = 799,
    STDDEV_SAMP_SYM = 800,
    STD_SYM = 801,
    STOP_SYM = 802,
    STORAGE_SYM = 803,
    STORED_SYM = 804,
    STRAIGHT_JOIN = 805,
    STRING_SYM = 806,
    SUBCLASS_ORIGIN_SYM = 807,
    SUBDATE_SYM = 808,
    SUBJECT_SYM = 809,
    SUBPARTITIONS_SYM = 810,
    SUBPARTITION_SYM = 811,
    SUBSTRING = 812,
    SUM_SYM = 813,
    SUPER_SYM = 814,
    SUSPEND_SYM = 815,
    SWAPS_SYM = 816,
    SWITCHES_SYM = 817,
    SYSDATE = 818,
    TABLES = 819,
    TABLESPACE_SYM = 820,
    TABLE_REF_PRIORITY = 821,
    TABLE_SYM = 822,
    TABLE_CHECKSUM_SYM = 823,
    TABLE_NAME_SYM = 824,
    TEMPORARY = 825,
    TEMPTABLE_SYM = 826,
    TERMINATED = 827,
    TEXT_STRING = 828,
    TEXT_SYM = 829,
    THAN_SYM = 830,
    THEN_SYM = 831,
    TIMESTAMP = 832,
    TIMESTAMP_ADD = 833,
    TIMESTAMP_DIFF = 834,
    TIME_SYM = 835,
    TINYBLOB = 836,
    TINYINT = 837,
    TINYTEXT = 838,
    TO_SYM = 839,
    TRAILING = 840,
    TRANSACTION_SYM = 841,
    TRIGGERS_SYM = 842,
    TRIGGER_SYM = 843,
    TRIM = 844,
    TRUE_SYM = 845,
    TRUNCATE_SYM = 846,
    TYPES_SYM = 847,
    TYPE_SYM = 848,
    UDF_RETURNS_SYM = 849,
    ULONGLONG_NUM = 850,
    UNCOMMITTED_SYM = 851,
    UNDEFINED_SYM = 852,
    UNDERSCORE_CHARSET = 853,
    UNDOFILE_SYM = 854,
    UNDO_BUFFER_SIZE_SYM = 855,
    UNDO_SYM = 856,
    UNICODE_SYM = 857,
    UNINSTALL_SYM = 858,
    UNION_SYM = 859,
    UNIQUE_SYM = 860,
    UNKNOWN_SYM = 861,
    UNLOCK_SYM = 862,
    UNSIGNED = 863,
    UNTIL_SYM = 864,
    UPDATE_SYM = 865,
    UPGRADE_SYM = 866,
    USAGE = 867,
    USER = 868,
    USE_FRM = 869,
    USE_SYM = 870,
    USING = 871,
    UTC_DATE_SYM = 872,
    UTC_TIMESTAMP_SYM = 873,
    UTC_TIME_SYM = 874,
    VALIDATION_SYM = 875,
    VALUES = 876,
    VALUE_SYM = 877,
    VARBINARY = 878,
    VARCHAR = 879,
    VARIABLES = 880,
    VARIANCE_SYM = 881,
    VARYING = 882,
    VAR_SAMP_SYM = 883,
    VIEW_SYM = 884,
    VIRTUAL_SYM = 885,
    WAIT_SYM = 886,
    WARNINGS = 887,
    WEEK_SYM = 888,
    WEIGHT_STRING_SYM = 889,
    WHEN_SYM = 890,
    WHERE = 891,
    WHILE_SYM = 892,
    WITH = 893,
    WITH_CUBE_SYM = 894,
    WITH_ROLLUP_SYM = 895,
    WITHOUT_SYM = 896,
    WORK_SYM = 897,
    WRAPPER_SYM = 898,
    WRITE_SYM = 899,
    X509_SYM = 900,
    XA_SYM = 901,
    XID_SYM = 902,
    XML_SYM = 903,
    XOR = 904,
    YEAR_MONTH_SYM = 905,
    YEAR_SYM = 906,
    ZEROFILL = 907,
    JSON_OBJECTAGG = 908,
    JSON_ARRAYAGG = 909
  };
#endif
/* Tokens.  */
#define ABORT_SYM 258
#define ACCESSIBLE_SYM 259
#define ACCOUNT_SYM 260
#define ACTION 261
#define ADD 262
#define ADDDATE_SYM 263
#define AFTER_SYM 264
#define AGAINST 265
#define AGGREGATE_SYM 266
#define ALGORITHM_SYM 267
#define ALL 268
#define ALTER 269
#define ALWAYS_SYM 270
#define ANALYSE_SYM 271
#define ANALYZE_SYM 272
#define AND_AND_SYM 273
#define AND_SYM 274
#define ANY_SYM 275
#define AS 276
#define ASC 277
#define ASCII_SYM 278
#define ASENSITIVE_SYM 279
#define AT_SYM 280
#define AUTOEXTEND_SIZE_SYM 281
#define AUTO_INC 282
#define AVG_ROW_LENGTH 283
#define AVG_SYM 284
#define BACKUP_SYM 285
#define BEFORE_SYM 286
#define BEGIN_SYM 287
#define BETWEEN_SYM 288
#define BIGINT 289
#define BINARY 290
#define BINLOG_SYM 291
#define BIN_NUM 292
#define BIT_AND 293
#define BIT_OR 294
#define BIT_SYM 295
#define BIT_XOR 296
#define BLOB_SYM 297
#define BLOCK_SYM 298
#define BOOLEAN_SYM 299
#define BOOL_SYM 300
#define BOTH 301
#define BTREE_SYM 302
#define BY 303
#define BYTE_SYM 304
#define CACHE_SYM 305
#define CALL_SYM 306
#define CASCADE 307
#define CASCADED 308
#define CASE_SYM 309
#define CAST_SYM 310
#define CATALOG_NAME_SYM 311
#define CHAIN_SYM 312
#define CHANGE 313
#define CHANGED 314
#define CHANNEL_SYM 315
#define CHARSET 316
#define CHAR_SYM 317
#define CHECKSUM_SYM 318
#define CHECK_SYM 319
#define CIPHER_SYM 320
#define CLASS_ORIGIN_SYM 321
#define CLIENT_SYM 322
#define CLOSE_SYM 323
#define COALESCE 324
#define CODE_SYM 325
#define COLLATE_SYM 326
#define COLLATION_SYM 327
#define COLUMNS 328
#define COLUMN_SYM 329
#define COLUMN_FORMAT_SYM 330
#define COLUMN_NAME_SYM 331
#define COMMENT_SYM 332
#define COMMITTED_SYM 333
#define COMMIT_SYM 334
#define COMPACT_SYM 335
#define COMPLETION_SYM 336
#define COMPRESSED_SYM 337
#define COMPRESSION_SYM 338
#define ENCRYPTION_SYM 339
#define CONCURRENT 340
#define CONDITION_SYM 341
#define CONNECTION_SYM 342
#define CONSISTENT_SYM 343
#define CONSTRAINT 344
#define CONSTRAINT_CATALOG_SYM 345
#define CONSTRAINT_NAME_SYM 346
#define CONSTRAINT_SCHEMA_SYM 347
#define CONTAINS_SYM 348
#define CONTEXT_SYM 349
#define CONTINUE_SYM 350
#define CONVERT_SYM 351
#define COUNT_SYM 352
#define CPU_SYM 353
#define CREATE 354
#define CROSS 355
#define CUBE_SYM 356
#define CURDATE 357
#define CURRENT_SYM 358
#define CURRENT_USER 359
#define CURSOR_SYM 360
#define CURSOR_NAME_SYM 361
#define CURTIME 362
#define DATABASE 363
#define DATABASES 364
#define DATAFILE_SYM 365
#define DATA_SYM 366
#define DATETIME 367
#define DATE_ADD_INTERVAL 368
#define DATE_SUB_INTERVAL 369
#define DATE_SYM 370
#define DAY_HOUR_SYM 371
#define DAY_MICROSECOND_SYM 372
#define DAY_MINUTE_SYM 373
#define DAY_SECOND_SYM 374
#define DAY_SYM 375
#define DEALLOCATE_SYM 376
#define DECIMAL_NUM 377
#define DECIMAL_SYM 378
#define DECLARE_SYM 379
#define DEFAULT 380
#define DEFAULT_AUTH_SYM 381
#define DEFINER_SYM 382
#define DELAYED_SYM 383
#define DELAY_KEY_WRITE_SYM 384
#define DELETE_SYM 385
#define DESC 386
#define DESCRIBE 387
#define DES_KEY_FILE 388
#define DETERMINISTIC_SYM 389
#define DIAGNOSTICS_SYM 390
#define DIRECTORY_SYM 391
#define DISABLE_SYM 392
#define DISCARD 393
#define DISK_SYM 394
#define DISTINCT 395
#define DIV_SYM 396
#define DOUBLE_SYM 397
#define DO_SYM 398
#define DROP 399
#define DUAL_SYM 400
#define DUMPFILE 401
#define DUPLICATE_SYM 402
#define DYNAMIC_SYM 403
#define EACH_SYM 404
#define ELSE 405
#define ELSEIF_SYM 406
#define ENABLE_SYM 407
#define ENCLOSED 408
#define END 409
#define ENDS_SYM 410
#define END_OF_INPUT 411
#define ENGINES_SYM 412
#define ENGINE_SYM 413
#define ENUM 414
#define EQ 415
#define EQUAL_SYM 416
#define ERROR_SYM 417
#define ERRORS 418
#define ESCAPED 419
#define ESCAPE_SYM 420
#define EVENTS_SYM 421
#define EVENT_SYM 422
#define EVERY_SYM 423
#define EXCHANGE_SYM 424
#define EXECUTE_SYM 425
#define EXISTS 426
#define EXIT_SYM 427
#define EXPANSION_SYM 428
#define EXPIRE_SYM 429
#define EXPORT_SYM 430
#define EXTENDED_SYM 431
#define EXTENT_SIZE_SYM 432
#define EXTRACT_SYM 433
#define FALSE_SYM 434
#define FAST_SYM 435
#define FAULTS_SYM 436
#define FETCH_SYM 437
#define FILE_SYM 438
#define FILE_BLOCK_SIZE_SYM 439
#define FILTER_SYM 440
#define FIRST_SYM 441
#define FIXED_SYM 442
#define FLOAT_NUM 443
#define FLOAT_SYM 444
#define FLUSH_SYM 445
#define FOLLOWS_SYM 446
#define FORCE_SYM 447
#define FOREIGN 448
#define FOR_SYM 449
#define FORMAT_SYM 450
#define FOUND_SYM 451
#define FROM 452
#define FULL 453
#define FULLTEXT_SYM 454
#define FUNCTION_SYM 455
#define GE 456
#define GENERAL 457
#define GENERATED 458
#define GROUP_REPLICATION 459
#define GEOMETRYCOLLECTION 460
#define GEOMETRY_SYM 461
#define GET_FORMAT 462
#define GET_SYM 463
#define GLOBAL_SYM 464
#define GRANT 465
#define GRANTS 466
#define GROUP_SYM 467
#define GROUP_CONCAT_SYM 468
#define GT_SYM 469
#define HANDLER_SYM 470
#define HASH_SYM 471
#define HAVING 472
#define HELP_SYM 473
#define HEX_NUM 474
#define HIGH_PRIORITY 475
#define HOST_SYM 476
#define HOSTS_SYM 477
#define HOUR_MICROSECOND_SYM 478
#define HOUR_MINUTE_SYM 479
#define HOUR_SECOND_SYM 480
#define HOUR_SYM 481
#define IDENT 482
#define IDENTIFIED_SYM 483
#define IDENT_QUOTED 484
#define IF 485
#define IGNORE_SYM 486
#define IGNORE_SERVER_IDS_SYM 487
#define IMPORT 488
#define INDEXES 489
#define INDEX_SYM 490
#define INFILE 491
#define INITIAL_SIZE_SYM 492
#define INNER_SYM 493
#define INOUT_SYM 494
#define INSENSITIVE_SYM 495
#define INSERT 496
#define INSERT_METHOD 497
#define INSTANCE_SYM 498
#define INSTALL_SYM 499
#define INTERVAL_SYM 500
#define INTO 501
#define INT_SYM 502
#define INVOKER_SYM 503
#define IN_SYM 504
#define IO_AFTER_GTIDS 505
#define IO_BEFORE_GTIDS 506
#define IO_SYM 507
#define IPC_SYM 508
#define IS 509
#define ISOLATION 510
#define ISSUER_SYM 511
#define ITERATE_SYM 512
#define JOIN_SYM 513
#define JSON_SEPARATOR_SYM 514
#define JSON_UNQUOTED_SEPARATOR_SYM 515
#define JSON_SYM 516
#define KEYS 517
#define KEY_BLOCK_SIZE 518
#define KEY_SYM 519
#define KILL_SYM 520
#define LANGUAGE_SYM 521
#define LAST_SYM 522
#define LE 523
#define LEADING 524
#define LEAVES 525
#define LEAVE_SYM 526
#define LEFT 527
#define LESS_SYM 528
#define LEVEL_SYM 529
#define LEX_HOSTNAME 530
#define LIKE 531
#define LIMIT 532
#define LINEAR_SYM 533
#define LINES 534
#define LINESTRING 535
#define LIST_SYM 536
#define LOAD 537
#define LOCAL_SYM 538
#define LOCATOR_SYM 539
#define LOCKS_SYM 540
#define LOCK_SYM 541
#define LOGFILE_SYM 542
#define LOGS_SYM 543
#define LONGBLOB 544
#define LONGTEXT 545
#define LONG_NUM 546
#define LONG_SYM 547
#define LOOP_SYM 548
#define LOW_PRIORITY 549
#define LT 550
#define MASTER_AUTO_POSITION_SYM 551
#define MASTER_BIND_SYM 552
#define MASTER_CONNECT_RETRY_SYM 553
#define MASTER_DELAY_SYM 554
#define MASTER_HOST_SYM 555
#define MASTER_LOG_FILE_SYM 556
#define MASTER_LOG_POS_SYM 557
#define MASTER_PASSWORD_SYM 558
#define MASTER_PORT_SYM 559
#define MASTER_RETRY_COUNT_SYM 560
#define MASTER_SERVER_ID_SYM 561
#define MASTER_SSL_CAPATH_SYM 562
#define MASTER_TLS_VERSION_SYM 563
#define MASTER_SSL_CA_SYM 564
#define MASTER_SSL_CERT_SYM 565
#define MASTER_SSL_CIPHER_SYM 566
#define MASTER_SSL_CRL_SYM 567
#define MASTER_SSL_CRLPATH_SYM 568
#define MASTER_SSL_KEY_SYM 569
#define MASTER_SSL_SYM 570
#define MASTER_SSL_VERIFY_SERVER_CERT_SYM 571
#define MASTER_SYM 572
#define MASTER_USER_SYM 573
#define MASTER_HEARTBEAT_PERIOD_SYM 574
#define MATCH 575
#define MAX_CONNECTIONS_PER_HOUR 576
#define MAX_QUERIES_PER_HOUR 577
#define MAX_ROWS 578
#define MAX_SIZE_SYM 579
#define MAX_SYM 580
#define MAX_UPDATES_PER_HOUR 581
#define MAX_USER_CONNECTIONS_SYM 582
#define MAX_VALUE_SYM 583
#define MEDIUMBLOB 584
#define MEDIUMINT 585
#define MEDIUMTEXT 586
#define MEDIUM_SYM 587
#define MEMORY_SYM 588
#define MERGE_SYM 589
#define MESSAGE_TEXT_SYM 590
#define MICROSECOND_SYM 591
#define MIGRATE_SYM 592
#define MINUTE_MICROSECOND_SYM 593
#define MINUTE_SECOND_SYM 594
#define MINUTE_SYM 595
#define MIN_ROWS 596
#define MIN_SYM 597
#define MODE_SYM 598
#define MODIFIES_SYM 599
#define MODIFY_SYM 600
#define MOD_SYM 601
#define MONTH_SYM 602
#define MULTILINESTRING 603
#define MULTIPOINT 604
#define MULTIPOLYGON 605
#define MUTEX_SYM 606
#define MYSQL_ERRNO_SYM 607
#define NAMES_SYM 608
#define NAME_SYM 609
#define NATIONAL_SYM 610
#define NATURAL 611
#define NCHAR_STRING 612
#define NCHAR_SYM 613
#define NDBCLUSTER_SYM 614
#define NE 615
#define NEG 616
#define NEVER_SYM 617
#define NEW_SYM 618
#define NEXT_SYM 619
#define NODEGROUP_SYM 620
#define NONE_SYM 621
#define NOT2_SYM 622
#define NOT_SYM 623
#define NOW_SYM 624
#define NO_SYM 625
#define NO_WAIT_SYM 626
#define NO_WRITE_TO_BINLOG 627
#define NULL_SYM 628
#define NUM 629
#define NUMBER_SYM 630
#define NUMERIC_SYM 631
#define NVARCHAR_SYM 632
#define OFFSET_SYM 633
#define ON 634
#define ONE_SYM 635
#define ONLY_SYM 636
#define OPEN_SYM 637
#define OPTIMIZE 638
#define OPTIMIZER_COSTS_SYM 639
#define OPTIONS_SYM 640
#define OPTION 641
#define OPTIONALLY 642
#define OR2_SYM 643
#define ORDER_SYM 644
#define OR_OR_SYM 645
#define OR_SYM 646
#define OUTER 647
#define OUTFILE 648
#define OUT_SYM 649
#define OWNER_SYM 650
#define PACK_KEYS_SYM 651
#define PAGE_SYM 652
#define PARAM_MARKER 653
#define PARSER_SYM 654
#define PARSE_GCOL_EXPR_SYM 655
#define PARTIAL 656
#define PARTITION_SYM 657
#define PARTITIONS_SYM 658
#define PARTITIONING_SYM 659
#define PASSWORD 660
#define PHASE_SYM 661
#define PLUGIN_DIR_SYM 662
#define PLUGIN_SYM 663
#define PLUGINS_SYM 664
#define POINT_SYM 665
#define POLYGON 666
#define PORT_SYM 667
#define POSITION_SYM 668
#define PRECEDES_SYM 669
#define PRECISION 670
#define PREPARE_SYM 671
#define PRESERVE_SYM 672
#define PREV_SYM 673
#define PRIMARY_SYM 674
#define PRIVILEGES 675
#define PROCEDURE_SYM 676
#define PROCESS 677
#define PROCESSLIST_SYM 678
#define PROFILE_SYM 679
#define PROFILES_SYM 680
#define PROXY_SYM 681
#define PURGE 682
#define QUARTER_SYM 683
#define QUERY_SYM 684
#define QUICK 685
#define RANGE_SYM 686
#define READS_SYM 687
#define READ_ONLY_SYM 688
#define READ_SYM 689
#define READ_WRITE_SYM 690
#define REAL 691
#define REBUILD_SYM 692
#define RECOVER_SYM 693
#define REDOFILE_SYM 694
#define REDO_BUFFER_SIZE_SYM 695
#define REDUNDANT_SYM 696
#define REFERENCES 697
#define REGEXP 698
#define RELAY 699
#define RELAYLOG_SYM 700
#define RELAY_LOG_FILE_SYM 701
#define RELAY_LOG_POS_SYM 702
#define RELAY_THREAD 703
#define RELEASE_SYM 704
#define RELOAD 705
#define REMOVE_SYM 706
#define RENAME 707
#define REORGANIZE_SYM 708
#define REPAIR 709
#define REPEATABLE_SYM 710
#define REPEAT_SYM 711
#define REPLACE 712
#define REPLICATION 713
#define REPLICATE_DO_DB 714
#define REPLICATE_IGNORE_DB 715
#define REPLICATE_DO_TABLE 716
#define REPLICATE_IGNORE_TABLE 717
#define REPLICATE_WILD_DO_TABLE 718
#define REPLICATE_WILD_IGNORE_TABLE 719
#define REPLICATE_REWRITE_DB 720
#define REQUIRE_SYM 721
#define RESET_SYM 722
#define RESIGNAL_SYM 723
#define RESOURCES 724
#define RESTORE_SYM 725
#define RESTRICT 726
#define RESUME_SYM 727
#define RETURNED_SQLSTATE_SYM 728
#define RETURNS_SYM 729
#define RETURN_SYM 730
#define REVERSE_SYM 731
#define REVOKE 732
#define RIGHT 733
#define ROLLBACK_SYM 734
#define ROLLUP_SYM 735
#define ROTATE_SYM 736
#define ROUTINE_SYM 737
#define ROWS_SYM 738
#define ROW_FORMAT_SYM 739
#define ROW_SYM 740
#define ROW_COUNT_SYM 741
#define RTREE_SYM 742
#define SAVEPOINT_SYM 743
#define SCHEDULE_SYM 744
#define SCHEMA_NAME_SYM 745
#define SECOND_MICROSECOND_SYM 746
#define SECOND_SYM 747
#define SECURITY_SYM 748
#define SELECT_SYM 749
#define SENSITIVE_SYM 750
#define SEPARATOR_SYM 751
#define SERIALIZABLE_SYM 752
#define SERIAL_SYM 753
#define SESSION_SYM 754
#define SERVER_SYM 755
#define SERVER_OPTIONS 756
#define SET 757
#define SET_VAR 758
#define SHARE_SYM 759
#define SHIFT_LEFT 760
#define SHIFT_RIGHT 761
#define SHOW 762
#define SHUTDOWN 763
#define SIGNAL_SYM 764
#define SIGNED_SYM 765
#define SIMPLE_SYM 766
#define SLAVE 767
#define SLOW 768
#define SMALLINT 769
#define SNAPSHOT_SYM 770
#define SOCKET_SYM 771
#define SONAME_SYM 772
#define SOUNDS_SYM 773
#define SOURCE_SYM 774
#define SPATIAL_SYM 775
#define SPECIFIC_SYM 776
#define SQLEXCEPTION_SYM 777
#define SQLSTATE_SYM 778
#define SQLWARNING_SYM 779
#define SQL_AFTER_GTIDS 780
#define SQL_AFTER_MTS_GAPS 781
#define SQL_BEFORE_GTIDS 782
#define SQL_BIG_RESULT 783
#define SQL_BUFFER_RESULT 784
#define SQL_CACHE_SYM 785
#define SQL_CALC_FOUND_ROWS 786
#define SQL_NO_CACHE_SYM 787
#define SQL_SMALL_RESULT 788
#define SQL_SYM 789
#define SQL_THREAD 790
#define SSL_SYM 791
#define STACKED_SYM 792
#define STARTING 793
#define STARTS_SYM 794
#define START_SYM 795
#define STATS_AUTO_RECALC_SYM 796
#define STATS_PERSISTENT_SYM 797
#define STATS_SAMPLE_PAGES_SYM 798
#define STATUS_SYM 799
#define STDDEV_SAMP_SYM 800
#define STD_SYM 801
#define STOP_SYM 802
#define STORAGE_SYM 803
#define STORED_SYM 804
#define STRAIGHT_JOIN 805
#define STRING_SYM 806
#define SUBCLASS_ORIGIN_SYM 807
#define SUBDATE_SYM 808
#define SUBJECT_SYM 809
#define SUBPARTITIONS_SYM 810
#define SUBPARTITION_SYM 811
#define SUBSTRING 812
#define SUM_SYM 813
#define SUPER_SYM 814
#define SUSPEND_SYM 815
#define SWAPS_SYM 816
#define SWITCHES_SYM 817
#define SYSDATE 818
#define TABLES 819
#define TABLESPACE_SYM 820
#define TABLE_REF_PRIORITY 821
#define TABLE_SYM 822
#define TABLE_CHECKSUM_SYM 823
#define TABLE_NAME_SYM 824
#define TEMPORARY 825
#define TEMPTABLE_SYM 826
#define TERMINATED 827
#define TEXT_STRING 828
#define TEXT_SYM 829
#define THAN_SYM 830
#define THEN_SYM 831
#define TIMESTAMP 832
#define TIMESTAMP_ADD 833
#define TIMESTAMP_DIFF 834
#define TIME_SYM 835
#define TINYBLOB 836
#define TINYINT 837
#define TINYTEXT 838
#define TO_SYM 839
#define TRAILING 840
#define TRANSACTION_SYM 841
#define TRIGGERS_SYM 842
#define TRIGGER_SYM 843
#define TRIM 844
#define TRUE_SYM 845
#define TRUNCATE_SYM 846
#define TYPES_SYM 847
#define TYPE_SYM 848
#define UDF_RETURNS_SYM 849
#define ULONGLONG_NUM 850
#define UNCOMMITTED_SYM 851
#define UNDEFINED_SYM 852
#define UNDERSCORE_CHARSET 853
#define UNDOFILE_SYM 854
#define UNDO_BUFFER_SIZE_SYM 855
#define UNDO_SYM 856
#define UNICODE_SYM 857
#define UNINSTALL_SYM 858
#define UNION_SYM 859
#define UNIQUE_SYM 860
#define UNKNOWN_SYM 861
#define UNLOCK_SYM 862
#define UNSIGNED 863
#define UNTIL_SYM 864
#define UPDATE_SYM 865
#define UPGRADE_SYM 866
#define USAGE 867
#define USER 868
#define USE_FRM 869
#define USE_SYM 870
#define USING 871
#define UTC_DATE_SYM 872
#define UTC_TIMESTAMP_SYM 873
#define UTC_TIME_SYM 874
#define VALIDATION_SYM 875
#define VALUES 876
#define VALUE_SYM 877
#define VARBINARY 878
#define VARCHAR 879
#define VARIABLES 880
#define VARIANCE_SYM 881
#define VARYING 882
#define VAR_SAMP_SYM 883
#define VIEW_SYM 884
#define VIRTUAL_SYM 885
#define WAIT_SYM 886
#define WARNINGS 887
#define WEEK_SYM 888
#define WEIGHT_STRING_SYM 889
#define WHEN_SYM 890
#define WHERE 891
#define WHILE_SYM 892
#define WITH 893
#define WITH_CUBE_SYM 894
#define WITH_ROLLUP_SYM 895
#define WITHOUT_SYM 896
#define WORK_SYM 897
#define WRAPPER_SYM 898
#define WRITE_SYM 899
#define X509_SYM 900
#define XA_SYM 901
#define XID_SYM 902
#define XML_SYM 903
#define XOR 904
#define YEAR_MONTH_SYM 905
#define YEAR_SYM 906
#define ZEROFILL 907
#define JSON_OBJECTAGG 908
#define JSON_ARRAYAGG 909

/* Value type.  */

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int MYSQLparse (class THD *YYTHD);

#endif /* !YY_MYSQL_EXPORT_HOME_PB2_BUILD_SB_3_30900853_1538998080_7_DIST_GPL_SQL_SQL_YACC_H_INCLUDED  */
 