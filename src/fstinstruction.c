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

G_DEFINE_ABSTRACT_TYPE (FstInstruction, fst_instruction, G_TYPE_OBJECT)

static void fst_instruction_class_init(FstInstructionClass* class) {
	class->do_instruction = NULL;
}

static void fst_instruction_init(__attribute__ ((unused)) FstInstruction* self) {}

gboolean fst_instruction_do_instruction(FstInstruction* self, FstStreamBundle* streams, gboolean log) {
	g_return_val_if_fail(FST_IS_INSTRUCTION(self), FALSE);

	FstInstructionClass* class = FST_INSTRUCTION_GET_CLASS(self);
	g_return_val_if_fail(class->do_instruction != NULL, FALSE);
	return class->do_instruction(self, streams, log);
}
