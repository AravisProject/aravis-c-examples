/* SPDX-License-Identifier:Unlicense */

/* Aravis header */

#include <arv.h>

/* Standard headers */

#include <stdlib.h>
#include <stdio.h>

static void
stream_callback (void *user_data, ArvStreamCallbackType type, ArvBuffer *buffer)
{
	/* This code is called from the stream receiving thread, which means all the time spent there is less time
	 * available for the reception of incoming packets */

	switch (type) {
		case ARV_STREAM_CALLBACK_TYPE_INIT:
			/* Stream thread started.
			 *
			 * Here you may want to change the thread priority arv_make_thread_realtime() or
			 * arv_make_thread_high_priority() */
			break;
		case ARV_STREAM_CALLBACK_TYPE_START_BUFFER:
			/* The first packet of a new frame was received */
			break;
		case ARV_STREAM_CALLBACK_TYPE_BUFFER_DONE:
			/* The buffer is received, successfully or not.
			 *
			 * You could here signal the new buffer to another thread than the main one, and pull/push the
			 * buffer from there. */
			break;
		case ARV_STREAM_CALLBACK_TYPE_EXIT:
			/* Stream thread ended */
			break;
	}
}

/*
 * Connect to the first available camera, then acquire 10 buffers.
 */

int
main (int argc, char **argv)
{
	ArvCamera *camera;
	GError *error = NULL;

	/* Connect to the first available camera */
	camera = arv_camera_new (NULL, &error);

	if (ARV_IS_CAMERA (camera)) {
		ArvStream *stream;

		printf ("Found camera '%s'\n", arv_camera_get_model_name (camera, NULL));

		/* Create the stream object without callback */
		stream = arv_camera_create_stream (camera, stream_callback, NULL, &error);
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

			if (error == NULL)
				/* Start the acquisition */
				arv_camera_start_acquisition (camera, &error);

			if (error == NULL) {
				/* Retrieve 10 buffers */
				for (i = 0; i < 10; i++) {
					ArvBuffer *buffer;

					buffer = arv_stream_pop_buffer (stream);
					if (ARV_IS_BUFFER (buffer)) {
						/* Display some informations about the retrieved buffer */
						printf ("Acquired %d×%d buffer\n",
							arv_buffer_get_image_width (buffer),
							arv_buffer_get_image_height (buffer));
						/* Don't destroy the buffer, but put it back into the buffer pool */
						arv_stream_push_buffer (stream, buffer);
					}
				}
			}

			if (error == NULL)
				/* Stop the acquisition */
				arv_camera_stop_acquisition (camera, &error);

			/* Destroy the stream object */
			g_clear_object (&stream);
		}

		/* Destroy the camera instance */
		g_clear_object (&camera);
	}

	if (error != NULL) {
		/* En error happened, display the correspdonding message */
		printf ("Error: %s\n", error->message);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

