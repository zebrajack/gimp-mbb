
#include <stdio.h>

#include <libgimp/gimp.h>

#include "mbb/mbb.h"

static void blend(gint32 img0_layer_id, gint32 img1_layer_id,
                  gint32 mask_layer_id, gint32 img_id);
static void query(void);
static void run(const gchar      *name,
                gint              nparams,
                const GimpParam  *param,
                gint             *nreturn_vals,
                GimpParam       **return_vals);

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,
  NULL,
  query,
  run
};

MAIN()

static void query(void) {
  static GimpParamDef args[] =
  {
    {
      GIMP_PDB_INT32,
      "run-mode",
      "Run mode"
    },
    {
      GIMP_PDB_IMAGE,
      "image",
      "Input image"
    },
    {
      GIMP_PDB_DRAWABLE,
      "drawable",
      "Input drawable"
    }
  };

  gimp_install_procedure(
    "plug-in-mbb",
    "Implements a multi-band blending technique",
    "Implements a multi-band blending technique...<write more>",
    "Saulo A. Pessoa <saulopessoa@gmail.com>",
    "Saulo A. Pessoa",
    "2013",
    "Multi-Band Blending...",
    "RGB*, GRAY*",
    GIMP_PLUGIN,
    G_N_ELEMENTS(args),
    0,
    args,
    NULL);

  gimp_plugin_menu_register("plug-in-mbb", "<Image>/Filters/Misc");
}

FILE *p_file;

static void run(const gchar      *name,
                gint              nparams,
                const GimpParam  *param,
                gint             *nreturn_vals,
                GimpParam       **return_vals) {
  p_file = fopen("gimp-mbb_log.txt","w");
  fprintf(p_file, "Test\n");

  static GimpParam values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  GimpRunMode run_mode;
  GimpDrawable *drawable;
  gint num_layers;
  gint *layers_ids;

  /* Setting mandatory output values */
  *nreturn_vals = 1;
  *return_vals = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  /* Getting run_mode - we won't display a dialog if 
   * we are in NONINTERACTIVE mode */
  run_mode = param[0].data.d_int32;

  /*  Get the specified drawable  */
  drawable = gimp_drawable_get(param[2].data.d_drawable);

  if (run_mode != GIMP_RUN_NONINTERACTIVE) {
    // g_message("Hello, world!\n");
    // TODO: Show gui to the user.
  }

  /* Let's time blur
   *mset
   *   GTimer timer = g_timer_new time ();
   */

  //blur (drawable);

  /*   g_print ("blur() took %g seconds.\n", g_timer_elapsed (timer));
   *   g_timer_destroy (timer);
   */

  layers_ids = gimp_image_get_layers(param[1].data.d_image, &num_layers);

  gimp_progress_init("Blending...");
  gimp_progress_update(0.0);

  blend(layers_ids[0], layers_ids[1], layers_ids[2], param[1].data.d_image);

  gimp_progress_update(1.0);

  gimp_displays_flush();
//  gimp_drawable_detach(drawable);

  fprintf(p_file, "num_layers = %d\n", num_layers);
  fclose(p_file);
}

static void blend(gint32 img0_layer_id, gint32 img1_layer_id,
                  gint32 mask_layer_id, gint32 img_id) {

  gint         width, height, channels, temp_channels;
  gint         x0, y0, x1, y1,
               temp_x0, temp_y0, temp_x1, temp_y1;
  GimpPixelRgn rgn_img0, rgn_img1, rgn_mask, rgn_out;

  /* Gets upper left and lower right coordinates. */
  gimp_drawable_mask_bounds(img0_layer_id, &x0, &y0, &x1, &y1);
  channels = gimp_drawable_bpp(img0_layer_id);

  gimp_drawable_mask_bounds(img1_layer_id, &temp_x0, &temp_y0, &temp_x1, &temp_y1);
  temp_channels = gimp_drawable_bpp(img1_layer_id);

  gimp_drawable_mask_bounds(mask_layer_id, &temp_x0, &temp_y0, &temp_x1, &temp_y1);
  temp_channels = gimp_drawable_bpp(mask_layer_id);

  // TODO: Send an error if img0 attributes are different from img1 and mask.

  width = x1 - x0;
  height = y1 - y0;

  gimp_pixel_rgn_init(&rgn_img0, gimp_drawable_get(img0_layer_id), x0, y0, width, height, FALSE, FALSE);
  gimp_pixel_rgn_init(&rgn_img1, gimp_drawable_get(img1_layer_id), x0, y0, width, height, FALSE, FALSE);
  gimp_pixel_rgn_init(&rgn_mask, gimp_drawable_get(mask_layer_id), x0, y0, width, height, FALSE, FALSE);


  //gint32 out_layer_id = gimp_layer_new_from_drawable(img0_layer_id, img_id);
  //gint32 out_layer_id = gimp_layer_new_from_visible(img_id, img_id, "Blended");
  gint32 out_layer_id = gimp_layer_new(img_id, "Blended", width, height, GIMP_RGB_IMAGE, 100.0, GIMP_NORMAL_MODE);
  gimp_image_add_layer(img_id, out_layer_id, 0);
  GimpDrawable *out_layer_drawable = gimp_drawable_get(out_layer_id);
  gimp_pixel_rgn_init(&rgn_out, out_layer_drawable, x0, y0, width, height, TRUE, TRUE);


  guchar *img0 = g_new(guchar, channels * width * height);
  guchar *img1 = g_new(guchar, channels * width * height);
  guchar *mask = g_new(guchar, channels * width * height);
  guchar *out = g_new(guchar, channels * width * height);

  gimp_pixel_rgn_get_rect(&rgn_img0, img0, x0, y0, width, height);
  gimp_pixel_rgn_get_rect(&rgn_img1, img1, x0, y0, width, height);
  gimp_pixel_rgn_get_rect(&rgn_mask, mask, x0, y0, width, height);

  mbb_blend(width, height, channels, kUint8, img0, img1, mask, out);

//  memset(out, 128, channels * width * height);

  gimp_pixel_rgn_set_rect(&rgn_out, out, x0, y0, width, height);

  g_free(img0);
  g_free(img1);
  g_free(mask);
  g_free(out);

  gimp_drawable_flush(out_layer_drawable);
  gimp_drawable_merge_shadow(out_layer_id, TRUE);
  gimp_drawable_update(out_layer_id, x0, y0, width, height);

  fprintf(p_file, "x0 = %d, x1 = %d, y0 = %d, y1 = %d\n", x0, x1, y0, y1);
  fprintf(p_file, "w = %d, h = %d, c = %d\n", width, height, channels);
}
