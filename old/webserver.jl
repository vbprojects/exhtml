include("./parser.jl")



open("index.html", "w+") do fo
    open("index.vpml", "r") do f
        vpml = read(f, String)
        print(vpml)
        lines = filter(x -> strip(x) != "", split(vpml,'\n')) |> collect
        nodes = process_line.(lines)
        indent_levels = lines .|> indentlevel
        hierarchy = get_hierarch(indent_levels)
        html_lines = [process_node(i, nodes) for i in hierarchy]
        html = join(html_lines, "\n")
        print(html_lines)
        write(fo, html)
    end
end

