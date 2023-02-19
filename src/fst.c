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

#include "fstinstruction.h"

#include <getopt.h>
#include <gio/gio.h>

GList* sct_parse_protocol(GFile* protocol, GStrvBuilder* args);

int main(int argc, char** argv) {
	int c;
	gchar* class_path = NULL;
	gchar* class = NULL;
	gboolean show_log = FALSE;
	gchar* protocol_path = NULL;

	static struct option long_options[] = {
		{"class-path", required_argument, 0, 'c'},
		{"show-log", no_argument, 0, 'l'},
		{0, 0, 0, 0}
	};

	const gchar* invocation  = "fst";
	if (argc > 0)
		invocation = argv[0];

	int option_idx = 0;
	while ((c = getopt_long(argc, argv, "c:l", long_options, &option_idx)) != -1) {
		switch (c) {
			case 'c':
				class_path = optarg;
				break;
			case 'l':
				show_log = TRUE;
				break;
			default: /* (?) */
				g_printerr("Usage: %s [--class-path class_path/] [--show-log] <tld.domain.name.Class> <./path/to/protocol.protocol>\n", invocation);
				return 2;
		}
	}

	if (optind != argc - 2) {
		g_printerr("Usage: %s [--class-path class_path/] [--show-log] <tld.domain.name.Class> <./path/to/protocol.protocol>\n", invocation);
		return 2;
	}

	class = argv[optind];
	protocol_path = argv[optind + 1];

	GFile* protocol = g_file_new_for_commandline_arg(protocol_path);

	GStrvBuilder* args_builder = g_strv_builder_new();
	g_strv_builder_add(args_builder, "java");
	if (class_path) {
		g_strv_builder_add_many(args_builder, "-classpath", class_path, NULL);
	}
	g_strv_builder_add(args_builder, class);

	GList* instructions = sct_parse_protocol(protocol, args_builder);
	if (!instructions) {
		g_critical("Failed parsing protocol file\n");
		g_strv_builder_unref(args_builder);
		g_object_unref(protocol);
		return 2;
	}

	g_auto(GStrv) args = g_strv_builder_end(args_builder);

	GError* err = NULL;
	GSubprocess* proc = g_subprocess_newv((const gchar**)args,
		G_SUBPROCESS_FLAGS_STDIN_PIPE | G_SUBPROCESS_FLAGS_STDOUT_PIPE | G_SUBPROCESS_FLAGS_STDERR_MERGE,
	&err);
	if (err) {
		g_critical("Failure spawning process: %s\n", err->message);
		g_error_free(err);
		g_list_free_full(instructions, g_object_unref);
		g_strv_builder_unref(args_builder);
		g_object_unref(protocol);
		return 1;
	}

	gboolean status = TRUE;

	FstStreamBundle streams = { .stdin = g_subprocess_get_stdin_pipe(proc), .output = g_subprocess_get_stdout_pipe(proc) };
	for (GList* active_instruction = instructions; active_instruction != NULL; active_instruction = active_instruction->next) {
		status &= fst_instruction_do_instruction(FST_INSTRUCTION(active_instruction->data), &streams, show_log);
		c++;
	}

	g_list_free_full(instructions, g_object_unref);
	g_strv_builder_unref(args_builder);
	g_object_unref(protocol);

	return !status;
}
