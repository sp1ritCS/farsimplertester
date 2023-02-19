/*
 * FarSimplerTester - Validate output of Java software.
 * Copyright (C) 2023 Florian "sp1rit" <sp1rit@national.shitposting.agency>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "fstutil.h"

gchar* fst_read_stream_to_eol(GInputStream* stream, const guchar* prepend, gsize prep_len) {
	GByteArray *byte_array = g_byte_array_new();
	if (prep_len != 0 && prepend)
		g_byte_array_append(byte_array, prepend, prep_len);

	while (TRUE) {
		guchar byte;
		gssize bytes_read = g_input_stream_read(stream, &byte, 1, NULL, NULL);
		if (bytes_read == 0 || byte == '\n') {
			// end of stream or newline
			break;
		}
		g_byte_array_append(byte_array, &byte, 1);
	}

	g_byte_array_append(byte_array, (guchar*)"\0", 1);
	return (gchar*)g_byte_array_free(byte_array, FALSE);
}
