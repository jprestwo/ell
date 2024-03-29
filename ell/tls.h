/*
 *  Embedded Linux library
 *
 *  Copyright (C) 2015  Intel Corporation. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __ELL_TLS_H
#define __ELL_TLS_H

#ifdef __cplusplus
extern "C" {
#endif

enum l_tls_version {
	L_TLS_V10 = ((3 << 8) | 1),
	L_TLS_V11 = ((3 << 8) | 2),
	L_TLS_V12 = ((3 << 8) | 3),
	L_TLS_V13 = ((3 << 8) | 4),	/* Not supported */
};

struct l_tls;
struct l_key;
struct l_certchain;
struct l_queue;

enum l_tls_alert_desc {
	TLS_ALERT_CLOSE_NOTIFY		= 0,
	TLS_ALERT_UNEXPECTED_MESSAGE	= 10,
	TLS_ALERT_BAD_RECORD_MAC	= 20,
	TLS_ALERT_DECRYPT_FAIL_RESERVED	= 21,
	TLS_ALERT_RECORD_OVERFLOW	= 22,
	TLS_ALERT_DECOMPRESS_FAIL	= 30,
	TLS_ALERT_HANDSHAKE_FAIL	= 40,
	TLS_ALERT_NO_CERT_RESERVED	= 41,
	TLS_ALERT_BAD_CERT		= 42,
	TLS_ALERT_UNSUPPORTED_CERT	= 43,
	TLS_ALERT_CERT_REVOKED		= 44,
	TLS_ALERT_CERT_EXPIRED		= 45,
	TLS_ALERT_CERT_UNKNOWN		= 46,
	TLS_ALERT_ILLEGAL_PARAM		= 47,
	TLS_ALERT_UNKNOWN_CA		= 48,
	TLS_ALERT_ACCESS_DENIED		= 49,
	TLS_ALERT_DECODE_ERROR		= 50,
	TLS_ALERT_DECRYPT_ERROR		= 51,
	TLS_ALERT_EXPORT_RES_RESERVED	= 60,
	TLS_ALERT_PROTOCOL_VERSION	= 70,
	TLS_ALERT_INSUFFICIENT_SECURITY	= 71,
	TLS_ALERT_INTERNAL_ERROR	= 80,
	TLS_ALERT_USER_CANCELED		= 90,
	TLS_ALERT_NO_RENEGOTIATION	= 100,
	TLS_ALERT_UNSUPPORTED_EXTENSION	= 110,
};

typedef void (*l_tls_write_cb_t)(const uint8_t *data, size_t len,
					void *user_data);
typedef void (*l_tls_ready_cb_t)(const char *peer_identity, void *user_data);
typedef void (*l_tls_disconnect_cb_t)(enum l_tls_alert_desc reason,
					bool remote, void *user_data);
typedef void (*l_tls_debug_cb_t)(const char *str, void *user_data);
typedef void (*l_tls_destroy_cb_t)(void *user_data);

/*
 * app_data_handler gets called with newly received decrypted data.
 * tx_handler gets called to send TLS payloads off to remote end.
 * ready_handler gets called when l_tls_write calls are first accepted.
 */
struct l_tls *l_tls_new(bool server, l_tls_write_cb_t app_data_handler,
			l_tls_write_cb_t tx_handler,
			l_tls_ready_cb_t ready_handler,
			l_tls_disconnect_cb_t disconnect_handler,
			void *user_data);

void l_tls_free(struct l_tls *tls);

/* Begin sending connection setup messages to the server */
bool l_tls_start(struct l_tls *tls);

/* Properly disconnect a connected session */
void l_tls_close(struct l_tls *tls);

/* Submit plaintext data to be encrypted and transmitted */
void l_tls_write(struct l_tls *tls, const uint8_t *data, size_t len);

/* Submit TLS payload from underlying transport to be decrypted */
void l_tls_handle_rx(struct l_tls *tls, const uint8_t *data, size_t len);

/* If peer is to be authenticated, supply the CA certificates */
bool l_tls_set_cacert(struct l_tls *tls, struct l_queue *ca_certs);

/*
 * If we are to be authenticated, supply our certificate and private key. On the
 * client this is optional.
 * TODO: allow NULL private key if certificate file contains the key.
 * TODO: it may also be useful for the caller to be able to supply one
 * certificate of each type so they can be used depending on which is compatible
 * with the negotiated parameters.
 *
 * Note: Providing certchain and priv_key will move memory ownership into the
 *       tls object. These objects should not be freed by the caller.
 */
bool l_tls_set_auth_data(struct l_tls *tls,
				struct l_certchain *certchain,
				struct l_key *priv_key);

void l_tls_set_version_range(struct l_tls *tls,
				enum l_tls_version min_version,
				enum l_tls_version max_version);

void l_tls_set_domain_mask(struct l_tls *tls, char **mask);

const char *l_tls_alert_to_str(enum l_tls_alert_desc desc);

enum l_checksum_type;

bool l_tls_prf_get_bytes(struct l_tls *tls, bool use_master_secret,
				const char *label, uint8_t *buf, size_t len);

bool l_tls_set_debug(struct l_tls *tls, l_tls_debug_cb_t function,
			void *user_data, l_tls_destroy_cb_t destroy);

#ifdef __cplusplus
}
#endif

#endif /* __ELL_TLS_H */
