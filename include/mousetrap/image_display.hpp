//
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap/image.hpp>
#include <mousetrap/widget.hpp>
#include <mousetrap/icon.hpp>
#include <mousetrap/file_descriptor.hpp>

namespace mousetrap
{
    #ifndef DOXYGEN
    class ImageDisplay;
    namespace detail
    {
        struct _ImageDisplayInternal;
        using ImageDisplayInternal = _ImageDisplayInternal;
        DEFINE_INTERNAL_MAPPING(ImageDisplay);
    }
    #endif

    /// @brief widget that display an iamge
    class ImageDisplay : public Widget
    {
        public:
            /// @brief default ctor, initialize as 0x0 image
            ImageDisplay();

            /// @brief create from internal
            ImageDisplay(detail::ImageDisplayInternal*);

            /// @brief destructor
            ~ImageDisplay();

            /// @brief expose internal
            NativeObject get_internal() const override;

            /// @brief construct from GtkImage \for_internal_use_only
            /// @param image GtkImage instance
            ImageDisplay(GtkImage*);

            /// @brief construct from image on disk
            /// @param path
            ImageDisplay(const std::string& path);

            /// @brief construct from image
            /// @param image
            ImageDisplay(const Image& image);

            /// @brief construct from icon
            /// @param icon
            ImageDisplay(const Icon& icon);

            /// @brief get resolution of image buffer
            /// @return resolution
            Vector2ui get_size() const;

            /// @brief load from image on disk
            /// @param path
            void create_from_file(const std::string& path);

            /// @brief load from image
            /// @param image
            void create_from_image(const Image& image);

            /// @brief load from icon
            /// @param icon
            void create_from_icon(const Icon& icon);

            /// @brief create as preview of a file, if the file can be opened as an image, will display image, otherwise will display icon of filetype
            /// @param file
            void create_as_file_preview(const FileDescriptor& file);

            /// @brief create from empty image
            void clear();

            /// @brief set scale, will use linear interpolation. For other types of interpolation, scale a mousetrap::Image instead
            /// @param scale (positive integer) or -1 for default size
            void set_scale(int scale);

        private:
            detail::ImageDisplayInternal* _internal = nullptr;

            void update_size(size_t, size_t);
            void initialize();
    };
}
