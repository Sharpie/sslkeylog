#include <ruby.h>
#include <openssl/ssl.h>

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

  // FIXME: Ensure this is called on an object of type SSLSocket that has an
  // established connection. Otherwise, the required bits of the SSL data
  // structure won't be present or initialized.

  // NOTE: Should be able to use Data_Get_Struct here, but for some reason the
  // SSLSocket instance passed to this function as `self` fails the type check.
  //
  // So, we live dangerously and go directly for the data pointer.
  ssl = (SSL*)DATA_PTR(socket);

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
  VALUE mSSLkeylog = rb_const_get(rb_cObject, rb_intern("SSLkeylog"));

  VALUE mOpenSSL  = rb_define_module_under(mSSLkeylog, "OpenSSL");
  rb_define_singleton_method(mOpenSSL, "to_keylog", to_keylog, 1);
}
