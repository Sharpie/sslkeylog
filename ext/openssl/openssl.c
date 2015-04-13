#include <ruby.h>
#include <openssl/ssl.h>

// Global variables pointing to Ruby namespaces of interest.
VALUE mOpenSSL, mSSL, cSSLSocket;

// Copies and hex encodes a char array, using two bytes in buffer for each byte
// in str.
static void
hex_encode(char *buffer, size_t size, const unsigned char *str)
{
  static const char hex[] = "0123456789ABCDEF";
  unsigned int i;

  for (i = 0; i < size; i++) {
    *(buffer++) = hex[str[i] >> 4];
    *(buffer++) = hex[str[i] & 15];
  }
}

/* Capture SSL session keys in NSS Key Log Format
 *
 * @param socket [OpenSSL::SSL::SSLSocket] A SSL socket instance to capture
 *   data from.
 * @return [String] A string containing SSL session data.
 *
 * @raise [TypeError] If `socket` is not an instance of
 *   `OpenSSL::SSL::SSLSocket`.
 * @raise [RuntimeError] If `socket` has not been connected.
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

  // Check to see if the SSL data structure has been populated.
  if ( !(ssl) || !(ssl->session) ){
    rb_raise(rb_eRuntimeError, "%s", "Can't log SSL state. No connection established!");
  }

  memcpy(buf, "CLIENT_RANDOM ", 14);
  i = 14;
  // NOTE: It is possible that the `s3` component is missing if SSLv2 is in
  // use. Such a thing is a bit crazy these days, but if it happens, we should
  // fall back to using the old format:
  //
  //     RSA <16 byte session id> <48 byte master key>
  hex_encode(buf + i, 32, ssl->s3->client_random);
  i += 64;
  buf[i++] = ' ';
  hex_encode(buf + i, 48, ssl->session->master_key);
  i += 96;
  buf[i++] = '\n';

  return rb_str_new(buf, buf_len);
}

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
