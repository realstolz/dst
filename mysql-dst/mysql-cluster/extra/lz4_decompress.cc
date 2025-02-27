/*
   Copyright (c) 2014, 2015, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <my_global.h>
#include <welcome_copyright_notice.h>

#include "lz4/lz4frame.h"
#include <string.h>

static void usage()
{
  printf("%s  Ver 1.7 for %s at %s\n","lz4_decompress",SYSTEM_TYPE,
	 MACHINE_TYPE);
  puts(ORACLE_WELCOME_COPYRIGHT_NOTICE("2015"));
  puts("Decompress data compressed by mysqlpump using lz4 compression "
    "algorithm by reading from input file and writing uncompressed "
    "data to output file");
  printf("Usage: %s input_file output_file\n", "lz4_decompress");
}

static const int INPUT_BUFFER_SIZE= 1024 * 1024;
static const int OUTPUT_BUFFER_SIZE= 1024 * 1024;

int main(int argc, char **argv)
{
  if (argc != 3)
  {
    usage();
    exit(1);
  }
  FILE* input_file= fopen(argv[1], "rb");
  FILE* output_file= fopen(argv[2], "wb");
  if (input_file == NULL)
  {
    fprintf(stderr,
      "lz4_decompress: [Error] Cannot open input file for reading.\n"
      );
    exit(1);
  }
  if (output_file == NULL)
  {
    fprintf(stderr,
      "lz4_decompress: [Error] Cannot create output file.\n"
      );
    exit(1);
  }
  LZ4F_decompressionContext_t decompression_context;
  LZ4F_createDecompressionContext(&decompression_context, LZ4F_VERSION);
  char* input_buffer= new char[INPUT_BUFFER_SIZE];
  char* output_buffer= new char[OUTPUT_BUFFER_SIZE];
  bool is_eof= false;

  while (!is_eof)
  {
    int bytes_read= fread(input_buffer, 1, INPUT_BUFFER_SIZE, input_file);

    if (bytes_read < INPUT_BUFFER_SIZE)
    {
      is_eof= feof(input_file);
      if (!is_eof)
      {
        fprintf(stderr,
          "lz4_decompress: [Error] Encountered problem during reading input.\n"
          );
        exit(1);
      }
    }

    size_t bytes_processed= 0;

    for (;;)
    {
      size_t current_bytes_processed= bytes_read - bytes_processed;
      size_t bytes_to_write= OUTPUT_BUFFER_SIZE;
      size_t result= LZ4F_decompress(decompression_context, output_buffer,
        &bytes_to_write, input_buffer + bytes_processed,
        &current_bytes_processed, NULL);
      if (LZ4F_isError(result))
      {
        fprintf(stderr,
          "lz4_decompress: [Error] Encountered problem during decompression.\n"
          );
        exit(1);
      }
      bytes_processed+= current_bytes_processed;

      if (bytes_to_write == 0)
        break;

      fwrite(output_buffer, 1, bytes_to_write, output_file);
    }
  }

  LZ4F_freeDecompressionContext(decompression_context);
  delete[] input_buffer;
  delete[] output_buffer;
  fclose(input_file);
  fclose(output_file);
  exit(0);
}
 