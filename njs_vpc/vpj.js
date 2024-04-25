

var tag_names = new Set(["!--...--", "!DOCTYPE", "a", "abbr", "acronym", "address", "applet", "area", "article", "aside", "audio", "b", "base", "basefont", "bdi", "bdo", "big", "blockquote", "body", "br", "button", "canvas", "caption", "center", "cite", "code", "col", "colgroup", "data", "datalist", "dd", "del", "details", "dfn", "dialog", "dir", "div", "dl", "dt", "em", "embed", "fieldset", "figcaption", "figure", "font", "footer", "form", "frame", "frameset", "h1", "h2", "h3", "h4", "h5", "h6", "head", "header", "hgroup", "hr", "html", "i", "iframe", "img", "input", "ins", "kbd", "label", "legend", "li", "link", "main", "map", "mark", "menu", "meta", "meter", "nav", "noframes", "noscript", "object", "ol", "optgroup", "option", "output", "p", "param", "picture", "pre", "progress", "q", "rp", "rt", "ruby", "s", "samp", "script", "search", "section", "select", "small", "source", "span", "strike", "strong", "style", "sub", "summary", "sup", "svg", "table", "tbody", "td", "template", "textarea", "tfoot", "th", "thead", "time", "title", "tr", "track", "tt", "u", "ul", "var", "video", "wbr"])

var global_state = {};
var macros = {};
module.exports = {process_string, macros, global_state};
class element{
    constructor(){
        this.children = [];
        this.text = "";
        this.parent = null;
    }
    forward(){return "";}
    backward(){return "";}
    children;
    get_children(){return this.children;}
    text;
    get_text(){return this.text;}
    parent;
    get_parent(){return this.parent;}
}



function process_line(line){
    line = line.trimStart();
    line = line.replaceAll("\r", "");
    line = line.replaceAll("\n", "");
    tokens = line.split(" ");
    is_tag = false;
    is_self_closing = false;
    maybe_tag = tokens[0]

    if(maybe_tag[0] == "<"){
        maybe_tag = maybe_tag.slice(1);
        is_tag = true;
    }
    if(maybe_tag[-1] == ">"){
        maybe_tag = maybe_tag.slice(0, -1);
    }
    if(tag_names.has(maybe_tag)){
        is_tag = true;
    }

    if(is_tag){
        return new tag_element(tokens, maybe_tag, is_self_closing, line);
    }
    // if(maybe_tag == ":set"){
    //     return new macro_setter(line);
    // }
    if(maybe_tag == ":macro"){
        return new macro_setter(tokens);
    }
    if(maybe_tag[0] == "!"){
        return new macro_element(maybe_tag);
    }
    return new text_element(line, maybe_tag == "raw");

}

class text_element extends element{
    text;
    constructor(line, starts_with_raw){
        super();
        if (starts_with_raw){
            this.text = line.trimStart().slice(4);
        }
        else{
            this.text = line.trimStart();
        }
    }
    forward(){
        return this.text;
    }
    backward(){
        return "\n";
    }
    get_text(){
        return this.text;
    }
    get_children(){
        return [];
    }
}

class tag_element extends element{ 
    tag;
    self_closing;
    attributes;
    classes;
    id;
    tokens;
    line;
    constructor(tokens, tag, is_self_closing, line){
        super();
        this.id = "";
        this.classes = [];
        this.attributes = [];
        this.tag = tag;
        this.self_closing = is_self_closing;
        this.tokens = tokens;
        this.line = line;
    }
    forward(){
        
        for(let i = 1; i < this.tokens.length; i++){
            var token = this.tokens[i];
            if(token[0] == "@"){
                token = global_state[token].text;
            }
            if(token.includes("=")){
                this.attributes.push(token);
            }else if(token[0] == "#"){
                this.id += tokens[i].slice(1);
            }else if(token == "|"){
                let post_tag = this.line.slice(this.line.indexOf("|") + 1).
                trimStart();
                var maybe_macros = post_tag.split("@")
                var maybe_macros_names = maybe_macros.map((x) => x.split(" "))
                if(maybe_macros.length == 1){this.text = post_tag;break;}
                var li;
                var ti;
                for(li in maybe_macros_names){
                    for(ti in maybe_macros_names[li]){
                        if(("@" + maybe_macros_names[li][ti]) in global_state){
                            maybe_macros_names[li][ti] = "@" + global_state["@" + maybe_macros_names[li][ti]].text;
                        }
                    }
                }
                this.text = maybe_macros_names.map((x) => x.join(" ")).join("@").replaceAll("@@", "")
                break;
            }else{
                this.classes.push(token);
            }
        }

        let result = "<" + this.tag;
        if(this.id != ""){
            result += " id=\"" + this.id + "\"";
        }
        if(this.classes.length > 0){
            result += " class=\"" + this.classes.join(" ") + "\"";
        }
        if(this.attributes.length > 0){
            result += " " + this.attributes.join(" ");
        }
        if(this.self_closing){
            result += " />";
        }else{
            result += ">";
        }
        result += this.text;
        return result;
    }
    backward(){
        if (this.self_closing){
            return "\n";
        }
        return "</" + this.tag + ">\n";
    }
    get_text(){
        return this.text;
    }
    get_children(){
        return this.children;
    }
}

class macro_setter extends element{
    name;
    constructor(tokens){
        super();
        this.name = ""
        if(tokens.length > 1)
            this.name = tokens[1];
    }
    forward(){
        macros[this.name] = this.children;
        return "";
    }
    backward(){
        return "";
    }
    get_text(){
        return "";
    }
    get_children(){
        return [];
    }
}

class macro_element extends element{
    constructor(name){
        super();
        this.name = name;
    }
    forward(){
        for(let i = 0; i < this.children.length; i++){
            global_state["@" + i] = this.children[i];
        }
        return "";
    }
    backward(){
        return "";
    }
    get_text(){
        return "";
    }
    get_children(){
        return macros[this.name];
    }
}
class mlstate{
    variables;
    macros;
    constructor(){
        this.variables = {};
        this.macros = {};
    }
}

function indent_level(line){
    trimmed_len = line.trimStart().length
    if(trimmed_len == 0) return -1
    if(line[0] == "#") return -1
    return Math.trunc((line.length - trimmed_len) / 4)
}

function get_hierarchy(indent_levels){
    // int stack[count*2 + 1];
    stack = [];
    hierarchy = [];
    var cur_pos = -1;
    var prev_indent = -1;
    var cur_h_pos = 0;
    count = indent_levels.length;
    for(let i = 0; i < count; i++){
        let line_number = i;
        let current_indent = indent_levels[i];
        if(current_indent ==  -1){
            continue;
        }
        if(current_indent > prev_indent)
            stack[++cur_pos] = line_number;
        else if (current_indent <= prev_indent){
            while((cur_pos > -1) && (indent_levels[stack[cur_pos]] >= current_indent)){
                hierarchy[cur_h_pos++] = -1 * (stack[cur_pos--]);
            }
            stack[++cur_pos] = line_number;
        }
        hierarchy[cur_h_pos++] = (line_number);
        prev_indent = current_indent;
    }
    while(cur_pos > -1){
        hierarchy[cur_h_pos++] = -1 * (stack[cur_pos--]);
    }
    return hierarchy;
}

function create_tree(input){
    var root = new element();
    current = root;
    indent_levels = input.map(indent_level);
    hierarchy = get_hierarchy(indent_levels);
    for(ln of hierarchy){
        if(ln < 0){
            current = current.get_parent();
        }else{
            elem = process_line(input[ln]);
            // console.log("Line:" + input[ln]);
            // console.log("Forward: " + elem.forward());
            current.children.push(elem);
            elem.parent = current;
            current = elem;
        }
    }
    return root
}

function recurse(root){
    result = "";
    let forward = root.forward();
    for(child of root.get_children()){
        result += recurse(child);
    }
    return forward + result + root.backward();
}

function process_string(vpml){
    input = vpml.split("\n")
    root = create_tree(input);
    return recurse(root);
}