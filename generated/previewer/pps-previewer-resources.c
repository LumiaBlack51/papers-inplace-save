#include <gio/gio.h>

#if defined (__ELF__) && ( __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6))
# define SECTION __attribute__ ((section (".gresource.pps_previewer"), aligned (sizeof(void *) > 8 ? sizeof(void *) : 8)))
#else
# define SECTION
#endif

static const SECTION union { const guint8 data[1952]; const double alignment; void * const ptr;}  pps_previewer_resource_data = {
  "\107\126\141\162\151\141\156\164\000\000\000\000\000\000\000\000"
  "\030\000\000\000\034\001\000\000\000\000\000\050\011\000\000\000"
  "\000\000\000\000\000\000\000\000\002\000\000\000\004\000\000\000"
  "\006\000\000\000\006\000\000\000\007\000\000\000\007\000\000\000"
  "\010\000\000\000\162\237\247\120\007\000\000\000\034\001\000\000"
  "\012\000\114\000\050\001\000\000\060\001\000\000\113\120\220\013"
  "\006\000\000\000\060\001\000\000\004\000\114\000\064\001\000\000"
  "\070\001\000\000\237\301\267\072\000\000\000\000\070\001\000\000"
  "\003\000\114\000\074\001\000\000\104\001\000\000\260\267\044\060"
  "\001\000\000\000\104\001\000\000\006\000\114\000\114\001\000\000"
  "\120\001\000\000\351\100\067\046\002\000\000\000\120\001\000\000"
  "\023\000\166\000\150\001\000\000\217\004\000\000\066\327\230\357"
  "\002\000\000\000\217\004\000\000\020\000\166\000\240\004\000\000"
  "\255\006\000\000\324\265\002\000\377\377\377\377\255\006\000\000"
  "\001\000\114\000\260\006\000\000\264\006\000\000\312\071\366\063"
  "\003\000\000\000\264\006\000\000\007\000\114\000\274\006\000\000"
  "\300\006\000\000\372\314\214\040\000\000\000\000\300\006\000\000"
  "\011\000\166\000\320\006\000\000\237\007\000\000\160\162\145\166"
  "\151\145\167\145\162\057\000\000\010\000\000\000\002\000\000\000"
  "\157\162\147\057\003\000\000\000\165\151\057\000\005\000\000\000"
  "\004\000\000\000\147\156\157\155\145\057\000\000\007\000\000\000"
  "\160\162\145\166\151\145\167\145\162\055\167\151\156\144\157\167"
  "\056\165\151\000\000\000\000\000\266\013\000\000\001\000\000\000"
  "\170\332\265\126\113\123\333\060\020\276\347\127\010\235\053\102"
  "\073\235\116\017\211\151\200\322\162\050\144\206\264\235\351\205"
  "\221\355\305\121\221\045\125\132\023\302\257\357\372\105\122\142"
  "\362\142\172\223\245\335\375\366\333\227\167\160\374\220\153\166"
  "\017\076\050\153\206\374\355\341\021\147\140\022\233\052\223\015"
  "\371\367\311\271\370\310\217\243\336\340\100\210\336\331\025\273"
  "\274\232\260\317\147\027\223\203\336\144\252\002\273\125\032\330"
  "\114\006\366\051\003\003\136\042\244\054\236\263\130\027\340\274"
  "\062\050\022\233\073\222\361\207\354\302\004\004\231\276\141\220"
  "\052\144\070\205\136\142\275\207\340\254\051\261\330\141\254\135"
  "\155\117\232\224\171\150\015\222\350\023\220\302\151\227\355\236"
  "\020\344\041\135\201\277\225\011\104\003\017\177\012\105\266\231"
  "\126\361\220\147\170\307\027\014\337\023\303\176\064\260\361\157"
  "\110\220\045\132\206\060\344\143\027\316\154\122\344\140\360\233"
  "\115\101\163\246\322\041\317\253\143\064\160\336\072\360\070\147"
  "\106\346\060\344\211\065\250\114\141\213\300\243\163\251\003\014"
  "\372\255\304\212\254\064\306\242\104\102\026\255\265\025\344\321"
  "\223\314\022\366\212\042\371\274\004\323\257\255\104\003\204\334"
  "\351\062\114\013\173\143\017\367\012\146\340\177\052\223\332\031"
  "\147\116\172\042\066\344\243\164\066\162\116\253\244\262\333\274"
  "\256\270\234\302\255\054\064\212\231\112\161\312\243\017\107\107"
  "\153\370\265\302\123\120\331\024\067\111\243\102\015\045\223\216"
  "\210\222\207\053\301\041\207\047\326\352\130\372\037\104\150\325"
  "\125\264\116\320\243\010\070\047\273\321\273\145\350\144\252\164"
  "\312\160\356\152\271\056\333\137\251\036\301\237\110\317\033\361"
  "\347\062\137\360\356\244\100\264\246\316\111\146\205\053\143\113"
  "\231\027\116\146\320\345\220\325\250\234\100\170\100\316\320\113"
  "\023\050\071\062\326\364\066\007\252\227\161\243\317\306\244\277"
  "\256\156\222\052\365\345\007\217\146\312\034\076\307\136\243\253"
  "\222\047\115\322\052\234\010\363\074\266\224\366\316\002\352\067"
  "\314\267\014\200\041\146\373\222\277\244\207\175\210\077\141\156"
  "\115\232\012\333\274\202\166\331\104\004\167\015\232\056\255\257"
  "\311\227\016\334\204\366\152\217\324\223\076\073\265\205\301\315"
  "\016\065\165\013\046\345\033\122\122\215\302\125\157\264\214\313"
  "\111\322\341\306\315\270\324\130\327\244\233\153\230\014\260\152"
  "\376\267\103\163\227\204\272\015\370\105\000\121\030\152\114\255"
  "\014\251\240\057\376\311\173\325\353\024\246\062\032\215\106\050"
  "\262\014\350\357\222\212\032\254\232\225\215\334\153\343\373\150"
  "\155\056\154\201\173\044\374\027\251\262\253\142\247\340\264\160"
  "\333\125\172\053\275\103\251\357\310\274\231\357\373\064\273\365"
  "\271\324\354\132\075\302\316\001\150\120\167\010\302\255\102\021"
  "\123\015\374\277\110\050\263\157\011\134\230\235\003\240\314\016"
  "\334\325\226\243\156\353\221\177\235\170\253\065\244\057\155\010"
  "\123\170\160\262\014\333\363\336\174\046\167\377\222\334\113\103"
  "\267\372\313\127\041\057\027\230\252\215\137\362\175\161\261\304"
  "\270\135\206\072\376\365\043\132\024\361\114\111\155\263\032\041"
  "\255\317\035\344\144\271\217\166\246\363\134\322\276\111\205\143"
  "\131\075\003\273\306\137\275\322\006\010\213\143\205\227\150\033"
  "\240\173\042\237\226\117\203\176\053\036\055\216\141\231\350\142"
  "\301\355\375\005\300\277\076\167\000\050\165\165\141\171\051\160"
  "\141\147\145\055\163\145\154\145\143\164\157\162\056\165\151\000"
  "\355\004\000\000\001\000\000\000\170\332\275\124\301\156\333\060"
  "\014\275\373\053\124\235\347\244\003\066\140\003\154\167\330\332"
  "\016\005\206\265\100\263\163\100\313\214\255\125\226\064\121\116"
  "\023\140\037\077\252\116\226\164\111\203\236\166\062\101\075\074"
  "\076\076\075\271\270\130\365\106\054\061\220\166\266\224\157\047"
  "\347\122\240\125\256\321\266\055\345\217\331\165\376\101\136\124"
  "\131\161\226\347\331\345\255\370\176\073\023\127\227\067\263\263"
  "\154\326\151\022\013\155\120\074\002\211\117\055\132\014\020\261"
  "\021\365\132\324\146\100\037\264\215\271\162\275\147\114\230\210"
  "\033\113\021\241\171\043\260\321\121\304\016\063\345\102\100\362"
  "\316\246\131\142\122\033\077\362\201\155\104\300\055\041\103\377"
  "\016\322\261\073\306\235\345\071\053\344\026\206\005\050\254\212"
  "\200\277\006\315\334\302\350\272\224\155\174\220\273\015\337\361"
  "\206\323\252\210\330\173\223\350\225\001\242\122\336\171\272\203"
  "\026\357\321\240\212\056\110\341\041\240\215\245\374\032\037\076"
  "\273\225\254\012\212\153\303\334\117\170\141\241\307\122\332\241"
  "\307\240\125\342\233\156\216\175\160\036\103\134\157\020\056\150"
  "\146\201\310\243\145\165\136\114\267\307\007\100\102\113\072\352"
  "\045\312\152\001\206\160\037\252\072\155\232\252\160\365\117\326"
  "\266\025\314\272\256\154\014\153\051\164\123\112\174\052\253\002"
  "\224\102\042\135\153\243\217\014\061\120\243\221\042\006\260\304"
  "\313\103\155\270\271\106\222\325\270\067\057\335\076\233\074\075"
  "\315\327\303\052\177\324\115\354\162\325\101\240\323\033\076\003"
  "\276\077\001\134\201\321\155\262\153\362\161\037\105\334\004\263"
  "\301\200\142\257\370\372\244\350\070\056\034\202\135\153\256\152"
  "\051\350\021\274\107\066\146\026\006\114\367\363\242\207\113\266"
  "\356\213\143\373\234\141\232\173\225\276\362\100\323\302\100\313"
  "\262\071\105\121\053\060\277\033\115\052\140\304\027\005\322\110"
  "\264\223\227\254\235\217\335\244\060\105\146\024\303\305\106\334"
  "\053\065\136\073\065\220\374\147\236\101\130\356\273\261\110\240"
  "\271\033\342\161\073\016\146\277\132\314\267\061\103\051\164\143"
  "\234\376\123\052\116\075\220\303\155\266\317\233\313\335\177\041"
  "\373\003\264\333\302\306\000\050\165\165\141\171\051\057\000\000"
  "\001\000\000\000\160\141\160\145\162\163\057\000\000\000\000\000"
  "\163\164\171\154\145\056\143\163\163\000\000\000\000\000\000\000"
  "\156\001\000\000\001\000\000\000\170\332\205\220\315\012\302\060"
  "\020\204\357\175\212\275\010\055\064\245\052\042\264\057\341\311"
  "\373\066\011\151\260\115\102\222\376\200\370\356\246\177\040\130"
  "\161\141\016\313\316\174\014\233\121\255\074\127\236\364\222\017"
  "\031\323\264\153\247\315\240\340\360\214\040\114\245\107\342\152"
  "\144\172\050\200\352\106\133\322\312\061\226\012\234\025\125\012"
  "\075\332\230\020\212\226\315\056\116\146\117\002\327\313\041\005"
  "\157\121\071\203\066\040\023\310\315\010\247\240\163\320\321\214"
  "\277\243\371\142\330\214\145\364\212\242\233\161\367\320\160\353"
  "\204\364\041\254\356\024\133\062\305\312\032\244\012\075\111\045"
  "\126\324\147\264\160\274\341\324\163\366\207\201\224\116\037\330"
  "\145\354\077\350\013\064\324\322\363\162\076\032\144\114\052\121"
  "\100\076\161\336\245\350\170\252\000\050\165\165\141\171\051" };

static GStaticResource static_resource = { pps_previewer_resource_data.data, sizeof (pps_previewer_resource_data.data) - 1 /* nul terminator */, NULL, NULL, NULL };

G_GNUC_INTERNAL
GResource *pps_previewer_get_resource (void);
GResource *pps_previewer_get_resource (void)
{
  return g_static_resource_get_resource (&static_resource);
}
/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#ifndef __G_CONSTRUCTOR_H__
#define __G_CONSTRUCTOR_H__

/*
  If G_HAS_CONSTRUCTORS is true then the compiler support *both* constructors and
  destructors, in a usable way, including e.g. on library unload. If not you're on
  your own.

  Some compilers need #pragma to handle this, which does not work with macros,
  so the way you need to use this is (for constructors):

  #ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
  #pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(my_constructor)
  #endif
  G_DEFINE_CONSTRUCTOR(my_constructor)
  static void my_constructor(void) {
   ...
  }

*/

#ifndef __GTK_DOC_IGNORE__

#if  __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR(_func) static void __attribute__((constructor)) _func (void);
#define G_DEFINE_DESTRUCTOR(_func) static void __attribute__((destructor)) _func (void);

#elif defined (_MSC_VER)

/*
 * Only try to include gslist.h if not already included via glib.h,
 * so that items using gconstructor.h outside of GLib (such as
 * GResources) continue to build properly.
 */
#ifndef __G_LIB_H__
#include "gslist.h"
#endif

#include <stdlib.h>

#define G_HAS_CONSTRUCTORS 1

/* We do some weird things to avoid the constructors being optimized
 * away on VS2015 if WholeProgramOptimization is enabled. First we
 * make a reference to the array from the wrapper to make sure its
 * references. Then we use a pragma to make sure the wrapper function
 * symbol is always included at the link stage. Also, the symbols
 * need to be extern (but not dllexport), even though they are not
 * really used from another object file.
 */

/* We need to account for differences between the mangling of symbols
 * for x86 and x64/ARM/ARM64 programs, as symbols on x86 are prefixed
 * with an underscore but symbols on x64/ARM/ARM64 are not.
 */
#ifdef _M_IX86
#define G_MSVC_SYMBOL_PREFIX "_"
#else
#define G_MSVC_SYMBOL_PREFIX ""
#endif

#define G_DEFINE_CONSTRUCTOR(_func) G_MSVC_CTOR (_func, G_MSVC_SYMBOL_PREFIX)
#define G_DEFINE_DESTRUCTOR(_func) G_MSVC_DTOR (_func, G_MSVC_SYMBOL_PREFIX)

#define G_MSVC_CTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _wrapper(void);              \
  int _func ## _wrapper(void) { _func(); g_slist_find (NULL,  _array ## _func); return 0; } \
  __pragma(comment(linker,"/include:" _sym_prefix # _func "_wrapper")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _wrapper;

#define G_MSVC_DTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _constructor(void);              \
  int _func ## _constructor(void) { atexit (_func); g_slist_find (NULL,  _array ## _func); return 0; } \
   __pragma(comment(linker,"/include:" _sym_prefix # _func "_constructor")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _constructor;

#elif defined(__SUNPRO_C)

/* This is not tested, but i believe it should work, based on:
 * http://opensource.apple.com/source/OpenSSL098/OpenSSL098-35/src/fips/fips_premain.c
 */

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA 1
#define G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA 1

#define G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(_func) \
  init(_func)
#define G_DEFINE_CONSTRUCTOR(_func) \
  static void _func(void);

#define G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(_func) \
  fini(_func)
#define G_DEFINE_DESTRUCTOR(_func) \
  static void _func(void);

#else

/* constructors not supported for this compiler */

#endif

#endif /* __GTK_DOC_IGNORE__ */
#endif /* __G_CONSTRUCTOR_H__ */

#ifdef G_HAS_CONSTRUCTORS

#ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(pps_previewerresource_constructor)
#endif
G_DEFINE_CONSTRUCTOR(pps_previewerresource_constructor)
#ifdef G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(pps_previewerresource_destructor)
#endif
G_DEFINE_DESTRUCTOR(pps_previewerresource_destructor)

#else
#warning "Constructor not supported on this compiler, linking in resources will not work"
#endif

static void pps_previewerresource_constructor (void)
{
  g_static_resource_init (&static_resource);
}

static void pps_previewerresource_destructor (void)
{
  g_static_resource_fini (&static_resource);
}
