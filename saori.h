#ifndef SAORI_H_
#define SAORI_H_

#include "base/request.h"
#include "base/response.h"

namespace saori {
    const char protocol_name[] = "SAORI";
    const char protocol_version[] = "1.0";
    const char request_value[] = "unused";
    const char request_arg[] = "Argument";
    const char response_value[] = "Result";
    const char response_arg[] = "Value";
    typedef base::Request<protocol_name, protocol_version, request_value, request_arg> Request;
    typedef base::Response<protocol_name, protocol_version, response_value, response_arg> Response;
}

#endif // SAORI_H_
