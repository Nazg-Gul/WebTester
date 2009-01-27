/**
 * WebTester Server - server of on-line testing system
 *
 * HTTP network stuff abstraction
 *
 * Copyright 2008 Sergey I. Sharybin <g,ulairi@gmail.com>
 *
 * This program can be distributed under the terms of the GNU GPL.
 * See the file COPYING.
 */

#include <malloc.h>
#include <string.h>
#include "network-soup.h"
#include "macrodef.h"

/**
 * Initialize HTTP stuff
 *
 * @return zero on success, non-zero otherwise
 */
int
http_init (void)
{
  g_type_init ();
  return 0;
}

/**
 * Uninitialize HTTP stuff
 */
void
http_done (void) {
}

/**
 * Start new HTTP session
 *
 * @param __async - is session asynchronic?
 * @return sessions's descriptor on success, NULL otherwise
 */
http_session_t*
http_session_new (int __async)
{
  SoupSession *session;

  DEBUG_LOG ("network-soup", "Open new session\n");

  if (!__async)
    {
      session = soup_session_sync_new ();
    }
  else
    {
      session = soup_session_async_new ();
    }

  if (!session)
    {
      return NULL;
    }

  return session;
}

/**
 * Estended start new HTTP session
 *
 * @param __async - is session asynchronic?
 * @param __proxy - name of proxy to use
 * @param __ca_file - certificate file to use
 * @return sessions's descriptor on success, NULL otherwise
 */
http_session_t*
http_session_new_extended (int __async, const char *__proxy,
                           const char *__ca_file)
{
  SoupUri *proxy = NULL;
  char *ca_file = 0;
  SoupSession *session;

  DEBUG_LOG ("network-soup", "Extended open new session\n");

  if (__proxy && strcmp (__proxy, ""))
    {
      proxy = soup_uri_new (__proxy);
    }

  if (__ca_file && strcmp (__ca_file, ""))
    {
      ca_file = (char*)__ca_file;
    }

  if (!__async)
    {
      session = soup_session_sync_new_with_options (SOUP_SESSION_SSL_CA_FILE,
                                                    ca_file,
                                                    SOUP_SESSION_PROXY_URI,
                                                    proxy, NULL);
    }
  else
    {
      session = soup_session_sync_new_with_options (SOUP_SESSION_SSL_CA_FILE,
                                                    ca_file,
                                                    SOUP_SESSION_PROXY_URI,
                                                    proxy, NULL);
    }

  if (!session)
    {
      return NULL;
    }

  return session;
}

/**
 * Close HTTP session
 *
 * @param __self - session to close
 */
void
http_session_free (http_session_t *__self)
{
  DEBUG_LOG ("network-soup", "Close session\n");

  if (!__self)
    {
      return;
    }

  g_object_unref (__self);
}

/**
 * Send message through HTTP session
 *
 * @param __session - HTTP session to send through
 * @param __message - HTTP message to send
 * @return sero on success, non-zero otherwise
 */
int
http_session_send_message (http_session_t *__session,
                           http_message_t *__message)
{
  int res;
  char *uri;
  if (!__session || !__message)
    {
      return -1;
    }

#if defined(__DEBUG) || defined(USER_DEBUG)
  uri = soup_uri_to_string (soup_message_get_uri (__message), 0);
  DEBUG_LOG ("network-soup", "Sending HTTP request to %s\n", uri);
  free (uri);
#endif

  res = soup_session_send_message (__session, __message);
  if (__message->response.length > 0)
    {
      char *sbody = __message->response.body;
      __message->response.body = malloc (__message->response.length + 1);
      memset (__message->response.body, 0, __message->response.length + 1);
      strncpy (__message->response.body, sbody, __message->response.length);
      free (sbody);
    }

  return res;
}

/**
 * Prepare HTTP message
 *
 * @param __method - method of sending HTTM message
 * @param __url - url to send message to
 * @return descriptor of HTTP message
 */
http_message_t*
http_message_prepare (const char *__method, const char *__url)
{
  SoupMessage *msg = NULL;
  msg = soup_message_new (__method, __url);

  if (!msg)
    {
      return NULL;
    }

  return msg;
}

/**
 * Free HTTP message
 *
 * @param __self - HTTP message to free
 */
void
http_message_free (http_message_t *__self)
{
  if (!__self)
    {
      return;
    }

  g_object_unref (__self);
}

/**
 * Get HTTP message response body
 *
 * @param __self - HTTP message
 * @return response body
 */
const char*
http_message_get_response_body (const http_message_t *__self)
{
  return __self->response.body;
}

/**
 * Get HTTP message response body length
 *
 * @param __self - HTTP message
 * @return response body length
 */
int
http_message_get_response_length (const http_message_t *__self)
{
  return __self->response.length;
}

/**
 * Get HTTP error description
 *
 * @param __msg - HTTP message to get error code from
 * @param __out - output buffer
 */
void
http_get_error (const http_message_t *__msg, char *__out)
{
  switch (__msg->status_code)
    {
    case 200: strcpy (__out, "200 OK");
      break;

    case 300: strcpy (__out, "300 Multiple Choices");
      break;

    case 301: strcpy (__out, "201 Moved Permanently");
      break;

    case 302: strcpy (__out, "302 Found");
      break;

    case 400: strcpy (__out, "400 Bad Request");
      break;

    case 403: strcpy (__out, "403 Forbidden");
      break;

    case 404: strcpy (__out, "404 Not Found");
      break;

    case 500: strcpy (__out, "500 Internal Server Error");
      break;

    case 501: strcpy (__out, "501 Not Implemented");
      break;

    default:
      sprintf (__out, "Sorry, but desription is unguessable "
                      "(status_code: %d)", __msg->status_code);
    }
}

/**
 * Encode URL
 *
 * @param __url - url to encode
 * @param __out - output buffer
 */
void
urlencode (const char *__url, char *__out)
{
  char *pchar = soup_uri_encode (__url, 0);
  strcpy (__out, pchar);
  free (pchar);
}
