/* SPDX-License-Identifier:Unlicense */

/* Aravis header */

#include <arv.h>

/* Standard headers */

#include <stdlib.h>
#include <stdio.h>

typedef struct {
	GMainLoop *main_loop;
	guint32 counter;
} AppData;

void
new_buffer_cb (ArvStream *stream, void *user_data)
{
	ArvBuffer *buffer;
	AppData *app_data = user_data;

	/* This code is called from the stream receiving thread, which means all the time spent there is less time
	 * available for the reception of incoming packets */

	buffer = arv_stream_pop_buffer (stream);

	/* Display some informations about the retrieved buffer */
	printf ("Acquired %d×%d buffer\n",
		arv_buffer_get_image_width (buffer),
		arv_buffer_get_image_height (buffer));

	/* Don't destroy the buffer, but put it back into the buffer pool */
	arv_stream_push_buffer (stream, buffer);

	app_data->counter++;

	if (app_data->counter == 10)
		g_main_loop_quit (app_data->main_loop);
}

/*
 * Connect to the first available camera, then acquire 10 buffers.
 */

int
main (int argc, char **argv)
{
	ArvCamera *camera;
	AppData app_data;
	GError *error = NULL;

	app_data.main_loop = g_main_loop_new (NULL, FALSE);
	app_data.counter = 0;

	/* Connect to the first available camera */
	camera = arv_camera_new (NULL, &error);

	if (ARV_IS_CAMERA (camera)) {
		ArvStream *stream;

		printf ("Found camera '%s'\n", arv_camera_get_model_name (camera, NULL));

		/* Create the stream object without callback */
		stream = arv_camera_create_stream (camera, NULL, NULL, &error);
		if (ARV_IS_STREAM (stream)) {
			int i;
			size_t payload;

			/* Retrieve the payload size for buffer creation */
			payload = arv_camera_get_payload (camera, &error);
			if (error == NULL) {
				/* Insert some buffers in the stream buffer pool */
				for (i = 0; i < 5; i++)
					arv_stream_push_buffer (stream, arv_buffer_new (payload, NULL));
			}

			g_signal_connect (stream, "new-buffer", G_CALLBACK (new_buffer_cb), &app_data);
			arv_stream_set_emit_signals (stream, TRUE);

			if (error == NULL)
				/* Start the acquisition */
				arv_camera_start_acquisition (camera, &error);

			if (error == NULL)
				g_main_loop_run (app_data.main_loop);

			if (error == NULL)
				/* Stop the acquisition */
				arv_camera_stop_acquisition (camera, &error);

			arv_stream_set_emit_signals (stream, FALSE);

			/* Destroy the stream object */
			g_clear_object (&stream);
		}

		/* Destroy the camera instance */
		g_clear_object (&camera);
	}

	g_main_loop_unref (app_data.main_loop);

	if (error != NULL) {
		/* En error happened, display the correspdonding message */
		printf ("Error: %s\n", error->message);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

