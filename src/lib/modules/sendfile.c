/*
 * Copyright (c) 2011-2012 Yaroslav Stavnichiy <yarosla@gmail.com>
 *
 * This file is part of NXWEB.
 *
 * NXWEB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * NXWEB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NXWEB. If not, see <http://www.gnu.org/licenses/>.
 */

#include "nxweb.h"

#include <unistd.h>
#include <pthread.h>
#include <sys/fcntl.h>

#define MAX_PATH 1024

static nxweb_result sendfile_generate_cache_key(nxweb_http_server_connection* conn, nxweb_http_request* req, nxweb_http_response* resp) {
  if (!req->get_method || req->content_length) return NXWEB_NEXT; // do not respond to POST requests, etc.

  nxweb_handler* handler=conn->handler;
  const char* document_root=handler->dir;
  assert(document_root);
  assert(handler->index_file);
  nxb_buffer* nxb=req->nxb;

  int rlen=strlen(document_root);
  nxb_append(nxb, document_root, rlen);
  const char* q=strchr(req->path_info, '?');
  int plen=q? q-req->path_info : strlen(req->path_info);
  nxb_append(nxb, req->path_info, plen);
  nxb_append_char(nxb, '\0');
  nxb_blank(nxb, strlen(handler->index_file));
  char* fpath=nxb_finish_stream(nxb, 0);
  char* path_info=fpath+rlen;
  nxweb_url_decode(path_info, 0);
  plen=strlen(path_info);
  if (plen>0 && path_info[plen-1]=='/') { // directory index
    strcat(path_info+plen, handler->index_file);
  }
  if (nxweb_remove_dots_from_uri_path(path_info)) {
    //nxweb_send_http_error(resp, 404, "Not Found");
    return NXWEB_NEXT;
  }
  resp->cache_key=fpath;
  resp->sendfile_path=fpath;

  resp->mtype=nxweb_get_mime_type_by_ext(fpath);
  if (resp->mtype) {
    resp->content_type=resp->mtype->mime;
    if (resp->mtype->charset_required) resp->content_charset=handler->charset;
  }
  return NXWEB_OK;
}

static nxweb_result sendfile_on_select(nxweb_http_server_connection* conn, nxweb_http_request* req, nxweb_http_response* resp) {
  if (!req->get_method || req->content_length) return NXWEB_NEXT; // do not respond to POST requests, etc.

  const char* fpath=resp->sendfile_path;
  assert(fpath);
  struct stat* finfo=&resp->sendfile_info;

  if (!finfo->st_ino && stat(fpath, finfo)==-1) {
    // file not found => let other handlers pick up this request
    return NXWEB_NEXT;
  }

  if (S_ISDIR(finfo->st_mode)) {
    const char* path_info=fpath+strlen(conn->handler->dir);
    nxweb_send_redirect2(resp, 302, path_info, "/", conn->secure);
    nxweb_start_sending_response(conn, resp);
    return NXWEB_OK;
  }

/*
  if (!S_ISREG(finfo.st_mode)) { // symlink or ...?
    nxweb_send_http_error(resp, 403, "Forbidden");
    nxweb_start_sending_response(conn, resp);
    return NXWEB_ERROR;
  }
*/

  if (req->if_modified_since && finfo->st_mtime<=req->if_modified_since
      && resp->mtype && !resp->mtype->ssi_on && !resp->mtype->templates_on) {
    resp->status_code=304;
    resp->status="Not Modified";
    nxweb_start_sending_response(conn, resp);
    return NXWEB_OK;
  }

  int result=nxweb_send_file(resp, (char*)fpath, finfo, 0, 0, 0, resp->mtype, conn->handler->charset);
  if (result!=0) { // should not happen
    nxweb_log_error("sendfile: [%s] stat() was OK, but open() failed", fpath);
    nxweb_send_http_error(resp, 500, "Internal Server Error");
    return NXWEB_ERROR;
  }

  if (S_ISVTX & finfo->st_mode) { // sTicky bit (use chmod +t filename to set)
    resp->templates_on=1; // activate templates processing
    // NOTE content type still depends on extension
  }

  nxweb_start_sending_response(conn, resp);
  return NXWEB_OK;
}

nxweb_handler nxweb_sendfile_handler={.on_select=sendfile_on_select,
        .on_generate_cache_key=sendfile_generate_cache_key,
        .flags=NXWEB_HANDLE_GET};
