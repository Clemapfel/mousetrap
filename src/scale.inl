//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 4/1/23
//

namespace mousetrap
{
    template<typename Function_t, typename Data_t>
    void Scale::set_format_value_function(Function_t function, Data_t data)
    {
        _internal->formatting_function = [f = function, d = data](float value) -> std::string{
            return f(value, d);
        };

        gtk_scale_set_format_value_func(get_native(), (GtkScaleFormatValueFunc) on_format_value, this, (GDestroyNotify) nullptr);
    }

    template<typename Function_t>
    void Scale::set_format_value_function(Function_t function)
    {
        _internal->formatting_function = [f = function](float value) -> std::string{
            return f(value);
        };

        gtk_scale_set_format_value_func(get_native(), (GtkScaleFormatValueFunc) on_format_value, this, (GDestroyNotify) nullptr);
    }
}