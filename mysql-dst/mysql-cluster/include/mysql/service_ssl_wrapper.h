#ifndef SSL_WRAPPER_INCLUDED
#define SSL_WRAPPER_INCLUDED

/* Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.

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

#include "violite.h"

namespace ssl_wrappe_service
{

int MY_ATTRIBUTE((visibility("default")))
dummy_function_to_ensure_we_are_linked_into_the_server();

} // ssl_wrappe_service

/**
  Return version of SSL used in currect connection

  @param vio              VIO connection descriptor
  @param buffer           Character buffer in which the version is going to be placed
  @param buffer_size      Size of the character buffer
*/
extern "C"
void ssl_wrapper_version(Vio *vio, char *version, const size_t version_size);

/**
  Return cipher used in current connection

  @param vio              VIO connection descriptor
  @param buffer           Character buffer in which the cipher name is going to be placed
  @param buffer_size      Size of the character buffer
*/
extern "C"
void ssl_wrapper_cipher(Vio *vio, char *cipher, const size_t cipher_size);

/**
  Return cipher list that can be used for SSL

  @param vio                       VIO connection descriptor
  @param clipher_list              Pointer to an array of c-strings
  @param maximun_num_of_elements   Size of the pointer array
*/
extern "C"
long ssl_wrapper_cipher_list(Vio *vio, const char **clipher_list, const size_t maximun_num_of_elements);

/**
  Return the verification depth limit set in SSL

  @param vio              VIO connection descriptor

  @return
    -1 default values should be used
    >0 verification depth
*/
extern "C"
long ssl_wrapper_verify_depth(Vio *vio);

/**
  Return the verification mode set in SSL

  @param vio              VIO connection descriptor

  @return
    -1 default values should be used
    >0 verification mode
*/
extern "C"
long ssl_wrapper_verify_mode(Vio *vio);

/**
  Return issuer name form peers ssl certificate

  @param vio              VIO connection descriptor
  @param issuer           Character buffer in which the issuer name is going to be placed
  @param issuer_size      Size of character buffer for the issuer name
*/
extern "C"
void ssl_wrapper_get_peer_certificate_issuer(Vio *vio, char *issuer, const size_t issuer_size);

/**
  Return subject field form peers ssl certificate

  @param vio              VIO connection descriptor
  @param subject          Character buffer in which the subject is going to be placed
  @param subject_size     Size of character buffer for the subject
*/
extern "C"
void ssl_wrapper_get_peer_certificate_subject(Vio *vio, char *subject, const size_t subject_size);

/**
  Check is peer certificate is present and try to verify it

  @param vio                 VIO connection descriptor

  @return
    X509_V_OK verification of peer certificate succeeded
    -1        verification failed
*/
extern "C"
long ssl_wrapper_get_verify_result_and_cert(Vio *vio);

/**
  Return the verification depth limit set in SSL context

  @param vio_ssl              VIO SSL contex descriptor

  @return
    -1 default values should be used
    >0 verification depth
*/
extern "C"
long ssl_wrapper_ctx_verify_depth(struct st_VioSSLFd *vio_ssl);

/**
  Return the verification mode set in SSL context

  @param vio_ssl              VIO SSL contex descriptor

  @return
    -1 default values should be used
    >0 verification mode
*/
extern "C"
long ssl_wrapper_ctx_verify_mode(struct st_VioSSLFd *vio_ssl);

/**
  Return the last day the server certificate is valid

  @param vio_ssl              VIO SSL contex descriptor
  @param no_after             Character buffer for to be filed with the date in human readble format
  @param no_after_size        Size of the character buffer
*/
extern "C"
void  ssl_wrapper_ctx_server_not_after(struct st_VioSSLFd *vio_ssl, char *no_after, const size_t no_after_size);

/**
  Return the first day the server certificate is valid

  @param vio_ssl              VIO SSL contex descriptor
  @param no_before            Character buffer for to be filed with the date in human readble format
  @param no_before_size       Size of the character buffer
*/
extern "C"
void ssl_wrapper_ctx_server_not_before(struct st_VioSSLFd *vio_ssl, char *no_before, const size_t no_before_size);

extern "C"
void ssl_wrapper_thread_cleanup();

extern "C"
long ssl_wrapper_sess_accept(struct st_VioSSLFd *vio_ssl);

/**
  Cleanup data allocated by SSL on thread stack

*/
extern "C"
long ssl_wrapper_sess_accept_good(struct st_VioSSLFd *vio_ssl);

#endif /* SSL_WRAPPER_INCLUDED */

 