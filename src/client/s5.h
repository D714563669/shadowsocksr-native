/* Copyright ssrlive, Inc. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef __S5_H__
#define __S5_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define S5_RESULT_MAP(V)                                                       \
  V(-1, s5_result_bad_version, "Bad protocol version.")                        \
  V(-2, s5_result_bad_cmd,     "Bad protocol command.")                        \
  V(-3, s5_result_bad_atyp,    "Bad address type.")                            \
  V( 0, s5_result_need_more,   "Need more data.")                              \
  V( 1, s5_result_auth_select, "Select authentication method.")                \
  V( 2, s5_result_auth_verify, "Verify authentication.")                       \
  V( 3, s5_result_exec_cmd,    "Execute command.")                             \

enum s5_result {
#define S5_RESULT_GEN(code, name, _) name = code,
    S5_RESULT_MAP(S5_RESULT_GEN)
#undef S5_RESULT_GEN
    s5_result_max,
};

typedef enum s5_auth_method {
    s5_auth_none = 1 << 0,
    s5_auth_gssapi = 1 << 1,
    s5_auth_passwd = 1 << 2
} s5_auth_method;

typedef enum s5_atyp {
    s5_atyp_ipv4 = 1,
    s5_atyp_host = 3,
    s5_atyp_ipv6 = 4,
} s5_atyp;

typedef enum s5_cmd {
    s5_cmd_tcp_connect = 1,
    s5_cmd_tcp_bind = 2,
    s5_cmd_udp_assoc = 3,
} s5_cmd;

enum s5_stage {
    s5_stage_version,
    s5_stage_nmethods,
    s5_stage_methods,
    s5_stage_auth_pw_version,
    s5_stage_auth_pw_userlen,
    s5_stage_auth_pw_username,
    s5_stage_auth_pw_passlen,
    s5_stage_auth_pw_password,
    s5_stage_req_version,
    s5_stage_req_cmd,
    s5_stage_req_reserved,
    s5_stage_req_atyp,
    s5_stage_req_atyp_host,
    s5_stage_req_daddr,
    s5_stage_req_dport0,
    s5_stage_req_dport1,
    s5_stage_dead,
};

typedef struct s5_ctx {
    uint32_t arg0;  /* Scratch space for the state machine. */
    uint32_t arg1;  /* Scratch space for the state machine. */
    enum s5_stage stage;
    enum s5_auth_method methods;
    enum s5_cmd cmd;
    enum s5_atyp atyp;
    uint8_t userlen;
    uint8_t passlen;
    uint16_t dport;
    uint8_t username[257];
    uint8_t password[257];
    uint8_t daddr[257];  /* TODO(bnoordhuis) Merge with username/password. */
} s5_ctx;

void s5_init(s5_ctx *ctx);

enum s5_result s5_parse(s5_ctx *cx, uint8_t **data, size_t *size);

/* Only call after s5_parse() has returned s5_want_auth_method. */
enum s5_auth_method s5_auth_methods(const s5_ctx *cx);

/* Call after s5_parse() has returned s5_want_auth_method. */
int s5_select_auth(s5_ctx *cx, s5_auth_method method);

const char * str_s5_result(enum s5_result result);

uint8_t * build_udp_assoc_package(bool allow, const char *addr_str, int port, uint8_t *buf, size_t *buf_len);

#endif  /* __S5_H__ */
