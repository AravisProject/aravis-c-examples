/* SPDX-License-Identifier:Unlicense */

/* Aravis header */

#include <arv.h>

/* Standard headers */

#include <stdio.h>

int
main (int argc, char **argv)
{
	ArvCamera *camera;
	ArvChunkParser *parser;
	GError *error =NULL;

	/* Instantiation of the first available camera */
	camera = arv_camera_new (NULL, &error);

	if (ARV_IS_CAMERA (camera)) {
		ArvBuffer *buffer = NULL;

		printf ("Found camera '%s'\n", arv_camera_get_model_name (camera, NULL));

		/* Instantiation of a chunk parser */
		parser = arv_camera_create_chunk_parser (camera);

		/* Enable chunk data */
		arv_camera_set_chunks (camera, "Width,Height", &error);

		/* Acquire a single buffer */
		if (error == NULL)
			buffer = arv_camera_acquisition	(camera, 1000000, &error);

		if (buffer != NULL) {
			printf ("ChunkWidth = %d\n", (int) arv_chunk_parser_get_integer_value (parser,
											       buffer, "ChunkWidth", NULL));
			printf ("ChunkHeight = %d\n", (int) arv_chunk_parser_get_integer_value (parser,
												buffer, "ChunkHeight", NULL));
		} else {
			printf ("Failed to acquire a single buffer\n");
		}

		g_clear_object (&buffer);
		g_clear_object (&parser);
		g_clear_object (&camera);
	}

	if (error != NULL) {
		/* En error happened, display the correspdonding message */
		printf ("Error: %s\n", error->message);
		g_clear_error (&error);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


