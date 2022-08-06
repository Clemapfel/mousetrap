// 
// Copyright 2022 Clemens Cords
// Created on 8/7/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/texture.hpp>

namespace mousetrap
{
    // 1-d floating point, square texture
    class Brush
    {
        public:
            Brush(size_t size);

            template<typename... Floats> requires (std::is_same_v<Floats, float> && ...)
            Brush(Floats...);

            std::string to_string();
            void create_from_string(const std::string&);

        private:
            static inline const std::string begin = "Brush(";
            static inline const std::string end = ")";

            size_t _width, _height;
            std::vector<float> _data; // x in [0, 1] where each pixel has the color RGBA(0, 0, 0, x)
    };
}

// ###

#include <sstream>

namespace mousetrap
{
    Brush::Brush(size_t size)
        : _width(size), _height(size)
    {
        _data.resize(size * size, 0.f);
    }

    void Brush::create_from_string(const std::string& str)
    {
        // syntax: Brush(0.1, 0.123, 0.142, 0.1511, 0.1)
        
        std::vector<float> data;
        size_t i = 0;

        if (str.substr(0, begin.size()) != begin)
            goto error;
        
        if (str.substr(str.size() - end.size()) != end)
            goto error;
        
        for (i = begin.size(); i < str.size();)
        {
            try 
            {
                std::string word = "";
                while (str.at(i) != ',' and i < str.size() - end.size())
                    word.push_back(str.at(i++));
                
                data.push_back(std::stof(word));
                i += 1;
            }
            catch (...)
            {
                goto error;
            }
            
            if (i < str.size() - end.size() and str.at(i) == ' ')
                i += 1;
        }

        if (glm::fract(sqrt(data.size())) > 0.0000001)
        {
            std::cerr << "[ERROR] In Brush::create_from_string: trying to read brush with " << data.size() << " element, which cannot be expressed as a square." << std::endl;
            return;
        }

        _data = data;
        _width = sqrt(_data.size());
        _height = _width;
        return;

        error:
            std::cerr << "[ERROR] In Brush::create_from_string: malformated string at position " << i << ": \"" << str.substr(0, i+1) << "\""  << std::endl;
            return;
    }

    std::string Brush::to_string()
    {
        std::stringstream out;
        out << begin;

        if (not _data.empty())
        {
            for (size_t i = 0; i < _data.size() - 1; ++i)
                out << _data.at(i) << ", ";

            out << _data.back();
        }

        out << end;
        return out.str();
    }

    template<typename... Floats> requires (std::is_same_v<Floats, float> && ...)
    Brush::Brush(Floats... in)
    {
        _data = {in ...};
    }
}
