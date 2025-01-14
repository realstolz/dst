/* Copyright (c) 2000, 2013, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/* Quick & light hash implementation for tab completion purposes
 *
 * by  Andi Gutmans <andi@zend.com>
 * and Zeev Suraski <zeev@zend.com>
 * Small portability changes by Monty. Changed also to use my_malloc/my_free
 */

#include <my_global.h>
#include <m_string.h>
#include <my_sys.h>
#include "completion_hash.h"

uint hashpjw(const char *arKey, uint nKeyLength)
{
  uint h = 0, g, i;

  for (i = 0; i < nKeyLength; i++) {
    h = (h << 4) + arKey[i];
    if ((g = (h & 0xF0000000))) {
      h = h ^ (g >> 24);
      h = h ^ g;
    }
  }
  return h;
}

int completion_hash_init(HashTable *ht, uint nSize)
{
  ht->arBuckets = (Bucket **) my_malloc(PSI_NOT_INSTRUMENTED,
                                        nSize* sizeof(Bucket *),
					MYF(MY_ZEROFILL | MY_WME));

  if (!ht->arBuckets)
  {
    ht->initialized = 0;
    return FAILURE;
  }
  init_alloc_root(PSI_NOT_INSTRUMENTED, &ht->mem_root, 8192, 0);
  ht->pHashFunction = hashpjw;
  ht->nTableSize = nSize;
  ht->initialized = 1;
  return SUCCESS;
}


int completion_hash_update(HashTable *ht, char *arKey, uint nKeyLength,
			   char *str)
{
  uint h, nIndex;

  Bucket *p;

  h = ht->pHashFunction(arKey, nKeyLength);
  nIndex = h % ht->nTableSize;

  if (nKeyLength <= 0) {
    return FAILURE;
  }
  p = ht->arBuckets[nIndex];
  while (p)
  {
    if ((p->h == h) && (p->nKeyLength == nKeyLength)) {
      if (!memcmp(p->arKey, arKey, nKeyLength)) {
	entry *n;

	if (!(n = (entry *) alloc_root(&ht->mem_root,sizeof(entry))))
          return FAILURE;
	n->pNext = p->pData;
	n->str = str;
	p->pData = n;
	p->count++;

	return SUCCESS;
      }
    }
    p = p->pNext;
  }

  if (!(p = (Bucket *) alloc_root(&ht->mem_root, sizeof(Bucket))))
    return FAILURE;

  p->arKey = arKey;
  p->nKeyLength = nKeyLength;
  p->h = h;

  if (!(p->pData = (entry*) alloc_root(&ht->mem_root, sizeof(entry))))
    return FAILURE;

  p->pData->str = str;
  p->pData->pNext = 0;
  p->count = 1;

  p->pNext = ht->arBuckets[nIndex];
  ht->arBuckets[nIndex] = p;

  return SUCCESS;
}

static Bucket *completion_hash_find(HashTable *ht, const char *arKey,
				    uint nKeyLength)
{
  uint h, nIndex;
  Bucket *p;

  h = ht->pHashFunction(arKey, nKeyLength);
  nIndex = h % ht->nTableSize;

  p = ht->arBuckets[nIndex];
  while (p)
  {
    if ((p->h == h) && (p->nKeyLength == nKeyLength)) {
      if (!memcmp(p->arKey, arKey, nKeyLength)) {
	return p;
      }
    }
    p = p->pNext;
  }
  return (Bucket*) 0;
}


int completion_hash_exists(HashTable *ht, char *arKey, uint nKeyLength)
{
  uint h, nIndex;
  Bucket *p;

  h = ht->pHashFunction(arKey, nKeyLength);
  nIndex = h % ht->nTableSize;

  p = ht->arBuckets[nIndex];
  while (p)
  {
    if ((p->h == h) && (p->nKeyLength == nKeyLength))
    {
      if (!strcmp(p->arKey, arKey)) {
	return 1;
      }
    }
    p = p->pNext;
  }
  return 0;
}

Bucket *find_all_matches(HashTable *ht, const char *str, uint length,
			 uint *res_length)
{
  Bucket *b;

  b = completion_hash_find(ht,str,length);
  if (!b) {
    *res_length = 0;
    return (Bucket*) 0;
  } else {
    *res_length = length;
    return b;
  }
}

Bucket *find_longest_match(HashTable *ht, char *str, uint length,
			   uint *res_length)
{
  Bucket *b,*return_b;
  char *s;
  uint count;
  uint lm;

  b = completion_hash_find(ht,str,length);
  if (!b) {
    *res_length = 0;
    return (Bucket*) 0;
  }

  count = b->count;
  lm = length;
  s = b->pData->str;

  return_b = b;
  while (s[lm]!=0 && (b=completion_hash_find(ht,s,lm+1))) {
    if (b->count<count) {
      *res_length=lm;
      return return_b;
    }
    return_b=b;
    lm++;
  }
  *res_length=lm;
  return return_b;
}


void completion_hash_clean(HashTable *ht)
{
  free_root(&ht->mem_root,MYF(0));
  memset(ht->arBuckets, 0, ht->nTableSize * sizeof(Bucket *));
}


void completion_hash_free(HashTable *ht)
{
  completion_hash_clean(ht);
  my_free(ht->arBuckets);
}


void add_word(HashTable *ht,char *str)
{
  int i;
  char *pos=str;
  for (i=1; *pos; i++, pos++)
    completion_hash_update(ht, str, i, str);
}
 