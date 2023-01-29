//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/29/23
//

#pragma once

#include <app/app_component.hpp>
#include <app/app_signals.hpp>

#include <set>

namespace mousetrap
{
    /// @note GTK does not allow a non-realized surface to have a render context, so we need to keep a dummy widget
    ///       realized so we can use it's surface for custom rendering. Canvas export stays on screen at all times but
    ///       is hidden by setting its opacity to 0 at all times

    class CanvasExport : public AppComponent,
        public signals::LayerImageUpdated,
        public signals::LayerCountChanged,
        public signals::LayerPropertiesChanged,
        public signals::LayerResolutionChanged
    {
        public:
            CanvasExport();
            operator Widget*() override;

            /// @brief merge layer textures respecting state properties
            Image merge_layers(const std::set<size_t>& layer_is, size_t frame_i);

            /// @brief merge arbitrary textures/shapes
            Image merge(const std::vector<RenderTask>& tasks);

        protected:
            void on_layer_image_updated() override;
            void on_layer_count_changed() override;
            void on_layer_properties_changed() override;
            void on_layer_resolution_changed() override;

        private:
            size_t _frame_i = 0;
            void set_frame(size_t);

            mutable GLArea _area;
            std::vector<Shape*> _shapes;
            Vector2i _canvas_size;

            AspectFrame _frame;
            Box _box = Box(GTK_ORIENTATION_VERTICAL);

            static void on_area_realize(Widget*, CanvasExport*);
            static void on_area_resize(GLArea*, int w, int h, CanvasExport*);
            static gboolean on_area_render(GLArea*, GdkGLContext*, CanvasExport*);

            std::vector<RenderTask> _render_tasks;
            void queue_render_tasks();
    };

    namespace state
    {
        inline CanvasExport* canvas_export = nullptr;
    }
}
