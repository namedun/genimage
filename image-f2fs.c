/*
 * Copyright (c) 2011 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <confuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "genimage.h"

static int f2fs_generate(struct image *image)
{
	int ret;
	const char *extraargs =cfg_getstr(image->imagesec, "extraargs");
	const char *label = cfg_getstr(image->imagesec, "label");

	ret = systemp(image, "%s if=/dev/zero of=\"%s\" seek=%lld count=0 bs=1 2>/dev/null",
			get_opt("dd"), imageoutfile(image), image->size);
	if (ret)
		return ret;

	if (label && label[0] == '\0')
		label = NULL;

	ret = systemp(image, "%s -f %s%s%s %s '%s'",
			get_opt("mkf2fs"),
			label ? "-l '" : "",
			label ? label  : "",
			label ? "'"    : "",
			extraargs,
			imageoutfile(image));
	if (ret)
		return ret;

	if (!image->empty)
		ret = systemp(image, "%s -f '%s' '%s'",
				get_opt("sloadf2fs"),
				mountpath(image),
				imageoutfile(image));

	return ret;
}

static cfg_opt_t f2fs_opts[] = {
	CFG_STR("extraargs", "", CFGF_NONE),
	CFG_STR("label", 0, CFGF_NONE),
	CFG_END()
};

struct image_handler f2fs_handler = {
	.type = "f2fs",
	.generate = f2fs_generate,
	.opts = f2fs_opts,
};
