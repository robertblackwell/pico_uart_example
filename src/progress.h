#ifndef H_progress_h
#define H_progress_h

typedef void(*f_t)(char) ;

typedef struct progress_s {
    int max_per_line;
    int count;
    f_t putc_func;
} progress_t;

void progress_init(progress_t* self, f_t putc_func, int max_per_line)
{
    self->count = 0;
    self->putc_func = putc_func;
    self->max_per_line = max_per_line;
}

void progress_update(progress_t * self)
{
    self->count++;
    self->putc_func('.');
    if(self->count >= self->max_per_line) {
        self->count = 0;
        self->putc_func('\n');
    }
}


#endif