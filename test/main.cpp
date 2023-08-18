#include <mousetrap.hpp>

using namespace mousetrap;

#if !defined(VALA_EXTERN)
#if defined(_MSC_VER)
#define VALA_EXTERN __declspec(dllexport) extern
#elif __GNUC__ >= 4
#define VALA_EXTERN __attribute__((visibility("default"))) extern
#else
#define VALA_EXTERN extern
#endif
#endif

#define TYPE_TRANSFORM_BIN (transform_bin_get_type ())
#define TRANSFORM_BIN(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TRANSFORM_BIN, TransformBin))
#define TRANSFORM_BIN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TRANSFORM_BIN, TransformBinClass))
#define IS_TRANSFORM_BIN(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TRANSFORM_BIN))
#define IS_TRANSFORM_BIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TRANSFORM_BIN))
#define TRANSFORM_BIN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TRANSFORM_BIN, TransformBinClass))

typedef struct _TransformBin TransformBin;
typedef struct _TransformBinClass TransformBinClass;
typedef struct _TransformBinPrivate TransformBinPrivate;
enum  {
    TRANSFORM_BIN_0_PROPERTY,
    TRANSFORM_BIN_TRANSFORM_PROPERTY,
    TRANSFORM_BIN_NUM_PROPERTIES
};
static GParamSpec* transform_bin_properties[TRANSFORM_BIN_NUM_PROPERTIES];
#define _gsk_transform_unref0(var) ((var == NULL) ? NULL : (var = (GskTransform*) (gsk_transform_unref(var), NULL)))

struct _TransformBin {
    AdwBin parent_instance;
    TransformBinPrivate * priv;
};

struct _TransformBinClass {
    AdwBinClass parent_class;
};

struct _TransformBinPrivate {
    GskTransform* _transform;
};

static gint TransformBin_private_offset;
static gpointer transform_bin_parent_class = NULL;

VALA_EXTERN GType transform_bin_get_type (void) G_GNUC_CONST ;
G_DEFINE_AUTOPTR_CLEANUP_FUNC (TransformBin, g_object_unref)
static void transform_bin_real_measure (GtkWidget* base,
GtkOrientation orientation,
gint for_size,
gint* minimum,
gint* natural,
gint* minimum_baseline,
gint* natural_baseline);
static void transform_bin_real_size_allocate (GtkWidget* base,
gint width,
gint height,
gint baseline);
VALA_EXTERN GskTransform* transform_bin_get_transform (TransformBin* self);
VALA_EXTERN TransformBin* transform_bin_new (void);
VALA_EXTERN TransformBin* transform_bin_construct (GType object_type);
VALA_EXTERN void transform_bin_set_transform (TransformBin* self,
GskTransform* value);
static GObject * transform_bin_constructor (GType type,
guint n_construct_properties,
GObjectConstructParam * construct_properties);
static void _gtk_widget_queue_allocate_g_object_notify (GObject* _sender,
GParamSpec* pspec,
gpointer self);
static void transform_bin_finalize (GObject * obj);
static GType transform_bin_get_type_once (void);
static void _vala_transform_bin_get_property (GObject * object,
guint property_id,
GValue * value,
GParamSpec * pspec);
static void _vala_transform_bin_set_property (GObject * object,
guint property_id,
const GValue * value,
GParamSpec * pspec);

static inline gpointer
transform_bin_get_instance_private (TransformBin* self)
{
    return G_STRUCT_MEMBER_P (self, TransformBin_private_offset);
}

static void
transform_bin_real_measure (GtkWidget* base,
GtkOrientation orientation,
gint for_size,
gint* minimum,
gint* natural,
gint* minimum_baseline,
gint* natural_baseline)
{
    TransformBin * self;
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
    self = (TransformBin*) base;
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

static gpointer
_gsk_transform_ref0 (gpointer self)
{
    return self ? gsk_transform_ref((GskTransform*) self) : NULL;
}

static void
transform_bin_real_size_allocate (GtkWidget* base,
gint width,
gint height,
gint baseline)
{
    TransformBin * self;
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
    self = (TransformBin*) base;
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
    _tmp16_ = (GskTransform*) _gsk_transform_ref0 (_tmp15_);
    gtk_widget_allocate (_tmp14_, width, height, baseline, _tmp16_);
    _gsk_transform_unref0 (t);
}

TransformBin*
transform_bin_construct (GType object_type)
{
    TransformBin * self = NULL;
    self = (TransformBin*) g_object_new (object_type, NULL);
    return self;
}

TransformBin*
transform_bin_new (void)
{
    return transform_bin_construct (TYPE_TRANSFORM_BIN);
}

GskTransform*
transform_bin_get_transform (TransformBin* self)
{
    GskTransform* result;
    GskTransform* _tmp0_;
    g_return_val_if_fail (self != NULL, NULL);
    _tmp0_ = self->priv->_transform;
    result = _tmp0_;
    return result;
}

void
transform_bin_set_transform (TransformBin* self,
GskTransform* value)
{
    GskTransform* old_value;
    g_return_if_fail (self != NULL);
    old_value = transform_bin_get_transform (self);
    if (old_value != value) {
        GskTransform* _tmp0_;
        _tmp0_ = (GskTransform*) _gsk_transform_ref0 (value);
        _gsk_transform_unref0 (self->priv->_transform);
        self->priv->_transform = _tmp0_;
        g_object_notify_by_pspec ((GObject *) self, transform_bin_properties[TRANSFORM_BIN_TRANSFORM_PROPERTY]);
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
transform_bin_constructor (GType type,
guint n_construct_properties,
GObjectConstructParam * construct_properties)
{
    GObject * obj;
    GObjectClass * parent_class;
    TransformBin * self;
    GskTransform* _tmp0_;
    GskTransform* _tmp1_;
    parent_class = G_OBJECT_CLASS (transform_bin_parent_class);
    obj = parent_class->constructor (type, n_construct_properties, construct_properties);
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, TYPE_TRANSFORM_BIN, TransformBin);
    gtk_widget_set_layout_manager ((GtkWidget*) self, NULL);
    _tmp0_ = gsk_transform_new ();
    _tmp1_ = _tmp0_;
    transform_bin_set_transform (self, _tmp1_);
    _gsk_transform_unref0 (_tmp1_);
    g_signal_connect_object ((GObject*) self, "notify::transform", G_CALLBACK(_gtk_widget_queue_allocate_g_object_notify), (GtkWidget*) self, (GConnectFlags) 0);
    return obj;
}

static void
transform_bin_class_init (TransformBinClass * klass,
gpointer klass_data)
{
    transform_bin_parent_class = g_type_class_peek_parent (klass);
    g_type_class_adjust_private_offset (klass, &TransformBin_private_offset);
    ((GtkWidgetClass *) klass)-> measure = (void (*) (GtkWidget*, GtkOrientation, gint, gint*, gint*, gint*, gint*)) transform_bin_real_measure;
    ((GtkWidgetClass *) klass)->size_allocate = (void (*) (GtkWidget*, gint, gint, gint)) transform_bin_real_size_allocate;
    G_OBJECT_CLASS (klass)->get_property = _vala_transform_bin_get_property;
    G_OBJECT_CLASS (klass)->set_property = _vala_transform_bin_set_property;
    G_OBJECT_CLASS (klass)->constructor = transform_bin_constructor;
    G_OBJECT_CLASS (klass)->finalize = transform_bin_finalize;
    g_object_class_install_property (G_OBJECT_CLASS (klass), TRANSFORM_BIN_TRANSFORM_PROPERTY, transform_bin_properties[TRANSFORM_BIN_TRANSFORM_PROPERTY] = g_param_spec_boxed ("transform", "transform", "transform", gsk_transform_get_type (), (GParamFlags) (G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE)));
    gtk_widget_class_set_css_name (GTK_WIDGET_CLASS (klass), "transform-bin");
}

static void
transform_bin_instance_init (TransformBin * self,
gpointer klass)
{
    self->priv = (TransformBinPrivate*) transform_bin_get_instance_private (self);
}

static void
transform_bin_finalize (GObject * obj)
{
    TransformBin * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (obj, TYPE_TRANSFORM_BIN, TransformBin);
    _gsk_transform_unref0 (self->priv->_transform);
    G_OBJECT_CLASS (transform_bin_parent_class)->finalize (obj);
}

static GType
transform_bin_get_type_once (void)
{
    static const GTypeInfo g_define_type_info = { sizeof (TransformBinClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) transform_bin_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (TransformBin), 0, (GInstanceInitFunc) transform_bin_instance_init, NULL };
    GType transform_bin_type_id;
    transform_bin_type_id = g_type_register_static (adw_bin_get_type (), "TransformBin", &g_define_type_info, (GTypeFlags) 0);
    TransformBin_private_offset = g_type_add_instance_private (transform_bin_type_id, sizeof (TransformBinPrivate));
    return transform_bin_type_id;
}

GType
transform_bin_get_type (void)
{
    static gsize transform_bin_type_id__once = 0;
    if (g_once_init_enter (&transform_bin_type_id__once)) {
        GType transform_bin_type_id;
        transform_bin_type_id = transform_bin_get_type_once ();
        g_once_init_leave (&transform_bin_type_id__once, transform_bin_type_id);
    }
    return transform_bin_type_id__once;
}

static void
_vala_transform_bin_get_property (GObject * object,
guint property_id,
GValue * value,
GParamSpec * pspec)
{
    TransformBin * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (object, TYPE_TRANSFORM_BIN, TransformBin);
    switch (property_id) {
        case TRANSFORM_BIN_TRANSFORM_PROPERTY:
            g_value_set_boxed (value, transform_bin_get_transform (self));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void
_vala_transform_bin_set_property (GObject * object,
guint property_id,
const GValue * value,
GParamSpec * pspec)
{
    TransformBin * self;
    self = G_TYPE_CHECK_INSTANCE_CAST (object, TYPE_TRANSFORM_BIN, TransformBin);
    switch (property_id) {
        case TRANSFORM_BIN_TRANSFORM_PROPERTY:
            transform_bin_set_transform (self, (GskTransform*) g_value_get_boxed (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

void on_animation_callback(double x, void* data)
{
    auto* bin = TRANSFORM_BIN(data);
    static auto* transform = gsk_transform_new();
    gsk_transform_ref(transform);

    float r = adw_lerp(0, 360, x);
    transform_bin_set_transform(bin, gsk_transform_rotate(transform, r));
}

void on_animation(double value, void* data)
{
    std::cout << (double) value << " " << data << std::endl;
}

void on_done()
{
    //std::cout << "done" << std::endl;
}

int main()
{
    auto app = Application("test.app");
    app.connect_signal_activate([](Application& app)
    {
        auto window = Window(app);
        auto child = Label("TEST");

        app.register_named_color("newgenta", RGBA(1, 0, 1, 1));

        auto style_class = StyleClass("test_class");
        style_class.set_property(STYLE_PROPERTY_COLOR, "@newgenta");

        app.add_style_class(style_class);
        child.add_css_class(style_class.get_name());

        static auto* target = adw_callback_animation_target_new(on_animation, nullptr, (GDestroyNotify) nullptr);
        static auto* animation = adw_timed_animation_new(
            child.operator NativeWidget(),
            0, 100, 10000,
            ADW_ANIMATION_TARGET(target)
        );
        adw_animation_set_follow_enable_animations_setting(ADW_ANIMATION(animation), false);
        g_signal_connect(animation, "done", G_CALLBACK(on_done), nullptr);

        auto* settings = gtk_settings_get_default();

        auto* value = new GValue();
        g_value_init(value, G_TYPE_STRING);
        g_object_get_property(G_OBJECT(settings), "gtk-enable-animations", value);

        std::cout << g_value_get_string(value) << std::endl;

        auto button = Button();
        button.connect_signal_clicked([](Button&){
            //adw_animation_reset(ADW_ANIMATION(animation));

            std::cout << adw_animation_get_value(animation) << std::endl;
            adw_animation_play(ADW_ANIMATION(animation));
            std::cout << adw_animation_get_value(animation) << std::endl;
        });

        auto box = Box(Orientation::HORIZONTAL);
        box.push_back(child);
        box.push_back(button);

        window.set_child(box);
        window.present();

    });
    return app.run();
}