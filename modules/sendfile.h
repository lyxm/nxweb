/*
 * Copyright (c) 2011 Yaroslav Stavnichiy <yarosla@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SENDFILE_H
#define	SENDFILE_H

#ifdef	__cplusplus
extern "C" {
#endif

// Other modules can use this function to send files:
nxweb_result nxweb_sendfile_try(nxweb_http_server_connection* conn, nxweb_http_response* resp,
        char* fpath, char* path_info, // path_info MUST point to fpath+doc_root_len; path_info MUST be contained in fpath; function modifies this string by appending index file name, etc.
        nxe_time_t use_cache_time, // 0= do not use cache; otherwise cache small files for this amount of time
        _Bool try_gzip_encoding, // try appending ".gz" to path_info to see if gzip-encoded content is available
        const struct stat* finfo, const nxweb_mime_type* mtype // optional; if you already have these, then provide to avoid extra calls
        );

#ifdef	__cplusplus
}
#endif

#endif	/* SENDFILE_H */

