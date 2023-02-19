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

#include "fstinstructionoutputbasic.h"
#include "fstutil.h"

struct _FstInstructionOutputBasic {
	FstInstruction parent_instance;
	gchar* expected_output;
};

G_DEFINE_TYPE (FstInstructionOutputBasic, fst_instruction_output_basic, FST_TYPE_INSTRUCTION)

static void fst_instruction_output_basic_object_finalize(GObject* object) {
	FstInstructionOutputBasic* self = FST_INSTRUCTION_OUTPUT_BASIC(object);
	g_free(self->expected_output);
	
	G_OBJECT_CLASS(fst_instruction_output_basic_parent_class)->finalize(object);
}

static gboolean fst_instruction_output_basic_instruction_do(FstInstruction* instruction, FstStreamBundle* streams, gboolean log) {
	FstInstructionOutputBasic* self = FST_INSTRUCTION_OUTPUT_BASIC(instruction);

	gchar* output = fst_read_stream_to_eol(streams->output, NULL, 0);
	gboolean does_match = g_strcmp0(self->expected_output, output) == 0;

	if (log) {
		if (does_match) {
			g_print("%s\n", output);
		} else {
			g_print("\x1b[32m%s\x1b[0m\n", self->expected_output);
			g_print("\x1b[31m%s\x1b[0m\n", output);
		}
	}

	g_free(output);
	
	return does_match;
}

static void fst_instruction_output_basic_class_init(FstInstructionOutputBasicClass* class) {
	G_OBJECT_CLASS(class)->finalize = fst_instruction_output_basic_object_finalize;
	FST_INSTRUCTION_CLASS(class)->do_instruction = fst_instruction_output_basic_instruction_do;
}

void fst_instruction_output_basic_init(FstInstructionOutputBasic* self) {
	self->expected_output = NULL;
}

FstInstruction* fst_instruction_output_basic_new(gchar* expected_output) {
	g_return_val_if_fail(expected_output != NULL, NULL);

	FstInstructionOutputBasic* self = g_object_new(FST_TYPE_INSTRUCTION_OUTPUT_BASIC, NULL);
	self->expected_output = expected_output;
	return (FstInstruction*)self;
}
