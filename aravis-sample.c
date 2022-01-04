#include <arv.h>
#include <stdlib.h>
#include <stdio.h>

int
main (int argc, char **argv)
{
	g_autoptr (ArvCamera) camera = NULL;
	g_autoptr (GError) error = NULL;
	g_autoptr (ArvBuffer) buffer = NULL;

	camera = arv_camera_new (NULL, &error);

	if (ARV_IS_CAMERA (camera)) {
		printf ("Found camera '%s'\n", arv_camera_get_model_name (camera, NULL));

		buffer = arv_camera_acquisition (camera, 0, &error);

		if (ARV_IS_BUFFER (buffer)) {
			printf ("Acquired %d×%d buffer\n",
				arv_buffer_get_image_width (buffer),
				arv_buffer_get_image_height (buffer));
		}
	}

	if (error != NULL) {
		printf ("Error: %s\n", error->message);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
