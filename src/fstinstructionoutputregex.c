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

#include "fstinstructionoutputregex.h"
#include "fstutil.h"

struct _FstInstructionOutputRegex {
	FstInstruction parent_instance;
	GRegex* expected_match;
};

G_DEFINE_TYPE (FstInstructionOutputRegex, fst_instruction_output_regex, FST_TYPE_INSTRUCTION)

static void fst_instruction_output_regex_object_dispose(GObject* object) {
	FstInstructionOutputRegex* self = FST_INSTRUCTION_OUTPUT_REGEX(object);
	g_clear_pointer(&self->expected_match, g_regex_unref);

	G_OBJECT_CLASS(fst_instruction_output_regex_parent_class)->dispose(object);
}

static gboolean fst_instruction_output_regex_instruction_do(FstInstruction* instruction, FstStreamBundle* streams, gboolean log) {
	FstInstructionOutputRegex* self = FST_INSTRUCTION_OUTPUT_REGEX(instruction);

	gchar* output = fst_read_stream_to_eol(streams->output, NULL, 0);

	GMatchInfo* match_info = NULL;
	gboolean matches = FALSE;
	if (g_regex_match_all(self->expected_match, output, 0, &match_info)) {
		gchar *match = g_match_info_fetch(match_info, 0);  // get the entire match
		matches = (g_strcmp0(output, match) == 0);  // compare the match with the input string
		g_free(match);
	}
	g_match_info_free(match_info);

	if (log) {
		if (matches) {
			g_print("%s\n", output);
		} else {
			g_print("\x1b[35m<r\x1b[32m%s\x1b[0m\n", g_regex_get_pattern(self->expected_match));
			g_print("\x1b[31m%s\x1b[0m\n", output);
		}
	}

	g_free(output);
	
	return matches;
}

static void fst_instruction_output_regex_class_init(FstInstructionOutputRegexClass* class) {
	G_OBJECT_CLASS(class)->dispose = fst_instruction_output_regex_object_dispose;
	FST_INSTRUCTION_CLASS(class)->do_instruction = fst_instruction_output_regex_instruction_do;
}

void fst_instruction_output_regex_init(FstInstructionOutputRegex* self) {
	self->expected_match = NULL;
}

FstInstruction* fst_instruction_output_regex_new(const gchar* pattern) {
	g_return_val_if_fail(pattern != NULL, NULL);

	GError* err = NULL;
	GRegex* regex = g_regex_new(pattern, 0, 0, &err);
	if (err) {
		g_critical("Failed to create regex: %s\n", err->message);
		g_error_free(err);
		return NULL;
	}
	FstInstructionOutputRegex* self = g_object_new(FST_TYPE_INSTRUCTION_OUTPUT_REGEX, NULL);
	self->expected_match = regex;
	return (FstInstruction*)self;
}
