module internal
    mutable struct leaf
        text :: AbstractString
    end
    mutable struct element
        tag :: AbstractString
        classes :: Union{Vector{AbstractString}, Nothing}
        attributes :: Union{Vector{AbstractString}, Nothing}
        id :: Union{AbstractString, Nothing}
    end

    function get_class_str(elem::element)
        if elem.classes === nothing
            return ""
        end
        return "class=\"$(join(elem.classes, " "))\""        
    end

    function get_attr_str(elem::element)
        if elem.attributes === nothing
            return ""
        end
        return join(elem.attributes, " ")
    end

    function get_id_str(elem::element)
        if elem.id === nothing
            return ""
        end
        return "id=\"$(elem.id)\""
    end

    function forward(node::leaf)
        return node.text
    end
    function forward(node::element)
        return "<$(node.tag) $(get_id_str(node)) $(get_class_str(node)) $(get_attr_str(node))>"
    end
    function backward(node::leaf)
        return ""
    end
    function backward(node::element)
        return "</$(node.tag)>"
    end
end
using .internal
leaf = internal.leaf
element = internal.element
forward = internal.forward
backward = internal.backward
using Base.Iterators: peel
reserved_tags = Set(["!--...--", "!DOCTYPE", "a", "abbr", "acronym", "address", "applet", "area", "article", "aside", "audio", "b", "base", "basefont", "bdi", "bdo", "big", "blockquote", "body", "br", "button", "canvas", "caption", "center", "cite", "code", "col", "colgroup", "data", "datalist", "dd", "del", "details", "dfn", "dialog", "dir", "div", "dl", "dt", "em", "embed", "fieldset", "figcaption", "figure", "font", "footer", "form", "frame", "frameset", "h1", "h2", "h3", "h4", "h5", "h6", "head", "header", "hgroup", "hr", "html", "i", "iframe", "img", "input", "ins", "kbd", "label", "legend", "li", "link", "main", "map", "mark", "menu", "meta", "meter", "nav", "noframes", "noscript", "object", "ol", "optgroup", "option", "output", "p", "param", "picture", "pre", "progress", "q", "rp", "rt", "ruby", "s", "samp", "script", "search", "section", "select", "small", "source", "span", "strike", "strong", "style", "sub", "summary", "sup", "svg", "table", "tbody", "td", "template", "textarea", "tfoot", "th", "thead", "time", "title", "tr", "track", "tt", "u", "ul", "var", "video", "wbr"]);



is_reserved_tag(tag) = tag in reserved_tags
is_raw(tag) = tag == "raw"
is_attr(tag) = occursin('=',tag)
is_id(tag) = occursin("#",tag)

function indentlevel(s)
    for i in 1:length(s)
        if s[i] != ' '
            return div(i, 4)
        end
    end
end
function process_node(i, nodes)
    if i > 0
        return forward(nodes[i])
    end
    return backward(nodes[-i])
end
function process_line(line :: AbstractString) :: Union{leaf, element}
    words = filter(!=(""), split(strip(line), ' '))
    if length(words) == 0
        return leaf("")
    end
    if first(words) in reserved_tags
        tag, rest = peel(words)
        classes = []
        attributes = []
        id = nothing
        
        for word in rest
            if is_attr(word)
                push!(attributes, word)
            elseif is_id(word)
                id = word[2:end]
            else
                push!(classes, word)
            end
        end
        if length(classes) == 0
            classes = nothing
        end
        if length(attributes) == 0
            attributes = nothing
        end
        return element(tag, classes, attributes, id)
    elseif first(words) == "raw"
            return lstrip(line)[4:end]
    end
    return leaf(strip(line))
end
function get_hierarchy(indent_levels)
    Stack = []
    result = []
    prev_indent = -1

    for (i, l) in enumerate(indent_levels)
        while !isempty(Stack) && (indent_levels[Stack[end]]) > l
            push!(result, -1 * pop!(Stack))
        end
        push!(Stack, i)
        push!(result, i)
        prev_indent = l
    end
    while !isempty(Stack)
        push!(result, -1 * pop!(Stack))
    end

    result
end
function process_file(file_path :: AbstractString)
    vpml = open(file_path, "r") do f
        read(f, String)
    end
    lines = split(vpml,'\n')
    print(lines)
    nodes = process_line.(lines)
    print(nodes)
    hierarchy = get_hierarchy(lines .|> indentlevel)
    print(hierarchy)
    html_lines = hierarchy .|> (i -> process_node(i, nodes))
    join(html_lines, "\n")
end
function get_hierarch(indent_levels)
    stack = Int[]  # Stack to keep track of line numbers for current open scopes
    result = Int[]  # Result list to hold the transformed line numbers
    prev_indent = -1  # Previous indent level, initialized to -1 for comparison

    for (i, current_indent) in enumerate(indent_levels)
        line_number = i  # Line numbers are 1-based in Julia's `enumerate`

        if current_indent > prev_indent
            # If indent level increases, start a new scope
            push!(stack, line_number)
        elseif current_indent <= prev_indent
            # If indent level decreases, close the current scopes
            while !isempty(stack) && indent_levels[stack[end]] >= current_indent
                push!(result, -pop!(stack))
            end
            push!(stack, line_number)  # Add the current line number to the stack
        end

        push!(result, line_number)  # Add the current line number to the result
        prev_indent = current_indent  # Update previous indent level
    end

    # Close all remaining open scopes
    while !isempty(stack)
        push!(result, -pop!(stack))
    end

    result
end