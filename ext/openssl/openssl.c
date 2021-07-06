#include <ruby.h>
#include <openssl/ssl.h>

// Global variables pointing to Ruby namespaces of interest.
VALUE mOpenSSL, mSSL, cSSLSocket;

// Copies and hex encodes a char array, using two bytes in buffer for each byte
// in str.
static unsigned int
hex_encode(char *buffer, size_t size, const unsigned char *str)
{
  static const char hex[] = "0123456789ABCDEF";
  unsigned int i;

  for (i = 0; i < size; i++) {
    *(buffer++) = hex[str[i] >> 4];
    *(buffer++) = hex[str[i] & 15];
  }
  return i;
}

/* Add SSL client random to buffer at designated position,
 * and increment buffer position accordingly
 *
 * @param ssl [const SSL*] An SSL connection.
 * @param buf [char*] A string to contain SSL session data.
 * @param buf_pos [unsigned int] The current position within buffer
 * @return [unsigned int] The size of the data written into the buffer
 * @return [0] If the SSL library function failed
 *
 */
static unsigned int
get_client_random(const SSL *ssl, char *buf, unsigned int buf_pos) {
    static const int client_random_len = 32;
    unsigned char client_random[client_random_len];
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    memcpy(client_random, ssl->s3->client_random, client_random_len);
#else
    if (SSL_get_client_random(ssl, client_random, client_random_len) != client_random_len) return 0;
#endif
    return hex_encode(buf + buf_pos, client_random_len, client_random);
}

/* Add SSL session master to buffer at designated position,
 * and increment buffer position accordingly
 *
 * @param ssl [const SSL*] An SSL connection.
 * @param buf [char*] A string to contain SSL session data.
 * @param buf_pos [unsigned int] The current position within buffer
 * @return [unsigned int] The size of the data written into the buffer
 * @return [0] If the SSL library function failed
 *
 */
static unsigned int
get_master_key(const SSL *ssl, char *buf, unsigned int buf_pos) {
    static const int master_key_len = 48;
    unsigned char master_key[master_key_len];

#if OPENSSL_VERSION_NUMBER < 0x10100000L
    memcpy(master_key, ssl->session->master_key, master_key_len);
#else
    const SSL_SESSION *sess = SSL_get_session(ssl);
    if (sess == NULL) return 0;
    if (SSL_SESSION_get_master_key(sess, master_key, master_key_len) != master_key_len) return 0;
#endif

    return hex_encode(buf + buf_pos, master_key_len, master_key);
}

/* Capture SSL session keys in NSS Key Log Format
 *
 * @param socket [OpenSSL::SSL::SSLSocket] A SSL socket instance to capture
 *   data from.
 * @return [String] A string containing SSL session data.
 * @return [nil] If `socket` has negotiated a SSLv2 connection.
 * @return [nil] If `socket` has not been connected.
 *
 * @raise [TypeError] If `socket` is not an instance of
 *   `OpenSSL::SSL::SSLSocket`.
 */
static VALUE
to_keylog(VALUE mod, VALUE socket)
{
  SSL *ssl;
  // 14 byte header string
  // + 64 byte hex encoded client random
  // + 1 space
  // + 96 byte hex encoded master secret
  // + newline.
  size_t buf_len = 14 + 64 + 1 + 96 + 1;
  char buf[buf_len];
  unsigned int i;

  // PRIsVALUE is a special format string that causes functions like rb_raise
  // to format data of type VALUE by calling `.to_s` on the Ruby object.
  if (!rb_obj_is_instance_of(socket, cSSLSocket)) {
    rb_raise(rb_eTypeError, "wrong argument (%"PRIsVALUE")! (Expected instance of %"PRIsVALUE")",
      rb_obj_class(socket), cSSLSocket);
  }

  // NOTE: Should be able to use Data_Get_Struct here, but for some reason the
  // SSLSocket instance passed to this function as `socket` fails the
  // type check.
  //
  // So, we live dangerously and go directly for the data pointer.
  ssl = (SSL*)DATA_PTR(socket);

#if OPENSSL_VERSION_NUMBER < 0x10100000L
  // Check to see if the SSL data structure has been populated.
  //
  // NOTE: If the `s3` component is missing then SSLv2 is in use and we bail
  // out. Such a thing is a bit crazy these days, but we could handle it in a
  // more meaningful way by falling back to the old NSS Key Log format:
  //
  //     RSA Session-ID:<16 byte session id> Master-Key:<48 byte master key>
  if ( !(ssl) || !(ssl->session) || !(ssl->s3) ){
    return Qnil;
  }
#endif

  memcpy(buf, "CLIENT_RANDOM ", 14);
  i = 14;
  if (get_client_random(ssl, buf, i) == 0) return Qnil;
  i += 64;
  buf[i++] = ' ';
  if (get_master_key(ssl, buf, i) == 0) return Qnil;
  i += 96;
  buf[i++] = '\n';

  return rb_str_new(buf, buf_len);
}

/* Document-module: SSLkeylog::OpenSSL
 *
 * Generate NSS Key Log entries from Ruby OpenSSL objects
 *
 * The NSS Key Log format contains two pieces of information that can be used
 * to decrypt SSL session data:
 *
 * 1. A string of 32 random bytes sent by the client during the
 *    "Client Hello" phase of the SSL handshake.
 *
 * 2. A string of 48 bytes generated during the handshake that is the
 *    "pre-master key" required to decrypt the data.
 *
 * The Ruby OpenSSL library exposes the pre-master key through the `to_text`
 * method of  `OpenSSL::SSL::Session` objects. Unfortunately, there is no way
 * to access the client random data at the Ruby level.  However, many Ruby
 * objects are simple wrappers for C data structures and the OpenSSL `SSL`
 * struct does contains all the data we need --- including the client random
 * data. The Ruby object which wraps `struct SSL` is
 * `OpenSSL::SSL::SSLSocket`. This `to_keylog` method provided is a simple C
 * extension which un-wraps `SSLSocket` objects at the C level and reads the
 * data required form a NSS Key Log entry.
 */
void
Init_openssl()
{
  rb_require("openssl");

  // Retrieve OpenSSL namespaces.
  mOpenSSL   = rb_const_get(rb_cObject, rb_intern("OpenSSL"));
  mSSL       = rb_const_get(mOpenSSL,   rb_intern("SSL"));
  cSSLSocket = rb_const_get(mSSL,       rb_intern("SSLSocket"));

  VALUE mSSLkeylog = rb_const_get(rb_cObject, rb_intern("SSLkeylog"));

  VALUE mSSLkeylogOpenSSL  = rb_define_module_under(mSSLkeylog, "OpenSSL");
  rb_define_singleton_method(mSSLkeylogOpenSSL, "to_keylog", to_keylog, 1);

  /* The version string of the OpenSSL headers used to build this library.
   *
   * @return [String] The OPENSSL_VERSION_TEXT definition from the OpenSSL
   *   header this library was built against.
   */
  rb_define_const(mSSLkeylogOpenSSL, "OPENSSL_VERSION", rb_str_new2(OPENSSL_VERSION_TEXT));

  /* The numeric version of the OpenSSL headers used to build this library.
   *
   * @return [Integer] The OPENSSL_VERSION_NUMBER definition from the OpenSSL
   *   header this library was built against.
   */
  rb_define_const(mSSLkeylogOpenSSL, "OPENSSL_VERSION_NUMBER", INT2NUM(OPENSSL_VERSION_NUMBER));
}
