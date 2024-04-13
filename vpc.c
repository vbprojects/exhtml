#include<stdio.h>
#include<stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

bool whitespace_char[256] = {false};
int initiliaze_whitespace_char(){
    whitespace_char[' '] = true;
    whitespace_char['\t'] = true;
    // whitespace_char['\n'] = true;
    whitespace_char['\r'] = true;
    return 0;
}

const int tag_count = 127;
const char* tag_names[] = {"!--...--", "!DOCTYPE", "a", "abbr", "acronym", "address", "applet", "area", "article", "aside", "audio", "b", "base", "basefont", "bdi", "bdo", "big", "blockquote", "body", "br", "button", "canvas", "caption", "center", "cite", "code", "col", "colgroup", "data", "datalist", "dd", "del", "details", "dfn", "dialog", "dir", "div", "dl", "dt", "em", "embed", "fieldset", "figcaption", "figure", "font", "footer", "form", "frame", "frameset", "h1", "h2", "h3", "h4", "h5", "h6", "head", "header", "hgroup", "hr", "html", "i", "iframe", "img", "input", "ins", "kbd", "label", "legend", "li", "link", "main", "map", "mark", "menu", "meta", "meter", "nav", "noframes", "noscript", "object", "ol", "optgroup", "option", "output", "p", "param", "picture", "pre", "progress", "q", "rp", "rt", "ruby", "s", "samp", "script", "search", "section", "select", "small", "source", "span", "strike", "strong", "style", "sub", "summary", "sup", "svg", "table", "tbody", "td", "template", "textarea", "tfoot", "th", "thead", "time", "title", "tr", "track", "tt", "u", "ul", "var", "video", "wbr"};

int tag_hashs[tag_count];

int get_hash(const char* s, const int n) {
    long long p = 31, m = 1e9 + 7;
    long long hash = 0;
    long long p_pow = 1;
    for(int i = 0; i < n; i++) {
        hash = (hash + (s[i] - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
    return hash;
}

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int initiliaze_tag_hashs(){
    for(int i = 0; i < tag_count; i++){
        long long p = 31, m = 1e9 + 7;
        long long hash = 0;
        long long p_pow = 1;
        for(int j = 0; tag_names[i][j] != '\0'; j++){
            hash = (hash + (tag_names[i][j] - 'a' + 1) * p_pow) % m;
            p_pow = (p_pow * p) % m;
        }
        tag_hashs[i] = (int)hash;
    }
    qsort(tag_hashs, tag_count, sizeof(int), cmpfunc);
    return 0;
}

int is_tag(const char* s, int n){
    int hash = get_hash(s, n);
    void* found = bsearch(&hash, tag_hashs, tag_count, sizeof(int), cmpfunc);
    // printf("%d %d", hash, )
    return ((int*)(found)) != NULL;
    // return hash;
}


int get_hierarch(int* indent_levels, int* hierarchy, int count){
    int stack[count*2];
    int cur_pos = -1;
    int prev_indent = -1;
    int cur_h_pos = 0;
    for(int i = 0; i < count; i++){
        int line_number = i;
        int current_indent = indent_levels[i];
        if(current_indent ==  -1){
            continue;
        }
        if(current_indent > prev_indent)
            stack[++cur_pos] = line_number;
        else if (current_indent <= prev_indent){
            while((cur_pos > -1) && (indent_levels[stack[cur_pos]] >= current_indent)){
                hierarchy[cur_h_pos++] = stack[cur_pos--];
            }
            stack[++cur_pos] = line_number;
        }
        hierarchy[cur_h_pos++] = line_number;
        prev_indent = current_indent;
    }
    while(cur_pos > -1){
        hierarchy[cur_h_pos++] = stack[cur_pos--];
    }
    return cur_h_pos;
}

int main1(){
    initiliaze_tag_hashs();
    // for(int i = 0; i < tag_count; i++){
    //     printf("%d ", tag_hashs[i]);
    // }
    // printf("\n");
    // printf("%d %d\n", is_tag("a", 1), tag_hashs[2]);
    printf("%d\n", is_tag("u", 1));
    return 0;
}




int get_indent_level(char* line){
    int i = 0;
    while(whitespace_char[line[i]]){
        if (line[i] == '\n'){
            return -1;
        }
        i++;
    }
    return i / 4;
}

int main(){
    initiliaze_whitespace_char();
    
    FILE* input = fopen("index.vpml", "r");
    if(input == NULL){
        printf("Error: File not found\n");
        return 1;
    }

    // Get line count and max line length
    int c;
    int count = 0;
    int max_line_length = 0;
    int cur_line_length = 0;
    for (c = getc(input); c != EOF; c = getc(input)){
        cur_line_length++;
        if (c == '\n'){ // Increment count if this character is newline 
                count = count + 1;
                if(cur_line_length > max_line_length){
                    max_line_length = cur_line_length;
                }
                cur_line_length = 0;
        }
    }
    max_line_length++;
    // Get indent levels
    int indent_levels[count]; //= (int*) alloca(count * sizeof(int));
    char buffer[max_line_length];// = (char*) alloca(max_line_length * sizeof(char));
    int line_starts[count];
    // int line_lengths[count];
    fseek(input, 0, SEEK_SET);
    for(int i = 0; i < count; i++){
        line_starts[i] = ftell(input);
        fgets(buffer, max_line_length, input);
        indent_levels[i] = get_indent_level(buffer);
    }

    fclose(input);
    printf("Indent levels: ");
    for(int i = 0; i < count; i++){
        printf("%d ", indent_levels[i]);
    }

    printf("\n");
    int hierarchy[count];
    int h_count = get_hierarch(indent_levels, hierarchy, count);

    for(int i = 0; i < h_count; i++){
        printf("%d ", hierarchy[i]);
    }
    printf("\n");
}