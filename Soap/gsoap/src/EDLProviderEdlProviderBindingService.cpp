/* EDLProviderEdlProviderBindingService.cpp
   Generated by gSOAP 2.8.17r from D:\Sandbox\EdlProvider\Soap\gsoap\src\EDLProviderInterface.h

Copyright(C) 2000-2013, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#include "EDLProviderEdlProviderBindingService.h"

EdlProviderBindingService::EdlProviderBindingService()
{	EdlProviderBindingService_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

EdlProviderBindingService::EdlProviderBindingService(const struct soap &_soap) : soap(_soap)
{ }

EdlProviderBindingService::EdlProviderBindingService(soap_mode iomode)
{	EdlProviderBindingService_init(iomode, iomode);
}

EdlProviderBindingService::EdlProviderBindingService(soap_mode imode, soap_mode omode)
{	EdlProviderBindingService_init(imode, omode);
}

EdlProviderBindingService::~EdlProviderBindingService()
{ }

void EdlProviderBindingService::EdlProviderBindingService_init(soap_mode imode, soap_mode omode)
{	soap_imode(this, imode);
	soap_omode(this, omode);
	static const struct Namespace namespaces[] =
{
	{"SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL},
	{"SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://schemas.xmlsoap.org/soap/encoding/", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"fimsdescription", "http://description.fims.tv", NULL, NULL},
	{"fims", "http://base.fims.tv", NULL, NULL},
	{"edlprovider", "http://temp/edlprovider", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};
	soap_set_namespaces(this, namespaces);
}

void EdlProviderBindingService::destroy()
{	soap_destroy(this);
	soap_end(this);
}

void EdlProviderBindingService::reset()
{	destroy();
	soap_done(this);
	soap_initialize(this);
	EdlProviderBindingService_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

#ifndef WITH_PURE_VIRTUAL
EdlProviderBindingService *EdlProviderBindingService::copy()
{	EdlProviderBindingService *dup = SOAP_NEW_COPY(EdlProviderBindingService(*(struct soap*)this));
	return dup;
}
#endif

int EdlProviderBindingService::soap_close_socket()
{	return soap_closesock(this);
}

int EdlProviderBindingService::soap_force_close_socket()
{	return soap_force_closesock(this);
}

int EdlProviderBindingService::soap_senderfault(const char *string, const char *detailXML)
{	return ::soap_sender_fault(this, string, detailXML);
}

int EdlProviderBindingService::soap_senderfault(const char *subcodeQName, const char *string, const char *detailXML)
{	return ::soap_sender_fault_subcode(this, subcodeQName, string, detailXML);
}

int EdlProviderBindingService::soap_receiverfault(const char *string, const char *detailXML)
{	return ::soap_receiver_fault(this, string, detailXML);
}

int EdlProviderBindingService::soap_receiverfault(const char *subcodeQName, const char *string, const char *detailXML)
{	return ::soap_receiver_fault_subcode(this, subcodeQName, string, detailXML);
}

void EdlProviderBindingService::soap_print_fault(FILE *fd)
{	::soap_print_fault(this, fd);
}

#ifndef WITH_LEAN
#ifndef WITH_COMPAT
void EdlProviderBindingService::soap_stream_fault(std::ostream& os)
{	::soap_stream_fault(this, os);
}
#endif

char *EdlProviderBindingService::soap_sprint_fault(char *buf, size_t len)
{	return ::soap_sprint_fault(this, buf, len);
}
#endif

void EdlProviderBindingService::soap_noheader()
{	this->header = NULL;
}

const SOAP_ENV__Header *EdlProviderBindingService::soap_header()
{	return this->header;
}

int EdlProviderBindingService::run(int port)
{	if (soap_valid_socket(this->master) || soap_valid_socket(bind(NULL, port, 100)))
	{	for (;;)
		{	if (!soap_valid_socket(accept()) || serve())
				return this->error;
			soap_destroy(this);
			soap_end(this);
		}
	}
	else
		return this->error;
	return SOAP_OK;
}

SOAP_SOCKET EdlProviderBindingService::bind(const char *host, int port, int backlog)
{	return soap_bind(this, host, port, backlog);
}

SOAP_SOCKET EdlProviderBindingService::accept()
{	return soap_accept(this);
}

#if defined(WITH_OPENSSL) || defined(WITH_GNUTLS)
int EdlProviderBindingService::ssl_accept()
{	return soap_ssl_accept(this);
}
#endif

int EdlProviderBindingService::serve()
{
#ifndef WITH_FASTCGI
	unsigned int k = this->max_keep_alive;
#endif
	do
	{

#ifndef WITH_FASTCGI
		if (this->max_keep_alive > 0 && !--k)
			this->keep_alive = 0;
#endif

		if (soap_begin_serve(this))
		{	if (this->error >= SOAP_STOP)
				continue;
			return this->error;
		}
		if (dispatch() || (this->fserveloop && this->fserveloop(this)))
		{
#ifdef WITH_FASTCGI
			soap_send_fault(this);
#else
			return soap_send_fault(this);
#endif
		}

#ifdef WITH_FASTCGI
		soap_destroy(this);
		soap_end(this);
	} while (1);
#else
	} while (this->keep_alive);
#endif
	return SOAP_OK;
}

static int serve___edlprovider__getInstalledEdls(EdlProviderBindingService*);
static int serve___edlprovider__getEdl(EdlProviderBindingService*);
static int serve___edlprovider__getEdlDouble(EdlProviderBindingService*);

int EdlProviderBindingService::dispatch()
{	soap_peek_element(this);
	if (!soap_match_tag(this, this->tag, "edlprovider:getEdlRequest"))
		return serve___edlprovider__getEdl(this);
	if (!soap_match_tag(this, this->tag, "edlprovider:getEdlDoubleRequest"))
		return serve___edlprovider__getEdlDouble(this);
	return serve___edlprovider__getInstalledEdls(this);
}

static int serve___edlprovider__getInstalledEdls(EdlProviderBindingService *soap)
{	struct __edlprovider__getInstalledEdls soap_tmp___edlprovider__getInstalledEdls;
	edlprovider__MapType edlprovider__installedEdlsResponse;
	edlprovider__installedEdlsResponse.soap_default(soap);
	soap_default___edlprovider__getInstalledEdls(soap, &soap_tmp___edlprovider__getInstalledEdls);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = soap->getInstalledEdls(&edlprovider__installedEdlsResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL;
	soap_serializeheader(soap);
	edlprovider__installedEdlsResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || edlprovider__installedEdlsResponse.soap_put(soap, "edlprovider:installedEdlsResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || edlprovider__installedEdlsResponse.soap_put(soap, "edlprovider:installedEdlsResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

static int serve___edlprovider__getEdl(EdlProviderBindingService *soap)
{	struct __edlprovider__getEdl soap_tmp___edlprovider__getEdl;
	edlprovider__EdlCreateResponseType edlprovider__getEdlResponse;
	edlprovider__getEdlResponse.soap_default(soap);
	soap_default___edlprovider__getEdl(soap, &soap_tmp___edlprovider__getEdl);
	if (!soap_get___edlprovider__getEdl(soap, &soap_tmp___edlprovider__getEdl, "-edlprovider:getEdl", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = soap->getEdl(soap_tmp___edlprovider__getEdl.edlprovider__getEdlRequest, &edlprovider__getEdlResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL;
	soap_serializeheader(soap);
	edlprovider__getEdlResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || edlprovider__getEdlResponse.soap_put(soap, "edlprovider:getEdlResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || edlprovider__getEdlResponse.soap_put(soap, "edlprovider:getEdlResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

static int serve___edlprovider__getEdlDouble(EdlProviderBindingService *soap)
{	struct __edlprovider__getEdlDouble soap_tmp___edlprovider__getEdlDouble;
	edlprovider__EdlCreateResponseType edlprovider__getEdlResponse;
	edlprovider__getEdlResponse.soap_default(soap);
	soap_default___edlprovider__getEdlDouble(soap, &soap_tmp___edlprovider__getEdlDouble);
	if (!soap_get___edlprovider__getEdlDouble(soap, &soap_tmp___edlprovider__getEdlDouble, "-edlprovider:getEdlDouble", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = soap->getEdlDouble(soap_tmp___edlprovider__getEdlDouble.edlprovider__getEdlDoubleRequest, &edlprovider__getEdlResponse);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = NULL;
	soap_serializeheader(soap);
	edlprovider__getEdlResponse.soap_serialize(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || edlprovider__getEdlResponse.soap_put(soap, "edlprovider:getEdlResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || edlprovider__getEdlResponse.soap_put(soap, "edlprovider:getEdlResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}
/* End of server object code */
