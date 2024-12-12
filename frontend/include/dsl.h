#ifndef DSL_H__
#define DSL_H__

#define _OPERATOR(_code)   node_ctor(ctx->node_allocator, OPERATOR,   {.operator_code = (_code)}, nullptr, nullptr)
#define _IDENTIFIER(_code) node_ctor(ctx->node_allocator, IDENTIFIER, {.id_index      = (_code)}, nullptr, nullptr)
#define _NUMBER(_numb)     node_ctor(ctx->node_allocator, NUMBER,     {.number        = (_numb)}, nullptr, nullptr)

#endif // DSL_H__
