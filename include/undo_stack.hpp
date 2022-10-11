//
// Copyright 2022 Clemens Cords
// Created on 10/10/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <functional>
#include <stack>
#include <exception>
#include <iostream>

namespace mousetrap
{
    class UndoStack
    {
        struct UndoTask;

        public:
            static inline size_t max_n_items = size_t(-1);

            UndoStack() = default;
            void pop();

            template<typename UndoPayload_t, typename FreeFunction_t, typename Data_t>
            void push(UndoPayload_t, Data_t, FreeFunction_t);

        private:
            static inline size_t _current_id = 1;
            struct UndoTask
            {
                size_t id;
                std::function<void(void*)> payload;
                std::function<void(void*)> free_function;
                void* data;
            };

            std::deque<UndoTask> _stack;
    };
}

//

namespace mousetrap
{
    template<typename UndoPayload_t, typename FreeFunction_t, typename Data_t>
    void UndoStack::push(UndoPayload_t payload, Data_t data, FreeFunction_t free)
    {
        std::function<void(Data_t)> payload_wrapper = payload;
        std::function<void(Data_t)> free_wrapper = free;

        using final_function_t = std::function<void(void*)>;

        _stack.push_front(UndoTask{
            _current_id++,
            final_function_t(*((final_function_t*) payload_wrapper)),
            final_function_t(*((final_function_t*) free_wrapper)),
            reinterpret_cast<void*>(data)
        });

        if (_stack.size() == max_n_items)
        {
            auto& back = _stack.back();

            try
            {
                back.payload(back.data);
            }
            catch (std::exception& e)
            {
                std::cerr << "[ERROR] In UndoStack::push: Exception occurred when attempting to free task #" << back.id
                          << ": " << e.what() << std::endl;
            }

            _stack.pop_back();
        }
    }

    void UndoStack::pop()
    {
        if (_stack.empty())
            return;

        auto& top = _stack.front();

        try
        {
            top.payload(top.data);
            top.free_function(top.data);
        }
        catch (std::exception& e)
        {
            std::cerr << "[ERROR] In UndoStack::pop: Exception occurred when attempting to execute task #" << top.id
                      << ": " << e.what() << std::endl;
        }
    }
}