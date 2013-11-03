//  Copyright (C) 2013 Saulo A. Pessoa <saulopessoa@gmail.com>.
//
//  This file is part of MBB GIMP.
//
//  MBB GIMP is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  MBB GIMP is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with MBB GIMP. If not, see <http://www.gnu.org/licenses/>.

//#define DEBUG_LOG

#ifdef DEBUG_LOG
#include <stdio.h>
FILE *p_file;
#endif

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "mbb/mbb.h"
#include "error.h"

static gboolean gui(gint32 img_id, gint init_img0_layer_id,
                    gint init_img1_layer_id, gint init_mask_layer_id,
                    gint *img0_layer_id, gint *img1_layer_id,
                    gint *mask_layer_id);
static gboolean blend(gint32 img0_layer_id, gint32 img1_layer_id,
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

/* Essential function required by gimp runtime. */
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
    "plug-in-mbb-blend",
    "Implements a multi-band blending technique",
    "Implements a multi-band blending technique...<write more>",
    "Saulo A. Pessoa <saulopessoa@gmail.com>",
    "Saulo A. Pessoa",
    "2013",
    "Blend...",
    "RGB*, GRAY*",
    GIMP_PLUGIN,
    G_N_ELEMENTS(args),
    0,
    args,
    NULL);

  gimp_plugin_menu_register("plug-in-mbb-blend",
                            "<Image>/Filters/Multi-Band Blending");
}

/* Essential function required by gimp runtime. */
static void run(const gchar      *name,
                gint              nparams,
                const GimpParam  *param,
                gint             *nreturn_vals,
                GimpParam       **return_vals) {
  #ifdef DEBUG_LOG
  p_file = fopen("gimp-mbb_log.txt","w");
  #endif

  static GimpParam values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  GimpRunMode run_mode;
//  GimpDrawable *drawable;
  gint img_id;
  gint num_layers;
  gint *layers_ids;
  gint img0_layer_id, img1_layer_id, mask_layer_id;

  // Setting mandatory output values.
  *nreturn_vals = 1;
  *return_vals = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  // Getting run_mode - dialog won't display if we are in NONINTERACTIVE mode.
  run_mode = param[0].data.d_int32;

//  drawable = gimp_drawable_get(param[2].data.d_drawable);
  img_id = param[1].data.d_image;

  layers_ids = gimp_image_get_layers(img_id, &num_layers);

  #ifdef DEBUG_LOG
  fprintf(p_file, "num_layers = %d\n", num_layers);
  gint i;
  for (i = 0; i < num_layers; ++i)
    fprintf(p_file, "layer[%d] = %d\n", i, layers_ids[i]);
  #endif

  // Blend function requires at least three layers to work.
  if (num_layers < 3) {
    gimp_message("Blend function requires at least three layers.");
    return;
  }

  if (run_mode != GIMP_RUN_NONINTERACTIVE) {
    if (!gui(img_id, layers_ids[0], layers_ids[1], layers_ids[2],
             &img0_layer_id, &img1_layer_id, &mask_layer_id))
      return;
  }

  gimp_progress_init("Blending...");
//  gimp_progress_update(0.0);

  if (!blend(img0_layer_id, img1_layer_id, mask_layer_id, img_id)) {
    gimp_message(get_error_msg());
  }

//  gimp_progress_update(1.0);

  gimp_displays_flush();
//  gimp_drawable_detach(drawable);

  #ifdef DEBUG_LOG
  fclose(p_file);
  #endif
}

void callback_progress_func(float progress) {
  gimp_progress_update(progress);
}

/* Filter the layers to be exhibited by gui. */
static gboolean layersConstraintFunc(gint32 image_id, gint32 item_id,
                                     gpointer data) {
  if (image_id == *(gint32*)data)
    // Layer will be exhibited.
    return TRUE;
  else
    // Layer will not be exhibited.
    return FALSE;
}

/* Plugin gui. */
static gboolean gui(gint32 img_id, gint init_img0_layer_id,
                    gint init_img1_layer_id, gint init_mask_layer_id,
                    gint *img0_layer_id, gint *img1_layer_id,
                    gint *mask_layer_id) {

  GtkWidget *dialog, *main_vbox, *img0_label, *img1_label, *mask_label,
            *combo_img0, *combo_img1, *combo_mask;
  gint      tmp_img0_layer_id, tmp_img1_layer_id, tmp_mask_layer_id;

  gimp_ui_init("mbb", FALSE);
  dialog = gimp_dialog_new(
      "Multi-Band Blending - Blend",
      "Multi-Band Blending - Blend", NULL,
      /*GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT*/0,
      /*gimp_standard_help_func*/NULL,
      "plug-in-mbb-blend",
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
      GTK_STOCK_OK,     GTK_RESPONSE_OK,
      NULL);
  // Setup plugin's dialog to be image window children
  gimp_window_set_transient((GtkWindow*)dialog);

  // Lock the dialog size
  gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);

  main_vbox = gtk_hbox_new(FALSE, 6);
  gtk_container_add (GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), main_vbox);
  gtk_widget_show(main_vbox);

  img0_label = gtk_label_new_with_mnemonic("_Image 1:");
  gtk_widget_show(img0_label);
  gtk_box_pack_start(GTK_BOX(main_vbox), img0_label, FALSE, FALSE, 6);
  gtk_label_set_justify(GTK_LABEL(img0_label), GTK_JUSTIFY_RIGHT);

  combo_img0 = gimp_layer_combo_box_new(layersConstraintFunc, &img_id);
  //combo_img0 = gimp_layer_combo_box_new(NULL, NULL);
  gimp_int_combo_box_set_active((GimpIntComboBox*)combo_img0,
                                init_img0_layer_id);
  gtk_widget_show(combo_img0);
  gtk_box_pack_start(GTK_BOX(main_vbox), combo_img0, FALSE, FALSE, 6);

  img1_label = gtk_label_new_with_mnemonic("_Image 2:");
  gtk_widget_show(img1_label);
  gtk_box_pack_start(GTK_BOX(main_vbox), img1_label, FALSE, FALSE, 6);
  gtk_label_set_justify(GTK_LABEL(img1_label), GTK_JUSTIFY_RIGHT);

  combo_img1 = gimp_layer_combo_box_new(layersConstraintFunc, &img_id);
  gimp_int_combo_box_set_active((GimpIntComboBox*)combo_img1,
                                init_img1_layer_id);
  gtk_widget_show(combo_img1);
  gtk_box_pack_start(GTK_BOX(main_vbox), combo_img1, FALSE, FALSE, 6);

  mask_label = gtk_label_new_with_mnemonic("_Mask:");
  gtk_widget_show(mask_label);
  gtk_box_pack_start(GTK_BOX(main_vbox), mask_label, FALSE, FALSE, 6);
  gtk_label_set_justify(GTK_LABEL(mask_label), GTK_JUSTIFY_RIGHT);

  combo_mask = gimp_layer_combo_box_new(layersConstraintFunc, &img_id);
  gimp_int_combo_box_set_active((GimpIntComboBox*)combo_mask,
                                init_mask_layer_id);
  gtk_widget_show(combo_mask);
  gtk_box_pack_start(GTK_BOX(main_vbox), combo_mask, FALSE, FALSE, 6);

  while (TRUE) {
    gint response;

    gtk_widget_show(dialog);
    response = gimp_dialog_run(GIMP_DIALOG(dialog));

    if (response == GTK_RESPONSE_OK) {
        gimp_int_combo_box_get_active((GimpIntComboBox*)combo_img0,
                                      &tmp_img0_layer_id);
        gimp_int_combo_box_get_active((GimpIntComboBox*)combo_img1,
                                      &tmp_img1_layer_id);
        gimp_int_combo_box_get_active((GimpIntComboBox*)combo_mask,
                                      &tmp_mask_layer_id);

        // Avoid the same layer to be used more than once.
        if (tmp_img0_layer_id != tmp_img1_layer_id &&
            tmp_img0_layer_id != tmp_mask_layer_id &&
            tmp_img1_layer_id != tmp_mask_layer_id) {

          *img0_layer_id = tmp_img0_layer_id;
          *img1_layer_id = tmp_img1_layer_id;
          *mask_layer_id = tmp_mask_layer_id;

          gtk_widget_destroy(dialog);
          return TRUE;
        } else {
          gimp_message("Each layer can be used only once. Please, select a different layer for...");
        }
    } else if (response == GTK_RESPONSE_CANCEL ||
               response == GTK_RESPONSE_DELETE_EVENT) {
        gtk_widget_destroy(dialog);
        return FALSE;
    }
  }
}

/* Read layers specified and blend them placing the result into a new layer. */
static gboolean blend(gint32 img0_layer_id, gint32 img1_layer_id,
                      gint32 mask_layer_id, gint32 img_id) {

  gint         width, height, channels, type,
               tmp_type;
  gint         x0, y0, x1, y1,
               tmp_x0, tmp_y0, tmp_x1, tmp_y1;
  GimpPixelRgn rgn_img0, rgn_img1, rgn_mask, rgn_out;
  gint32       out_layer_id;
  GimpDrawable *out_layer_drawable;
  guchar       *img0_buf, *img1_buf, *mask_buf, *out_buf;


  // Gets upper left and lower right coordinates.
  gimp_drawable_mask_bounds(img0_layer_id, &x0, &y0, &x1, &y1);

  channels = gimp_drawable_bpp(img0_layer_id);
  type = gimp_drawable_type(img0_layer_id);

  #ifdef DEBUG_LOG
  fprintf(p_file, "x0 = %d, x1 = %d, y0 = %d, y1 = %d\n", x0, x1, y0, y1);
  fprintf(p_file, "w = %d, h = %d, c = %d\n", x1-x0, y1-y0, channels);
  #endif

  gimp_drawable_mask_bounds(img1_layer_id, &tmp_x0, &tmp_y0, &tmp_x1, &tmp_y1);
  tmp_type = gimp_drawable_type(img1_layer_id);

  if (x0!=tmp_x0 || x1!=tmp_x1 || y0!=tmp_y0 || y1!=tmp_y1) {
    set_error_msg("Layers of the input images have different boundaries size. Please, set equal boundaries size to continue.");
    return FALSE;
  }

  if (type!=tmp_type) {
    set_error_msg("Layers of the input images have different types. Please, choose layers with the same type.");
    return FALSE;
  }

  gimp_drawable_mask_bounds(mask_layer_id, &tmp_x0, &tmp_y0, &tmp_x1, &tmp_y1);
  tmp_type = gimp_drawable_type(mask_layer_id);

  if (x0!=tmp_x0 || x1!=tmp_x1 || y0!=tmp_y0 || y1!=tmp_y1) {
    set_error_msg("Layers of the input images and of the mask have different boundaries size. Please, set equal boundaries size to continue.");
    return FALSE;
  }

  if (type!=tmp_type) {
    set_error_msg("Layers of the input images and of the mask have different types. Please, choose layers with the same type.");
    return FALSE;
  }

  width = x1 - x0;
  height = y1 - y0;

  gimp_pixel_rgn_init(&rgn_img0, gimp_drawable_get(img0_layer_id), x0, y0,
                      width, height, FALSE, FALSE);
  gimp_pixel_rgn_init(&rgn_img1, gimp_drawable_get(img1_layer_id), x0, y0,
                      width, height, FALSE, FALSE);
  gimp_pixel_rgn_init(&rgn_mask, gimp_drawable_get(mask_layer_id), x0, y0,
                      width, height, FALSE, FALSE);

  img0_buf = g_new(guchar, channels * width * height);
  img1_buf = g_new(guchar, channels * width * height);
  mask_buf = g_new(guchar, channels * width * height);
  out_buf = g_new(guchar, channels * width * height);

  // Read input layers to buffers.
  gimp_pixel_rgn_get_rect(&rgn_img0, img0_buf, x0, y0, width, height);
  gimp_pixel_rgn_get_rect(&rgn_img1, img1_buf, x0, y0, width, height);
  gimp_pixel_rgn_get_rect(&rgn_mask, mask_buf, x0, y0, width, height);

  mbb_blend(width, height, channels, kUint8, img0_buf, img1_buf, mask_buf,
            out_buf, callback_progress_func);

  gimp_image_undo_group_start(img_id);

  // Create new layer with the same characteristics to place the result.
  //out_layer_id = gimp_layer_new_from_drawable(img0_layer_id, img_id);
  //out_layer_id = gimp_layer_new_from_visible(img_id, img_id, "Blended");
//  out_layer_id = gimp_layer_new(img_id, "Blended", width, height, type,
//                                100.0, GIMP_NORMAL_MODE);
  out_layer_id = gimp_layer_new(img_id, "Blended", gimp_drawable_width(img_id),
                                gimp_drawable_height(img_id), type,
                                100.0, GIMP_NORMAL_MODE);
  gimp_image_add_layer(img_id, out_layer_id, 0);
  out_layer_drawable = gimp_drawable_get(out_layer_id);
  gimp_pixel_rgn_init(&rgn_out, out_layer_drawable, x0, y0, width, height,
                      TRUE, TRUE);

  // Write the output buffer to the new created layer.
  gimp_pixel_rgn_set_rect(&rgn_out, out_buf, x0, y0, width, height);

  gimp_drawable_flush(out_layer_drawable);
  gimp_drawable_merge_shadow(out_layer_id, TRUE);
  gimp_drawable_update(out_layer_id, x0, y0, width, height);

  gimp_image_undo_group_end(img_id);

  g_free(img0_buf);
  g_free(img1_buf);
  g_free(mask_buf);
  g_free(out_buf);

  return TRUE;
}
