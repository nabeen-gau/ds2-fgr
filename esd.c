#include <stdbool.h>
#include <stddef.h>

#include "common.c"

typedef packed_struct {
	int32_t a;
	int32_t dark_souls_count; // 1, 2 or 3
	int32_t dark_souls_count_repeat;
	int32_t b; // 0x54
	int32_t data_size;
	int32_t c; // 6
	int32_t d; // 0x48
	int32_t e; // 1;
	int32_t stateGroupSize; // 0x20
	int32_t stateGroupCount;
	int32_t stateSize; // 0x48
	int32_t stateCount;
	int32_t f; // 0x38
	int32_t conditionCount;
	int32_t g; // 0x18
	int32_t commandCallCount;
	int32_t h; // 0x10
	int32_t commandArgCount;
	int32_t conditionOffsetsOffset;
	int32_t conditionOffsetsCount;
	int32_t nameBlockOffset;
	int32_t nameLength;
	int32_t unkOffset1;
	int32_t i; // 0
	int32_t unkOffset2;
	int32_t j; // 0
} ESD_HEADER;

typedef packed_struct {
	int32_t a; // 1
	int32_t Unk70;
	int32_t Unk74;
	int32_t Unk78;
	int32_t Unk7C;
	int32_t b; // 0
	varint stateGroupsOffset;
	varint stateGroupCount; // =header.stateGroupCount
	varint nameOffset;
	varint nameLength; // = header.nameLength
	varint unkNull; // -1
	varint unkNull2; // -1
} StateGroupHeader;

typedef packed_struct {
	varint ID;
	varint conditionOffsetsOffset;
	varint conditionOffsetCount;
	varint entryCommandsOffset;
	varint entryCommandCount;
	varint exitCommandsOffset;
	varint exitCommandCount;
	varint whileCommandsOffset;
	varint whileCommandCount;
} StateHeader;

typedef packed_struct {
	varint argOffset;
	varint argSize;
} ArgumentHeader;

typedef packed_struct {
	int32_t CommandBank; // 1, 5, 6 or 7
	int32_t CommandID;
	varint argsOffset;
	varint argsCount;
} CommandCallHeader;

typedef packed_struct {
	varint stateOffset;
	varint passCommandsOffset;
	varint passCommandCount;
	varint conditionOffsetsOffset;
	varint conditionOffsetCount;
	varint evaluatorOffset;
	varint evaluatorLength;
} ConditionHeader;

DA(uint8_t, Arg);
DA(Arg, DAArg);

typedef struct { 
	int32_t command_bank;
	int32_t command_id;
	DArrz args_ids;
} Command;

DA(Command, DACommand);

// reference from https://gist.github.com/JKAnderson/e3fd38d4efd5f3350e13a613319b8bed
typedef enum {
	EzNumber = 0x7F, // from 0x00 to 0x7f | 0
	EzF32 = 0x80, // | 0
	EzF64 = 0x81, // | 0
	EzS32 = 0x82, // | 0
	EzUnk1 = 0x83,
	EzFuncCall1 = 0x84, // from 0x84 to 0x8b | 1-8 | 1
	EzFuncCall2 = 0x85, // from 0x84 to 0x8b | 1-8 | 2
	EzFuncCall3 = 0x86, // from 0x84 to 0x8b | 1-8 | 3
	EzFuncCall4 = 0x87, // from 0x84 to 0x8b | 1-8 | 4
	EzFuncCall5 = 0x88, // from 0x84 to 0x8b | 1-8 | 5
	EzFuncCall6 = 0x89, // from 0x84 to 0x8b | 1-8 | 6
	EzFuncCall7 = 0x8a, // from 0x84 to 0x8b | 1-8 | 7
	EzFuncCall8 = 0x8b, // from 0x84 to 0x8b | 1-8 | 8
	EzAdd = 0x8c, // | 2
	EzNeg = 0x8e, // | 1
	EzMul = 0x8f, // | 2
	EzDiv = 0x90, // | 2
	EzLt = 0x91, // | 2
	EzGt = 0x92, // | 2
	EzLte = 0x93, // | 2
	EzGte = 0x94, // | 2
	EzEq = 0x95, // | 2
	EzNeq = 0x96, // | 2
	EzAbs = 0x97, // | 1
	EzAnd = 0x98, // | 2
	EzOr = 0x99, // | 2
	EzNot = 0x9a, // | 1
	EzLt0 = 0x9b, // | 1
	EzGt0 = 0x9c, // | 1
	EzLte0 = 0x9d, // | 1
	EzGte0 = 0x9e, // | 1
	EzEq0 = 0x9f, // | 1
	EzFin = 0xa1, // | 0
	EzUnk2 = 0xa2,
	EzUnk3 = 0xa3,
	EzUnk4 = 0xa4,
	EzCstr = 0xa5, // | 0
	EzTry = 0xa6, // | 1
	EzSetReg0 = 0xa7, // from 0xa7 - 0xae | 1
	EzSetReg1 = 0xa8, // from 0xa7 - 0xae | 1
	EzSetReg2 = 0xa9, // from 0xa7 - 0xae | 1
	EzSetReg3 = 0xaa, // from 0xa7 - 0xae | 1
	EzSetReg4 = 0xab, // from 0xa7 - 0xae | 1
	EzSetReg5 = 0xac, // from 0xa7 - 0xae | 1
	EzSetReg6 = 0xad, // from 0xa7 - 0xae | 1
	EzSetReg7 = 0xae, // from 0xa7 - 0xae | 1
	EzGetReg0 = 0xaf, // from 0xaf - 0xb6 | 0
	EzGetReg1 = 0xb0, // from 0xaf - 0xb6 | 0
	EzGetReg2 = 0xb1, // from 0xaf - 0xb6 | 0
	EzGetReg3 = 0xb2, // from 0xaf - 0xb6 | 0
	EzGetReg4 = 0xb3, // from 0xaf - 0xb6 | 0
	EzGetReg5 = 0xb4, // from 0xaf - 0xb6 | 0
	EzGetReg6 = 0xb5, // from 0xaf - 0xb6 | 0
	EzGetReg7 = 0xb6, // from 0xaf - 0xb6 | 0
	EzAssert = 0xb7, // | 1
	EzGetArg = 0xb8, // get machine argument | 1
	EzGetRet = 0xb9, // | 0
	EzRetVal = 0xba, // | 0
	EzEnd = 0xff, // from 0xbb - 0xff, represents the constant 0x7fffffff (return value of machine)
} EzType;

bool ez_is_unop(EzType type) {
	return (type == EzNeg || type == EzAbs);
}

bool ez_is_binop(EzType type) {
	if (ez_is_unop(type)) return false;
	return type >= EzAdd && type <= EzOr;
}

bool ez_is_funcall(EzType type) {
	return type >= EzFuncCall1 && type <= EzFuncCall8;
}

typedef struct {
	size_t lhs_idx;
	size_t rhs_idx;
} Binop;

typedef struct {
	DArrz arg_idx;
	int id;
} FunCall;

typedef enum {
	CH_NONE = 0,
	CH_ABORT,
	CH_TRY,
} ConditionHandle;

typedef struct {
	EzType type;
	ConditionHandle cond_handle;
	union {
		int8_t number;
		float f32;
		double f64;
		int32_t s32;
		Binop binop;
		size_t arg_idx;
		FunCall fun_call;
		struct { 
			uint8_t reg_index;
			int arg_idx;
		} regist;
	} as;
} EzExprType;

DA(EzExprType, DAEzExpr);

typedef struct {
	DAEzExpr exprs;
	DArrz stack;
} Expression;

void free_funcall(FunCall *f) {
	da_free(f->arg_idx);
}

void dealloc_funcalls(Expression *e) {
	da_foreach(EzExprType, expr, &e->exprs) {
		if (ez_is_funcall(expr->type)) {
			free_funcall(&expr->as.fun_call);
		}
	}
}

void dealloc_expression(Expression *e) {
	dealloc_funcalls(e);
	da_free(e->exprs);
	da_free(e->stack);
}

typedef struct {
	uint8_t *items;
	size_t count;
} Evaluator;

size_t expr_pop_arg(Expression *e) {
	assert(e->stack.count > 0 && "stack is empty");
	size_t idx = e->stack.items[e->stack.count-1];
	e->stack.count--;
	return idx;
}

void darrz_reverse(DArrz *arr) {
	if (arr == NULL || arr->count <= 1 || arr->items == NULL) return;
	size_t l = 0;
	size_t r = arr->count - 1;
	while (l < r) {
		size_t value = arr->items[l];
		arr->items[l] = arr->items[r];
		arr->items[r] = value;
		l++;
		r--;
	}
}



void expr_pop_args(Expression *e, FunCall *f, size_t n) {
	for (size_t i=0; i<n; i++) {
		da_append(&f->arg_idx, expr_pop_arg(e));
	}
	darrz_reverse(&f->arg_idx);
}

void *ez_get_val(EzExprType *e) {
	size_t offset = offsetof(EzExprType, as);
	return (void *)e + offset;
}

EzExprType* expr_peek(Expression *e) {
	return &e->exprs.items[e->stack.items[e->stack.count-1]];
}

#define ez_get(type, expr) *(type *)ez_get_val((expr));

void parse_expression(const Evaluator *e, Expression *expression) {
	for (size_t i=0; i<e->count; i++) {
		const unsigned char b = e->items[i];
		EzType type = (EzType)b;
		EzExprType expr = {0};
		if (b <= EzNumber) {
			expr.type = EzNumber;
			expr.as.number = (int8_t)(b - 64);
		} else if (ez_is_unop(type)) {
			expr.type = type;
			expr.as.arg_idx = expr_pop_arg(expression);
		} else if (ez_is_binop(type)) {
			expr.type = type;
			expr.as.binop.rhs_idx = expr_pop_arg(expression);
			expr.as.binop.lhs_idx = expr_pop_arg(expression);
		} else if (b >= EzSetReg0 && b <= EzSetReg7) {
			expr.type = type;
			expr.as.regist.reg_index = (uint8_t)(b - EzSetReg0);
			expr.as.regist.arg_idx = expr_pop_arg(expression);
		} else if (b >= EzGetReg0 && b <= EzGetReg7) {
			expr.type = type;
			expr.as.regist.reg_index = (uint8_t)(b - EzGetReg0);
			expr.as.regist.arg_idx = -1;
		} else {
			expr.type = type;
			size_t n = 0;
			switch (type) {
				case EzF32: {
					assert(i < e->count - sizeof(float));
					float val = *(float *)(e->items+i+1);
					i += sizeof(float);
					expr.as.f32 = val;
				} break;
				case EzF64: {
					assert(i < e->count - sizeof(double));
					double val = *(double *)(e->items+i+1);
					i += sizeof(double);
					expr.as.f64 = val;
				} break;
				case EzS32: {
					assert(i < e->count - sizeof(int32_t));
					int32_t val = *(int32_t *)(e->items+i+1);
					i += sizeof(int32_t);
					expr.as.s32 = val;
			    } break;
				case EzAssert: {
				    EzExprType *expn = expr_peek(expression);
					expn->cond_handle = CH_ABORT;
					continue;
				} break;
				case EzTry: {
				    EzExprType *expn = expr_peek(expression);
					expn->cond_handle = CH_TRY;
					continue;
			    } break;
				case EzFuncCall8: n++;
				case EzFuncCall7: n++;
				case EzFuncCall6: n++;
				case EzFuncCall5: n++;
				case EzFuncCall4: n++;
				case EzFuncCall3: n++;
				case EzFuncCall2: n++;
				case EzFuncCall1: {
					FunCall f = {0};
					expr_pop_args(expression, &f, n);
					EzExprType *expn = &expression->exprs.items[expr_pop_arg(expression)];
					f.id = ez_get(int, expn);
					expr.as.fun_call = f;
				} break;
				case EzGetArg:
					expr.as.arg_idx = expr_pop_arg(expression);
					break;
				case EzRetVal:
				case EzGetRet:
					break;
				case EzFin:
					assert(i == e->count - 1 && "End of expression before the end of data");
					break;
				default:
					TODO("EzType not implemented");
			}
		}
		da_append(&expression->stack, expression->exprs.count);
		da_append(&expression->exprs, expr);
	}
}

void print_expr(const Expression *e, EzExprType *expr);
void print_binop(const Expression *e, EzExprType *expr, const char *sep) {
	size_t id = expr->as.binop.lhs_idx;
	printf("(");
	print_expr(e, &e->exprs.items[id]);
	printf(")");
	printf(" %s ", sep);
	id = expr->as.binop.rhs_idx;
	printf("(");
	print_expr(e, &e->exprs.items[id]);
	printf(")");
}

void print_funcall(const Expression *e, EzExprType *expr) {
	printf("func%zu_%d(", expr->as.fun_call.arg_idx.count, expr->as.fun_call.id);
	for (size_t i=0; i<expr->as.fun_call.arg_idx.count; i++) {
		if (i != 0) printf(" , ");
		EzExprType *arg = &e->exprs.items[expr->as.fun_call.arg_idx.items[i]];
		print_expr(e, arg);
	}
	printf(")");
}

void print_expr(const Expression *e, EzExprType *expr) {
	switch (expr->cond_handle) {
		case CH_NONE: break;
		case CH_TRY: printf("try "); break;
		case CH_ABORT: printf("assert "); break;
		default: UNREACHABLE("expr.cond_handle");
	}
	switch (expr->type) {
		case EzF32: printf("%f", expr->as.f32); break;
		case EzF64: printf("%f", expr->as.f64); break;
		case EzS32: printf("%d", expr->as.s32); break;
		case EzSetReg0:
		case EzSetReg1:
		case EzSetReg2:
		case EzSetReg3:
		case EzSetReg4:
		case EzSetReg5:
		case EzSetReg6:
		case EzSetReg7:
			printf("set_register%d(", expr->as.regist.reg_index);
			print_expr(e, &e->exprs.items[expr->as.regist.arg_idx]);
			printf(")");
			break;
		case EzGetReg0:
		case EzGetReg1:
		case EzGetReg2:
		case EzGetReg3:
		case EzGetReg4:
		case EzGetReg5:
		case EzGetReg6:
		case EzGetReg7:
			printf("get_register%d(", expr->as.regist.reg_index);
			printf(")");
			break;
		case EzLte: print_binop(e, expr, "<="); break;
		case EzGte: print_binop(e, expr, ">="); break;
		case EzLt: print_binop(e, expr, "<"); break;
		case EzGt: print_binop(e, expr, ">"); break;
		case EzAnd: print_binop(e, expr, "&&"); break;
		case EzOr: print_binop(e, expr, "||"); break;
		case EzAdd: print_binop(e, expr, "+"); break;
		case EzMul: 
			print_binop(e, expr, "x"); 
			break;
		case EzDiv: print_binop(e, expr, "x"); break;
		case EzGetArg: 
			printf("GetArg("); 
			print_expr(e, &e->exprs.items[expr->as.arg_idx]);
			printf(")");
			break;
		case EzFuncCall8:
		case EzFuncCall7:
		case EzFuncCall6:
		case EzFuncCall5:
		case EzFuncCall4:
		case EzFuncCall3:
		case EzFuncCall2:
		case EzFuncCall1: print_funcall(e, expr); break;
		case EzEq: print_binop(e, expr, "=="); break;
		case EzNeq: print_binop(e, expr, "!="); break;
		case EzNumber:
			printf("%d", expr->as.number);
			break;
		case EzFin:
			break;
		case EzGetRet:
			printf("CallResult()");
			break;
		case EzRetVal:
			printf("0x7fffffff");
			break;
		default: TODO("print expression (expression type)");
	}
}

void print_expression(const Expression *e) {
	assert(e->stack.count == 2);
	for (size_t i=0; i<e->stack.count; i++) {
		size_t id = e->stack.items[i];
		EzExprType *expr = &e->exprs.items[id];
		print_expr(e, expr);
	}
}

typedef struct {
	varint target_state_offset;
	int target_state_id;
	DArrz pass_commands_ids;
	DArrz sub_conditions_ids;
	Evaluator evaluator;
} Condition;

DA(Condition, DACondition);
DA(Condition *, DAConditionRef);

typedef struct {
	varint id;
	DArrz conditions_id;
	DArrz entry_commands_ids;
	DArrz exit_commands_ids;
	DArrz while_commands_ids;
} State;

DA(State, DAState);
typedef struct {
	varint id;
	DArrz states_ids;
} StateGroup;

DA(StateGroup, DAStateGroup);

typedef struct {
	wchar_t *name;
	size_t nameLength;

	int32_t Unk70;
	int32_t Unk74;
	int32_t Unk78;
	int32_t Unk7C;

	DAStateGroup state_groups;
	DAState states;
	DACondition conditions;
	DACommand commands;
	DAArg args;

	Dictlldz dict_cond; // [varint (offset), size_t (index)]
	Dictlldz dict_state; // [varint (offset), size_t (index)]
} ESD;

void print_condition(const Condition *c, Expression *e) {
	parse_expression(&c->evaluator, e);
	print_expression(e);
	dealloc_funcalls(e);
	e->stack.count = 0;
	e->exprs.count = 0;
}

void print_command(ESD *esd, const Command *c) {
	printf("        fun_call_%d(", c->command_id);
	size_t i=0;
	Expression e = {0};
	da_foreach(size_t, aid, &c->args_ids) {
		if (i!=0) printf(", ");
		Evaluator *arg = (Evaluator *)&esd->args.items[*aid];
		parse_expression(arg, &e);
		print_expression(&e);
		dealloc_funcalls(&e);
		e.stack.count = 0;
		e.exprs.count = 0;
		i++;
	}
	da_free(e.exprs);
	da_free(e.stack);
	printf(")\n");
}

void print_state(ESD *esd, State *state) {
	printf("    State_%lld:\n", state->id);
	da_foreach(size_t, ci, &state->entry_commands_ids) {
		Command *command = &esd->commands.items[*ci];
		print_command(esd, command);
	}
	Expression e = {0};
	da_foreach(size_t, cid, &state->conditions_id) {
		printf("        if (");
		const Condition *c = &esd->conditions.items[*cid];
		print_condition(c, &e);
		printf(")");
		if (c->target_state_offset > 0) {
			State *ts = &esd->states.items[c->target_state_id];
			printf(" goto State %lld\n", ts->id);
		}
		da_foreach(size_t, pcid, &c->pass_commands_ids) {
			const Command *pc = &esd->commands.items[*pcid];
			assert (pc->command_id == -1);
			printf(" return ");
			Expression e = {0};
			size_t i=0;
			da_foreach(size_t, pcai, &pc->args_ids) {
				if (i!=0) printf(", ");
				Evaluator *arg = (Evaluator *)&esd->args.items[*pcai];
				parse_expression(arg, &e);
				print_expression(&e);
				dealloc_funcalls(&e);
				e.stack.count = 0;
				e.exprs.count = 0;
				i++;
			}
			da_free(e.exprs);
			da_free(e.stack);
			printf("\n");
		}
		da_foreach(size_t, scid, &c->sub_conditions_ids) {
			const Condition *sc = &esd->conditions.items[*scid];
			if (sc->target_state_offset > 0) {
				State *ts = &esd->states.items[sc->target_state_id];
				printf(" goto State %lld\n", ts->id);
			}
		}
	}
	// if (state->conditions_id.count == 0) printf("\n");
	da_free(e.exprs);
	da_free(e.stack);
	// dealloc_expression(&e);
}

void print_state_group(ESD *esd, StateGroup *sg) {
	printf("StateGroup_%lld {\n", sg->id);
	da_foreach(size_t, id, &sg->states_ids) {
		State *state = &esd->states.items[*id];
		print_state(esd, state);
	}
	printf("}\n\n");
}

void free_condition(Condition *condition) {
	da_free(condition->pass_commands_ids);
	da_free(condition->sub_conditions_ids);
}

void free_commands(DACommand *commands) {
	da_foreach(Command, command, commands) {
		da_free(command->args_ids);
	}
	da_free(*commands);
}

void free_esd(ESD *esd) {
	da_foreach(State, state, &esd->states) {
		da_free(state->conditions_id);
		da_free(state->entry_commands_ids);
		da_free(state->exit_commands_ids);
		da_free(state->while_commands_ids);
	}
	da_free(esd->states);
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_free(sg->states_ids);
	}
	da_foreach(Condition, cond, &esd->conditions) {
		free_condition(cond);
	}
	da_free(esd->state_groups);
	ht_free(&esd->dict_cond);
	free_commands(&esd->commands);
	da_foreach(Arg, arg, &esd->args) {
		da_free(*arg);
	}
	da_free(esd->args);
	ht_free(&esd->dict_state);
}

const char *ESD_MAGIC = "fsSL";
#define ESD_MAGIC_LENGTH  4

#define STATE_SIZE 0x48

void test_all_expressions(ESD *esd) {
	Expression e = {0};
	da_foreach(Condition, c, &esd->conditions) {
		parse_expression(&c->evaluator, &e);
		print_expression(&e);
		dealloc_funcalls(&e);
		e.stack.count = 0;
		e.exprs.count = 0;
	}
	da_free(e.exprs);
	da_free(e.stack);
	// dealloc_expression(&e);
}

typedef packed_struct {
    varint id;
    varint offset;
    varint count;
    varint offset_dup;
} StateGroupFile;

typedef packed_struct {
	char magic[4];
	ESD_HEADER header;
	StateGroupHeader data;
	StateGroupFile *sg;
	StateHeader *sh;
	ConditionHeader *ch;
	CommandCallHeader *cch;
	ArgumentHeader *arg;
	varint *condition_offsets;
	uint8_t *binary;
} ESDFile;

ESDFile read_esd_file(String_View sv) {
	ESDFile esd = {0};
	size_t length = offsetof(ESDFile, sg);
	memcpy(&esd, sv.data, length);
	sv.data += length;
	esd.sg = (void *)sv.data;
	sv.data += sizeof(StateGroupFile)*esd.header.stateGroupCount;
	esd.sh = (void *)sv.data;
	sv.data += sizeof(StateHeader)*esd.header.stateCount;
	esd.ch = (void *)sv.data;
	sv.data += sizeof(ConditionHeader)*esd.header.conditionCount;
	esd.cch = (void *)sv.data;
	sv.data += sizeof(CommandCallHeader)*esd.header.commandCallCount;
	esd.arg = (void *)sv.data;
	sv.data += sizeof(ArgumentHeader)*esd.header.commandArgCount;
	esd.condition_offsets = (void *)sv.data;
	sv.data += sizeof(varint)*esd.header.conditionOffsetsCount;
	esd.binary = (void *)sv.data;
	return esd;
}

static inline void *get_from_offset(ESDFile *esd, size_t offset) {
	size_t header_size = sizeof(StateGroupHeader);
	return (void *)((char *)esd->sg - header_size + offset);
}

#define esd_file_get(esd, type, offset_at) (type *)get_from_offset((esd), (offset_at))

Arg read_arg(ESDFile *esd_file, size_t offset) {
	ArgumentHeader *header = esd_file_get(esd_file, ArgumentHeader, offset);
	Arg arg = {0};
	da_init(&arg, header->argSize);
	for (int i=0; i<header->argSize; i++) {
		da_append(&arg, *esd_file_get(esd_file, uint8_t, header->argOffset+i));
	}
	return arg;
}

Command read_comm(ESD *esd, ESDFile *esd_file, size_t offset) {
	CommandCallHeader *header = esd_file_get(esd_file, CommandCallHeader, offset);
	Command comm = {0};
	comm.command_bank = header->CommandBank;
	comm.command_id = header->CommandID;
	da_init(&comm.args_ids, header->argsCount);
	for (int i=0; i<header->argsCount; i++) {
		assert(header->argsOffset > 0);
		size_t arg_offset = header->argsOffset + i*sizeof(ArgumentHeader);
		Arg arg = read_arg(esd_file, arg_offset);
		da_append(&comm.args_ids, esd->args.count);
		da_append(&esd->args, arg);
	}
	return comm;
}

// returns the condition id of the saved condition if it already exists
// creates a new condition if it does not exist
size_t check_and_read_cond(ESD *esd, ESDFile *esd_file, size_t cond_offset);

Condition read_cond(ESD *esd, ESDFile *esd_file, size_t offset) {
	ConditionHeader *header = esd_file_get(esd_file, ConditionHeader, offset);
	Condition cond = {0};
	cond.target_state_offset = header->stateOffset;
	cond.target_state_id = -1;
	da_init(&cond.pass_commands_ids, header->passCommandCount);
	for (int i=0; i<header->passCommandCount; i++) {
		size_t comm_offset = header->passCommandsOffset + i*sizeof(CommandCallHeader);
		Command comm = read_comm(esd, esd_file, comm_offset);
		da_append(&cond.pass_commands_ids, esd->commands.count);
		da_append(&esd->commands, comm);
	}
	da_init(&cond.sub_conditions_ids, header->conditionOffsetCount);
	for (int i=0; i<header->conditionOffsetCount; i++) {
		varint cond_offset_offset = *esd_file_get(esd_file, varint, header->conditionOffsetsOffset);
		assert(cond_offset_offset > 0);
		size_t cond_offset = (size_t)(cond_offset_offset + i*sizeof(ConditionHeader));
		size_t sub_cond_id = check_and_read_cond(esd, esd_file, cond_offset);
		da_append(&cond.sub_conditions_ids, sub_cond_id);
	}
	if (header->evaluatorOffset > 0) {
		cond.evaluator.items = get_from_offset(esd_file, header->evaluatorOffset);
		cond.evaluator.count = header->evaluatorLength;
	} else {
		cond.evaluator.items = NULL;
		cond.evaluator.count = 0;
	}
	return cond;
}

size_t check_and_read_cond(ESD *esd, ESDFile *esd_file, size_t cond_offset) {
	size_t *idx = ht_find(&esd->dict_cond, cond_offset);
	if (idx != NULL) return *idx;
	Condition cond = read_cond(esd, esd_file, cond_offset);
	size_t index = esd->conditions.count;
	*ht_put(&esd->dict_cond, cond_offset) = index;
	da_append(&esd->conditions, cond);
	return index;
}

State read_state(ESD *esd, ESDFile *esd_file, size_t offset) {
	StateHeader *header = esd_file_get(esd_file, StateHeader, offset);
	State state = {0};
	state.id = header->ID;
	da_init(&state.conditions_id, header->conditionOffsetCount);
	for (int i=0; i<header->conditionOffsetCount; i++) {
		varint cond_offset = *esd_file_get(esd_file, varint, header->conditionOffsetsOffset + i*sizeof(varint));
		assert(cond_offset > 0);
		size_t cond_id = check_and_read_cond(esd, esd_file, (size_t)cond_offset);
		da_append(&state.conditions_id, cond_id);
	}
	da_init(&state.entry_commands_ids, header->entryCommandCount);
	for (int i=0; i<header->entryCommandCount; i++) {
		size_t comm_offset = header->entryCommandsOffset + i*sizeof(CommandCallHeader);
		Command comm = read_comm(esd, esd_file, comm_offset);
		da_append(&state.entry_commands_ids, esd->commands.count);
		da_append(&esd->commands, comm);
	}
	da_init(&state.exit_commands_ids, header->exitCommandCount);
	for (int i=0; i<header->exitCommandCount; i++) {
		size_t comm_offset = header->exitCommandsOffset + i*sizeof(CommandCallHeader);
		Command comm = read_comm(esd, esd_file, comm_offset);
		da_append(&state.exit_commands_ids, esd->commands.count);
		da_append(&esd->commands, comm);
	}
	da_init(&state.while_commands_ids, header->whileCommandCount);
	for (int i=0; i<header->whileCommandCount; i++) {
		size_t comm_offset = header->whileCommandsOffset + i*sizeof(CommandCallHeader);
		Command comm = read_comm(esd, esd_file, comm_offset);
		da_append(&state.while_commands_ids, esd->commands.count);
		da_append(&esd->commands, comm);
	}
	*ht_put(&esd->dict_state, offset) = esd->states.count;
	return state;
}

StateGroup read_state_group(ESD *esd, ESDFile *esd_file, size_t i) {
	StateGroupFile *sgf = &esd_file->sg[i];
	StateGroup sg = {0};
	sg.id = sgf->id;
	assert(sgf->offset == sgf->offset_dup);
	da_init(&sg.states_ids, sgf->count);
	for (int i=0; i<sgf->count; i++) {
		size_t state_offset = sgf->offset + esd_file->header.stateSize*i;
		State s = read_state(esd, esd_file, state_offset);
		da_append(&sg.states_ids, esd->states.count);
		da_append(&esd->states, s);
	}
	return sg;
}

void read_esd_info(const ESDFile *esd_file, ESD *esd) {
	size_t size = sizeof(StateGroupHeader);
	if (esd_file->data.nameOffset > 0) {
		esd->name = (void *)((char *)esd_file->sg - size + esd_file->data.nameOffset);
		esd->nameLength = esd_file->data.nameLength;
	}
	esd->Unk70 = esd_file->data.Unk70;
	esd->Unk74 = esd_file->data.Unk74;
	esd->Unk78 = esd_file->data.Unk78;
	esd->Unk7C = esd_file->data.Unk7C;
}

ESD read_esd(String_View sv) {
	ESD esd = {0};
	ESDFile esd_file = read_esd_file(sv);
	read_esd_info(&esd_file, &esd);
	da_init(&esd.state_groups, esd_file.header.stateGroupCount);
	da_init(&esd.states, esd_file.header.stateCount);
	da_init(&esd.conditions, esd_file.header.conditionCount);
	da_init(&esd.commands, esd_file.header.commandCallCount);
	da_init(&esd.args, esd_file.header.commandArgCount);
	for (int i=0; i<esd_file.header.stateGroupCount; i++) {
		StateGroup sg = read_state_group(&esd, &esd_file, i);
		da_append(&esd.state_groups, sg);
	}
	// populate the target states that is not initialized when creating the conditions
	for (size_t i=0; i<esd.conditions.count; i++) {
		Condition *cond = &esd.conditions.items[i];
		if (cond->target_state_offset <= 0) continue;
		size_t *state_id = ht_find(&esd.dict_state, cond->target_state_offset);
		assert(state_id);
		cond->target_state_id = *state_id;
	}
	return esd;
}

void write_esd_header(String_Builder *sb, const ESD *esd) {
	sb_append_cstr(sb, ESD_MAGIC);
	sb_write_int32t(sb, 1);
	sb_write_int32t(sb, 2);
	sb_write_int32t(sb, 2);
	sb_write_int32t(sb, 0x54);
	sb_reserve(sb, sizeof(int32_t)); // data_size
	sb_write_int32t(sb, 6);
	sb_write_int32t(sb, 0x48);
	sb_write_int32t(sb, 1);
	sb_write_int32t(sb, 0x20);
	sb_write_int32t(sb, esd->state_groups.count);
	sb_write_int32t(sb, STATE_SIZE);
	sb_reserve(sb, sizeof(int32_t)); // state_count-x
	sb_write_int32t(sb, 0x38);
	sb_write_int32t(sb, esd->conditions.count);
	sb_write_int32t(sb, 0x18);
	sb_write_int32t(sb, esd->commands.count);
	sb_write_int32t(sb, 0x10);
	sb_write_int32t(sb, esd->args.count);
	sb_reserve(sb, sizeof(int32_t)); // conditionOffsetsOffset
	sb_reserve(sb, sizeof(int32_t)); // conditionOffsetCount
	sb_reserve(sb, sizeof(int32_t)); // nameBlockOffset
	sb_reserve(sb, sizeof(int32_t)); // nameLength
	sb_reserve(sb, sizeof(int32_t)); // unkOffset1
	sb_write_int32t(sb, 0);
	sb_reserve(sb, sizeof(int32_t)); // unkOffset2
	sb_write_int32t(sb, 0);
}

void write_state_group_header(String_Builder *sb, const ESD *esd) {
	sb_write_int32t(sb, 1);
	sb_write_int32t(sb, esd->Unk70);
	sb_write_int32t(sb, esd->Unk74);
	sb_write_int32t(sb, esd->Unk78);
	sb_write_int32t(sb, esd->Unk7C);
	sb_write_int32t(sb, 0);
	sb_reserve(sb, sizeof(varint)); // stateGroupsOffset-x
	sb_write_varint(sb, esd->state_groups.count);
	sb_reserve(sb, sizeof(varint)); // nameOffset
	sb_reserve(sb, sizeof(varint)); // nameLength
	sb_write_varint(sb, -1);
	sb_write_varint(sb, -1);
}

#define sb_offset(sb) (sb)->count - sizeof(ESD_HEADER) - ESD_MAGIC_LENGTH
#define sb_base(sb) (sb)->items + ESD_MAGIC_LENGTH + sizeof(ESD_HEADER)

void write_state(String_Builder *sb, const State *s) {
	sb_write_varint(sb, s->id);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // conditionOffsetsOffset
	sb_write_varint(sb, s->conditions_id.count);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // entryCommandsOffset
	sb_write_varint(sb, s->entry_commands_ids.count);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // exitCommandsOffset
	sb_write_varint(sb, s->exit_commands_ids.count);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // whileCommandsOffset
	sb_write_varint(sb, s->while_commands_ids.count);
}

void write_comm(String_Builder *sb, const Command *comm) {
	sb_write_int32t(sb, comm->command_bank);
	sb_write_int32t(sb, comm->command_id);
	sb_reserve(sb, sizeof(varint)); // argsOffset
	sb_write_varint(sb, comm->args_ids.count);
}

void write_cond(String_Builder *sb, const Condition *cond, varint state_offset) {
	// const Condition *cond = &esd->conditions.items[i];
	// sb_reserve(sb, sizeof(varint)); // stateOffset (target)
	sb_write_varint(sb, state_offset);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // passCommandsOffset
	// sb_reserve(sb, sizeof(varint)); // passCommandsCount
	sb_write_varint(sb, cond->pass_commands_ids.count);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // conditionOffsetsOffset
	// sb_reserve(sb, sizeof(varint)); // conditionOffsetsCount
	sb_write_varint(sb, cond->sub_conditions_ids.count);
	sb_write_varint(sb, -1); // sb_reserve(sb, sizeof(varint)); // evaluatorOffset
	// sb_reserve(sb, sizeof(varint)); // evaluatorLength
	sb_write_varint(sb, cond->evaluator.count);
}

bool da_containsz(const DArrz *arr, size_t item) {
	da_foreach(size_t, i, arr) {
		if (*i == item) return true;
	}
	return false;
}

// if there is assert (condition) 0x41 and 0xa1 is added to the beginning of the state
// except if the target state has already been added
void write_sub_cond_evaluator(String_Builder *sb, ESD *esd, State *s, DArrz *added_states, Dictzuzu *evaluator_offsets) {
	da_foreach(size_t, cid, &s->conditions_id) {
		Condition *assert_cond = &esd->conditions.items[*cid];
		if (assert_cond->evaluator.count == 4) {
			if (assert_cond->evaluator.items[0] == EzGetRet
					&& (assert_cond->evaluator.items[1] == EzRetVal || assert_cond->evaluator.items[1] <= EzNumber)
					&& (assert_cond->evaluator.items[2] == EzNeq || assert_cond->evaluator.items[2] == EzEq)
					&& assert_cond->evaluator.items[3] == EzFin
					&& assert_cond->target_state_id == -1) {
				// this writing is necessary to create true copy of esd for ds2 unless touched by other mods
				assert(assert_cond->sub_conditions_ids.count == 1);
				Condition *scond = &esd->conditions.items[assert_cond->sub_conditions_ids.items[0]];
				if (da_containsz(added_states, scond->target_state_id)) continue;
				size_t *offs = ht_find(evaluator_offsets, assert_cond->sub_conditions_ids.items[0]);
				if (offs) continue;
				*ht_put(evaluator_offsets, assert_cond->sub_conditions_ids.items[0]) = sb_offset(sb);
				sb_append_buf(sb, scond->evaluator.items, scond->evaluator.count);
				da_append(added_states, scond->target_state_id);
			}
		}
	}
}

void write_args(String_Builder *sb, ESD *esd, Dictzuzu *end_args_offsets, Command *comm) {
	for (size_t i=0; i<comm->args_ids.count; i++) {
		size_t aid = comm->args_ids.items[i];
		Arg *arg = &esd->args.items[aid];
		if (ht_find(end_args_offsets, aid) != NULL) continue;
		*ht_put(end_args_offsets, aid) = sb_offset(sb);
		sb_append_buf(sb, arg->items, arg->count);
	}
}

void write_entry_commands_args(String_Builder *sb, ESD *esd, Dictzuzu *end_args_offsets, State *s, DArrz *added_states, Dictzuzu *evaluator_offsets) {
	write_sub_cond_evaluator(sb, esd, s, added_states, evaluator_offsets);
	da_foreach(size_t, ecid, &s->entry_commands_ids) {
		Command *comm = &esd->commands.items[*ecid];
		write_args(sb, esd, end_args_offsets, comm);
	}
}

void write_pass_commands_args(String_Builder *sb, ESD *esd, Dictzuzu *end_args_offsets, State *s) {
	da_foreach(size_t, cid, &s->conditions_id) {
		Condition *cond = &esd->conditions.items[*cid];
		da_foreach(size_t, comid, &cond->pass_commands_ids) {
			Command *comm = &esd->commands.items[*comid];
			write_args(sb, esd, end_args_offsets, comm);
		}
	}
}

void write_esd(String_Builder *sb, ESD *esd, const char *name) {
	write_esd_header(sb, esd);
	write_state_group_header(sb, esd);
	void *dst =  sb_base(sb) + offsetof(StateGroupHeader, stateGroupsOffset);
	size_t size = sizeof(varint);
	size_t src = sb_offset(sb);
	memcpy(dst, &src, size);

	// write state groups
	for (size_t i=0; i<esd->state_groups.count; i++) {
		const StateGroup *sg = &esd->state_groups.items[i];
		sb_write_varint(sb, sg->id);
		sb_reserve(sb, sizeof(varint)); // stateOffset
		sb_write_varint(sb, sg->states_ids.count);
		sb_reserve(sb, sizeof(varint)); // stateOffsetDup
	}

	// write states
	size_t state_count = 0;
	size_t state_zero_id = 0;
	DArrz state_offsets = {0};
	DAPair dup_states = {0};
	for (size_t i=0; i<esd->states.count; i++) {
		const State *s = &esd->states.items[i];
		if (s->id == 0) {
			state_zero_id = i;
		}
		da_append(&state_offsets, sb_offset(sb));
		write_state(sb, s);
		state_count++;
		if (i == 0 || s->id == 0) continue;
		if (i < esd->states.count - 1) {
			if (esd->states.items[i+1].id != 0) continue;
		}
		s = &esd->states.items[state_zero_id];
		Pair p = {.from = state_offsets.items[state_zero_id], .to = sb_offset(sb)};
		da_append(&dup_states, p);
		write_state(sb, s);
		state_count++;
	}

	// write state count to header
	dst =  sb->items + ESD_MAGIC_LENGTH + offsetof(ESD_HEADER, stateCount);
	size = sizeof(int32_t);
	src = state_count;
	memcpy(dst, &src, size);

	// back patch the state offsets
	for (size_t i=0; i<esd->state_groups.count; i++) {
		const StateGroup *sg = &esd->state_groups.items[i];
		if (sg->states_ids.count == 0) continue;
		size_t idx = sg->states_ids.items[0];
		assert(idx < state_offsets.count);
		size_t offset = state_offsets.items[idx];

		dst =  sb_base(sb) + sizeof(StateGroupHeader) + offsetof(StateGroupFile, offset) + i*sizeof(StateGroupFile);
		size = sizeof(varint);
		src = offset;
		memcpy(dst, &src, size);
		dst =  sb_base(sb) + sizeof(StateGroupHeader) + offsetof(StateGroupFile, offset_dup) + i*sizeof(StateGroupFile);
		memcpy(dst, &src, size);
	}

	// write conditions
	size_t condition_count = 0;
	ht_reset(&esd->dict_cond);
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				size_t *found = ht_find(&esd->dict_cond, *cid);
				if (found == NULL) {
					*ht_put(&esd->dict_cond, *cid) = sb_offset(sb);
					if (cond->target_state_id >= 0) {
						write_cond(sb, cond, state_offsets.items[cond->target_state_id]);
					} else {
						write_cond(sb, cond, -1);
					}
					condition_count++;
				}
			}
		}
	}
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				da_foreach(size_t, scid, &cond->sub_conditions_ids) {
					Condition *scond = &esd->conditions.items[*scid];
					size_t *found = ht_find(&esd->dict_cond, *scid);
					if (found == NULL) {
						*ht_put(&esd->dict_cond, *scid) = sb_offset(sb);
						if (scond->target_state_id >= 0) {
							write_cond(sb, scond, state_offsets.items[scond->target_state_id]);
						} else {
							write_cond(sb, scond, -1);
						}
						condition_count++;
					}
				}
			}
		}
	}
	assert(condition_count == esd->conditions.count);

	// write commands
	size_t commands_count = 0;
	Dictzuzu entry_commands_offsets = {0};
	Dictzuzu exit_commands_offsets = {0};
	Dictzuzu while_commands_offsets = {0};
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, ecid, &s->entry_commands_ids) {
				Command *comm = &esd->commands.items[*ecid];
				*ht_put(&entry_commands_offsets, *ecid) = sb_offset(sb);
				write_comm(sb, comm);
				commands_count++;
			}
			da_foreach(size_t, ecid, &s->exit_commands_ids) {
				Command *comm = &esd->commands.items[*ecid];
				*ht_put(&exit_commands_offsets, *ecid) = sb_offset(sb);
				write_comm(sb, comm);
				commands_count++;
			}
			da_foreach(size_t, ecid, &s->while_commands_ids) {
				Command *comm = &esd->commands.items[*ecid];
				*ht_put(&while_commands_offsets, *ecid) = sb_offset(sb);
				write_comm(sb, comm);
				commands_count++;
			}
		}
	}
	Dictzuzu pass_commands_offsets = {0};
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				da_foreach(size_t, pcid, &cond->pass_commands_ids) {
					Command *comm = &esd->commands.items[*pcid];
					*ht_put(&pass_commands_offsets, *pcid) = sb_offset(sb);
					write_comm(sb, comm);
					commands_count++;
				}
			}
		}
	}
	assert(commands_count == esd->commands.count);

	// back patch commands offsets
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			size_t state_offset = state_offsets.items[*sid];
			dst =  sb_base(sb) + state_offset + offsetof(StateHeader, entryCommandsOffset);
			size = sizeof(varint);
			size_t count = 0;
			varint first_offset = -1;
			da_foreach(size_t, ecid, &s->entry_commands_ids) {
				if (count == 0) {
					first_offset = *ht_find(&entry_commands_offsets, *ecid);
				}
				count++;
				if (count == s->entry_commands_ids.count && s->entry_commands_ids.count > 0) {
					// if there are multiple entry commands for the same state id of a state group,
					// the last entry command offset is set to the first entry command offset
					assert(first_offset > 0);
					memcpy(dst, &first_offset, size);
				} else {
					memcpy(dst, ht_find(&entry_commands_offsets, *ecid), size);
				}
			}
			dst =  sb_base(sb) + state_offset + offsetof(StateHeader, exitCommandsOffset);
			da_foreach(size_t, ecid, &s->exit_commands_ids) {
				TODO("Not Implemented");
				memcpy(dst, ht_find(&exit_commands_offsets, *ecid), size);
			}
			dst =  sb_base(sb) + state_offset + offsetof(StateHeader, whileCommandsOffset);
			da_foreach(size_t, ecid, &s->while_commands_ids) {
				TODO("Not Implemented");
				memcpy(dst, ht_find(&while_commands_offsets, *ecid), size);
			}
		}
	}
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				size_t *cond_offset = ht_find(&esd->dict_cond, *cid);
				assert(cond_offset);
				dst =  sb_base(sb) + (*cond_offset) + offsetof(ConditionHeader, passCommandsOffset);
				size_t count = 0;
				varint first_offset = -1;
				da_foreach(size_t, pcid, &cond->pass_commands_ids) {
					if (count == 0) {
						first_offset = *ht_find(&pass_commands_offsets, *pcid);
					}
					count++;
					if (count == cond->pass_commands_ids.count && cond->pass_commands_ids.count > 0) {
						// if there are multiple entry commands for the same state id of a state group,
						// the last entry command offset is set to the first entry command offset
						assert(first_offset > 0);
						memcpy(dst, &first_offset, size);
					} else {
						memcpy(dst, ht_find(&pass_commands_offsets, *pcid), size);
					}
				}
			}
		}
	}

	// write args
	size_t args_count = 0;
	Dictzuzu args_offsets = {0};
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, ecid, &s->entry_commands_ids) {
				Command *comm = &esd->commands.items[*ecid];
				da_foreach(size_t, aid, &comm->args_ids) {
					Arg *arg = &esd->args.items[*aid];
					*ht_put(&args_offsets, *aid) = sb_offset(sb);
					sb_reserve(sb, sizeof(varint)); // argOffset
					sb_write_varint(sb, arg->count);
					args_count++;
				}
			}
			da_foreach(size_t, ecid, &s->exit_commands_ids) {
				Command *comm = &esd->commands.items[*ecid];
				da_foreach(size_t, aid, &comm->args_ids) {
					Arg *arg = &esd->args.items[*aid];
					*ht_put(&args_offsets, *aid) = sb_offset(sb);
					sb_reserve(sb, sizeof(varint)); // argOffset
					sb_write_varint(sb, arg->count);
					args_count++;
				}
			}
			da_foreach(size_t, ecid, &s->while_commands_ids) {
				Command *comm = &esd->commands.items[*ecid];
				da_foreach(size_t, aid, &comm->args_ids) {
					Arg *arg = &esd->args.items[*aid];
					*ht_put(&args_offsets, *aid) = sb_offset(sb);
					sb_reserve(sb, sizeof(varint)); // argOffset
					sb_write_varint(sb, arg->count);
					args_count++;
				}
			}
		}
	}
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				da_foreach(size_t, pcid, &cond->pass_commands_ids) {
					Command *comm = &esd->commands.items[*pcid];
					da_foreach(size_t, aid, &comm->args_ids) {
						Arg *arg = &esd->args.items[*aid];
						*ht_put(&args_offsets, *aid) = sb_offset(sb);
						sb_reserve(sb, sizeof(varint)); // argOffset
						sb_write_varint(sb, arg->count);
						args_count++;
					}
				}
			}
		}
	}
	assert(args_count == esd->args.count);

	// back patch args
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, ecid, &s->entry_commands_ids) {
				size_t *offset = ht_find(&entry_commands_offsets, *ecid);
				assert(offset);
				dst =  sb_base(sb) + (*offset) + offsetof(CommandCallHeader, argsOffset);
				size = sizeof(varint);
				Command *comm = &esd->commands.items[*ecid];
				if (comm->args_ids.count == 0) {
					varint value = -1;
					memcpy(dst, &value, size);
				} else {
					size_t *arg_offset = ht_find(&args_offsets, comm->args_ids.items[0]);
					assert(arg_offset);
					memcpy(dst, arg_offset, size);
				}
			}
			da_foreach(size_t, ecid, &s->exit_commands_ids) {
				size_t *offset = ht_find(&exit_commands_offsets, *ecid);
				assert(offset);
				dst =  sb_base(sb) + (*offset) + offsetof(CommandCallHeader, argsOffset);
				size = sizeof(varint);
				Command *comm = &esd->commands.items[*ecid];
				if (comm->args_ids.count == 0) {
					varint value = -1;
					memcpy(dst, &value, size);
				} else {
					size_t *arg_offset = ht_find(&args_offsets, comm->args_ids.items[0]);
					assert(arg_offset);
					memcpy(dst, arg_offset, size);
				}
			}
			da_foreach(size_t, ecid, &s->while_commands_ids) {
				size_t *offset = ht_find(&while_commands_offsets, *ecid);
				assert(offset);
				dst =  sb_base(sb) + (*offset) + offsetof(CommandCallHeader, argsOffset);
				size = sizeof(varint);
				Command *comm = &esd->commands.items[*ecid];
				if (comm->args_ids.count == 0) {
					varint value = -1;
					memcpy(dst, &value, size);
				} else {
					size_t *arg_offset = ht_find(&args_offsets, comm->args_ids.items[0]);
					assert(arg_offset);
					memcpy(dst, arg_offset, size);
				}
			}
		}
	}
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				da_foreach(size_t, pcid, &cond->pass_commands_ids) {
					size_t *offset = ht_find(&pass_commands_offsets, *pcid);
					assert(offset);
					dst =  sb_base(sb) + (*offset) + offsetof(CommandCallHeader, argsOffset);
					size = sizeof(varint);
					Command *comm = &esd->commands.items[*pcid];
					if (comm->args_ids.count == 0) {
						varint value = -1;
						memcpy(dst, &value, size);
					} else {
						size_t *arg_offset = ht_find(&args_offsets, comm->args_ids.items[0]);
						assert(arg_offset);
						memcpy(dst, arg_offset, size);
					}
				}
			}
		}
	}

	// write condition_offsets_offsets to header
	dst =  sb->items + ESD_MAGIC_LENGTH + offsetof(ESD_HEADER, conditionOffsetsOffset);
	size = sizeof(int32_t);
	src = sb_offset(sb);
	memcpy(dst, &src, size);

	// write conditions offsets
	DictPairzu condition_offset_offsets = {0}; // key = (cond_index, state_id)
	size_t condition_offsets_offsets_count = 0;
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				size_t *offset = ht_find(&esd->dict_cond, *cid);
				assert(offset);
				Pair key = {*cid, (size_t)s->id};
				size_t *coo = ht_find(&condition_offset_offsets, key);
				if (coo == NULL) {
					*ht_put(&condition_offset_offsets, key) = sb_offset(sb);
				} else {
					*ht_put(&condition_offset_offsets, key) = *coo;
				}
				sb_write_varint(sb, *offset);
				condition_offsets_offsets_count++;
			}
		}
		if (sg->states_ids.count > 1) {
			State *fs = &esd->states.items[sg->states_ids.items[0]];
			if (fs->conditions_id.count > 0) {
				size_t *fco = ht_find(&esd->dict_cond, fs->conditions_id.items[0]);
				assert(fco);
				condition_offsets_offsets_count++;
				sb_write_varint(sb, *fco);
			}
		}
	}
	DictPairzu scond_offset_offsets = {0};
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				da_foreach(size_t, scid, &cond->sub_conditions_ids) {
					size_t *offset = ht_find(&esd->dict_cond, *scid);
					assert(offset);
					Pair key = {*scid, *cid};
					size_t *found = ht_find(&scond_offset_offsets, key);
					// if the cid and scid are same we skip those
					if (found) continue;
					*ht_put(&scond_offset_offsets, key) = sb_offset(sb);

					condition_offsets_offsets_count++;
					sb_write_varint(sb, *offset);
				}
			}
		}
	}

	// write condition_offsets_offsets_count to header
	dst =  sb->items + ESD_MAGIC_LENGTH + offsetof(ESD_HEADER, conditionOffsetsCount);
	size = sizeof(int32_t);
	src = condition_offsets_offsets_count;
	memcpy(dst, &src, size);

	// back patch condition offsets offset in state header (condition_offset_offsets)
	for (size_t i=0; i<esd->state_groups.count; i++) {
		StateGroup *sg = &esd->state_groups.items[i];
		for (size_t j=0; j<sg->states_ids.count; j++) {
			size_t state_index = sg->states_ids.items[j];
			State *s = &esd->states.items[state_index];
			size_t offset = state_offsets.items[state_index];
			dst =  sb_base(sb) + offset + offsetof(StateHeader, conditionOffsetsOffset);
			if (s->conditions_id.count > 0) {
				size_t cid = s->conditions_id.items[0];
				Pair key = {cid, s->id};
				size_t *condition_offset = ht_find(&condition_offset_offsets, key);
				assert(condition_offset);
				memcpy(dst, condition_offset, sizeof(varint));
			} else {
				varint value = -1;
				memcpy(dst, &value, sizeof(varint));
			}
		}
	}

	// back patch condition offsets offset in condition header (scond_offset_offsets)
	for (size_t i=0; i<esd->state_groups.count; i++) {
		StateGroup *sg = &esd->state_groups.items[i];
		for (size_t j=0; j<sg->states_ids.count; j++) {
			size_t state_index = sg->states_ids.items[j];
			State *s = &esd->states.items[state_index];
			for (size_t k=0; k<s->conditions_id.count; k++) {
				size_t cond_index = s->conditions_id.items[k];
				Condition *cond = &esd->conditions.items[cond_index];
				if (cond->sub_conditions_ids.count == 0) continue;
				size_t scid = cond->sub_conditions_ids.items[0];

				size_t *condition_offset = ht_find(&esd->dict_cond, cond_index);
				assert(condition_offset);

				Pair key = {scid, cond_index};
				size_t *offset = ht_find(&scond_offset_offsets, key);
				assert(offset);

				dst = sb_base(sb) + (*condition_offset) + offsetof(ConditionHeader, conditionOffsetsOffset);
				memcpy(dst, offset, sizeof(varint));
			}
		}
	}

	// write evalulator from conditions
	Dictzuzu evaluator_offsets = {0};
	Evaluator *first_eval = NULL;
	for (size_t i=0; i<esd->state_groups.count; i++) {
		StateGroup *sg = &esd->state_groups.items[i];
		for (size_t j=0; j<sg->states_ids.count; j++) {
			size_t state_index = sg->states_ids.items[j];
			State *s = &esd->states.items[state_index];
			for (size_t k=0; k<s->conditions_id.count; k++) {
				size_t cond_index = s->conditions_id.items[k];
				Condition *cond = &esd->conditions.items[cond_index];
				if (ht_find(&evaluator_offsets, cond_index) != NULL) continue;
				*ht_put(&evaluator_offsets, cond_index) = sb_offset(sb);
				sb_append_buf(sb, cond->evaluator.items, cond->evaluator.count);
				if (!first_eval) {
					first_eval = &cond->evaluator;
				}
			}

		}
	}

	// write args
	Dictzuzu end_args_offsets = {0};
	DArrz added_states = {0};
	da_foreach(StateGroup, sg, &esd->state_groups) {
		added_states.count = 0;
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			write_entry_commands_args(sb, esd, &end_args_offsets, s, &added_states, &evaluator_offsets);
		}
	}
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			write_pass_commands_args(sb, esd, &end_args_offsets, s);
		}
	}


	// back patch evaluator offsets
	for (size_t i=0; i<esd->state_groups.count; i++) {
		StateGroup *sg = &esd->state_groups.items[i];
		for (size_t j=0; j<sg->states_ids.count; j++) {
			size_t state_index = sg->states_ids.items[j];
			State *s = &esd->states.items[state_index];
			for (size_t k=0; k<s->conditions_id.count; k++) {
				size_t cond_index = s->conditions_id.items[k];
				Condition *cond = &esd->conditions.items[cond_index];
				if (cond->evaluator.count == 0) continue;
				size_t *condition_offset = ht_find(&esd->dict_cond, cond_index);
				assert(condition_offset);
				dst = sb_base(sb) + (*condition_offset) + offsetof(ConditionHeader, evaluatorOffset);
				size_t *eval_offset = ht_find(&evaluator_offsets, cond_index);
				assert(eval_offset);
				memcpy(dst, eval_offset, sizeof(varint));

				for (size_t l=0; l<cond->sub_conditions_ids.count; l++) {
					size_t scond_index = cond->sub_conditions_ids.items[l];
					Condition *scond = &esd->conditions.items[scond_index];
					if (scond->evaluator.count == 0) continue;
					size_t *scond_offset = ht_find(&esd->dict_cond, scond_index);
					assert(scond_offset);
					dst = sb_base(sb) + (*scond_offset) + offsetof(ConditionHeader, evaluatorOffset);
					size_t *eval_offset = ht_find(&evaluator_offsets, scond_index);
					assert(eval_offset);
					memcpy(dst, eval_offset, sizeof(varint));
				}
			}
		}
	}

	// back patch arg offsets
	da_foreach(StateGroup, sg, &esd->state_groups) {
		da_foreach(size_t, sid, &sg->states_ids) {
			State *s = &esd->states.items[*sid];
			da_foreach(size_t, ecid, &s->entry_commands_ids) {
				Command *comm = &esd->commands.items[*ecid];
				da_foreach(size_t, aid, &comm->args_ids) {
					size_t *ao = ht_find(&args_offsets, *aid);
					assert(ao);
					size_t *eao = ht_find(&end_args_offsets, *aid);
					assert(eao);
					dst = sb_base(sb) + (*ao) + offsetof(ArgumentHeader, argOffset);
					memcpy(dst, eao, sizeof(varint));
				}
			}
			da_foreach(size_t, cid, &s->conditions_id) {
				Condition *cond = &esd->conditions.items[*cid];
				da_foreach(size_t, pcid, &cond->pass_commands_ids) {
					Command *comm = &esd->commands.items[*pcid];
					da_foreach(size_t, aid, &comm->args_ids) {
						size_t *ao = ht_find(&args_offsets, *aid);
						assert(ao);
						size_t *eao = ht_find(&end_args_offsets, *aid);
						assert(eao);
						dst = sb_base(sb) + (*ao) + offsetof(ArgumentHeader, argOffset);
						memcpy(dst, eao, sizeof(varint));
					}
				}
			}
		}
	}

	// write name block offset
	dst =  sb->items + ESD_MAGIC_LENGTH + offsetof(ESD_HEADER, nameBlockOffset);
	size = sizeof(int32_t);
	src = sb_offset(sb);
	memcpy(dst, &src, size);

	// some mod will not write the esd names so build it yourself
	char name_buf[128] = {0};
	if (name == NULL) {
		assert(esd->name != NULL);
	} else if (esd->name == NULL) {
		assert(name != NULL);
		int len = strlen(name);
		for (int i=0; i<len; i++) {
			name_buf[2*i] = name[i];
			name_buf[2*i+1] = 0;
		}
		name_buf[2*len] = 0;
		name_buf[2*len+1] = 0;
		esd->name = (wchar_t *)name_buf;
	}
	sb_pad_align(sb, sizeof(wchar_t));

	// write name offset
	dst =  sb_base(sb) + offsetof(StateGroupHeader, nameOffset);
	size = sizeof(varint);
	src = sb_offset(sb);
	memcpy(dst, &src, size);

	// write name
	size_t name_length = sb->count;
	sb_append_utf16t(sb, esd->name);
	name_length = (sb->count - name_length)/sizeof(wchar_t);

	// write name length
	dst =  sb->items + ESD_MAGIC_LENGTH + offsetof(ESD_HEADER, nameLength);
	size = sizeof(int32_t);
	memcpy(dst, &name_length, size);
	dst =  sb_base(sb) + offsetof(StateGroupHeader, nameLength);
	memcpy(dst, &name_length, size);

	// write data_size, unkOffset1 & unkOffset2
	dst =  sb->items + ESD_MAGIC_LENGTH + offsetof(ESD_HEADER, unkOffset1);
	size = sizeof(int32_t);
	src = sb_offset(sb);
	memcpy(dst, &src, size);
	dst =  sb->items + ESD_MAGIC_LENGTH + offsetof(ESD_HEADER, unkOffset2);
	memcpy(dst, &src, size);
	dst =  sb->items + ESD_MAGIC_LENGTH + offsetof(ESD_HEADER, data_size);
	memcpy(dst, &src, size);

	sb_pad_align(sb, 0x10);

	// copy all backpatching to duplicate state
	for (size_t i=0; i<dup_states.count; i++) {
		Pair *p = &dup_states.items[i];
		char *from = sb_base(sb) + p->from;
		char *to = sb_base(sb) + p->to;
		memcpy(to, from, STATE_SIZE);
	}

	da_free(dup_states);
	da_free(state_offsets);
	ht_free(&entry_commands_offsets);
	ht_free(&exit_commands_offsets);
	ht_free(&while_commands_offsets);
	ht_free(&pass_commands_offsets);
	ht_free(&args_offsets);
	ht_free(&scond_offset_offsets);
	ht_free(&condition_offset_offsets);
	ht_free(&evaluator_offsets);
	ht_free(&end_args_offsets);
	da_free(added_states);
}

