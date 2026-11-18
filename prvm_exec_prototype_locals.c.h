// prvm_exec_prototype_locals.c.h

//#define QC_MAX_LOCALS 32 // Baker: There is no max

CALLERS_ (PRVM_PrintFunctionPrototype_f)
typedef enum {
    qc_ev_void, 
    qc_ev_string, 
    qc_ev_float, 
    qc_ev_vector, 
    qc_ev_entity, 
    qc_ev_field, 
    qc_ev_function, 
    qc_ev_pointer,
    qc_ev_maybe_float,	// UNREFERENCED
    qc_ev_maybe_vector	// UNREFERENCED
} qc_datatype_e;

// Helper to map type name string for output/display
const char* QC_GetTypeName(qc_datatype_e type) {
    switch (type) {
        case qc_ev_void:         return "void";
        case qc_ev_string:       return "string";
        case qc_ev_float:        return "float";
        case qc_ev_vector:       return "vector";
        case qc_ev_entity:       return "entity";
        case qc_ev_field:        return ".field";
        case qc_ev_function:     return "function";
        case qc_ev_pointer:      return "pointer";
        case qc_ev_maybe_float:  return "maybe_float";
        case qc_ev_maybe_vector: return "maybe_vector";
        default:                 return "unknown";
    }
}



typedef struct {
    int local_index; // 0-based index among non-arg locals
    int global_offset; // Actual global register offset
    qc_datatype_e type;
} qc_local_var_t;

void AnalyzeAndPrintLocals(prvm_prog_t *prog, mfunction_t *f) {
    int total_parm_slots = 0;
    for (int i = 0; i < f->numparms; i++) {
        total_parm_slots += f->parm_size[i];
    }

    int local_slots = f->locals - total_parm_slots;
    if (local_slots <= 0) {
        Con_Printf("Local variables: None\n");
        return;
    }

    int locals_base_reg = OFS_PARM0 + total_parm_slots;

    // Allocate dynamically to support any number of locals without arbitrary limits
    qc_local_var_t *locals = (qc_local_var_t *)Z_Malloc(local_slots * sizeof(qc_local_var_t));
    int local_count = 0;

    for (int i = 0; i < local_slots; ) {
        locals[local_count].local_index = local_count;
        locals[local_count].global_offset = locals_base_reg + i;
        
        if ((local_slots - i) >= 3) {
            locals[local_count].type = qc_ev_maybe_vector;
            i += 3;
        } else {
            locals[local_count].type = qc_ev_maybe_float;
            i += 1;
        }
        local_count++;
    }

    // Scan statements to refine types...
    if (f->first_statement >= 0) {
        int firststatement = f->first_statement;
        int endstatement = prog->numstatements;
        for (int i = 0; i < prog->numfunctions; i++) {
            if (endstatement > prog->functions[i].first_statement 
                && firststatement < prog->functions[i].first_statement) {
                endstatement = prog->functions[i].first_statement;
            }
        }

        for (int i = firststatement; i < endstatement; i++) {
            mstatement_t *st = &prog->statements[i];
            
            for (int l = 0; l < local_count; l++) {
                int reg = locals[l].global_offset;
                int a = st->operand[0];
                int b = st->operand[1];
                int c = st->operand[2];

                if (a == reg || b == reg || c == reg) {
                    if (st->op >= OP_LOAD_F && st->op <= OP_STOREP_FNC) {
                        locals[l].type = qc_ev_entity;
                    } else {
                        if (locals[l].type == qc_ev_maybe_vector) {
                            locals[l].type = qc_ev_vector;
                        } else if (locals[l].type == qc_ev_maybe_float) {
                            locals[l].type = qc_ev_float;
                        }
                    }
                }
            }
            if (st->op == OP_DONE) break;
        }
    }

    Con_Printf("Local Variables (%d total slots):\n", local_slots);
    for (int l = 0; l < local_count; l++) {
        Con_Printf("  %s local_%d (global reg %d)\n", 
            QC_GetTypeName(locals[l].type), 
            locals[l].local_index, 
            locals[l].global_offset);
    }

    Z_Free(locals);
}



#define QC_MAX_PARMS 8






typedef struct {
    char name[64];
    qc_datatype_e return_type;
    int num_parms;
    qc_datatype_e parm_types[QC_MAX_PARMS];
} qc_function_prototype_t;


qc_function_prototype_t GenerateFunctionPrototype(prvm_prog_t *prog, mfunction_t *f) {
    qc_function_prototype_t proto;
    memset(&proto, 0, sizeof(proto));

    // 1. Get function name
    const char *fname = PRVM_GetString(prog, f->s_name);
    if (fname && fname[0]) {
        dp_strlcpy(proto.name, fname, sizeof(proto.name));
    } else {
        dp_strlcpy(proto.name, "unknown", sizeof(proto.name));
    }

    proto.num_parms = f->numparms;
    if (proto.num_parms > QC_MAX_PARMS) proto.num_parms = QC_MAX_PARMS;

    // 2. Initialize parameter types using qc_ev_maybe_vector for size 3, qc_ev_maybe_float for size 1
    for (int i = 0; i < proto.num_parms; i++) {
        int size = f->parm_size[i];
        if (size == 3) {
            proto.parm_types[i] = qc_ev_maybe_vector;
        } else {
            proto.parm_types[i] = qc_ev_maybe_float;
        }
    }

    proto.return_type = qc_ev_void;

    if (f->first_statement < 0) {
        // Print prototype for built-ins before returning
        char parm_str[256] = "";
        for (int i = 0; i < proto.num_parms; i++) {
            dp_strlcat(parm_str, QC_GetTypeName(proto.parm_types[i]), sizeof(parm_str));
            if (i < proto.num_parms - 1) dp_strlcat(parm_str, ", ", sizeof(parm_str));
        }
        Con_Printf("%s (%s) %s;\n", QC_GetTypeName(proto.return_type), parm_str, proto.name);
        return proto;
    }

    int firststatement = f->first_statement;
    int endstatement = prog->numstatements;
    for (int i = 0; i < prog->numfunctions; i++) {
        if (endstatement > prog->functions[i].first_statement 
            && firststatement < prog->functions[i].first_statement) {
            endstatement = prog->functions[i].first_statement;
        }
    }

    qbool parm_resolved[QC_MAX_PARMS] = {false};

    for (int i = firststatement; i < endstatement; i++) {
        mstatement_t *st = &prog->statements[i];
        
        if ((st->op == OP_RETURN || st->op == OP_DONE) && st->operand[0] != 0) {
            if (proto.return_type == qc_ev_void) {
                proto.return_type = qc_ev_float; 
            }
        }

        for (int p = 0; p < proto.num_parms; p++) {
            int parm_base_reg = OFS_PARM0 + (p * 3);
            int a = st->operand[0];
            int b = st->operand[1];
            int c = st->operand[2];

            if (a == parm_base_reg || b == parm_base_reg || c == parm_base_reg) {
                if (st->op >= OP_LOAD_F && st->op <= OP_STOREP_FNC) {
                    proto.parm_types[p] = qc_ev_entity;
                    parm_resolved[p] = true;
                } else if (!parm_resolved[p]) {
                    if (proto.parm_types[p] == qc_ev_maybe_vector) {
                        proto.parm_types[p] = qc_ev_vector;
                    } else if (proto.parm_types[p] == qc_ev_maybe_float) {
                        proto.parm_types[p] = qc_ev_float;
                    }
                }
            }
        }

        if (st->op == OP_DONE) break;
    }

    // Build parameter string and print prototype before returning
    char parm_str[256] = "";
    for (int i = 0; i < proto.num_parms; i++) {
        dp_strlcat(parm_str, QC_GetTypeName(proto.parm_types[i]), sizeof(parm_str));
        if (i < proto.num_parms - 1) dp_strlcat(parm_str, ", ", sizeof(parm_str));
    }
    Con_Printf("%s (%s) %s;\n", QC_GetTypeName(proto.return_type), parm_str, proto.name);

    return proto;
}



void PRVM_PrintFunctionPrototype2 (prvm_prog_t *prog, mfunction_t *f, ccs *s_function_name)
{
	GenerateFunctionPrototype (prog, f);
	AnalyzeAndPrintLocals (prog, f);
	
}