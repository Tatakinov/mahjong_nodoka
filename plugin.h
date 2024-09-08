#ifndef PLUGIN_H_
#define PLUGIN_H_

#include "base/request.h"
#include "base/response.h"

namespace plugin {
    const char protocol_name[] = "PLUGIN";
    const char protocol_version[] = "2.0";
    const char request_value[] = "ID";
    const char request_arg[] = "Reference";
    const char response_value[] = "Event";
    const char response_arg[] = "Reference";
    typedef base::Request<protocol_name, protocol_version, request_value, request_arg> Request;
    typedef base::Response<protocol_name, protocol_version, response_value, response_arg> Response;
}

#endif // PLUGIN_H_
