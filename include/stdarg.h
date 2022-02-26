/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef _STDARG_H
#define _STDARG_H

typedef __builtin_va_list va_list;

#define va_start(ap, parameter) __builtin_va_start(ap, parameter)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)

/* _STDARG_H */
#endif
