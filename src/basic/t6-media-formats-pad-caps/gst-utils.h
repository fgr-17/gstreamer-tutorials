#include <string>

namespace gst_utils {

gboolean print_field (GQuark field, const GValue * value, gpointer pfx);

void print_caps (const GstCaps * caps, const gchar * pfx);

void print_pad_templates_information (GstElementFactory * factory);

void print_pad_capabilities (GstElement *element, std::string pad_name);

};
