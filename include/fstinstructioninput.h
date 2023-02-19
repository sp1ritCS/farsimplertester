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

#ifndef __FSTINSTRUCTIONINPUT_H__
#define __FSTINSTRUCTIONINPUT_H__

#include <gio/gio.h>
#include <fstinstruction.h>

G_BEGIN_DECLS

#define FST_TYPE_INSTRUCTION_INPUT (fst_instruction_input_get_type())
G_DECLARE_FINAL_TYPE (FstInstructionInput, fst_instruction_input, FST, INSTRUCTION_INPUT, FstInstruction)

FstInstruction* fst_instruction_input_new(gchar* input);

G_END_DECLS

#endif // __FSTINSTRUCTIONINPUT_H__
