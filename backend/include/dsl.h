#ifndef DSL_H__
#define DSL_H__

#define _OPERATOR(_code)   node_ctor(ctx->node_allocator, OPERATOR,   {.operator_code = (_code)}, ctx->cur_line, nullptr, nullptr)
#define _IDENTIFIER(_code) node_ctor(ctx->node_allocator, IDENTIFIER, {.id_index      = (_code)}, ctx->cur_line, nullptr, nullptr)
#define _NUMBER(_numb)     node_ctor(ctx->node_allocator, NUMBER,     {.number        = (_numb)}, ctx->cur_line, nullptr, nullptr)

#endif // DSL_H__
