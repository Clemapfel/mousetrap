//
// Created by clem on 8/16/23.
//

#include <mousetrap/transform_bin.hpp>

namespace mousetrap::detail 
{
    // Transpiled from Vala, cf: https://gitlab.gnome.org/World/elastic/-/blob/main/src/transforms-view.vala

    /* using Gtk;
    using Adw;

    public class TransformBinInternal : Adw.Bin {

        public Gsk.Transform transform { get; set; }

        construct {
            layout_manager = null;
            transform = new Gsk.Transform();
            notify["transform"].connect(queue_allocate);
        }

        static construct {
            set_css_name ("transform-bin-internal");
        }

        public void rotate(float dg) {
            var transform = this.transform;
            transform = transform.rotate(dg);
            this.transform = transform;
        }

        public void reset() {
            this.transform = new Gsk.Transform();
        }

        public void translate(float x, float y) {
            var transform = this.transform;
            var point = Graphene.Point();
            point.x = x;
            point.y = y;
            transform = transform.translate(point);
            this.transform = transform;
        }

        public void translate_3d(float x, float y, float z) {
            var transform = this.transform;
            var point = Graphene.Point3D();
            point.x = x;
            point.y = y;
            point.z = z;
            transform = transform.translate_3d(point);
            this.transform = transform;
        }

        public void perspective(float x) {
            var transform = this.transform;
            transform = transform.perspective(x);
            this.transform = transform;
        }

        public void scale(float x_factor, float y_factor) {
            var transform = this.transform;
            transform = transform.scale(x_factor, y_factor);
            this.transform = transform;
        }

        public void skew(float x_skew, float y_skew) {
            var transform = this.transform;
            transform = transform.skew(x_skew, y_skew);
            this.transform = transform;
        }

        protected override void measure (
            Gtk.Orientation orientation,
            int for_size,
            out int minimum,
            out int natural,
            out int minimum_baseline,
            out int natural_baseline
        ) {
            if (child == null) {
                minimum = 0;
                natural = 0;
                minimum_baseline = -1;
                natural_baseline = -1;
                return;
            }

            child.measure (
                orientation, for_size,
                out minimum, out natural,
                out minimum_baseline, out natural_baseline
            );
        }

        protected override void size_allocate (int width, int height, int baseline) {
            if (child == null)
                return;

            var t = new Gsk.Transform();
            t = t.translate({ width / 2, height / 2});

            if (transform != null)
                t = t.transform(transform);

            t = t.translate({ -width / 2, -height / 2 });
            child.allocate(width, height, baseline, t);
        }
    }
     */



#if !defined(VALA_EXTERN)
#if defined(_MSC_VER)
#define VALA_EXTERN __declspec(dllexport) extern
#elif __GNUC__ >= 4
#define VALA_EXTERN __attribute__((visibility("default"))) extern
#else
#define VALA_EXTERN extern
#endif
#endif

#define TYPE_TRANSFORM_BIN_INTERNAL (transform_bin_internal_get_type ())
#define TRANSFORM_BIN_INTERNAL(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TRANSFORM_BIN_INTERNAL, TransformBinInternal))
#define TRANSFORM_BIN_INTERNAL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TRANSFORM_BIN_INTERNAL, TransformBinInternalClass))
#define IS_TRANSFORM_BIN_INTERNAL(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TRANSFORM_BIN_INTERNAL))
#define IS_TRANSFORM_BIN_INTERNAL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TRANSFORM_BIN_INTERNAL))
#define TRANSFORM_BIN_INTERNAL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TRANSFORM_BIN_INTERNAL, TransformBinInternalClass))

    typedef struct _TransformBinInternal TransformBinInternal;
    typedef struct _TransformBinInternalClass TransformBinInternalClass;
    typedef struct _TransformBinInternalPrivate TransformBinInternalPrivate;
    enum  {
        TRANSFORM_BIN_INTERNAL_0_PROPERTY,
        TRANSFORM_BIN_INTERNAL_TRANSFORM_PROPERTY,
        TRANSFORM_BIN_INTERNAL_NUM_PROPERTIES
    };
    static GParamSpec* transform_bin_internal_properties[TRANSFORM_BIN_INTERNAL_NUM_PROPERTIES];
#define _gsk_transform_unref0(var) ((var == NULL) ? NULL : (var = (GskTransform*) (gsk_transform_unref (var), NULL)))

    struct _TransformBinInternal {
        AdwBin parent_instance;
        TransformBinInternalPrivate * priv;
    };

    struct _TransformBinInternalClass {
        AdwBinClass parent_class;
    };

    struct _TransformBinInternalPrivate {
        GskTransform* _transform;
    };

    static gint TransformBinInternal_private_offset;
    static gpointer transform_bin_internal_parent_class = NULL;

    VALA_EXTERN GType transform_bin_internal_get_type (void) G_GNUC_CONST ;
    G_DEFINE_AUTOPTR_CLEANUP_FUNC (TransformBinInternal, g_object_unref)
    VALA_EXTERN void transform_bin_internal_rotate (TransformBinInternal* self,
    gfloat dg);
    VALA_EXTERN GskTransform* transform_bin_internal_get_transform (TransformBinInternal* self);
    VALA_EXTERN void transform_bin_internal_set_transform (TransformBinInternal* self,
    GskTransform* value);
    VALA_EXTERN void transform_bin_internal_reset (TransformBinInternal* self);
    VALA_EXTERN void transform_bin_internal_translate (TransformBinInternal* self,
    gfloat x,
    gfloat y);
    VALA_EXTERN void transform_bin_internal_translate_3d (TransformBinInternal* self,
    gfloat x,
    gfloat y,
    gfloat z);
    VALA_EXTERN void transform_bin_internal_perspective (TransformBinInternal* self,
    gfloat x);
    VALA_EXTERN void transform_bin_internal_scale (TransformBinInternal* self,
    gfloat x_factor,
    gfloat y_factor);
    VALA_EXTERN void transform_bin_internal_skew (TransformBinInternal* self,
    gfloat x_skew,
    gfloat y_skew);
    static void transform_bin_internal_real_measure (GtkWidget* base,
    GtkOrientation orientation,
    gint for_size,
    gint* minimum,
    gint* natural,
    gint* minimum_baseline,
    gint* natural_baseline);
    static void transform_bin_internal_real_size_allocate (GtkWidget* base,
    gint width,
    gint height,
    gint baseline);
    VALA_EXTERN TransformBinInternal* transform_bin_internal_new (void);
    VALA_EXTERN TransformBinInternal* transform_bin_internal_construct (GType object_type);
    static GObject * transform_bin_internal_constructor (GType type,
    guint n_construct_properties,
    GObjectConstructParam * construct_properties);
    static void _gtk_widget_queue_allocate_g_object_notify (GObject* _sender,
    GParamSpec* pspec,
    gpointer self);
    static void transform_bin_internal_finalize (GObject * obj);
    static GType transform_bin_internal_get_type_once (void);
    static void _vala_transform_bin_internal_get_property (GObject * object,
    guint property_id,
    GValue * value,
    GParamSpec * pspec);
    static void _vala_transform_bin_internal_set_property (GObject * object,
    guint property_id,
    const GValue * value,
    GParamSpec * pspec);

    static inline gpointer
    transform_bin_internal_get_instance_private (TransformBinInternal* self)
    {
        return G_STRUCT_MEMBER_P (self, TransformBinInternal_private_offset);
    }

    static GskTransform*
    _gsk_transform_ref0 (gpointer self)
    {
        return self ? gsk_transform_ref ((GskTransform*) self) : NULL;
    }

    void
    transform_bin_internal_rotate (TransformBinInternal* self,
    gfloat dg)
    {
        GskTransform* transform = NULL;
        GskTransform* _tmp0_;
        GskTransform* _tmp1_;
        GskTransform* _tmp2_;
        GskTransform* _tmp3_;
        GskTransform* _tmp4_;
        g_return_if_fail (self != NULL);
        _tmp0_ = self->priv->_transform;
        _tmp1_ = _gsk_transform_ref0 (_tmp0_);
        transform = _tmp1_;
        _tmp2_ = transform;
        transform = NULL;
        _tmp3_ = gsk_transform_rotate (_tmp2_, dg);
        _gsk_transform_unref0 (transform);
        transform = _tmp3_;
        _tmp4_ = transform;
        transform_bin_internal_set_transform (self, _tmp4_);
        _gsk_transform_unref0 (transform);
    }

    void
    transform_bin_internal_reset (TransformBinInternal* self)
    {
        GskTransform* _tmp0_;
        GskTransform* _tmp1_;
        g_return_if_fail (self != NULL);
        _tmp0_ = gsk_transform_new ();
        _tmp1_ = _tmp0_;
        transform_bin_internal_set_transform (self, _tmp1_);
        _gsk_transform_unref0 (_tmp1_);
    }

    void
    transform_bin_internal_translate (TransformBinInternal* self,
    gfloat x,
    gfloat y)
    {
        GskTransform* transform = NULL;
        GskTransform* _tmp0_;
        GskTransform* _tmp1_;
        graphene_point_t point = {0};
        GskTransform* _tmp2_;
        graphene_point_t _tmp3_;
        GskTransform* _tmp4_;
        GskTransform* _tmp5_;
        g_return_if_fail (self != NULL);
        _tmp0_ = self->priv->_transform;
        _tmp1_ = _gsk_transform_ref0 (_tmp0_);
        transform = _tmp1_;
        memset (&point, 0, sizeof (graphene_point_t));
        point.x = x;
        point.y = y;
        _tmp2_ = transform;
        transform = NULL;
        _tmp3_ = point;
        _tmp4_ = gsk_transform_translate (_tmp2_, &_tmp3_);
        _gsk_transform_unref0 (transform);
        transform = _tmp4_;
        _tmp5_ = transform;
        transform_bin_internal_set_transform (self, _tmp5_);
        _gsk_transform_unref0 (transform);
    }

    void
    transform_bin_internal_translate_3d (TransformBinInternal* self,
    gfloat x,
    gfloat y,
    gfloat z)
    {
        GskTransform* transform = NULL;
        GskTransform* _tmp0_;
        GskTransform* _tmp1_;
        graphene_point3d_t point = {0};
        GskTransform* _tmp2_;
        graphene_point3d_t _tmp3_;
        GskTransform* _tmp4_;
        GskTransform* _tmp5_;
        g_return_if_fail (self != NULL);
        _tmp0_ = self->priv->_transform;
        _tmp1_ = _gsk_transform_ref0 (_tmp0_);
        transform = _tmp1_;
        memset (&point, 0, sizeof (graphene_point3d_t));
        point.x = x;
        point.y = y;
        point.z = z;
        _tmp2_ = transform;
        transform = NULL;
        _tmp3_ = point;
        _tmp4_ = gsk_transform_translate_3d (_tmp2_, &_tmp3_);
        _gsk_transform_unref0 (transform);
        transform = _tmp4_;
        _tmp5_ = transform;
        transform_bin_internal_set_transform (self, _tmp5_);
        _gsk_transform_unref0 (transform);
    }

    void
    transform_bin_internal_perspective (TransformBinInternal* self,
    gfloat x)
    {
        GskTransform* transform = NULL;
        GskTransform* _tmp0_;
        GskTransform* _tmp1_;
        GskTransform* _tmp2_;
        GskTransform* _tmp3_;
        GskTransform* _tmp4_;
        g_return_if_fail (self != NULL);
        _tmp0_ = self->priv->_transform;
        _tmp1_ = _gsk_transform_ref0 (_tmp0_);
        transform = _tmp1_;
        _tmp2_ = transform;
        transform = NULL;
        _tmp3_ = gsk_transform_perspective (_tmp2_, x);
        _gsk_transform_unref0 (transform);
        transform = _tmp3_;
        _tmp4_ = transform;
        transform_bin_internal_set_transform (self, _tmp4_);
        _gsk_transform_unref0 (transform);
    }

    void
    transform_bin_internal_scale (TransformBinInternal* self,
    gfloat x_factor,
    gfloat y_factor)
    {
        GskTransform* transform = NULL;
        GskTransform* _tmp0_;
        GskTransform* _tmp1_;
        GskTransform* _tmp2_;
        GskTransform* _tmp3_;
        GskTransform* _tmp4_;
        g_return_if_fail (self != NULL);
        _tmp0_ = self->priv->_transform;
        _tmp1_ = _gsk_transform_ref0 (_tmp0_);
        transform = _tmp1_;
        _tmp2_ = transform;
        transform = NULL;
        _tmp3_ = gsk_transform_scale (_tmp2_, x_factor, y_factor);
        _gsk_transform_unref0 (transform);
        transform = _tmp3_;
        _tmp4_ = transform;
        transform_bin_internal_set_transform (self, _tmp4_);
        _gsk_transform_unref0 (transform);
    }

    void
    transform_bin_internal_skew (TransformBinInternal* self,
    gfloat x_skew,
    gfloat y_skew)
    {
        GskTransform* transform = NULL;
        GskTransform* _tmp0_;
        GskTransform* _tmp1_;
        GskTransform* _tmp2_;
        GskTransform* _tmp3_;
        GskTransform* _tmp4_;
        g_return_if_fail (self != NULL);
        _tmp0_ = self->priv->_transform;
        _tmp1_ = _gsk_transform_ref0 (_tmp0_);
        transform = _tmp1_;
        _tmp2_ = transform;
        transform = NULL;
        _tmp3_ = gsk_transform_skew (_tmp2_, x_skew, y_skew);
        _gsk_transform_unref0 (transform);
        transform = _tmp3_;
        _tmp4_ = transform;
        transform_bin_internal_set_transform (self, _tmp4_);
        _gsk_transform_unref0 (transform);
    }

    static void
    transform_bin_internal_real_measure (GtkWidget* base,
    GtkOrientation orientation,
    gint for_size,
    gint* minimum,
    gint* natural,
    gint* minimum_baseline,
    gint* natural_baseline)
    {
        TransformBinInternal * self;
        gint _vala_minimum = 0;
        gint _vala_natural = 0;
        gint _vala_minimum_baseline = 0;
        gint _vala_natural_baseline = 0;
        GtkWidget* _tmp0_;
        GtkWidget* _tmp1_;
        GtkWidget* _tmp2_;
        GtkWidget* _tmp3_;
        gint _tmp4_ = 0;
        gint _tmp5_ = 0;
        gint _tmp6_ = 0;
        gint _tmp7_ = 0;
        self = (TransformBinInternal*) base;
        _tmp0_ = adw_bin_get_child ((AdwBin*) self);
        _tmp1_ = _tmp0_;
        if (_tmp1_ == NULL) {
            _vala_minimum = 0;
            _vala_natural = 0;
            _vala_minimum_baseline = -1;
            _vala_natural_baseline = -1;
            if (minimum) {
                *minimum = _vala_minimum;
            }
            if (natural) {
                *natural = _vala_natural;
            }
            if (minimum_baseline) {
                *minimum_baseline = _vala_minimum_baseline;
            }
            if (natural_baseline) {
                *natural_baseline = _vala_natural_baseline;
            }
            return;
        }
        _tmp2_ = adw_bin_get_child ((AdwBin*) self);
        _tmp3_ = _tmp2_;
        gtk_widget_measure (_tmp3_, orientation, for_size, &_tmp4_, &_tmp5_, &_tmp6_, &_tmp7_);
        _vala_minimum = _tmp4_;
        _vala_natural = _tmp5_;
        _vala_minimum_baseline = _tmp6_;
        _vala_natural_baseline = _tmp7_;
        if (minimum) {
            *minimum = _vala_minimum;
        }
        if (natural) {
            *natural = _vala_natural;
        }
        if (minimum_baseline) {
            *minimum_baseline = _vala_minimum_baseline;
        }
        if (natural_baseline) {
            *natural_baseline = _vala_natural_baseline;
        }
    }

    static void
    transform_bin_internal_real_size_allocate (GtkWidget* base,
    gint width,
    gint height,
    gint baseline)
    {
        TransformBinInternal * self;
        GtkWidget* _tmp0_;
        GtkWidget* _tmp1_;
        GskTransform* t = NULL;
        GskTransform* _tmp2_;
        GskTransform* _tmp3_;
        graphene_point_t _tmp4_ = {0};
        GskTransform* _tmp5_;
        GskTransform* _tmp6_;
        GskTransform* _tmp10_;
        graphene_point_t _tmp11_ = {0};
        GskTransform* _tmp12_;
        GtkWidget* _tmp13_;
        GtkWidget* _tmp14_;
        GskTransform* _tmp15_;
        GskTransform* _tmp16_;
        self = (TransformBinInternal*) base;
        _tmp0_ = adw_bin_get_child ((AdwBin*) self);
        _tmp1_ = _tmp0_;
        if (_tmp1_ == NULL) {
            return;
        }
        _tmp2_ = gsk_transform_new ();
        t = _tmp2_;
        _tmp3_ = t;
        t = NULL;
        _tmp4_.x = (gfloat) (width / 2);
        _tmp4_.y = (gfloat) (height / 2);
        _tmp5_ = gsk_transform_translate (_tmp3_, &_tmp4_);
        _gsk_transform_unref0 (t);
        t = _tmp5_;
        _tmp6_ = self->priv->_transform;
        if (_tmp6_ != NULL) {
            GskTransform* _tmp7_;
            GskTransform* _tmp8_;
            GskTransform* _tmp9_;
            _tmp7_ = t;
            t = NULL;
            _tmp8_ = self->priv->_transform;
            _tmp9_ = gsk_transform_transform (_tmp7_, _tmp8_);
            _gsk_transform_unref0 (t);
            t = _tmp9_;
        }
        _tmp10_ = t;
        t = NULL;
        _tmp11_.x = (gfloat) ((-width) / 2);
        _tmp11_.y = (gfloat) ((-height) / 2);
        _tmp12_ = gsk_transform_translate (_tmp10_, &_tmp11_);
        _gsk_transform_unref0 (t);
        t = _tmp12_;
        _tmp13_ = adw_bin_get_child ((AdwBin*) self);
        _tmp14_ = _tmp13_;
        _tmp15_ = t;
        _tmp16_ = _gsk_transform_ref0 (_tmp15_);
        gtk_widget_allocate (_tmp14_, width, height, baseline, _tmp16_);
        _gsk_transform_unref0 (t);
    }

    TransformBinInternal*
    transform_bin_internal_construct (GType object_type)
    {
        TransformBinInternal * self = NULL;
        self = (TransformBinInternal*) g_object_new (object_type, NULL);
        return self;
    }

    TransformBinInternal*
    transform_bin_internal_new (void)
    {
        return transform_bin_internal_construct (TYPE_TRANSFORM_BIN_INTERNAL);
    }

    GskTransform*
    transform_bin_internal_get_transform (TransformBinInternal* self)
    {
        GskTransform* result;
        GskTransform* _tmp0_;
        g_return_val_if_fail (self != NULL, NULL);
        _tmp0_ = self->priv->_transform;
        result = _tmp0_;
        return result;
    }

    void
    transform_bin_internal_set_transform (TransformBinInternal* self,
    GskTransform* value)
    {
        GskTransform* old_value;
        g_return_if_fail (self != NULL);
        old_value = transform_bin_internal_get_transform (self);
        if (old_value != value) {
            GskTransform* _tmp0_;
            _tmp0_ = _gsk_transform_ref0 (value);
            _gsk_transform_unref0 (self->priv->_transform);
            self->priv->_transform = _tmp0_;
            g_object_notify_by_pspec ((GObject *) self, transform_bin_internal_properties[TRANSFORM_BIN_INTERNAL_TRANSFORM_PROPERTY]);
        }
    }

    static void
    _gtk_widget_queue_allocate_g_object_notify (GObject* _sender,
    GParamSpec* pspec,
    gpointer self)
    {
        gtk_widget_queue_allocate ((GtkWidget*) self);
    }

    static GObject *
    transform_bin_internal_constructor (GType type,
    guint n_construct_properties,
    GObjectConstructParam * construct_properties)
    {
        GObject * obj;
        GObjectClass * parent_class;
        TransformBinInternal * self;
        GskTransform* _tmp0_;
        GskTransform* _tmp1_;
        parent_class = G_OBJECT_CLASS (transform_bin_internal_parent_class);
        obj = parent_class->constructor (type, n_construct_properties, construct_properties);
        self = G_TYPE_CHECK_INSTANCE_CAST (obj, TYPE_TRANSFORM_BIN_INTERNAL, TransformBinInternal);
        gtk_widget_set_layout_manager ((GtkWidget*) self, NULL);
        _tmp0_ = gsk_transform_new ();
        _tmp1_ = _tmp0_;
        transform_bin_internal_set_transform (self, _tmp1_);
        _gsk_transform_unref0 (_tmp1_);
        g_signal_connect_object ((GObject*) self, "notify::transform", (GCallback) _gtk_widget_queue_allocate_g_object_notify, (GtkWidget*) self, (GConnectFlags) 0);
        return obj;
    }

    static void
    transform_bin_internal_class_init (TransformBinInternalClass * klass,
    gpointer klass_data)
    {
        transform_bin_internal_parent_class = g_type_class_peek_parent (klass);
        g_type_class_adjust_private_offset (klass, &TransformBinInternal_private_offset);
        ((GtkWidgetClass *) klass)->measure = (void (*) (GtkWidget*, GtkOrientation, gint, gint*, gint*, gint*, gint*)) transform_bin_internal_real_measure;
        ((GtkWidgetClass *) klass)->size_allocate = (void (*) (GtkWidget*, gint, gint, gint)) transform_bin_internal_real_size_allocate;
        G_OBJECT_CLASS (klass)->get_property = _vala_transform_bin_internal_get_property;
        G_OBJECT_CLASS (klass)->set_property = _vala_transform_bin_internal_set_property;
        G_OBJECT_CLASS (klass)->constructor = transform_bin_internal_constructor;
        G_OBJECT_CLASS (klass)->finalize = transform_bin_internal_finalize;
        g_object_class_install_property (G_OBJECT_CLASS (klass), TRANSFORM_BIN_INTERNAL_TRANSFORM_PROPERTY, transform_bin_internal_properties[TRANSFORM_BIN_INTERNAL_TRANSFORM_PROPERTY] = g_param_spec_boxed ("transform", "transform", "transform", gsk_transform_get_type (), (GParamFlags) (G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE)));
        gtk_widget_class_set_css_name (GTK_WIDGET_CLASS (klass), "transform-bin-internal");
    }

    static void
    transform_bin_internal_instance_init (TransformBinInternal * self,
    gpointer klass)
    {
        self->priv = (TransformBinInternalPrivate*) transform_bin_internal_get_instance_private (self);
    }

    static void
    transform_bin_internal_finalize (GObject * obj)
    {
        TransformBinInternal * self;
        self = G_TYPE_CHECK_INSTANCE_CAST (obj, TYPE_TRANSFORM_BIN_INTERNAL, TransformBinInternal);
        _gsk_transform_unref0 (self->priv->_transform);
        G_OBJECT_CLASS (transform_bin_internal_parent_class)->finalize (obj);
    }

    static GType
    transform_bin_internal_get_type_once (void)
    {
        static const GTypeInfo g_define_type_info = { sizeof (TransformBinInternalClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) transform_bin_internal_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (TransformBinInternal), 0, (GInstanceInitFunc) transform_bin_internal_instance_init, NULL };
        GType transform_bin_internal_type_id;
        transform_bin_internal_type_id = g_type_register_static (adw_bin_get_type (), "TransformBinInternal", &g_define_type_info, (GTypeFlags) 0);
        TransformBinInternal_private_offset = g_type_add_instance_private (transform_bin_internal_type_id, sizeof (TransformBinInternalPrivate));
        return transform_bin_internal_type_id;
    }

    GType
    transform_bin_internal_get_type (void)
    {
        static gsize transform_bin_internal_type_id__once = 0;
        if (g_once_init_enter (&transform_bin_internal_type_id__once)) {
            GType transform_bin_internal_type_id;
            transform_bin_internal_type_id = transform_bin_internal_get_type_once ();
            g_once_init_leave (&transform_bin_internal_type_id__once, transform_bin_internal_type_id);
        }
        return transform_bin_internal_type_id__once;
    }

    static void
    _vala_transform_bin_internal_get_property (GObject * object,
    guint property_id,
    GValue * value,
    GParamSpec * pspec)
    {
        TransformBinInternal * self;
        self = G_TYPE_CHECK_INSTANCE_CAST (object, TYPE_TRANSFORM_BIN_INTERNAL, TransformBinInternal);
        switch (property_id) {
            case TRANSFORM_BIN_INTERNAL_TRANSFORM_PROPERTY:
                g_value_set_boxed (value, transform_bin_internal_get_transform (self));
                break;
            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
    }

    static void
    _vala_transform_bin_internal_set_property (GObject * object,
    guint property_id,
    const GValue * value,
    GParamSpec * pspec)
    {
        TransformBinInternal * self;
        self = G_TYPE_CHECK_INSTANCE_CAST (object, TYPE_TRANSFORM_BIN_INTERNAL, TransformBinInternal);
        switch (property_id) {
            case TRANSFORM_BIN_INTERNAL_TRANSFORM_PROPERTY:
                transform_bin_internal_set_transform (self, (GskTransform*) g_value_get_boxed (value));
                break;
            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
    }
}

namespace mousetrap
{
    TransformBin::TransformBin()
        : Widget(GTK_WIDGET(detail::transform_bin_internal_new())),
          CTOR_SIGNAL(TransformBin, realize),
          CTOR_SIGNAL(TransformBin, unrealize),
          CTOR_SIGNAL(TransformBin, destroy),
          CTOR_SIGNAL(TransformBin, hide),
          CTOR_SIGNAL(TransformBin, show),
          CTOR_SIGNAL(TransformBin, map),
          CTOR_SIGNAL(TransformBin, unmap)
    {
        using namespace detail;
        _internal = TRANSFORM_BIN_INTERNAL(operator NativeWidget());
        g_object_ref(_internal);
    }
    
    TransformBin::TransformBin(detail::TransformBinInternal* internal)
        : Widget(GTK_WIDGET(internal)),
          CTOR_SIGNAL(TransformBin, realize),
          CTOR_SIGNAL(TransformBin, unrealize),
          CTOR_SIGNAL(TransformBin, destroy),
          CTOR_SIGNAL(TransformBin, hide),
          CTOR_SIGNAL(TransformBin, show),
          CTOR_SIGNAL(TransformBin, map),
          CTOR_SIGNAL(TransformBin, unmap)
    {
        _internal = g_object_ref(_internal);
    }

    TransformBin::~TransformBin()
    {
        g_object_unref(_internal);
    }

    NativeObject TransformBin::get_internal() const
    {
        return G_OBJECT(_internal);
    }

    void TransformBin::set_child(const Widget& child)
    {
        adw_bin_set_child(ADW_BIN(_internal), child.operator NativeWidget());
    }

    void TransformBin::remove_child()
    {
        adw_bin_set_child(ADW_BIN(_internal), nullptr);
    }

    void TransformBin::reset()
    {
        detail::transform_bin_internal_reset(_internal);
    }

    void TransformBin::rotate(Angle angle)
    {
        detail::transform_bin_internal_rotate(_internal, angle.as_degrees());
    }

    void TransformBin::translate(Vector2f offset)
    {
        detail::transform_bin_internal_translate(_internal, offset.x, offset.y);
    }

    void TransformBin::scale(float x, float y)
    {
        detail::transform_bin_internal_scale(_internal, x, y);
    }

    void TransformBin::skew(float x_skew, float y_skew)
    {
        detail::transform_bin_internal_skew(_internal, x_skew, y_skew);
    }
}
