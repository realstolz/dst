/* Copyright (c) 2011, 2014, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA */
   
#include <my_global.h>
#include <m_ctype.h>
#include <my_base.h>
#include <my_compare.h>
#include <my_sys.h>

#define CMP_NUM(a,b)    (((a) < (b)) ? -1 : ((a) == (b)) ? 0 : 1)

int ha_compare_text(const CHARSET_INFO *charset_info, uchar *a, uint a_length,
		    uchar *b, uint b_length, my_bool part_key,
		    my_bool skip_end_space)
{
  if (!part_key)
    return charset_info->coll->strnncollsp(charset_info, a, a_length,
                                           b, b_length, (my_bool)!skip_end_space);
  return charset_info->coll->strnncoll(charset_info, a, a_length,
                                       b, b_length, part_key);
}


static int compare_bin(uchar *a, uint a_length, uchar *b, uint b_length,
                       my_bool part_key, my_bool skip_end_space)
{
  uint length= MY_MIN(a_length, b_length);
  uchar *end= a+ length;
  int flag;

  while (a < end)
    if ((flag= (int) *a++ - (int) *b++))
      return flag;
  if (part_key && b_length < a_length)
    return 0;
  if (skip_end_space && a_length != b_length)
  {
    int swap= 1;
    /*
      We are using space compression. We have to check if longer key
      has next character < ' ', in which case it's less than the shorter
      key that has an implicite space afterwards.

      This code is identical to the one in
      strings/ctype-simple.c:my_strnncollsp_simple
    */
    if (a_length < b_length)
    {
      /* put shorter key in a */
      a_length= b_length;
      a= b;
      swap= -1;					/* swap sign of result */
    }
    for (end= a + a_length-length; a < end ; a++)
    {
      if (*a != ' ')
	return (*a < ' ') ? -swap : swap;
    }
    return 0;
  }
  return (int) (a_length-b_length);
}


/*
  Compare two keys

  SYNOPSIS
    ha_key_cmp()
    keyseg	Array of key segments of key to compare
    a		First key to compare, in format from _mi_pack_key()
		This is normally key specified by user
    b		Second key to compare.  This is always from a row
    key_length	Length of key to compare.  This can be shorter than
		a to just compare sub keys
    next_flag	How keys should be compared
		If bit SEARCH_FIND is not set the keys includes the row
		position and this should also be compared
    diff_pos    OUT Number of first keypart where values differ, counting 
                from one.
    diff_pos[1] OUT  (b + diff_pos[1]) points to first value in tuple b
                      that is different from corresponding value in tuple a.
  
  EXAMPLES 
   Example1: if the function is called for tuples
     ('aaa','bbb') and ('eee','fff'), then
     diff_pos[0] = 1 (as 'aaa' != 'eee')
     diff_pos[1] = 0 (offset from beggining of tuple b to 'eee' keypart).

   Example2: if the index function is called for tuples
     ('aaa','bbb') and ('aaa','fff'),
     diff_pos[0] = 2 (as 'aaa' != 'eee')
     diff_pos[1] = 3 (offset from beggining of tuple b to 'fff' keypart,
                      here we assume that first key part is CHAR(3) NOT NULL)

  NOTES
    Number-keys can't be splited

  RETURN VALUES
    <0	If a < b
    0	If a == b
    >0	If a > b
*/

#define FCMP(A,B) ((int) (A) - (int) (B))

int ha_key_cmp(HA_KEYSEG *keyseg, uchar *a,
	       uchar *b, uint key_length, uint nextflag,
	       uint *diff_pos)
{
  int flag;
  int16 s_1,s_2;
  int32 l_1,l_2;
  uint32 u_1,u_2;
  float f_1,f_2;
  double d_1,d_2;
  uint next_key_length;
  uchar *orig_b= b;

  *diff_pos=0;
  for ( ; (int) key_length >0 ; key_length=next_key_length, keyseg++)
  {
    uchar *end;
    uint piks=! (keyseg->flag & HA_NO_SORT);
    (*diff_pos)++;
    diff_pos[1]= (uint)(b - orig_b);

    /* Handle NULL part */
    if (keyseg->null_bit)
    {
      key_length--;
      if (*a != *b && piks)
      {
        flag = (int) *a - (int) *b;
        return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
      }
      b++;
      if (!*a++)                                /* If key was NULL */
      {
        if (nextflag == (SEARCH_FIND | SEARCH_UPDATE))
          nextflag=SEARCH_SAME;                 /* Allow duplicate keys */
  	else if (nextflag & SEARCH_NULL_ARE_NOT_EQUAL)
	{
	  /*
	    This is only used from mi_check() to calculate cardinality.
	    It can't be used when searching for a key as this would cause
	    compare of (a,b) and (b,a) to return the same value.
	  */
	  return -1;
	}
        next_key_length=key_length;
        continue;                               /* To next key part */
      }
    }
    end= a + MY_MIN(keyseg->length, key_length);
    next_key_length=key_length-keyseg->length;

    switch ((enum ha_base_keytype) keyseg->type) {
    case HA_KEYTYPE_TEXT:                       /* Ascii; Key is converted */
      if (keyseg->flag & HA_SPACE_PACK)
      {
        int a_length,b_length,pack_length;
        get_key_length(a_length,a);
        get_key_pack_length(b_length,pack_length,b);
        next_key_length=key_length-b_length-pack_length;

        if (piks &&
            (flag=ha_compare_text(keyseg->charset,a,a_length,b,b_length,
				  (my_bool) ((nextflag & SEARCH_PREFIX) &&
					     next_key_length <= 0),
				  (my_bool)!(nextflag & SEARCH_PREFIX))))
          return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
        a+=a_length;
        b+=b_length;
        break;
      }
      else
      {
	uint length=(uint) (end-a), a_length=length, b_length=length;
        if (piks &&
            (flag= ha_compare_text(keyseg->charset, a, a_length, b, b_length,
				   (my_bool) ((nextflag & SEARCH_PREFIX) &&
					      next_key_length <= 0),
				   (my_bool)!(nextflag & SEARCH_PREFIX))))
          return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
        a=end;
        b+=length;
      }
      break;
    case HA_KEYTYPE_BINARY:
    case HA_KEYTYPE_BIT:
      if (keyseg->flag & HA_SPACE_PACK)
      {
        int a_length,b_length,pack_length;
        get_key_length(a_length,a);
        get_key_pack_length(b_length,pack_length,b);
        next_key_length=key_length-b_length-pack_length;

        if (piks &&
	    (flag=compare_bin(a,a_length,b,b_length,
                              (my_bool) ((nextflag & SEARCH_PREFIX) &&
                                         next_key_length <= 0),1)))
          return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
        a+=a_length;
        b+=b_length;
        break;
      }
      else
      {
        uint length=keyseg->length;
        if (piks &&
	    (flag=compare_bin(a,length,b,length,
                              (my_bool) ((nextflag & SEARCH_PREFIX) &&
                                         next_key_length <= 0),0)))
          return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
        a+=length;
        b+=length;
      }
      break;
    case HA_KEYTYPE_VARTEXT1:
    case HA_KEYTYPE_VARTEXT2:
      {
        int a_length,b_length,pack_length;
        get_key_length(a_length,a);
        get_key_pack_length(b_length,pack_length,b);
        next_key_length=key_length-b_length-pack_length;

        if (piks &&
	    (flag= ha_compare_text(keyseg->charset,a,a_length,b,b_length,
                                   (my_bool) ((nextflag & SEARCH_PREFIX) &&
                                              next_key_length <= 0),
				   (my_bool) ((nextflag & (SEARCH_FIND |
							   SEARCH_UPDATE)) ==
					      SEARCH_FIND &&
                                              ! (keyseg->flag &
                                                 HA_END_SPACE_ARE_EQUAL)))))
          return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
        a+= a_length;
        b+= b_length;
        break;
      }
      break;
    case HA_KEYTYPE_VARBINARY1:
    case HA_KEYTYPE_VARBINARY2:
      {
        int a_length,b_length,pack_length;
        get_key_length(a_length,a);
        get_key_pack_length(b_length,pack_length,b);
        next_key_length=key_length-b_length-pack_length;

        if (piks &&
	    (flag=compare_bin(a,a_length,b,b_length,
                              (my_bool) ((nextflag & SEARCH_PREFIX) &&
                                         next_key_length <= 0), 0)))
          return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
        a+=a_length;
        b+=b_length;
      }
      break;
    case HA_KEYTYPE_INT8:
    {
      int i_1= (int) *((signed char*) a);
      int i_2= (int) *((signed char*) b);
      if (piks && (flag = CMP_NUM(i_1,i_2)))
        return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
      a= end;
      b++;
      break;
    }
    case HA_KEYTYPE_SHORT_INT:
      s_1= mi_sint2korr(a);
      s_2= mi_sint2korr(b);
      if (piks && (flag = CMP_NUM(s_1,s_2)))
        return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
      a=  end;
      b+= 2; /* sizeof(short int); */
      break;
    case HA_KEYTYPE_USHORT_INT:
      {
        uint16 us_1,us_2;
        us_1= mi_sint2korr(a);
        us_2= mi_sint2korr(b);
        if (piks && (flag = CMP_NUM(us_1,us_2)))
          return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
        a=  end;
        b+=2; /* sizeof(short int); */
        break;
      }
    case HA_KEYTYPE_LONG_INT:
      l_1= mi_sint4korr(a);
      l_2= mi_sint4korr(b);
      if (piks && (flag = CMP_NUM(l_1,l_2)))
        return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
      a=  end;
      b+= 4; /* sizeof(long int); */
      break;
    case HA_KEYTYPE_ULONG_INT:
      u_1= mi_sint4korr(a);
      u_2= mi_sint4korr(b);
      if (piks && (flag = CMP_NUM(u_1,u_2)))
        return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
      a=  end;
      b+= 4; /* sizeof(long int); */
      break;
    case HA_KEYTYPE_INT24:
      l_1=mi_sint3korr(a);
      l_2=mi_sint3korr(b);
      if (piks && (flag = CMP_NUM(l_1,l_2)))
        return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
      a=  end;
      b+= 3;
      break;
    case HA_KEYTYPE_UINT24:
      l_1=mi_uint3korr(a);
      l_2=mi_uint3korr(b);
      if (piks && (flag = CMP_NUM(l_1,l_2)))
        return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
      a=  end;
      b+= 3;
      break;
    case HA_KEYTYPE_FLOAT:
      mi_float4get(f_1,a);
      mi_float4get(f_2,b);
      /*
        The following may give a compiler warning about floating point
        comparison not being safe, but this is ok in this context as
        we are bascily doing sorting
      */
      if (piks && (flag = CMP_NUM(f_1,f_2)))
        return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
      a=  end;
      b+= 4; /* sizeof(float); */
      break;
    case HA_KEYTYPE_DOUBLE:
      mi_float8get(d_1,a);
      mi_float8get(d_2,b);
      /*
        The following may give a compiler warning about floating point
        comparison not being safe, but this is ok in this context as
        we are bascily doing sorting
      */
      if (piks && (flag = CMP_NUM(d_1,d_2)))
        return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
      a=  end;
      b+= 8;  /* sizeof(double); */
      break;
    case HA_KEYTYPE_NUM:                                /* Numeric key */
    {
      int swap_flag= 0;
      int alength,blength;

      if (keyseg->flag & HA_REVERSE_SORT)
      {
        swap_variables(uchar*, a, b);
        swap_flag=1;                            /* Remember swap of a & b */
        end= a+ (int) (end-b);
      }
      if (keyseg->flag & HA_SPACE_PACK)
      {
        alength= *a++; blength= *b++;
        end=a+alength;
        next_key_length=key_length-blength-1;
      }
      else
      {
        alength= (int) (end-a);
        blength=keyseg->length;
        /* remove pre space from keys */
        for ( ; alength && *a == ' ' ; a++, alength--) ;
        for ( ; blength && *b == ' ' ; b++, blength--) ;
      }
      if (piks)
      {
	if (*a == '-')
	{
	  if (*b != '-')
	    return -1;
	  a++; b++;
	  swap_variables(uchar*, a, b);
	  swap_variables(int, alength, blength);
	  swap_flag=1-swap_flag;
	  alength--; blength--;
	  end=a+alength;
	}
	else if (*b == '-')
	  return 1;
	while (alength && (*a == '+' || *a == '0'))
	{
	  a++; alength--;
	}
	while (blength && (*b == '+' || *b == '0'))
	{
	  b++; blength--;
	}
	if (alength != blength)
	  return (alength < blength) ? -1 : 1;
	while (a < end)
	  if (*a++ !=  *b++)
	    return ((int) a[-1] - (int) b[-1]);
      }
      else
      {
        b+=(end-a);
        a=end;
      }

      if (swap_flag)                            /* Restore pointers */
        swap_variables(uchar*, a, b);
      break;
    }
    case HA_KEYTYPE_LONGLONG:
    {
      longlong ll_a,ll_b;
      ll_a= mi_sint8korr(a);
      ll_b= mi_sint8korr(b);
      if (piks && (flag = CMP_NUM(ll_a,ll_b)))
        return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
      a=  end;
      b+= 8;
      break;
    }
    case HA_KEYTYPE_ULONGLONG:
    {
      ulonglong ll_a,ll_b;
      ll_a= mi_uint8korr(a);
      ll_b= mi_uint8korr(b);
      if (piks && (flag = CMP_NUM(ll_a,ll_b)))
        return ((keyseg->flag & HA_REVERSE_SORT) ? -flag : flag);
      a=  end;
      b+= 8;
      break;
    }
    case HA_KEYTYPE_END:                        /* Ready */
      goto end;                                 /* diff_pos is incremented */
    }
  }
  (*diff_pos)++;
end:
  if (!(nextflag & SEARCH_FIND))
  {
    uint i;
    if (nextflag & (SEARCH_NO_FIND | SEARCH_LAST)) /* Find record after key */
      return (nextflag & (SEARCH_BIGGER | SEARCH_LAST)) ? -1 : 1;
    flag=0;
    for (i=keyseg->length ; i-- > 0 ; )
    {
      if (*a++ != *b++)
      {
        flag= FCMP(a[-1],b[-1]);
        break;
      }
    }
    if (nextflag & SEARCH_SAME)
      return (flag);                            /* read same */
    if (nextflag & SEARCH_BIGGER)
      return (flag <= 0 ? -1 : 1);              /* read next */
    return (flag < 0 ? -1 : 1);                 /* read previous */
  }
  return 0;
} /* ha_key_cmp */


 