/*
 * vprintf.c
 *
 *  Created on: 18 jul. 2015
 *      Author: Arjan van Vught
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>

#include "console.h"

struct context {
	int flag;
	int prec;
	int width;
	int total;
};

enum {
	FLAG_PRECISION	    =	(1 << 0 ),
	FLAG_UPPERCASE	    =	(1 << 1 ),
	FLAG_LONG     	    =	(1 << 2 ),
	FLAG_NEGATIVE	    =	(1 << 3 ),
	FLAG_MIN_WIDTH	    =	(1 << 4 ),
	FLAG_ZERO_PADDED    =	(1 << 5 ),
	FLAG_LEFT_JUSTIFIED =	(1 << 6 )
};

/*@null@*/static char *outptr = NULL;

inline static void xputch(struct context *ctx, int c) {
	ctx->total++;

	if (outptr != NULL) {
		*outptr++ = (char) c;
		return;
	}

	(void)console_putc(c);
}

inline static bool is_digit(char c) {
	return (c >= (char)'0') && (c <= (char)'9');
}

inline static void format_hex(struct context *ctx, unsigned int arg) {
	char buffer[16];
	char *p = buffer + (sizeof(buffer) / sizeof(buffer[0])) - 1;
	char *o = p;
	char alpha;
	char u;

	if (arg == 0) {
		*p = '0';
		p--;
	} else {
		alpha = ((ctx->flag & FLAG_UPPERCASE) != 0) ? ((char)'A' - (char)10) : ((char)'a' - (char)10);

		while (arg != 0) {
			u = (char)arg & (char)0x0F;
			*p = (u < (char)10) ? ((char)'0' + u) : (alpha + u);
			p--;
			arg = arg >> 4;
		}
	}

	if ((ctx->flag & FLAG_PRECISION) != 0) {
		while (((o - p) < ctx->prec) && (p > buffer)) {
			*p-- = '0';
		}
	}

	if ((ctx->flag & FLAG_ZERO_PADDED) != 0) {
		while (((o - p) < ctx->width) && (p > buffer)) {
			*p-- = '0';
		}
	}

	if ((ctx->flag & FLAG_MIN_WIDTH) != 0) {
		while (((o - p) < ctx->width) && (p > buffer)) {
			*p-- = ' ';
		}
	}

	p++;

	while (p < buffer + (sizeof(buffer) / sizeof(buffer[0]))) {
		xputch(ctx, (int )*p++);
	}
}

inline static void format_int(struct context *ctx, long int arg) {
	char buffer[16];
	char *p = buffer + (sizeof(buffer) / sizeof(buffer[0])) - 1;
	char *o = p;

	if (arg == 0) {
		*p = '0';
		p--;
	} else {
		while (arg != 0) {
			*p = (char)(arg % 10) + (char)'0';
			p--;
			arg = arg / 10;
		}
	}

	if ((ctx->flag & FLAG_PRECISION) != 0) {
		while (((o - p) < ctx->prec) && (p > buffer)) {
			*p-- = '0';
		}
	}

	if ((ctx->flag & FLAG_ZERO_PADDED) != 0) {
		while (((o - p) < ctx->width) && (p > buffer)) {
			*p-- = '0';
		}
	}

	if((ctx->flag & FLAG_NEGATIVE) != 0) {
		*p-- = '-';
	}

	if ((ctx->flag & FLAG_MIN_WIDTH) != 0) {
		while (((o - p) < ctx->width) && (p > buffer)) {
			*p-- = ' ';
		}
	}

	p++;

	while (p < buffer + (sizeof(buffer) / sizeof(buffer[0]))) {
		xputch(ctx, (int )*p++);
	}
}

int vprintf(const char *fmt, va_list va) {
	struct context ctx;
	long int l;
	int j;
	const char *s;

	ctx.total = 0;

	while (*fmt != (char)0) {

		if (*fmt != '%') {
			xputch(&ctx, (int )*fmt++);
			continue;
		}

		fmt++;

		ctx.flag = 0;
		ctx.prec = 0;
		ctx.width = 0;

		if (*fmt == '0') {
			ctx.flag |= FLAG_ZERO_PADDED;
			fmt++;
		} else if (*fmt == '-') {
			ctx.flag |= FLAG_LEFT_JUSTIFIED;
			fmt++;
		}

		while (is_digit(*fmt)) {
			ctx.width = ctx.width * 10 + (int)(*fmt - '0');
			fmt++;
		}

		if (ctx.width != 0) {
			ctx.flag |= FLAG_MIN_WIDTH;
		}

		if (*fmt == '.') {
			fmt++;
			while (is_digit(*fmt)) {
				ctx.prec = ctx.prec * 10 + (int)(*fmt - '0');
				fmt++;
			}
			ctx.flag |= FLAG_PRECISION;
		}

		if (*fmt == 'l') {
			fmt++;
			ctx.flag |= FLAG_LONG;
		}

		switch (*fmt) {
		case 'c':
			xputch(&ctx, va_arg(va, int));
			break;
		case 'd':
			l = ((ctx.flag & FLAG_LONG) != 0) ? va_arg(va, long int) : (long int) va_arg(va, int);
			if (l < 0) {
				ctx.flag |= FLAG_NEGATIVE;
				l = -l;
			}
			format_int(&ctx, l);
			break;
		case 's':
			s = va_arg(va, const char *);
			for (j = 0; s[j] != (char)0; j++)
				;	// strlen

			if ((ctx.flag & FLAG_PRECISION) != 0) {
				if (ctx.prec < j) {
					j = ctx.prec;
				}
			}

			while ((((ctx.flag & FLAG_LEFT_JUSTIFIED) ==0 )) && (j++ < ctx.width)) {
				xputch(&ctx, (int)' ');
			}

			while ((((ctx.flag & FLAG_PRECISION) == 0) || (ctx.prec != 0)) && (*s != (char)0)) {
				xputch(&ctx, (int )*s++);
				ctx.prec--;
			}

			while (j++ < ctx.width) {
				xputch(&ctx, (int)' ');
			}
			break;
		case 'X':
			ctx.flag |= FLAG_UPPERCASE;
			/*@fallthrough@*/
			/* no break */
		case 'x':
			format_hex(&ctx, va_arg(va, unsigned int));
			break;
		default:
			xputch(&ctx, (int)*fmt);
			continue;
			//break;
		}

		fmt++;
	}

	return ctx.total;
}

int printf(const char* fmt, ...) {
	int i;
	va_list arp;
	va_start(arp, fmt);

	i = vprintf(fmt, arp);

	va_end(arp);

	return i;
}

int sprintf(char *buffer, const char *fmt, ...) {
	int i;
	va_list arp;

	outptr = buffer;
	va_start(arp, fmt);

	i = vprintf(fmt, arp);

	va_end(arp);

	*outptr = (char)0;
	outptr = NULL;

	return i;
}
