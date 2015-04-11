#include <ruby.h>

static VALUE
to_keylog(VALUE self)
{
  return rb_str_new2("CLIENT_RANDOM RANDOM HOSTKEY");
}

void
Init_session_extensions()
{
  VALUE mOpenSSL = rb_const_get(rb_cObject, rb_intern("OpenSSL"));
  VALUE mKeylog  = rb_const_get(mOpenSSL, rb_intern("Keylog"));

  VALUE mSessionExtensions = rb_define_module_under(mKeylog, "SessionExtensions");

  rb_define_method(mSessionExtensions, "to_keylog", to_keylog, 0);
}
