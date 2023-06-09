####### detail.jl

@info "Importing `libmousetrap.so`..."
__cxxwrap_compile_time_start = time()

module detail
    using CxxWrap
    function __init__() @initcxx end
    @wrapmodule("./libjulia_binding.so")
end

@info "Done (" * string(round(time() - __cxxwrap_compile_time_start; digits=2)) * "s)"

