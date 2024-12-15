#ifndef FRONTEND_H__
#define FRONTEND_H__

//———————————————————————————————————————————————————————————————————//

#include <stdio.h>
#include "lang.h"

//———————————————————————————————————————————————————————————————————//

const char* const DefaultInput  = "exmpl_src.txt";
const char* const DefaultOutput = "exmpl_res.txt";

//———————————————————————————————————————————————————————————————————//

lang_status_t tokenize        (lang_ctx_t* ctx);
lang_status_t syntax_analysis (lang_ctx_t* ctx);

//———————————————————————————————————————————————————————————————————//

#endif // FRONTEND_H__
