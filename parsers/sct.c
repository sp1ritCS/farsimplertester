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

#include <glib.h>
#include <gio/gio.h>

#include "fstinstructioninput.h"
#include "fstinstructionoutputbasic.h"
#include "fstinstructionoutputregex.h"

#include "fstutil.h"

GList* sct_parse_protocol(GFile* protocol, GStrvBuilder* args) {
	g_return_val_if_fail(G_IS_FILE(protocol), NULL);

	GError* err = NULL;
	GInputStream* protocol_stream_real = G_INPUT_STREAM(g_file_read(protocol, NULL, &err));
	GInputStream* protocol_stream = g_buffered_input_stream_new(protocol_stream_real);
	if (err) {
		g_critical("Failed to open file for reading: %s\n", err->message);
		return NULL;
	}

	GList* instructions = NULL;

	guchar byte;
	while ( g_input_stream_read(protocol_stream, &byte, 1, NULL, NULL) > 0 ) {
		switch (byte) {
			case '#':
				g_free(fst_read_stream_to_eol(protocol_stream, NULL, 0));
				break;
			case '<':
				if (g_input_stream_read(protocol_stream, &byte, 1, NULL, NULL) < 0) {
					g_critical("Found EOF after <\n");
					goto err;
				}
				switch (byte) {
					case 'r': {
						gchar* pattern = fst_read_stream_to_eol(protocol_stream, NULL, 0);
						instructions = g_list_prepend(instructions, fst_instruction_output_regex_new(pattern));
						g_free(pattern);
						} break;
					default:
						g_critical("Found invalid instruction: <%c\n", byte);
						goto err;
				}
				break;
			case '>':
				g_input_stream_read(protocol_stream, &byte, 1, NULL, NULL);
				instructions = g_list_prepend(instructions, fst_instruction_input_new(fst_read_stream_to_eol(protocol_stream, NULL, 0)));
				break;
			case '\n':
				instructions = g_list_prepend(instructions, fst_instruction_output_basic_new(g_strdup("")));
				break;
			case '$': 
				if (g_input_stream_read(protocol_stream, &byte, 1, NULL, NULL) < 0) {
					g_critical("Found EOF after $\n");
					goto err;
				}
				switch (byte) {
					case '$': {
						gchar* args_cont = fst_read_stream_to_eol(protocol_stream, NULL, 0);
						gchar* skip_space = args_cont;
						if (*skip_space == ' ')
							skip_space++;
						
						gchar** args_parsed;
						if (g_shell_parse_argv(skip_space, NULL, &args_parsed, NULL)) {
							g_strv_builder_addv(args, (const gchar**)args_parsed);
							g_strfreev(args_parsed);
						}
					
						g_free(args_cont);
						} break;
					default:
						g_critical("Found invalid instruction: $%c\n", byte);
						goto err;
				}
				break;
			default:
				instructions = g_list_prepend(instructions, fst_instruction_output_basic_new(fst_read_stream_to_eol(protocol_stream, &byte, 1)));
		}
	}

	g_object_unref(protocol_stream);
	g_object_unref(protocol_stream_real);
	return g_list_reverse(instructions);
err:
	g_list_free_full(instructions, g_object_unref);
	g_object_unref(protocol_stream);
	g_object_unref(protocol_stream_real);
	return NULL;
}
