/* SPDX-License-Identifier:Unlicense */

/* Aravis header */

#include <arv.h>

/* Standard headers */

#include <stdlib.h>
#include <stdio.h>

/*
 * Connect to the first available camera, then display the current settings for image width and height, as well as the
 * pixel format, using the more generic ArvCamera feature API.
 */

int
main (int argc, char **argv)
{
	ArvCamera *camera;
	GError *error = NULL;

	/* Connect to the first available camera */
	camera = arv_camera_new (NULL, &error);

	if (ARV_IS_CAMERA (camera)) {
		int width;
		int height;
		const char *pixel_format;

		printf ("Found camera '%s'\n", arv_camera_get_model_name (camera, NULL));

		/* Retrieve generally mandatory features for transmitters */

		if (!error) width = arv_camera_get_integer (camera, "Width", &error);
		if (!error) height = arv_camera_get_integer (camera, "Height", &error);
		if (!error) pixel_format = arv_camera_get_string (camera, "PixelFormat", &error);

		if (error == NULL) {
			printf ("Width = %d\n", width);
			printf ("Height = %d\n", height);
			printf ("Pixel format = %s\n", pixel_format);
		}

		g_clear_object (&camera);
	}

	if (error != NULL) {
		/* En error happened, display the correspdonding message */
		printf ("Error: %s\n", error->message);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
