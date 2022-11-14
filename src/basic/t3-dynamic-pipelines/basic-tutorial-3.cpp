#include <iostream>
#include <gst/gst.h>

#include "gst-utils.h"

void print_all_structs_from_caps (GstCaps * caps);

/* Structure to contain all our information, so we can pass it to callbacks */
typedef struct _CustomData {
  GstElement *pipeline;
  GstElement *source;
  GstElement *convert;
  GstElement *resample;
  GstElement *sink;
  
  GstElement *videoconvert;
  GstElement *videosink;
} CustomData;

/* Handler for the pad-added signal */
static void pad_added_handler (GstElement *src, GstPad *pad, CustomData *data);

int main(int argc, char *argv[]) {
  CustomData data;
  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;
  gboolean terminate = FALSE;

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Create the elements */
  data.source = gst_element_factory_make ("uridecodebin", "source");
  data.convert = gst_element_factory_make ("audioconvert", "convert");
  data.resample = gst_element_factory_make ("audioresample", "resample");
  data.sink = gst_element_factory_make ("autoaudiosink", "sink");

  // data.videoconvert = gst_element_factory_make ("autovideoconvert", "videoconvert");
  // data.videosink = gst_element_factory_make ("autovideosink", "videosink");

  auto videoconvert_factory =  gst_element_factory_find ("autovideoconvert");
  auto videosink_factory =  gst_element_factory_find ("autovideosink");
  g_print("================ autovideoconvert pad info: ================\n");
  gst_utils::print_pad_templates_information(videoconvert_factory);
  g_print("================ autovideosink pad info: ================\n");
  gst_utils::print_pad_templates_information(videosink_factory);

  data.videoconvert = gst_element_factory_create (videoconvert_factory, "vconvert");
  data.videosink = gst_element_factory_create(videosink_factory, "vsink");




  /* Create the empty pipeline */
  data.pipeline = gst_pipeline_new ("test-pipeline");

  if (!data.pipeline || !data.source || !data.convert || !data.resample || !data.sink || !data.videoconvert || !data.videosink) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }

  /* Build the pipeline. Note that we are NOT linking the source at this
   * point. We will do it later. */
  gst_bin_add_many (GST_BIN (data.pipeline), data.source, data.convert, data.resample, data.sink, data.videoconvert, data.videosink, NULL);
  
  /* linking audio elements */
  if (!gst_element_link_many (data.convert, data.resample, data.sink, NULL)) {
    g_printerr ("Audio elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  /* linking video elements */
  if (!gst_element_link (data.videoconvert, data.videosink)) {
    g_printerr ("Video elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  /* Set the URI to play */
  g_object_set (data.source, "uri", "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", NULL);

  /* Connect to the pad-added signal */
  g_signal_connect (data.source, "pad-added", G_CALLBACK (pad_added_handler), &data);

  /* Start playing */
  ret = gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }

  /* Listen to the bus */
  bus = gst_element_get_bus (data.pipeline);
  do {
    msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
        static_cast<GstMessageType>(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* Parse message */
    if (msg != NULL) {
      GError *err;
      gchar *debug_info;

      switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
          gst_message_parse_error (msg, &err, &debug_info);
          g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
          g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
          g_clear_error (&err);
          g_free (debug_info);
          terminate = TRUE;
          break;
        case GST_MESSAGE_EOS:
          g_print ("End-Of-Stream reached.\n");
          terminate = TRUE;
          break;
        case GST_MESSAGE_STATE_CHANGED:
          /* We are only interested in state-changed messages from the pipeline */
          if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data.pipeline)) {
            GstState old_state, new_state, pending_state;
            gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
            g_print ("Pipeline state changed from %s to %s:\n",
                gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
          }
          break;
        default:
          /* We should not reach here */
          g_printerr ("Unexpected message received.\n");
          break;
      }
      gst_message_unref (msg);
    }
  } while (!terminate);

  /* Free resources */
  gst_object_unref (bus);
  gst_element_set_state (data.pipeline, GST_STATE_NULL);
  gst_object_unref (data.pipeline);
  return 0;
}

/* This function will be called by the pad-added signal */
static void pad_added_handler (GstElement *src, GstPad *new_pad, CustomData *data) {
  GstPad *sink_pad = NULL;
  GstPad *audiosink_pad = gst_element_get_static_pad (data->convert, "sink");
  GstPad *videosink_pad = gst_element_get_static_pad (data->videoconvert, "sink");

  GstPadLinkReturn ret;
  GstCaps *new_pad_caps = NULL;
  GstStructure *new_pad_struct = NULL;
  const gchar *new_pad_type = NULL;

  std::cout << "received pad is pad?: " << GST_IS_PAD(new_pad) << std::endl;
  std::cout << "audiosink pad is pad?: " << GST_IS_PAD(audiosink_pad) << std::endl;
  std::cout << "videosink pad is pad?: " << GST_IS_PAD(videosink_pad) << std::endl;

  g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src));

  /* If our converter is already linked, we have nothing to do here */
  if (gst_pad_is_linked (audiosink_pad) && gst_pad_is_linked (videosink_pad)) {
    g_print ("We are already linked. Ignoring.\n");
    /* Unreference the new pad's caps, if we got them */
    if (new_pad_caps != NULL) gst_caps_unref (new_pad_caps);
    if (videosink_pad != NULL) gst_object_unref (videosink_pad);
    if (audiosink_pad != NULL) gst_object_unref (audiosink_pad);
    return;
  }

  /* Check the new pad's type */
  new_pad_caps = gst_pad_get_current_caps (new_pad);
  new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
  new_pad_type = gst_structure_get_name (new_pad_struct);
  if (g_str_has_prefix (new_pad_type, "audio/x-raw")) {
    sink_pad = audiosink_pad;
  }
  else if (g_str_has_prefix (new_pad_type, "video/x-raw")) {
    sink_pad = videosink_pad;
  }
  else {
    g_print ("It has type '%s' which is not raw audio. Ignoring.\n", new_pad_type);
    if (new_pad_caps != NULL) gst_caps_unref (new_pad_caps);
    if (videosink_pad != NULL) gst_object_unref (videosink_pad);
    if (audiosink_pad != NULL) gst_object_unref (audiosink_pad);
    return;
  }

  /* Attempt the link */
  ret = gst_pad_link (new_pad, sink_pad);
  if (GST_PAD_LINK_FAILED (ret)) {
    g_print ("Type is '%s' but link failed.\n", new_pad_type);
  } else {
    g_print ("Link succeeded (type '%s').\n", new_pad_type);
  }

  if (new_pad_caps != NULL) gst_caps_unref (new_pad_caps);
  if (videosink_pad != NULL) gst_object_unref (videosink_pad);
  if (audiosink_pad != NULL) gst_object_unref (audiosink_pad);
}