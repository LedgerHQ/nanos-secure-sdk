#define SYSCALL_STUB

#if defined(HAVE_BOLOS)
# include "bolos_privileged_ux.h"
#endif // HAVE_BOLOS

#include "bolos_target.h"
#include "exceptions.h"
#include "lcx_aes.h"
#include "lcx_des.h"
#include "lcx_eddsa.h"
#include "lcx_wrappers.h"
#include "cx_errors.h"
#include "os_task.h"
#include "os_memory.h"
#include "os_registry.h"
#include "os_ux.h"
#ifdef HAVE_SE_TOUCH
#include "os_io.h"
#endif // HAVE_SE_TOUCH
#include "ox_ec.h"
#include "ox_bn.h"
#include "syscalls.h"
#if defined(HAVE_LANGUAGE_PACK)
#include "ux.h"
#endif //defined(HAVE_LANGUAGE_PACK)
#ifdef HAVE_NBGL
#include "nbgl_types.h"
#include "os_pic.h"
#endif
#include <string.h>

unsigned int SVC_Call(unsigned int syscall_id, void *parameters);
unsigned int SVC_cx_call(unsigned int syscall_id, unsigned int * parameters);

unsigned int get_api_level(void) {
  unsigned int parameters[2];
  parameters[0] = 0;
  parameters[1] = 0;
  return SVC_Call(SYSCALL_get_api_level_ID, parameters);
}

void halt ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_Call(SYSCALL_halt_ID, parameters);
  return;
}

#ifdef HAVE_NBGL
void nbgl_frontDrawRect(nbgl_area_t *area)
{
  unsigned int parameters[1];
  parameters[0] = (unsigned int)area;
  SVC_Call(SYSCALL_nbgl_front_draw_rect_ID, parameters);
  return;
}

void nbgl_frontDrawHorizontalLine(nbgl_area_t *area, uint8_t mask, color_t lineColor)
{
  unsigned int parameters[3];
  parameters[0] = (unsigned int)area;
  parameters[1] = (unsigned int)mask;
  parameters[2] = (unsigned int)lineColor;
  SVC_Call(SYSCALL_nbgl_front_draw_horizontal_line_ID, parameters);
  return;
}

void nbgl_frontDrawImage(nbgl_area_t *area, uint8_t *buffer, nbgl_transformation_t transformation, nbgl_color_map_t colorMap)
{
  unsigned int parameters[4];
  parameters[0] = (unsigned int)area;
  parameters[1] = (unsigned int)PIC(buffer);
  parameters[2] = (unsigned int)transformation;
  parameters[3] = (unsigned int)colorMap;
  SVC_Call(SYSCALL_nbgl_front_draw_img_ID, parameters);
  return;
}

void nbgl_frontDrawImageFile(nbgl_area_t *area, uint8_t *buffer,
                             nbgl_color_map_t colorMap, uint8_t *optional_uzlib_work_buffer)
{
  unsigned int parameters[4];
  parameters[0] = (unsigned int)area;
  parameters[1] = (unsigned int)PIC(buffer);
  parameters[2] = (unsigned int)colorMap;
  parameters[3] = (unsigned int)optional_uzlib_work_buffer;
  SVC_Call(SYSCALL_nbgl_front_draw_img_file_ID, parameters);
  return;
}

void nbgl_frontRefreshArea(nbgl_area_t *area, nbgl_refresh_mode_t mode)
{
  unsigned int parameters[2];
  parameters[0] = (unsigned int)area;
  parameters[1] = (unsigned int)mode;
  SVC_Call(SYSCALL_nbgl_front_refresh_area_ID, parameters);
  return;
}

void nbgl_sideDrawRect(nbgl_area_t *area)
{
  unsigned int parameters[1];
  parameters[0] = (unsigned int)area;
  SVC_Call(SYSCALL_nbgl_side_draw_rect_ID, parameters);
  return;
}

void nbgl_sideDrawHorizontalLine(nbgl_area_t *area, uint8_t mask, color_t lineColor)
{
  unsigned int parameters[3];
  parameters[0] = (unsigned int)area;
  parameters[1] = (unsigned int)mask;
  parameters[2] = (unsigned int)lineColor;
  SVC_Call(SYSCALL_nbgl_side_draw_horizontal_line_ID, parameters);
  return;
}

void nbgl_sideDrawImage(nbgl_area_t *area, uint8_t *buffer, nbgl_transformation_t transformation, nbgl_color_map_t colorMap)
{
  unsigned int parameters[4];
  parameters[0] = (unsigned int)area;
  parameters[1] = (unsigned int)buffer;
  parameters[2] = (unsigned int)transformation;
  parameters[3] = (unsigned int)colorMap;
  SVC_Call(SYSCALL_nbgl_side_draw_img_ID, parameters);
  return;
}

void nbgl_sideRefreshArea(nbgl_area_t *area)
{
  unsigned int parameters[1];
  parameters[0] = (unsigned int)area;
  SVC_Call(SYSCALL_nbgl_side_refresh_area_ID, parameters);
  return;
}

unsigned int nbgl_font_getFont(unsigned int fontId)
{
  unsigned int parameters[1];
  parameters[0] = (unsigned int)fontId;
  return SVC_Call(SYSCALL_nbgl_get_font_ID, parameters);
}
#endif

void nvm_write ( void * dst_adr, void * src_adr, unsigned int src_len ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)dst_adr;
  parameters[1] = (unsigned int)src_adr;
  parameters[2] = (unsigned int)src_len;
  SVC_Call(SYSCALL_nvm_write_ID, parameters);
  return;
}

void nvm_erase ( void * dst_adr, unsigned int len ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)dst_adr;
  parameters[1] = (unsigned int)len;
  SVC_Call(SYSCALL_nvm_erase_ID, parameters);
  return;
}

cx_err_t cx_aes_set_key_hw ( const cx_aes_key_t * keys, uint32_t mode ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)keys;
  parameters[1] = (unsigned int)mode;
  return SVC_cx_call(SYSCALL_cx_aes_set_key_hw_ID, parameters);
}

void cx_aes_reset_hw ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_cx_call(SYSCALL_cx_aes_reset_hw_ID, parameters);
}

cx_err_t cx_aes_block_hw ( const unsigned char * inblock, unsigned char * outblock ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)inblock;
  parameters[1] = (unsigned int)outblock;
  return SVC_cx_call(SYSCALL_cx_aes_block_hw_ID, parameters);
}

cx_err_t cx_des_set_key_hw ( const cx_des_key_t * keys, uint32_t mode ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)keys;
  parameters[1] = (unsigned int)mode;
  return SVC_cx_call(SYSCALL_cx_des_set_key_hw_ID, parameters);
}

void cx_des_reset_hw ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_cx_call(SYSCALL_cx_des_reset_hw_ID, parameters);
}

void cx_des_block_hw ( const unsigned char * inblock, unsigned char * outblock ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)inblock;
  parameters[1] = (unsigned int)outblock;
  SVC_cx_call(SYSCALL_cx_des_block_hw_ID, parameters);
}

cx_err_t cx_bn_lock ( size_t word_nbytes, uint32_t flags ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)word_nbytes;
  parameters[1] = (unsigned int)flags;
  return SVC_cx_call(SYSCALL_cx_bn_lock_ID, parameters);
}

uint32_t cx_bn_unlock ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (uint32_t) SVC_cx_call(SYSCALL_cx_bn_unlock_ID, parameters);
}

_Bool cx_bn_is_locked ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (_Bool) SVC_cx_call(SYSCALL_cx_bn_is_locked_ID, parameters);
}

cx_err_t cx_bn_alloc ( cx_bn_t * x, size_t nbytes ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)nbytes;
  return SVC_cx_call(SYSCALL_cx_bn_alloc_ID, parameters);
}

cx_err_t cx_bn_alloc_init ( cx_bn_t * x, size_t nbytes, const uint8_t * value, size_t value_nbytes ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)nbytes;
  parameters[2] = (unsigned int)value;
  parameters[3] = (unsigned int)value_nbytes;
  return SVC_cx_call(SYSCALL_cx_bn_alloc_init_ID, parameters);
}

cx_err_t cx_bn_destroy ( cx_bn_t * x ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)x;
  parameters[1] = 0;
  return SVC_cx_call(SYSCALL_cx_bn_destroy_ID, parameters);
}

cx_err_t cx_bn_nbytes ( const cx_bn_t x, size_t *nbytes ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)nbytes;
  return SVC_cx_call(SYSCALL_cx_bn_nbytes_ID, parameters);
}

cx_err_t cx_bn_init ( cx_bn_t x, const uint8_t * value, size_t value_nbytes ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)value;
  parameters[2] = (unsigned int)value_nbytes;
  return SVC_cx_call(SYSCALL_cx_bn_init_ID, parameters);
}

cx_err_t cx_bn_rand ( cx_bn_t x ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)x;
  parameters[1] = 0;
  return SVC_cx_call(SYSCALL_cx_bn_rand_ID, parameters);
}

cx_err_t cx_bn_copy ( cx_bn_t a, const cx_bn_t b ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)a;
  parameters[1] = (unsigned int)b;
  return SVC_cx_call(SYSCALL_cx_bn_copy_ID, parameters);
}

cx_err_t cx_bn_set_u32 ( cx_bn_t x, uint32_t n ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_bn_set_u32_ID, parameters);
}

cx_err_t cx_bn_get_u32(const cx_bn_t x, uint32_t *n) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)n;
  parameters[2] = 0;
  return SVC_cx_call(SYSCALL_cx_bn_get_u32_ID, parameters);
}

cx_err_t cx_bn_export ( const cx_bn_t x, uint8_t * bytes, size_t nbytes ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)bytes;
  parameters[2] = (unsigned int)nbytes;
  return SVC_cx_call(SYSCALL_cx_bn_export_ID, parameters);
}

cx_err_t cx_bn_cmp ( const cx_bn_t a, const cx_bn_t b, int *diff ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)a;
  parameters[1] = (unsigned int)b;
  parameters[2] = (unsigned int)diff;
  return SVC_cx_call(SYSCALL_cx_bn_cmp_ID, parameters);
}

cx_err_t cx_bn_cmp_u32 ( const cx_bn_t a, uint32_t b, int *diff ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)a;
  parameters[1] = (unsigned int)b;
  parameters[2] = (unsigned int)diff;
  return SVC_cx_call(SYSCALL_cx_bn_cmp_u32_ID, parameters);
}

cx_err_t cx_bn_is_odd ( const cx_bn_t n, bool *odd ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)n;
  parameters[1] = (unsigned int)odd;
  return SVC_cx_call(SYSCALL_cx_bn_is_odd_ID, parameters);
}

cx_err_t cx_bn_xor ( cx_bn_t r, const cx_bn_t a, const cx_bn_t b ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  return SVC_cx_call(SYSCALL_cx_bn_xor_ID, parameters);
}

cx_err_t cx_bn_or ( cx_bn_t r, const cx_bn_t a, const cx_bn_t b ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  return SVC_cx_call(SYSCALL_cx_bn_or_ID, parameters);
}

cx_err_t cx_bn_and ( cx_bn_t r, const cx_bn_t a, const cx_bn_t b ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  return SVC_cx_call(SYSCALL_cx_bn_and_ID, parameters);
}

cx_err_t cx_bn_tst_bit ( const cx_bn_t x, uint32_t pos, bool *set ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)pos;
  parameters[2] = (unsigned int)set;
  return SVC_cx_call(SYSCALL_cx_bn_tst_bit_ID, parameters);
}

cx_err_t cx_bn_set_bit ( cx_bn_t x, uint32_t pos ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)pos;
  return SVC_cx_call(SYSCALL_cx_bn_set_bit_ID, parameters);
}

cx_err_t cx_bn_clr_bit ( cx_bn_t x, uint32_t pos ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)pos;
  return SVC_cx_call(SYSCALL_cx_bn_clr_bit_ID, parameters);
}

cx_err_t cx_bn_shr ( cx_bn_t x, uint32_t n ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_bn_shr_ID, parameters);
}

cx_err_t cx_bn_shl ( cx_bn_t x, uint32_t n ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_bn_shl_ID, parameters);
}

cx_err_t cx_bn_cnt_bits ( cx_bn_t n, uint32_t *nbits ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)n;
  parameters[1] = (unsigned int)nbits;
  return SVC_cx_call(SYSCALL_cx_bn_cnt_bits_ID, parameters);
}

cx_err_t cx_bn_add ( cx_bn_t r, const cx_bn_t a, const cx_bn_t b ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  return SVC_cx_call(SYSCALL_cx_bn_add_ID, parameters);
}

cx_err_t cx_bn_sub ( cx_bn_t r, const cx_bn_t a, const cx_bn_t b ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  return SVC_cx_call(SYSCALL_cx_bn_sub_ID, parameters);
}

cx_err_t cx_bn_mul ( cx_bn_t r, const cx_bn_t a, const cx_bn_t b ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  return SVC_cx_call(SYSCALL_cx_bn_mul_ID, parameters);
}

cx_err_t cx_bn_mod_add ( cx_bn_t r, const cx_bn_t a, const cx_bn_t b, const cx_bn_t n ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_bn_mod_add_ID, parameters);
}

cx_err_t cx_bn_mod_sub ( cx_bn_t r, const cx_bn_t a, const cx_bn_t b, const cx_bn_t n ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_bn_mod_sub_ID, parameters);
}

cx_err_t cx_bn_mod_mul ( cx_bn_t r, const cx_bn_t a, const cx_bn_t b, const cx_bn_t n ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_bn_mod_mul_ID, parameters);
}

cx_err_t cx_bn_reduce ( cx_bn_t r, const cx_bn_t d, const cx_bn_t n ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)d;
  parameters[2] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_bn_reduce_ID, parameters);
}

cx_err_t cx_bn_mod_sqrt ( cx_bn_t bn_r, const cx_bn_t bn_a, const cx_bn_t bn_n, uint32_t sign ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)bn_r;
  parameters[1] = (unsigned int)bn_a;
  parameters[2] = (unsigned int)bn_n;
  parameters[3] = (unsigned int)sign;
  return SVC_cx_call(SYSCALL_cx_bn_mod_sqrt_ID, parameters);
}

cx_err_t cx_bn_mod_pow_bn ( cx_bn_t r, const cx_bn_t a, const cx_bn_t e, const cx_bn_t n ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)e;
  parameters[3] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_bn_mod_pow_bn_ID, parameters);
}

cx_err_t cx_bn_mod_pow ( cx_bn_t r, const cx_bn_t a, const uint8_t * e, uint32_t e_len, const cx_bn_t n ) {
  unsigned int parameters[5];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)e;
  parameters[3] = (unsigned int)e_len;
  parameters[4] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_bn_mod_pow_ID, parameters);
}

cx_err_t cx_bn_mod_pow2 ( cx_bn_t r, const cx_bn_t a, const uint8_t * e, uint32_t e_len, const cx_bn_t n ) {
  unsigned int parameters[5];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)e;
  parameters[3] = (unsigned int)e_len;
  parameters[4] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_bn_mod_pow2_ID, parameters);
}

cx_err_t cx_bn_mod_invert_nprime ( cx_bn_t r, const cx_bn_t a, const cx_bn_t n ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_bn_mod_invert_nprime_ID, parameters);
}

cx_err_t cx_bn_mod_u32_invert ( cx_bn_t r, uint32_t a, cx_bn_t n ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_bn_mod_u32_invert_ID, parameters);
}

cx_err_t cx_bn_is_prime ( const cx_bn_t n, bool *prime ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)n;
  parameters[1] = (unsigned int)prime;
  return SVC_cx_call(SYSCALL_cx_bn_is_prime_ID, parameters);
}

cx_err_t cx_bn_next_prime ( cx_bn_t n ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)n;
  parameters[1] = 0;
  return SVC_cx_call(SYSCALL_cx_bn_next_prime_ID, parameters);
}

cx_err_t cx_bn_rng ( cx_bn_t r, const cx_bn_t n ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_bn_rng_ID, parameters);
}

cx_err_t cx_bn_gf2_n_mul(cx_bn_t r, const cx_bn_t a, const cx_bn_t b, const cx_bn_t n, const cx_bn_t h) {
  unsigned int parameters[5];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)n;
  parameters[4] = (unsigned int)h;
  return SVC_cx_call(SYSCALL_cx_bn_gf2_n_mul_ID, parameters);
}

cx_err_t cx_mont_alloc ( cx_bn_mont_ctx_t * ctx, size_t length ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)ctx;
  parameters[1] = (unsigned int)length;
  return SVC_cx_call(SYSCALL_cx_mont_alloc_ID, parameters);
}

cx_err_t cx_mont_init ( cx_bn_mont_ctx_t * ctx, const cx_bn_t n ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)ctx;
  parameters[1] = (unsigned int)n;
  return SVC_cx_call(SYSCALL_cx_mont_init_ID, parameters);
}

cx_err_t cx_mont_init2 ( cx_bn_mont_ctx_t * ctx, const cx_bn_t n, const cx_bn_t h ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)ctx;
  parameters[1] = (unsigned int)n;
  parameters[2] = (unsigned int)h;
  return SVC_cx_call(SYSCALL_cx_mont_init2_ID, parameters);
}

cx_err_t cx_mont_to_montgomery ( cx_bn_t x, const cx_bn_t z, const cx_bn_mont_ctx_t * ctx ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)z;
  parameters[2] = (unsigned int)ctx;
  return SVC_cx_call(SYSCALL_cx_mont_to_montgomery_ID, parameters);
}

cx_err_t cx_mont_from_montgomery ( cx_bn_t z, const cx_bn_t x, const cx_bn_mont_ctx_t * ctx ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)z;
  parameters[1] = (unsigned int)x;
  parameters[2] = (unsigned int)ctx;
  return SVC_cx_call(SYSCALL_cx_mont_from_montgomery_ID, parameters);
}

cx_err_t cx_mont_mul ( cx_bn_t r, const cx_bn_t a, const cx_bn_t b, const cx_bn_mont_ctx_t * ctx ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)b;
  parameters[3] = (unsigned int)ctx;
  return SVC_cx_call(SYSCALL_cx_mont_mul_ID, parameters);
}

cx_err_t cx_mont_pow ( cx_bn_t r, const cx_bn_t a, const uint8_t * e, uint32_t e_len, const cx_bn_mont_ctx_t * ctx ) {
  unsigned int parameters[5];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)e;
  parameters[3] = (unsigned int)e_len;
  parameters[4] = (unsigned int)ctx;
  return SVC_cx_call(SYSCALL_cx_mont_pow_ID, parameters);
}

cx_err_t cx_mont_pow_bn ( cx_bn_t r, const cx_bn_t a, const cx_bn_t e, const cx_bn_mont_ctx_t * ctx ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)e;
  parameters[3] = (unsigned int)ctx;
  return SVC_cx_call(SYSCALL_cx_mont_pow_bn_ID, parameters);
}

cx_err_t cx_mont_invert_nprime ( cx_bn_t r, const cx_bn_t a, const cx_bn_mont_ctx_t * ctx ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)r;
  parameters[1] = (unsigned int)a;
  parameters[2] = (unsigned int)ctx;
  return SVC_cx_call(SYSCALL_cx_mont_invert_nprime_ID, parameters);
}

cx_err_t cx_ecdomain_size ( cx_curve_t cv, size_t *length ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)cv;
  parameters[1] = (unsigned int)length;
  return SVC_cx_call(SYSCALL_cx_ecdomain_size_ID, parameters);
}

cx_err_t cx_ecdomain_parameters_length ( cx_curve_t cv, size_t *length ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)cv;
  parameters[1] = (unsigned int)length;
  return SVC_cx_call(SYSCALL_cx_ecdomain_parameters_length_ID, parameters);
}

cx_err_t cx_ecdomain_parameter ( cx_curve_t cv, cx_curve_dom_param_t id, uint8_t * p, uint32_t p_len ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)cv;
  parameters[1] = (unsigned int)id;
  parameters[2] = (unsigned int)p;
  parameters[3] = (unsigned int)p_len;
  return SVC_cx_call(SYSCALL_cx_ecdomain_parameter_ID, parameters);
}

cx_err_t cx_ecdomain_parameter_bn ( cx_curve_t cv, cx_curve_dom_param_t id, cx_bn_t p ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)cv;
  parameters[1] = (unsigned int)id;
  parameters[2] = (unsigned int)p;
  return SVC_cx_call(SYSCALL_cx_ecdomain_parameter_bn_ID, parameters);
}

cx_err_t cx_ecdomain_generator ( cx_curve_t cv, uint8_t * Gx, uint8_t * Gy, size_t len ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)cv;
  parameters[1] = (unsigned int)Gx;
  parameters[2] = (unsigned int)Gy;
  parameters[3] = (unsigned int)len;
  return SVC_cx_call(SYSCALL_cx_ecdomain_generator_ID, parameters);
}

cx_err_t cx_ecdomain_generator_bn ( cx_curve_t cv, cx_ecpoint_t * P ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)cv;
  parameters[1] = (unsigned int)P;
  return SVC_cx_call(SYSCALL_cx_ecdomain_generator_bn_ID, parameters);
}

cx_err_t cx_ecpoint_alloc ( cx_ecpoint_t * P, cx_curve_t cv ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)P;
  parameters[1] = (unsigned int)cv;
  return SVC_cx_call(SYSCALL_cx_ecpoint_alloc_ID, parameters);
}

cx_err_t cx_ecpoint_destroy ( cx_ecpoint_t * P ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)P;
  parameters[1] = 0;
  return SVC_cx_call(SYSCALL_cx_ecpoint_destroy_ID, parameters);
}

cx_err_t cx_ecpoint_init ( cx_ecpoint_t * P, const uint8_t * x, size_t x_len, const uint8_t * y, size_t y_len ) {
  unsigned int parameters[5];
  parameters[0] = (unsigned int)P;
  parameters[1] = (unsigned int)x;
  parameters[2] = (unsigned int)x_len;
  parameters[3] = (unsigned int)y;
  parameters[4] = (unsigned int)y_len;
  return SVC_cx_call(SYSCALL_cx_ecpoint_init_ID, parameters);
}

cx_err_t cx_ecpoint_init_bn ( cx_ecpoint_t * P, const cx_bn_t x, const cx_bn_t y ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)P;
  parameters[1] = (unsigned int)x;
  parameters[2] = (unsigned int)y;
  return SVC_cx_call(SYSCALL_cx_ecpoint_init_bn_ID, parameters);
}

cx_err_t cx_ecpoint_export ( const cx_ecpoint_t * P, uint8_t * x, size_t x_len, uint8_t * y, size_t y_len ) {
  unsigned int parameters[5];
  parameters[0] = (unsigned int)P;
  parameters[1] = (unsigned int)x;
  parameters[2] = (unsigned int)x_len;
  parameters[3] = (unsigned int)y;
  parameters[4] = (unsigned int)y_len;
  return SVC_cx_call(SYSCALL_cx_ecpoint_export_ID, parameters);
}

cx_err_t cx_ecpoint_export_bn ( const cx_ecpoint_t * P, cx_bn_t * x, cx_bn_t * y ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)P;
  parameters[1] = (unsigned int)x;
  parameters[2] = (unsigned int)y;
  return SVC_cx_call(SYSCALL_cx_ecpoint_export_bn_ID, parameters);
}

cx_err_t cx_ecpoint_compress ( const cx_ecpoint_t * P, uint8_t * xy_compressed, size_t xy_compressed_len, uint32_t * sign ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)P;
  parameters[1] = (unsigned int)xy_compressed;
  parameters[2] = (unsigned int)xy_compressed_len;
  parameters[3] = (unsigned int)sign;
  return SVC_cx_call(SYSCALL_cx_ecpoint_compress_ID, parameters);
}

cx_err_t cx_ecpoint_decompress ( cx_ecpoint_t * P, const uint8_t * xy_compressed, size_t xy_compressed_len, uint32_t sign ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)P;
  parameters[1] = (unsigned int)xy_compressed;
  parameters[2] = (unsigned int)xy_compressed_len;
  parameters[3] = (unsigned int)sign;
  return SVC_cx_call(SYSCALL_cx_ecpoint_decompress_ID, parameters);
}

cx_err_t cx_ecpoint_add ( cx_ecpoint_t * R, const cx_ecpoint_t * P, const cx_ecpoint_t * Q ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)R;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)Q;
  return SVC_cx_call(SYSCALL_cx_ecpoint_add_ID, parameters);
}

cx_err_t cx_ecpoint_neg ( cx_ecpoint_t * P ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)P;
  parameters[1] = 0;
  return SVC_cx_call(SYSCALL_cx_ecpoint_neg_ID, parameters);
}

cx_err_t cx_ecpoint_scalarmul ( cx_ecpoint_t * P, const uint8_t * k, size_t k_len ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)P;
  parameters[1] = (unsigned int)k;
  parameters[2] = (unsigned int)k_len;
  return SVC_cx_call(SYSCALL_cx_ecpoint_scalarmul_ID, parameters);
}

cx_err_t cx_ecpoint_scalarmul_bn ( cx_ecpoint_t * P, const cx_bn_t bn_k ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)P;
  parameters[1] = (unsigned int)bn_k;
  return SVC_cx_call(SYSCALL_cx_ecpoint_scalarmul_bn_ID, parameters);
}

cx_err_t cx_ecpoint_rnd_scalarmul ( cx_ecpoint_t * P, const uint8_t * k, size_t k_len ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)P;
  parameters[1] = (unsigned int)k;
  parameters[2] = (unsigned int)k_len;
  return SVC_cx_call(SYSCALL_cx_ecpoint_rnd_scalarmul_ID, parameters);
}

cx_err_t cx_ecpoint_rnd_scalarmul_bn ( cx_ecpoint_t * P, const cx_bn_t bn_k ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)P;
  parameters[1] = (unsigned int)bn_k;
  return SVC_cx_call(SYSCALL_cx_ecpoint_rnd_scalarmul_bn_ID, parameters);
}

#ifdef HAVE_FIXED_SCALAR_LENGTH
cx_err_t cx_ecpoint_rnd_fixed_scalarmul ( cx_ecpoint_t * P, const uint8_t * k, size_t k_len ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)P;
  parameters[1] = (unsigned int)k;
  parameters[2] = (unsigned int)k_len;
  return SVC_cx_call(SYSCALL_cx_ecpoint_rnd_fixed_scalarmul_ID, parameters);
}
#endif // HAVE_FIXED_SCALAR_LENGTH

cx_err_t cx_ecpoint_double_scalarmul ( cx_ecpoint_t * R, cx_ecpoint_t *P, cx_ecpoint_t *Q, const uint8_t * k, size_t k_len, const uint8_t * r, size_t r_len ) {
  unsigned int parameters[7];
  parameters[0] = (unsigned int)R;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)Q;
  parameters[3] = (unsigned int)k;
  parameters[4] = (unsigned int)k_len;
  parameters[5] = (unsigned int)r;
  parameters[6] = (unsigned int)r_len;
  return SVC_cx_call(SYSCALL_cx_ecpoint_double_scalarmul_ID, parameters);
}

cx_err_t cx_ecpoint_double_scalarmul_bn ( cx_ecpoint_t * R, cx_ecpoint_t *P, cx_ecpoint_t *Q, const cx_bn_t bn_k, const cx_bn_t bn_r ) {
  unsigned int parameters[5];
  parameters[0] = (unsigned int)R;
  parameters[1] = (unsigned int)P;
  parameters[2] = (unsigned int)Q;
  parameters[3] = (unsigned int)bn_k;
  parameters[4] = (unsigned int)bn_r;
  return SVC_cx_call(SYSCALL_cx_ecpoint_double_scalarmul_bn_ID, parameters);
}

cx_err_t cx_ecpoint_cmp(const cx_ecpoint_t * P, const cx_ecpoint_t * Q, bool *is_equal) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)P;
  parameters[1] = (unsigned int)Q;
  parameters[2] = (unsigned int)is_equal;
  return SVC_cx_call(SYSCALL_cx_ecpoint_cmp_ID, parameters);
}

cx_err_t cx_ecpoint_is_on_curve ( const cx_ecpoint_t * R, bool *is_on_curve ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)R;
  parameters[1] = (unsigned int)is_on_curve;
  return SVC_cx_call(SYSCALL_cx_ecpoint_is_on_curve_ID, parameters);
}

cx_err_t cx_ecpoint_is_at_infinity( const cx_ecpoint_t *R, bool *is_infinite ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)R;
  parameters[1] = (unsigned int)is_infinite;
  return SVC_cx_call(SYSCALL_cx_ecpoint_is_at_infinity_ID, parameters);
}

#ifdef HAVE_X25519
cx_err_t cx_ecpoint_x25519(const cx_bn_t u, const uint8_t *k, size_t k_len) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)u;
  parameters[1] = (unsigned int)k;
  parameters[2] = (unsigned int)k_len;
  return SVC_cx_call(SYSCALL_cx_ecpoint_x25519_ID, parameters);
}
#endif // HAVE_X25519

#ifdef HAVE_X448
cx_err_t cx_ecpoint_x448(const cx_bn_t u, const uint8_t *k, size_t k_len) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)u;
  parameters[1] = (unsigned int)k;
  parameters[2] = (unsigned int)k_len;
  return SVC_cx_call(SYSCALL_cx_ecpoint_x448_ID, parameters);
}
#endif // HAVE_X448

#ifdef HAVE_BLS
cx_err_t ox_bls12381_sign(const cx_ecfp_384_private_key_t *key, const uint8_t * message, size_t message_len, uint8_t * signature, size_t signature_len) {
  unsigned int parameters [5];
  parameters[0] = (unsigned int)key;
  parameters[1] = (unsigned int)message;
  parameters[2] = (unsigned int)message_len;
  parameters[3] = (unsigned int)signature;
  parameters[4] = (unsigned int)signature_len;
  return SVC_cx_call(SYSCALL_ox_bls12381_sign_ID, parameters);
}

cx_err_t cx_bls12381_key_gen(uint8_t mode,
                             const uint8_t *secret, size_t secret_len,
                             const uint8_t *salt, size_t salt_len,
                             uint8_t *key_info, size_t key_info_len,
                             cx_ecfp_384_private_key_t *private_key,
                             uint8_t *public_key,
                             size_t public_key_len) {
  unsigned int parameters[10];
  parameters[0] = (unsigned int)mode;
  parameters[1] = (unsigned int)secret;
  parameters[2] = (unsigned int)secret_len;
  parameters[3] = (unsigned int)salt;
  parameters[4] = (unsigned int)salt_len;
  parameters[5] = (unsigned int)key_info;
  parameters[6] = (unsigned int)key_info_len;
  parameters[7] = (unsigned int)private_key;
  parameters[8] = (unsigned int)public_key;
  parameters[9] = (unsigned int)public_key_len;
  return SVC_cx_call(SYSCALL_cx_bls12381_key_gen_ID, parameters);
}

cx_err_t cx_hash_to_field(const uint8_t *msg, size_t msg_len, const uint8_t *dst, size_t dst_len, uint8_t *hash, size_t hash_len) {
  unsigned int parameters [6];
  parameters[0] = (unsigned int)msg;
  parameters[1] = (unsigned int)msg_len;
  parameters[2] = (unsigned int)dst;
  parameters[3] = (unsigned int)dst_len;
  parameters[4] = (unsigned int)hash;
  parameters[5] = (unsigned int)hash_len;
  return SVC_cx_call(SYSCALL_cx_hash_to_field_ID, parameters);
}

cx_err_t cx_bls12381_aggregate(const uint8_t *in, size_t in_len, bool first, uint8_t *aggregated_signature, size_t signature_len) {
  unsigned int parameters [5];
  parameters[0] = (unsigned int)in;
  parameters[1] = (unsigned int)in_len;
  parameters[2] = (unsigned int)first;
  parameters[3] = (unsigned int)aggregated_signature;
  parameters[4] = (unsigned int)signature_len;
  return SVC_cx_call(SYSCALL_cx_bls12381_aggregate_ID, parameters);
}
#endif // HAVE_BLS

uint32_t cx_crc32_hw ( const void * buf, size_t len ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)buf;
  parameters[1] = (unsigned int)len;
  return (uint32_t) SVC_cx_call(SYSCALL_cx_crc32_hw_ID, parameters);
}

cx_err_t cx_get_random_bytes(void *buffer, size_t len) {
  unsigned int parameters[2 + 2];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)len;
  return SVC_cx_call(SYSCALL_cx_get_random_bytes_ID, parameters);
}

void cx_trng_get_random_data ( uint8_t * buf, size_t size ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)buf;
  parameters[1] = (unsigned int)size;
  SVC_cx_call(SYSCALL_cx_trng_get_random_data_ID, parameters);
}

void os_perso_erase_all ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_perso_erase_all_ID, parameters);
  return;
}

void os_perso_set_seed ( unsigned int identity, unsigned int algorithm, unsigned char * seed, unsigned int length ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)identity;
  parameters[1] = (unsigned int)algorithm;
  parameters[2] = (unsigned int)seed;
  parameters[3] = (unsigned int)length;
  SVC_Call(SYSCALL_os_perso_set_seed_ID, parameters);
  return;
}

void os_perso_derive_and_set_seed ( unsigned char identity, const char * prefix, unsigned int prefix_length, const char * passphrase, unsigned int passphrase_length, const char * words, unsigned int words_length ) {
  unsigned int parameters[7];
  parameters[0] = (unsigned int)identity;
  parameters[1] = (unsigned int)prefix;
  parameters[2] = (unsigned int)prefix_length;
  parameters[3] = (unsigned int)passphrase;
  parameters[4] = (unsigned int)passphrase_length;
  parameters[5] = (unsigned int)words;
  parameters[6] = (unsigned int)words_length;
  SVC_Call(SYSCALL_os_perso_derive_and_set_seed_ID, parameters);
  return;
}

#if defined(HAVE_VAULT_RECOVERY_ALGO)
void os_perso_derive_and_prepare_seed(const char* words, unsigned int words_length, uint8_t *vault_recovery_work_buffer) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)words;
  parameters[1] = (unsigned int)words_length;
  parameters[2] = (unsigned int)vault_recovery_work_buffer;
  SVC_Call(SYSCALL_os_perso_derive_and_prepare_seed_ID, parameters);
  return;
}

void os_perso_derive_and_xor_seed(uint8_t *vault_recovery_work_buffer) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)vault_recovery_work_buffer;
  SVC_Call(SYSCALL_os_perso_derive_and_xor_seed_ID, parameters);
  return;
}

unsigned char os_perso_get_seed_algorithm(void) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned char)SVC_Call(SYSCALL_os_perso_get_seed_algorithm_ID, parameters);
}
#endif // HAVE_VAULT_RECOVERY_ALGO

void os_perso_set_words ( const unsigned char * words, unsigned int length ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)words;
  parameters[1] = (unsigned int)length;
  SVC_Call(SYSCALL_os_perso_set_words_ID, parameters);
  return;
}

void os_perso_finalize ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_perso_finalize_ID, parameters);
  return;
}

bolos_bool_t os_perso_isonboarded ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (bolos_bool_t) SVC_Call(SYSCALL_os_perso_isonboarded_ID, parameters);
}

void os_perso_set_onboarding_status(unsigned int state, unsigned int count, unsigned int total) {
  unsigned int parameters [3];
  parameters[0] = (unsigned int)state;
  parameters[1] = (unsigned int)count;
  parameters[2] = (unsigned int)total;
  SVC_Call(SYSCALL_os_perso_setonboardingstatus_ID, parameters);
  return;
}

void os_perso_derive_node_bip32 ( cx_curve_t curve, const unsigned int * path, unsigned int pathLength, unsigned char * privateKey, unsigned char * chain ) {
  unsigned int parameters[5];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)path;
  parameters[2] = (unsigned int)pathLength;
  parameters[3] = (unsigned int)privateKey;
  parameters[4] = (unsigned int)chain;
  SVC_Call(SYSCALL_os_perso_derive_node_bip32_ID, parameters);
  return;
}

void os_perso_derive_node_with_seed_key ( unsigned int mode, cx_curve_t curve, const unsigned int * path, unsigned int pathLength, unsigned char * privateKey, unsigned char * chain, unsigned char * seed_key, unsigned int seed_key_length ) {
  unsigned int parameters[8];
  parameters[0] = (unsigned int)mode;
  parameters[1] = (unsigned int)curve;
  parameters[2] = (unsigned int)path;
  parameters[3] = (unsigned int)pathLength;
  parameters[4] = (unsigned int)privateKey;
  parameters[5] = (unsigned int)chain;
  parameters[6] = (unsigned int)seed_key;
  parameters[7] = (unsigned int)seed_key_length;
  SVC_Call(SYSCALL_os_perso_derive_node_with_seed_key_ID, parameters);
  return;
}

void os_perso_derive_eip2333 ( cx_curve_t curve, const unsigned int * path, unsigned int pathLength, unsigned char * privateKey ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)curve;
  parameters[1] = (unsigned int)path;
  parameters[2] = (unsigned int)pathLength;
  parameters[3] = (unsigned int)privateKey;
  SVC_Call(SYSCALL_os_perso_derive_eip2333_ID, parameters);
  return;
}

#if defined(HAVE_SEED_COOKIE)
bolos_bool_t os_perso_seed_cookie ( unsigned char * seed_cookie ) {
  unsigned int parameters[1];
  parameters[0] = (unsigned int)seed_cookie;
  return (bolos_bool_t) SVC_Call(SYSCALL_os_perso_seed_cookie_ID, parameters);
}
#endif // HAVE_SEED_COOKIE

unsigned int os_endorsement_get_code_hash ( unsigned char * buffer ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_endorsement_get_code_hash_ID, parameters);
}

unsigned int os_endorsement_get_public_key ( unsigned char index, unsigned char * buffer, unsigned char * length ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)index;
  parameters[1] = (unsigned int)buffer;
  parameters[2] = (unsigned int)length;
  return (unsigned int) SVC_Call(SYSCALL_os_endorsement_get_public_key_ID, parameters);
}

unsigned int os_endorsement_get_public_key_certificate ( unsigned char index, unsigned char * buffer, unsigned char * length ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)index;
  parameters[1] = (unsigned int)buffer;
  parameters[2] = (unsigned int)length;
  return (unsigned int) SVC_Call(SYSCALL_os_endorsement_get_public_key_certificate_ID, parameters);
}

unsigned int os_endorsement_key1_get_app_secret ( unsigned char * buffer ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_endorsement_key1_get_app_secret_ID, parameters);
}

unsigned int os_endorsement_key1_sign_data ( unsigned char * src, unsigned int srcLength, unsigned char * signature ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)src;
  parameters[1] = (unsigned int)srcLength;
  parameters[2] = (unsigned int)signature;
  return (unsigned int) SVC_Call(SYSCALL_os_endorsement_key1_sign_data_ID, parameters);
}

unsigned int os_endorsement_key2_derive_sign_data ( unsigned char * src, unsigned int srcLength, unsigned char * signature ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)src;
  parameters[1] = (unsigned int)srcLength;
  parameters[2] = (unsigned int)signature;
  return (unsigned int) SVC_Call(SYSCALL_os_endorsement_key2_derive_sign_data_ID, parameters);
}

void os_perso_set_pin ( unsigned int identity, unsigned char * pin, unsigned int length ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)identity;
  parameters[1] = (unsigned int)pin;
  parameters[2] = (unsigned int)length;
  SVC_Call(SYSCALL_os_perso_set_pin_ID, parameters);
  return;
}

void os_perso_set_current_identity_pin ( unsigned char * pin, unsigned int length ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)pin;
  parameters[1] = (unsigned int)length;
  SVC_Call(SYSCALL_os_perso_set_current_identity_pin_ID, parameters);
  return;
}

bolos_bool_t os_global_pin_is_validated ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (bolos_bool_t) SVC_Call(SYSCALL_os_global_pin_is_validated_ID, parameters);
}

bolos_bool_t os_global_pin_check ( unsigned char * pin_buffer, unsigned char pin_length ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)pin_buffer;
  parameters[1] = (unsigned int)pin_length;
  return (bolos_bool_t) SVC_Call(SYSCALL_os_global_pin_check_ID, parameters);
}

void os_global_pin_invalidate ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_global_pin_invalidate_ID, parameters);
  return;
}

unsigned int os_global_pin_retries ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_global_pin_retries_ID, parameters);
}

unsigned int os_registry_count ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_registry_count_ID, parameters);
}

void os_registry_get ( unsigned int app_idx, application_t * out_application_entry ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)app_idx;
  parameters[1] = (unsigned int)out_application_entry;
  SVC_Call(SYSCALL_os_registry_get_ID, parameters);
  return;
}

#if !defined(APP_UX)
unsigned int os_ux ( bolos_ux_params_t * params ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)params;
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_ux_ID, parameters);
}

void os_ux_result ( bolos_ux_params_t * params ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)params;
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_ux_result_ID, parameters);
  return;
}
#endif // !defined(APP_UX)

void os_lib_call ( unsigned int * call_parameters ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)call_parameters;
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_lib_call_ID, parameters);
  return;
}

void os_lib_end ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_lib_end_ID, parameters);
  return;
}

unsigned int os_flags ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_flags_ID, parameters);
}

unsigned int os_version ( unsigned char * version, unsigned int maxlength ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)version;
  parameters[1] = (unsigned int)maxlength;
  return (unsigned int) SVC_Call(SYSCALL_os_version_ID, parameters);
}

unsigned int os_serial ( unsigned char * serial, unsigned int maxlength ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)serial;
  parameters[1] = (unsigned int)maxlength;
  return (unsigned int) SVC_Call(SYSCALL_os_serial_ID, parameters);
}

unsigned int os_seph_features ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_seph_features_ID, parameters);
}

unsigned int os_seph_version ( unsigned char * version, unsigned int maxlength ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)version;
  parameters[1] = (unsigned int)maxlength;
  return (unsigned int) SVC_Call(SYSCALL_os_seph_version_ID, parameters);
}

unsigned int os_bootloader_version ( unsigned char * version, unsigned int maxlength ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)version;
  parameters[1] = (unsigned int)maxlength;
  return (unsigned int) SVC_Call(SYSCALL_os_bootloader_version_ID, parameters);
}

unsigned int os_setting_get ( unsigned int setting_id, unsigned char * value, unsigned int maxlen ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)setting_id;
  parameters[1] = (unsigned int)value;
  parameters[2] = (unsigned int)maxlen;
  return (unsigned int) SVC_Call(SYSCALL_os_setting_get_ID, parameters);
}

void os_setting_set ( unsigned int setting_id, unsigned char * value, unsigned int length ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)setting_id;
  parameters[1] = (unsigned int)value;
  parameters[2] = (unsigned int)length;
  SVC_Call(SYSCALL_os_setting_set_ID, parameters);
  return;
}

void os_get_memory_info ( meminfo_t * meminfo ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)meminfo;
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_get_memory_info_ID, parameters);
  return;
}

unsigned int os_registry_get_tag ( unsigned int app_idx, unsigned int * tlvoffset, unsigned int tag, unsigned int value_offset, void * buffer, unsigned int maxlength ) {
  unsigned int parameters[6];
  parameters[0] = (unsigned int)app_idx;
  parameters[1] = (unsigned int)tlvoffset;
  parameters[2] = (unsigned int)tag;
  parameters[3] = (unsigned int)value_offset;
  parameters[4] = (unsigned int)buffer;
  parameters[5] = (unsigned int)maxlength;
  return (unsigned int) SVC_Call(SYSCALL_os_registry_get_tag_ID, parameters);
}

unsigned int os_registry_get_current_app_tag ( unsigned int tag, unsigned char * buffer, unsigned int maxlen ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)tag;
  parameters[1] = (unsigned int)buffer;
  parameters[2] = (unsigned int)maxlen;
  return (unsigned int) SVC_Call(SYSCALL_os_registry_get_current_app_tag_ID, parameters);
}

void os_registry_delete_app_and_dependees ( unsigned int app_idx ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)app_idx;
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_registry_delete_app_and_dependees_ID, parameters);
  return;
}

void os_registry_delete_all_apps ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_registry_delete_all_apps_ID, parameters);
  return;
}

#ifndef HAVE_BOLOS_NO_CUSTOMCA
unsigned int os_customca_verify ( unsigned char * hash, unsigned char * sign, unsigned int sign_length ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)hash;
  parameters[1] = (unsigned int)sign;
  parameters[2] = (unsigned int)sign_length;
  return (unsigned int) SVC_Call(SYSCALL_os_customca_verify_ID, parameters);
}
#endif // HAVE_BOLOS_NO_CUSTOMCA

void os_sched_exec ( unsigned int app_idx ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)app_idx;
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_sched_exec_ID, parameters);
  return;
}

void __attribute__((noreturn)) os_sched_exit ( bolos_task_status_t exit_code ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)exit_code;
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_sched_exit_ID, parameters);

  // The os_sched_exit syscall should never return. Just in case, prevent the
  // device from freezing (because of the following infinite loop) thanks to an
  // undefined instruction.
  asm volatile ("udf #255");

  // remove the warning caused by -Winvalid-noreturn
  while (1) {
  }
}

bolos_bool_t os_sched_is_running ( unsigned int task_idx ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)task_idx;
  parameters[1] = 0;
  return (bolos_bool_t) SVC_Call(SYSCALL_os_sched_is_running_ID, parameters);
}

unsigned int os_sched_create ( void * main, void * nvram, unsigned int nvram_length, void * ram0, unsigned int ram0_length, void * stack, unsigned int stack_length ) {
  unsigned int parameters[7];
  parameters[0] = (unsigned int)main;
  parameters[1] = (unsigned int)nvram;
  parameters[2] = (unsigned int)nvram_length;
  parameters[3] = (unsigned int)ram0;
  parameters[4] = (unsigned int)ram0_length;
  parameters[5] = (unsigned int)stack;
  parameters[6] = (unsigned int)stack_length;
  return (unsigned int) SVC_Call(SYSCALL_os_sched_create_ID, parameters);
}

void os_sched_kill ( unsigned int taskidx ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)taskidx;
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_sched_kill_ID, parameters);
  return;
}

void io_seph_send ( const unsigned char * buffer, unsigned short length ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)length;
  SVC_Call(SYSCALL_io_seph_send_ID, parameters);
  return;
}

unsigned int io_seph_is_status_sent ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_io_seph_is_status_sent_ID, parameters);
}

unsigned short io_seph_recv ( unsigned char * buffer, unsigned short maxlength, unsigned int flags ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)maxlength;
  parameters[2] = (unsigned int)flags;
  return (unsigned short) SVC_Call(SYSCALL_io_seph_recv_ID, parameters);
}

void nvm_write_page(unsigned int page_adr, bool force) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int) page_adr;
  parameters[1] = (unsigned int) force;
  SVC_Call(SYSCALL_nvm_write_page_ID, parameters);
  return;
}

void nvm_erase_page ( unsigned int page_adr ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)page_adr;
  parameters[1] = 0;
  SVC_Call(SYSCALL_nvm_erase_page_ID, parameters);
  return;
}

try_context_t * try_context_get ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (try_context_t *) SVC_Call(SYSCALL_try_context_get_ID, parameters);
}

try_context_t * try_context_set ( try_context_t *context ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)context;
  parameters[1] = 0;
  return (try_context_t *) SVC_Call(SYSCALL_try_context_set_ID, parameters);
}

bolos_task_status_t os_sched_last_status ( unsigned int task_idx ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)task_idx;
  parameters[1] = 0;
  return (bolos_task_status_t) SVC_Call(SYSCALL_os_sched_last_status_ID, parameters);
}

void os_sched_yield ( bolos_task_status_t status ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)status;
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_sched_yield_ID, parameters);
  return;
}

void os_sched_switch ( unsigned int task_idx, bolos_task_status_t status ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)task_idx;
  parameters[1] = (unsigned int)status;
  SVC_Call(SYSCALL_os_sched_switch_ID, parameters);
  return;
}

unsigned int os_sched_current_task ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_sched_current_task_ID, parameters);
}

unsigned int os_allow_protected_ram( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_allow_protected_ram_ID, parameters);
}

unsigned int os_deny_protected_ram( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_deny_protected_ram_ID, parameters);
}

unsigned int os_allow_protected_flash( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_allow_protected_flash_ID, parameters);
}

unsigned int os_deny_protected_flash( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_deny_protected_flash_ID, parameters);
}

#ifdef HAVE_CUSTOM_CA_SETTINGS
bolos_bool_t os_bolos_custom_ca_get_info(char* name, uint8_t* pubkey) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int) name;
  parameters[1] = (unsigned int) pubkey;
  bolos_bool_t ret = (bolos_bool_t) SVC_Call(SYSCALL_os_bolos_custom_ca_get_info_ID, parameters);
  return ret;
}

void os_bolos_custom_ca_revoke(void) {
  unsigned int parameters[1];
  parameters[0] = 0;
  SVC_Call(SYSCALL_os_bolos_custom_ca_revoke_ID, parameters);
  return;
}
#endif //HAVE_CUSTOM_CA_SETTINGS

#ifdef HAVE_MCU_SERIAL_STORAGE
unsigned int os_seph_serial ( unsigned char * serial, unsigned int maxlength ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)serial;
  parameters[1] = (unsigned int)maxlength;
  return (unsigned int) SVC_Call(SYSCALL_os_seph_serial_ID, parameters);
}
#endif

#ifdef HAVE_SE_SCREEN
void screen_clear ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_Call(SYSCALL_screen_clear_ID, parameters);
  return;
}

void screen_update ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_Call(SYSCALL_screen_update_ID, parameters);
  return;
}

#ifdef HAVE_BRIGHTNESS_SYSCALL
void screen_set_brightness ( unsigned int percent ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)percent;
  SVC_Call(SYSCALL_screen_set_brightness_ID, parameters);
  return;
}
#endif // HAVE_BRIGHTNESS_SYSCALL

void screen_set_keepout ( unsigned int x, unsigned int y, unsigned int width, unsigned int height ) {
  unsigned int parameters[4];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)y;
  parameters[2] = (unsigned int)width;
  parameters[3] = (unsigned int)height;
  SVC_Call(SYSCALL_screen_set_keepout_ID, parameters);
  return;
}

void bagl_hal_draw_bitmap_within_rect ( int x, int y, unsigned int width, unsigned int height, unsigned int color_count, const unsigned int * colors, unsigned int bit_per_pixel, const unsigned char * bitmap, unsigned int bitmap_length_bits ) {
  unsigned int parameters[9];
  parameters[0] = (unsigned int)x;
  parameters[1] = (unsigned int)y;
  parameters[2] = (unsigned int)width;
  parameters[3] = (unsigned int)height;
  parameters[4] = (unsigned int)color_count;
  parameters[5] = (unsigned int)colors;
  parameters[6] = (unsigned int)bit_per_pixel;
  parameters[7] = (unsigned int)bitmap;
  parameters[8] = (unsigned int)bitmap_length_bits;
  SVC_Call(SYSCALL_bagl_hal_draw_bitmap_within_rect_ID, parameters);
  return;
}

void bagl_hal_draw_rect ( unsigned int color, int x, int y, unsigned int width, unsigned int height ) {
  unsigned int parameters[5];
  parameters[0] = (unsigned int)color;
  parameters[1] = (unsigned int)x;
  parameters[2] = (unsigned int)y;
  parameters[3] = (unsigned int)width;
  parameters[4] = (unsigned int)height;
  SVC_Call(SYSCALL_bagl_hal_draw_rect_ID, parameters);
  return;
}
#endif

#ifdef HAVE_BLE
void os_ux_set_status ( unsigned int ux_id, unsigned int status ) {
  unsigned int parameters [2+2];
  parameters[0] = (unsigned int)ux_id;
  parameters[1] = (unsigned int)status;
  SVC_Call(SYSCALL_os_ux_set_status_ID_IN, parameters);
  return;
}

unsigned int os_ux_get_status ( unsigned int ux_id ) {
  unsigned int parameters [2+1];
  parameters[0] = (unsigned int)ux_id;
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_ux_get_status_ID_IN, parameters);
}
#endif // HAVE_BLE

#ifdef HAVE_SE_BUTTON
unsigned int io_button_read ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_io_button_read_ID, parameters);
}
#endif // HAVE_SE_BUTTON

#ifdef HAVE_AEM_PIN
void os_aem_set_pin ( unsigned char * aem_pin, unsigned int aem_pin_length ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)aem_pin;
  parameters[1] = (unsigned int)aem_pin_length;
  SVC_Call(SYSCALL_os_aem_set_pin_ID, parameters);
  return;
}

void os_aem_unset_pin ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_aem_unset_pin_ID, parameters);
  return;
}

bolos_bool_t os_aem_is_pin_set ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (bolos_bool_t) SVC_Call(SYSCALL_os_aem_is_pin_set_ID, parameters);
}

void os_aem_set_response ( unsigned char * aem_response, unsigned int aem_response_length, unsigned char aem_response_format ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)aem_response;
  parameters[1] = (unsigned int)aem_response_length;
  parameters[2] = (unsigned int)aem_response_format;
  SVC_Call(SYSCALL_os_aem_set_response_ID, parameters);
  return;
}

void os_aem_activate_response ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_aem_activate_response_ID, parameters);
  return;
}

void os_aem_deactivate_response ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_aem_deactivate_response_ID, parameters);
  return;
}

bolos_bool_t os_aem_is_response_active ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (bolos_bool_t) SVC_Call(SYSCALL_os_aem_is_response_active_ID, parameters);
}

bolos_bool_t os_aem_is_feature_active ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (bolos_bool_t) SVC_Call(SYSCALL_os_aem_is_feature_active_ID, parameters);
}

unsigned char os_aem_get_response_length ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned char) SVC_Call(SYSCALL_os_aem_get_response_length_ID, parameters);
}

unsigned char os_aem_get_response_format ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned char) SVC_Call(SYSCALL_os_aem_get_response_format_ID, parameters);
}

void os_aem_get_response ( char * output_response_buffer, unsigned char input_required_length ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)output_response_buffer;
  parameters[1] = (unsigned int)input_required_length;
  SVC_Call(SYSCALL_os_aem_get_response_ID, parameters);
  return;
}

bolos_bool_t os_aem_check_pin ( unsigned char * aem_pin_buffer, unsigned char aem_pin_length ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)aem_pin_buffer;
  parameters[1] = (unsigned int)aem_pin_length;
  return (bolos_bool_t) SVC_Call(SYSCALL_os_aem_check_pin_ID, parameters);
}

void os_aem_invalidate_pin ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_Call(SYSCALL_os_aem_invalidate_pin_ID, parameters);
  return;
}

unsigned int os_aem_get_ptc ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (unsigned int) SVC_Call(SYSCALL_os_aem_get_ptc_ID, parameters);
}

bolos_bool_t os_aem_is_pin_validated ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  return (bolos_bool_t) SVC_Call(SYSCALL_os_aem_is_pin_validated_ID, parameters);
}
#endif // HAVE_AEM_PIN

#if (defined(HAVE_BOLOS_NOTWIPED_ENDORSEMENT) && defined(HAVE_ENDORSEMENTS_DISPLAY))
unsigned int os_endorsement_get_metadata ( unsigned char index, unsigned char * buffer ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)index;
  parameters[1] = (unsigned int)buffer;
  return (unsigned int) SVC_Call(SYSCALL_os_endorsement_get_metadata_ID, parameters);
}
#endif // (defined(HAVE_BOLOS_NOTWIPED_ENDORSEMENT) && defined(HAVE_ENDORSEMENTS_DISPLAY))

#if defined(HAVE_LANGUAGE_PACK)
const LANGUAGE_PACK *fetch_language_packs(UX_LOC_LANGUAGE_PACK_INFO *packs, unsigned int language, const LANGUAGE_PACK *built_in, unsigned int built_in_length) {
  unsigned int parameters [4];
  parameters[0] = (unsigned int)packs;
  parameters[1] = language;
  parameters[2] = (unsigned int)built_in;
  parameters[3] = built_in_length;
  return (LANGUAGE_PACK *)SVC_Call(SYSCALL_fetch_language_packs_ID, parameters);
}
#endif //defined(HAVE_LANGUAGE_PACK)

#if defined(HAVE_BACKGROUND_IMG)
uint8_t *fetch_background_img(bool allow_candidate) {
  uint8_t parameters[1] = {(uint8_t) allow_candidate};
  return (uint8_t *) SVC_Call(SYSCALL_fetch_background_img, parameters);
}

bolos_err_t delete_background_img(bool force_skip_consent) {
  uint8_t parameters[1] = {force_skip_consent};
  return SVC_Call(SYSCALL_delete_background_img, parameters);
}
#endif

#ifdef HAVE_SE_TOUCH
void touch_get_last_info( io_touch_info_t *info ) {
  unsigned int parameters[1] = {(unsigned int) info};
  SVC_Call(SYSCALL_touch_get_last_info_ID, parameters);
}
#endif // HAVE_SE_TOUCH

#ifdef HAVE_IO_I2C
void io_i2c_setmode ( unsigned int speed_and_master, unsigned int address ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)speed_and_master;
  parameters[1] = (unsigned int)address;
  SVC_Call(SYSCALL_io_i2c_setmode_ID, parameters);
  return;
}

void io_i2c_prepare ( unsigned int maxlength ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)maxlength;
  parameters[1] = 0;
  SVC_Call(SYSCALL_io_i2c_prepare_ID, parameters);
  return;
}

void io_i2c_xfer ( void * buffer, unsigned int length, unsigned int flags ) {
  unsigned int parameters[3];
  parameters[0] = (unsigned int)buffer;
  parameters[1] = (unsigned int)length;
  parameters[2] = (unsigned int)flags;
  SVC_Call(SYSCALL_io_i2c_xfer_ID, parameters);
  return;
}

#ifndef BOLOS_RELEASE
#ifdef BOLOS_DEBUG
void io_i2c_dumpstate ( void ) {
  unsigned int parameters[2];
  parameters[1] = 0;
  SVC_Call(SYSCALL_io_i2c_dumpstate_ID, parameters);
  return;
}

void io_debug ( char * chars, unsigned int len ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)chars;
  parameters[1] = (unsigned int)len;
  SVC_Call(SYSCALL_io_debug_ID, parameters);
  return;
}
#endif // BOLOS_DEBUG
#endif // BOLOS_RELEASE
#endif // HAVE_IO_I2C

#ifdef DEBUG_OS_STACK_CONSUMPTION
int os_stack_operations(unsigned char mode) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)mode;
  return (unsigned int) SVC_Call(SYSCALL_os_stack_operations_ID, parameters);
}
#endif // DEBUG_OS_STACK_CONSUMPTION

#ifdef BOLOS_DEBUG
void log_debug ( const char * string ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)string;
  parameters[1] = 0;
  SVC_Call(SYSCALL_log_debug_ID, parameters);
  return;
}

void log_debug_nw ( const char * string ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)string;
  parameters[1] = 0;
  SVC_Call(SYSCALL_log_debug_nw_ID, parameters);
  return;
}

void log_debug_int ( char * fmt, unsigned int i ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)fmt;
  parameters[1] = (unsigned int)i;
  SVC_Call(SYSCALL_log_debug_int_ID, parameters);
  return;
}

void log_debug_int_nw ( char * fmt, unsigned int i ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)fmt;
  parameters[1] = (unsigned int)i;
  SVC_Call(SYSCALL_log_debug_int_nw_ID, parameters);
  return;
}

void log_mem ( unsigned int * adr, unsigned int len32 ) {
  unsigned int parameters[2];
  parameters[0] = (unsigned int)adr;
  parameters[1] = (unsigned int)len32;
  SVC_Call(SYSCALL_log_mem_ID, parameters);
  return;
}
#endif // BOLOS_DEBUG
