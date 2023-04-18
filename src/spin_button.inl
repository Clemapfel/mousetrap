//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 3/26/23
//

namespace mousetrap
{
    template<typename Function_t, typename Data_t>
    void SpinButton::set_text_to_value_function(Function_t f_in, Data_t data_in)
    {
        _internal->text_to_value_function = [f = f_in, data = data_in](const SpinButton* instance, const std::string& text) -> float
        {
            return f(instance, text, data);
        };

        connect_signal("input", on_input, _internal);
        set_allow_only_numeric(false);
    }

    template<typename Function_t>
    void SpinButton::set_text_to_value_function(Function_t f_in)
    {
        _internal->text_to_value_function = [f = f_in](const SpinButton* instance, const std::string& text) -> float
        {
            return f(instance, text);
        };

        connect_signal("input", on_input, _internal);
        set_allow_only_numeric(false);
    }

    template<typename Function_t, typename Data_t>
    void SpinButton::set_value_to_text_function(Function_t f_in, Data_t data_in)
    {
        _internal->value_to_text_function = [f = f_in, data = data_in](const SpinButton* instance, float value)
        {
            return f(instance, value, data);
        };

        connect_signal("output", on_output, _internal);
        set_allow_only_numeric(false);
    }

    template<typename Function_t>
    void SpinButton::set_value_to_text_function(Function_t f_in)
    {
        _internal->value_to_text_function = [f = f_in](const SpinButton* instance, float value)
        {
            return f(instance, value);
        };

        connect_signal("output", on_output, _internal);
        set_allow_only_numeric(false);
    }
}
