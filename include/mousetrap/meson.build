data = configuration_data()
data.set10('MOUSETRAP_ENABLE_OPENGL_COMPONENT_BOOL', OPENGL.found())
configure_file(
    input: 'gl_common.hpp.in',
    output: 'gl_common.hpp',
    configuration: data
)
message('`RenderArea` enabled: ', OPENGL.found())
