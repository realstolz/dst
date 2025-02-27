# ==== Purpose ====
#
# Generate a random number in such a way that the sequence of numbers
# generated by successive calls can be deterministic.
#
# ==== Usage ====
#
# --let $rand_type= { int | float | decide }
# [--let $rand_min= MIN_VALUE]
# [--let $rand_max= MAX_VALUE]
# [--let $rand_probability= THRESHOLD]
# [--let $rand_seed= SEED]
# --source include/rand.inc
# --echo Random number: $rand
#
# Parameters:
#
#   $rand_type
#     If set to 'int', generates an integer in the range
#     [$rand_min, $rand_max).
#
#     If set to 'float', generates a floating point number in the
#     range [$rand_min, $rand_max).
#
#     If set to 'decide', generates 1 with probability
#     $rand_probability and 0 with probability 1-$rand_probability.
#
#   $rand_min, $rand_max
#     Range of values for $rand_type=int or $rand_type=float.
#
#   $rand_probability
#     Probability to get a '1' if $rand_type=decide.
#
#   $rand_seed
#     The seed value.  If you want to set the seed to a random number
#     that is different for each invokation of the test, set
#     $rand_seed to the empty string.  Normally, $rand_seed is only
#     read in the first invokation of rand.inc.  If you want to reset
#     the seed later, set $_rand_state to the empty string.
#
# ==== Implementation ====
#
# This RNG uses the following multiply-with-carry algorithm:
#   int32 z, w;
#   if (z == 0) z = 1;
#   if (w == 0) w = 1;
#   z = 36969 * (z & 65535) + (z >> 16);
#   w = 18000 * (w & 65535) + (w >> 16);
#   rand = (z << 16) + w;
# In this implementation, z is stored in the low 32 bits and w in the
# high 32 bits of $_rand_state.

--let $_maxint32= ((1 << 32) - 1)

if ($_rand_state == '')
{
  if ($rand_seed == '')
  {
    --let $rand_seed= `SELECT FLOOR(RAND() * (1 << 63))`
  }
  --let $write_var= $rand_seed
  --let $write_to_file= $MYSQLTEST_VARDIR/tmp/rand_seed
  --source include/write_var_to_file.inc
  --let $_rand_state= $rand_seed
  --let $_rand_lo= IF(($_rand_state) & $_maxint32 = 0, 1, ($_rand_state) & $_maxint32)
  --let $_rand_hi= IF(($_rand_state) >> 32 = 0, 1, ($_rand_state) >> 32)
  --let $_rand_state= `SELECT ((36969 * $_rand_lo + ($_rand_lo >> 16)) & $_maxint32) + (((18000 * ($_rand_hi & 65535) + ($_rand_hi >> 16)) & $_maxint32) << 32)`
}

--let $_rand_lo= IF(($_rand_state) & $_maxint32 = 0, 1, ($_rand_state) & $_maxint32)
--let $_rand_hi= IF(($_rand_state) >> 32 = 0, 1, ($_rand_state) >> 32)
--let $_rand_state= `SELECT ((36969 * $_rand_lo + ($_rand_lo >> 16)) & $_maxint32) + (((18000 * ($_rand_hi & 65535) + ($_rand_hi >> 16)) & $_maxint32) << 32)`
--let $rand= ((($_rand_state << 16) + ($_rand_state >> 32)) & $_maxint32)

if ($rand_type == decide)
{
  --let $_rand_probability= 0.5
  if ($rand_probability != '')
  {
    --let $_rand_probability= $rand_probability
  }
  --let $rand= `SELECT IF($rand / (1 << 32) < ($rand_probability), 1, 0)`
}
if ($rand_type != decide)
{
  --let $_rand_min= 0
  if ($rand_min)
  {
    --let $_rand_min= $rand_min
  }
  --let $_rand_max= 1
  if ($rand_max)
  {
    --let $_rand_max= $rand_max
  }
  if ($rand_type == int)
  {
    --let $rand= `SELECT ($_rand_min) + ($rand % (($_rand_max) - ($_rand_min)))`
  }
  if ($rand_type == float)
  {
    --let $rand= `SELECT ($_rand_min) + ($rand * (($_rand_max) - ($_rand_min)) / (1 << 32))`
  }
}
 