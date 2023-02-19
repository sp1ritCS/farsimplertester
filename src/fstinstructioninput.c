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

#include "fstinstructioninput.h"

struct _FstInstructionInput {
	FstInstruction parent_instance;
	gchar* input;
};

G_DEFINE_TYPE (FstInstructionInput, fst_instruction_input, FST_TYPE_INSTRUCTION)

static void fst_instruction_input_object_finalize(GObject* object) {
	FstInstructionInput* self = FST_INSTRUCTION_INPUT(object);
	g_free(self->input);

	G_OBJECT_CLASS(fst_instruction_input_parent_class)->finalize(object);
}

static gboolean fst_instruction_input_instruction_do(FstInstruction* instruction, FstStreamBundle* streams, gboolean log) {
	FstInstructionInput* self = FST_INSTRUCTION_INPUT(instruction);

	GError* err = NULL;
	if (!g_output_stream_printf(streams->stdin, NULL, NULL, &err, "%s\n",  self->input)) {
		g_critical("Stream ended prematurely (couldn't write): %s\n", err->message);
		g_error_free(err);
		return FALSE;
	}

	if (log) {
		g_print("\x1b[36m> \x1b[0m%s\n", self->input);
	}

	return TRUE;
}

static void fst_instruction_input_class_init(FstInstructionInputClass* class) {
	G_OBJECT_CLASS(class)->finalize = fst_instruction_input_object_finalize;
	FST_INSTRUCTION_CLASS(class)->do_instruction = fst_instruction_input_instruction_do;
}

void fst_instruction_input_init(FstInstructionInput* self) {
	self->input = NULL;
}

FstInstruction* fst_instruction_input_new(gchar* input) {
	g_return_val_if_fail(input != NULL, NULL);

	FstInstructionInput* self = g_object_new(FST_TYPE_INSTRUCTION_INPUT, NULL);
	self->input = input;
	return (FstInstruction*)self;
}
