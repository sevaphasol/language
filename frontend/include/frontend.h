#ifndef FRONTEND_H__
#define FRONTEND_H__

//———————————————————————————————————————————————————————————————————//

#include <stdio.h>
#include "lang.h"

//———————————————————————————————————————————————————————————————————//


//———————————————————————————————————————————————————————————————————//

lang_status_t tokenize        (lang_ctx_t* ctx);
lang_status_t syntax_analysis (lang_ctx_t* ctx);

//———————————————————————————————————————————————————————————————————//

#endif // FRONTEND_H__
