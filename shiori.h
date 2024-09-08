#ifndef SHIORI_H_
#define SHIORI_H_

#include "base/request.h"
#include "base/response.h"

namespace shiori {
    const char protocol_name[] = "SHIORI";
    const char protocol_version[] = "3.0";
    const char request_value[] = "ID";
    const char request_arg[] = "Reference";
    const char response_value[] = "Value";
    const char response_arg[] = "Reference";
    typedef base::Request<protocol_name, protocol_version, request_value, request_arg> Request;
    typedef base::Response<protocol_name, protocol_version, response_value, response_arg> Response;
}

#endif // SHIORI_H_
