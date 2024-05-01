#include<stdio.h>
#include<stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

bool whitespace_char[256] = {false};
int init_whitespace_char(){
    whitespace_char[' '] = true;
    whitespace_char['\t'] = true;
    // whitespace_char['\n'] = true;
    whitespace_char['\r'] = true;
    return 0;
}

#define tag_count 127
const char* tag_names[] = {"!--...--", "!DOCTYPE", "a", "abbr", "acronym", "address", "applet", "area", "article", "aside", "audio", "b", "base", "basefont", "bdi", "bdo", "big", "blockquote", "body", "br", "button", "canvas", "caption", "center", "cite", "code", "col", "colgroup", "data", "datalist", "dd", "del", "details", "dfn", "dialog", "dir", "div", "dl", "dt", "em", "embed", "fieldset", "figcaption", "figure", "font", "footer", "form", "frame", "frameset", "h1", "h2", "h3", "h4", "h5", "h6", "head", "header", "hgroup", "hr", "html", "i", "iframe", "img", "input", "ins", "kbd", "label", "legend", "li", "link", "main", "map", "mark", "menu", "meta", "meter", "nav", "noframes", "noscript", "object", "ol", "optgroup", "option", "output", "p", "param", "picture", "pre", "progress", "q", "rp", "rt", "ruby", "s", "samp", "script", "search", "section", "select", "small", "source", "span", "strike", "strong", "style", "sub", "summary", "sup", "svg", "table", "tbody", "td", "template", "textarea", "tfoot", "th", "thead", "time", "title", "tr", "track", "tt", "u", "ul", "var", "video", "wbr"};
const char raw[] = "raw";
int raw_hash;

int tag_hashs[tag_count];

int get_hash(const char* s, const int n) {
    const long long p = 31, m = 1e9 + 7;
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

int init_tag_hashs(){
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


int get_hierarchy(int* indent_levels, int* hierarchy, int count){
    int stack[count*2 + 1];
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
                hierarchy[cur_h_pos++] = -1 * (stack[cur_pos--] + 1);
            }
            stack[++cur_pos] = line_number;
        }
        hierarchy[cur_h_pos++] = (line_number + 1);
        prev_indent = current_indent;
    }
    while(cur_pos > -1){
        hierarchy[cur_h_pos++] = -1 * (stack[cur_pos--] + 1);
    }
    return cur_h_pos;
}


bool is_atr(char* s, int n){
    for(int i = 0; i < n; i++){
        if(s[i] == '='){
            return true;
        }
    }
    return false;
}


int forward(char* line, int line_num, int max_line_length, char* line_out, char line_tags[][100]){
    // printf("Forward Start Line: %s", line);
    char buf[max_line_length];
    memset(buf, 0, max_line_length);
    strncpy(buf, line, max_line_length);
    char* pchr;
    pchr = strtok(line, " \r");
    if(pchr == NULL){
        return -1;
    }
    char* pch = (char*) alloca(strlen(pchr) + 1);
    strncpy(pch, pchr, strlen(pchr) + 1);
    pch[strcspn(pch,"\n\r")] = 0;
    int pchl = strlen(pch);
    printf("Tag: %s %d %d\n", pch, pchl, is_tag(pch, pchl) || is_tag(pch, pchl - 1));
    // printf("Forward Start Line: %s", line);
    if(is_tag(pch, pchl) || is_tag(pch, pchl - 1) || pch[0] == '<'){
        int forward_tag = 
        strncpy(line_tags[line_num], pch, pchl);
        char ids[max_line_length], classes[max_line_length], attrs[max_line_length];
        memset(ids, 0, max_line_length); memset(classes, 0, max_line_length); memset(attrs, 0, max_line_length);

        for(pch = strtok(NULL, " "); pch != NULL; pch = strtok(NULL, " ")){
            int pchl = strlen(pch);
            pch[strcspn(pch,"\n\r")] = 0;
            if(pch[0] == '#'){
                strncat(ids, pch + 1, pchl); //strncat(ids, " ", 1);
            }else if(is_atr(pch, pchl)){
                strncat(attrs, pch, pchl);  strncat(attrs, " ", 1);
            }else{
                strncat(classes, pch, pchl);  strncat(classes, " ", 1);
            }
        }
        if(ids[0] != 0){
            sprintf(line_out, "<%s class=\"%s\" id=\"%s\" %s>", line_tags[line_num], classes, ids, attrs);
        }else{
            sprintf(line_out, "<%s class=\"%s\" %s>", line_tags[line_num], classes, attrs);
        }
    }
    else{
        // strncpy(line_tags[line_num],"\0",1);
        buf[strcspn(buf,"\n\r")] = 0;
        int i = 0;
        for(char c= buf[i]; whitespace_char[c]; c = buf[i++]);
        if(get_hash(pch, pchl) == raw_hash)
            i += 4;
        // printf("Forward Line: %s\n", buf);
        sprintf(line_out, "%s", (buf + (i-1)));
    }
    return 0;
}

int backward(int line_num, char line_tags[][100], char* line_out){
    if(line_tags[line_num][0] != '\0'){
        sprintf(line_out, "</%s>", line_tags[line_num]);
    }else{
        sprintf(line_out, "");
    }
    return 0;
}


int get_indent_level(char* line){
    int i = 0;
    while(whitespace_char[line[i]]){
        i++;
    }
    if((i == 0 || i == 1) && (whitespace_char[line[i]] || line[i] == '\n'))
        return -1;
    if((line[0] == '?'))
        return -1;
    return (i) / 4;
}

int first_pass(FILE* input, int* count_p, int* max_line_length_p){
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
    *count_p = count;
    *max_line_length_p = max_line_length;
    return 0;
}



int main(int argc, char* argv[]){
    init_whitespace_char();
    init_tag_hashs();
    raw_hash = get_hash(raw, 3);



    FILE* input = fopen(argv[1], "r");
    // FILE* input = fopen("index.vpmlp", "r");
    if(input == NULL){
        printf("Error: File not found\n");
        return 1;
    }

    // Get line count and max line length
    // int c;
    // int count = 0;
    // int max_line_length = 0;
    // int cur_line_length = 0;
    // for (c = getc(input); c != EOF; c = getc(input)){
    //     cur_line_length++;
    //     if (c == '\n'){ // Increment count if this character is newline 
    //             count = count + 1;
    //             if(cur_line_length > max_line_length){
    //                 max_line_length = cur_line_length;
    //             }
    //             cur_line_length = 0;
    //     }
    // }

    int count;
    int max_line_length;
    first_pass(input, &count, &max_line_length);
    max_line_length += 100;
    // Get indent levels
    int indent_levels[count]; //= (int*) alloca(count * sizeof(int));
    char buffer[max_line_length];// = (char*) alloca(max_line_length * sizeof(char));
    int line_starts[count];
    // printf("Count: %d\n", count);
    
    memset(buffer, 0, max_line_length);
    fseek(input, 0, SEEK_SET);
    for(int i = 0; i < count; i++){
        line_starts[i] = ftell(input);
        fgets(buffer, max_line_length, input);
        printf("Position %d: %s\n", line_starts[i], buffer);
        indent_levels[i] = get_indent_level(buffer);
    }
    char line_tags[count][100];
    memset(line_tags, 0, count * 100);
    printf("Compiler Testing\n");
    char line_out[max_line_length];
    for(int i = 0; i < count; i++){
        char line_out[max_line_length];
        memset(line_out, 0, max_line_length);
        // printf("%d %d\n", indent_levels[i], line_starts[i]);
        fseek(input, line_starts[i], SEEK_SET);
        fgets(buffer, max_line_length, input);
        forward(buffer, i, max_line_length, line_out, line_tags);
        fflush(stdout);
        printf("Forward: %s ", line_out);
        backward(i, line_tags, line_out);
        printf("Backward %s \n", line_out);
    }
    
    printf("Indent levels: ");
    for(int i = 0; i < count; i++){
        printf("%d ", indent_levels[i]);
    }

    int hierarchy[count * 2 + 10];
    int h_count = get_hierarchy(indent_levels, hierarchy, count);

    printf("Hierarchy: ");
    for(int i = 0; i < h_count; i++){
        printf("%d ", hierarchy[i]);
    }

    fseek(input, 0, SEEK_SET);

    memset(buffer, 0, max_line_length);
    memset(line_out, 0, max_line_length);
    FILE* output = fopen(argv[2], "w+");
    for(int i = 0; i < h_count; i++){
        int line_num = hierarchy[i];
        int pos = abs(line_num) - 1;
        fseek(input, line_starts[pos], SEEK_SET);
        fgets(buffer, max_line_length, input);

        line_num > 0 ? forward(buffer, pos, max_line_length, line_out, line_tags) : backward(pos, line_tags, line_out);
        if(line_out[0] != '\0' && line_out[0] != '\n')
            fprintf(output, "%s\n", line_out);
    }
    fclose(input);
    fclose(output);
    return 0;
}