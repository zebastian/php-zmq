/*
+-----------------------------------------------------------------------------------+
|  ZMQ extension for PHP                                                            |
|  Copyright (c) 2010-2013, Mikko Koppanen <mkoppanen@php.net>                      |
|  All rights reserved.                                                             |
+-----------------------------------------------------------------------------------+
|  Redistribution and use in source and binary forms, with or without               |
|  modification, are permitted provided that the following conditions are met:      |
|     * Redistributions of source code must retain the above copyright              |
|       notice, this list of conditions and the following disclaimer.               |
|     * Redistributions in binary form must reproduce the above copyright           |
|       notice, this list of conditions and the following disclaimer in the         |
|       documentation and/or other materials provided with the distribution.        |
|     * Neither the name of the copyright holder nor the                            |
|       names of its contributors may be used to endorse or promote products        |
|       derived from this software without specific prior written permission.       |
+-----------------------------------------------------------------------------------+
|  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  |
|  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED    |
|  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           |
|  DISCLAIMED. IN NO EVENT SHALL MIKKO KOPPANEN BE LIABLE FOR ANY                   |
|  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES       |
|  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;     |
|  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND      |
|  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       |
|  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    |
|  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                     |
+-----------------------------------------------------------------------------------+
*/

#include "php_zmq.h"
#include "php_zmq_private.h"

static size_t php_zmq_fd_read(php_stream *stream, char *buf, size_t count)
{
	return 0;
}

static size_t php_zmq_fd_write(php_stream *stream, const char *buf, size_t count)
{
	return 0;
}

static int php_zmq_fd_close(php_stream *stream, int close_handle)
{
	zval *obj = (zval *) stream->abstract;
	zend_objects_store_del_ref(obj);
	Z_DELREF_P(obj);
	return EOF;
}

static int php_zmq_fd_flush(php_stream *stream)
{
	return FAILURE;
}

static int php_zmq_fd_cast(php_stream *stream, int cast_as, void **ret)
{
	zval *obj = (zval *) stream->abstract;
	php_zmq_socket_object *intern = (php_zmq_socket_object *) zend_object_store_get_object(obj);

	switch (cast_as)	{
		case PHP_STREAM_AS_FD_FOR_SELECT:
		case PHP_STREAM_AS_FD:
		case PHP_STREAM_AS_SOCKETD:
			if (ret) {
				size_t optsiz = sizeof (int);
				if (zmq_getsockopt(intern->socket->z_socket, ZMQ_FD, (int*) ret, &optsiz) != 0) {
					return FAILURE;
				}
			}
			return SUCCESS;
		default:
			return FAILURE;
	}
}

static php_stream_ops php_stream_zmq_fd_ops = {
	php_zmq_fd_write, php_zmq_fd_read,
	php_zmq_fd_close, php_zmq_fd_flush,
	"ZMQ_FD",
	NULL, /* seek */
	php_zmq_fd_cast, /* cast */
	NULL, /* stat */
	NULL  /* set_option */
};

php_stream *php_zmq_create_zmq_fd(zval *obj)
{
	php_stream *stream;
	stream = php_stream_alloc(&php_stream_zmq_fd_ops, obj, NULL, "r");

	if (stream) {
		zend_objects_store_add_ref(obj);
		Z_ADDREF_P(obj);
		return stream;
	}
	return NULL;
}
/* }}} */
