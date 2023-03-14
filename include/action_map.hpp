// 
// Copyright 2022 Clemens Cords
// Created on 10/22/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/action.hpp>

namespace mousetrap
{
    class ActionMap
    {
        public:
            virtual operator GActionMap*() = 0;

            void add_action(Action& action);
            void remove_action(const ActionID& id);
            const Action& get_action(const ActionID& id);
            bool has_action(const ActionID& id);

        protected:
            virtual operator GtkApplication*() = 0;

        private:
            std::unordered_map<ActionID, Action> _actions;
    };
}

#include <src/action_map.inl>
