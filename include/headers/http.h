#ifndef SNAIL_HTTP_H
#define SNAIL_HTTP_H

#define GET                             1
#define HEAD                            2
#define POST                            3
#define PUT                             4
#define DELETE                          5
#define CONNECT                         6
#define OPTIONS                         7
#define TRACE                           8
#define PATH                            9

#define HTTP_METHOD_MAP(XX)                                                  \
  XX(GET                                , "GET")                             \
  XX(HEAD                               , "HEAD")                            \
  XX(POST                               , "POST")                            \
  XX(PUT                                , "PUT")                             \
  XX(DELETE                             , "DELETE")                          \
  XX(CONNECT                            , "CONNECT")                         \
  XX(OPTIONS                            , "OPTIONS")                         \
  XX(TRACE                              , "TRACE")                           \
  XX(PATH                               , "PATH")                            \

#define CONTINUE                        100
#define SWITCHING_PROTOCOLS             101
#define PROCESSING                      102
#define OK                              200
#define CREATED                         201
#define ACCEPTED                        202
#define NON_AUTHORITATIVE_INFORMATION   203
#define NO_CONTENT                      204
#define RESET_CONTENT                   205
#define PARTIAL_CONTENT                 206
#define MULTI_STATUS                    207
#define ALREADY_REPORTED                208
#define IM_USED                         226
#define MULTIPLE_CHOICES                300
#define MOVED_PERMANENTLY               301
#define FOUND                           302
#define SEE_OTHER                       303
#define NOT_MODIFIED                    304
#define USE_PROXY                       305
#define SWITCH_PROXY                    306
#define TEMPORARY_REDIRECT              307
#define PERMANENT_REDIRECT              308
#define BAD_REQUEST                     400
#define UNAUTHORIZED                    401
#define PAYMENT_REQUIRED                402
#define FORBIDDEN                       403
#define NOT_FOUND                       404
#define METHOD_NOT_ALLOWED              405
#define NOT_ACCEPTABLE                  406
#define PROXY_AUTHENTICATION_REQUIRED   407
#define REQUEST_TIMEOUT                 408
#define CONFLICT                        409
#define GONE                            410
#define LENGTH_REQUIRED                 411
#define PRECONDITION_FAILED             412
#define PAYLOAD_TOO_LARGE               413
#define URI_TOO_LONG                    414
#define UNSUPPORTED_MEDIA_TYPE          415
#define RANGE_NOT_SATISFIABLE           416
#define EXPECTATION_FAILED              417
#define I_AM_A_TEAPOT                   418
#define MISDIRECTED_REQUEST             421
#define UNPROCESSABLE_ENTITY            422
#define LOCKED                          423
#define FAILED_DEPENDENCY               424
#define UPGRADE_REQUIRED                426
#define PRECONDITION_REQUIRED           428
#define TOO_MANY_REQUESTS               429
#define REQUEST_HEADER_FIELDS_TOO_LARGE 431
#define UNAVAILABLE_FOR_LEGAL_REASONS   451
#define INTERNAL_SERVER_ERROR           500
#define NOT_IMPLEMENTED                 501
#define BAD_GATEWAY                     502
#define SERVICE_UNAVAILABLE             503
#define GATEWAY_TIMEOUT                 504
#define HTTP_VERSION_NOT_SUPPORTED      505
#define VARIANT_ALSO_NEGOTIATES         506
#define INSUFFICIENT_STORAGE            507
#define LOOP_DETECTED                   508
#define NOT_EXTENDED                    510
#define NETWORK_AUTHENTICATION_REQUIRED 511

#define HTTP_CODE_MAP(XX)                                                    \
  XX(CONTINUE                           , "Continue")                        \
  XX(SWITCHING_PROTOCOLS                , "Switching Protocols")             \
  XX(PROCESSING                         , "Processing")                      \
  XX(OK                                 , "OK")                              \
  XX(CREATED                            , "Created")                         \
  XX(ACCEPTED                           , "Accepted")                        \
  XX(NON_AUTHORITATIVE_INFORMATION      , "Non-Authoritative Information")   \
  XX(NO_CONTENT                         , "No Content")                      \
  XX(RESET_CONTENT                      , "Reset Content")                   \
  XX(PARTIAL_CONTENT                    , "Partial Content")                 \
  XX(MULTI_STATUS                       , "Multi-Status")                    \
  XX(ALREADY_REPORTED                   , "Already Reported")                \
  XX(IM_USED                            , "IM Used")                         \
  XX(MULTIPLE_CHOICES                   , "Multiple Choices")                \
  XX(MOVED_PERMANENTLY                  , "Moved Permanently")               \
  XX(FOUND                              , "Found")                           \
  XX(SEE_OTHER                          , "See Other")                       \
  XX(NOT_MODIFIED                       , "Not Modified")                    \
  XX(USE_PROXY                          , "Use Proxy")                       \
  XX(SWITCH_PROXY                       , "Switch Proxy")                    \
  XX(TEMPORARY_REDIRECT                 , "Temporary Redirect ")             \
  XX(PERMANENT_REDIRECT                 , "Permanent Redirect")              \
  XX(BAD_REQUEST                        , "Bad Request")                     \
  XX(UNAUTHORIZED                       , "Unauthorized")                    \
  XX(PAYMENT_REQUIRED                   , "Payment Required")                \
  XX(FORBIDDEN                          , "Forbidden")                       \
  XX(NOT_FOUND                          , "Not Found")                       \
  XX(METHOD_NOT_ALLOWED                 , "Method Not Allowed")              \
  XX(NOT_ACCEPTABLE                     , "Not Acceptable")                  \
  XX(PROXY_AUTHENTICATION_REQUIRED      , "Proxy Authentication Required")   \
  XX(REQUEST_TIMEOUT                    , "Request Timeout")                 \
  XX(CONFLICT                           , "Conflict")                        \
  XX(GONE                               , "Gone")                            \
  XX(LENGTH_REQUIRED                    , "Length Required")                 \
  XX(PRECONDITION_FAILED                , "Precondition Failed")             \
  XX(PAYLOAD_TOO_LARGE                  , "Payload Too Large")               \
  XX(URI_TOO_LONG                       , "URI Too Long")                    \
  XX(UNSUPPORTED_MEDIA_TYPE             , "Unsupported Media Type")          \
  XX(RANGE_NOT_SATISFIABLE              , "Range Not Satisfiable")           \
  XX(EXPECTATION_FAILED                 , "Expectation Failed")              \
  XX(I_AM_A_TEAPOT                      , "I'm a teapot")                    \
  XX(MISDIRECTED_REQUEST                , "Misdirected Request")             \
  XX(UNPROCESSABLE_ENTITY               , "Unprocessable Content")           \
  XX(LOCKED                             , "Locked")                          \
  XX(FAILED_DEPENDENCY                  , "Failed Dependency")               \
  XX(UPGRADE_REQUIRED                   , "Upgrade Required")                \
  XX(PRECONDITION_REQUIRED              , "Precondition Required")           \
  XX(TOO_MANY_REQUESTS                  , "Too Many Requests")               \
  XX(REQUEST_HEADER_FIELDS_TOO_LARGE    , "Request Header Fields Too Large") \
  XX(UNAVAILABLE_FOR_LEGAL_REASONS      , "Unavailable For Legal Reasons")   \
  XX(INTERNAL_SERVER_ERROR              , "Internal Server Error")           \
  XX(NOT_IMPLEMENTED                    , "Not Implemented")                 \
  XX(BAD_GATEWAY                        , "Bad Gateway")                     \
  XX(SERVICE_UNAVAILABLE                , "Service Unavailable")             \
  XX(GATEWAY_TIMEOUT                    , "Gateway Timeout")                 \
  XX(HTTP_VERSION_NOT_SUPPORTED         , "HTTP Version Not Supported")      \
  XX(VARIANT_ALSO_NEGOTIATES            , "Variant Also Negotiates")         \
  XX(INSUFFICIENT_STORAGE               , "Insufficient Storage")            \
  XX(LOOP_DETECTED                      , "Loop Detected")                   \
  XX(NOT_EXTENDED                       , "Not Extended")                    \
  XX(NETWORK_AUTHENTICATION_REQUIRED    , "Network Authentication Required") \

typedef enum {
  #define DEF_HTTP_METHOD_ENUM(code, _) HTTP_ ## code = code,
    HTTP_METHOD_MAP(DEF_HTTP_METHOD_ENUM)
  #undef DEF_HTTP_METHOD_ENUM
} sn_http_method_t;

typedef enum {
  #define DEF_HTTP_CODE_ENUM(code, _) HTTP_ ## code = code,
    HTTP_CODE_MAP(DEF_HTTP_CODE_ENUM)
  #undef DEF_HTTP_ENUM
} sn_http_code_t;

const char *sn_http_code_get_description(sn_http_code_t code);

#endif //SNAIL_HTTP_H