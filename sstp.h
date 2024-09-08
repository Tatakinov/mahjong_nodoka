#ifndef SSTP_H_
#define SSTP_H_

#include "base/request.h"
#include "base/response.h"

namespace sstp {
    const char protocol_name[] = "SSTP";
    const char protocol_version[] = "1.4";
    const char request_value[] = "Event";
    const char request_arg[] = "Reference";
    const char response_value[] = "Script";
    const char response_arg[] = "unused";
    typedef base::Request<protocol_name, protocol_version, request_value, request_arg> Request;
    typedef base::Response<protocol_name, protocol_version, response_value, response_arg> Response;
}

#endif // SSTP_H_
