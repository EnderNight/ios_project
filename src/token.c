
#include "token.h"
#include "input.h"
#include <stdlib.h>




Token **tokenize(input *in) {
    
    long list_len = 0;
    Token **tokens = malloc(sizeof(Token *));
    char buf;


    skip(in);


    while (look(in) != EOF) {

        buf = next(in);

        if (is_delimiter(buf)) {



            skip(in);
        
        }



    }


    return tokens;
    
}
