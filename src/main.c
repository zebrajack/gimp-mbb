
#include <stdio.h>

#include <libgimp/gimp.h>

#include "mbb/mbb.h"

static void blend(gint drawable_ID, gint image_ID);
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

  /* Setting mandatory output values */
  *nreturn_vals = 1;
  *return_vals = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  /* Getting run_mode - we won't display a dialog if 
   * we are in NONINTERACTIVE mode */
  run_mode = param[0].data.d_int32;

  /*  Get the specified drawable  */
//  drawable = gimp_drawable_get(param[2].data.d_drawable);

  //if (run_mode != GIMP_RUN_NONINTERACTIVE)
  //  g_message("Hello, world!\n");

  /* Let's time blur
   *mset
   *   GTimer timer = g_timer_new time ();
   */

  //blur (drawable);

  /*   g_print ("blur() took %g seconds.\n", g_timer_elapsed (timer));
   *   g_timer_destroy (timer);
   */

//  gint num_layers = 0;
//  gint *layers = gimp_image_get_layers(param[1].data.d_image, &num_layers);
//  fprintf(p_file, "num_l = %d\n", num_layers);
//  int i = 0;
//  for (i = 0; i < num_layers; ++i)
//    fprintf(p_file, "l %d = %d\n", i, layers[i]);

//  fprintf(p_file, "drawable = %d\n", param[2].data.d_drawable);

  gimp_progress_init("Blending...");
  gimp_progress_update(0.0);

  blend(param[2].data.d_drawable, param[1].data.d_image);

  gimp_progress_update(1.0);

  gimp_displays_flush();
  gimp_drawable_detach(drawable);

  fclose(p_file);
}

static void blend(gint drawable_ID, gint image_ID) {
  gint         width, height, channels;
  gint         x1, y1, x2, y2;
  GimpPixelRgn rgn_in, rgn_in0, rgn_in1, rgn_inmask, rgn_out;

  GimpDrawable *drawable = gimp_drawable_get(drawable_ID);


  /* Gets upper left and lower right coordinates,
  * and layers number in the image */
  gimp_drawable_mask_bounds(drawable->drawable_id, &x1, &y1, &x2, &y2);
  width = x2 - x1;
  height = y2 - y1;

  channels = gimp_drawable_bpp(drawable->drawable_id);

  /* Initialises two PixelRgns, one to read original data,
  * and the other to write output data. That second one will
  * be merged at the end by the call to
  * gimp_drawable_merge_shadow() */
//  gimp_pixel_rgn_init(&rgn_in, drawable, x1, y1, width, height, FALSE, FALSE);
  gimp_pixel_rgn_init(&rgn_out, drawable, x1, y1, width, height, TRUE, TRUE);


  gint num_layers = 0;
  gint *layers = gimp_image_get_layers(image_ID, &num_layers);

  fprintf(p_file, "x1 = %d, x2 = %d, y1 = %d, y2 = %d\n", x1, x2, y1, y2);
  fprintf(p_file, "w = %d, h = %d, c = %d, num_layers = %d\n", width, height, channels, num_layers);
  fprintf(p_file, "l0 = %d, l1 = %d, l2 = %d, l3 = %d, d = %d\n", layers[0], layers[1], layers[2], layers[3], drawable_ID);

  gimp_pixel_rgn_init(&rgn_in0, gimp_drawable_get(layers[1]), x1, y1, width, height, FALSE, FALSE);
  gimp_pixel_rgn_init(&rgn_in1, gimp_drawable_get(layers[2]), x1, y1, width, height, FALSE, FALSE);
  gimp_pixel_rgn_init(&rgn_inmask, gimp_drawable_get(layers[3]), x1, y1, width, height, FALSE, FALSE);


  guchar *img0 = g_new(guchar, channels * width * height);
  guchar *img1 = g_new(guchar, channels * width * height);
  guchar *mask = g_new(guchar, channels * width * height);
  guchar *out = g_new(guchar, channels * width * height);

  gimp_pixel_rgn_get_rect(&rgn_in0, img0, x1, y1, width, height);
  gimp_pixel_rgn_get_rect(&rgn_in1, img1, x1, y1, width, height);
  gimp_pixel_rgn_get_rect(&rgn_inmask, mask, x1, y1, width, height);

  mbb_blend(width, height, channels, kUint8, img0, img1, mask, out);

//  memset(out, 128, channels * width * height);

  gimp_pixel_rgn_set_rect(&rgn_out, out, x1, y1, width, height);

  g_free(img0);
  g_free(img1);
  g_free(mask);
  g_free(out);

//  if (i % 10 == 0)
//    gimp_progress_update ((gdouble) (i - x1) / (gdouble) (x2 - x1));

  /*  Update the modified region */
  gimp_drawable_flush(drawable);
  gimp_drawable_merge_shadow(drawable->drawable_id, TRUE);
  gimp_drawable_update(drawable->drawable_id, x1, y1, width, height);
}
