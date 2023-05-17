/* SPDX-License-Identifier:Unlicense */

/* Aravis header */

#include <arv.h>

/* Standard headers */

#include <stdlib.h>
#include <stdio.h>

typedef struct {
	ArvStream *stream;
	int counter;
	gboolean done;
} ArvStreamCallbackData;

static void
stream_callback (void *user_data, ArvStreamCallbackType type, ArvBuffer *buffer)
{
	ArvStreamCallbackData *callback_data = (ArvStreamCallbackData *) user_data;

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
			/* The buffer is received, successfully or not. It is already pushed in the output FIFO.
			 *
			 * You could here signal the new buffer to another thread than the main one, and pull/push the
			 * buffer from this another thread.
			 *
			 * Or use the buffer here. We need to pull it, process it, then push it back for reuse by the
			 * stream receiving thread */

			g_assert (buffer == arv_stream_pop_buffer(callback_data->stream));
			g_assert (buffer != NULL);

			/* Retrieve 10 buffers */
			if (callback_data->counter < 10) {
				if (arv_buffer_get_status(buffer) == ARV_BUFFER_STATUS_SUCCESS)
					printf ("Acquired %d×%d buffer\n",
						arv_buffer_get_image_width (buffer),
						arv_buffer_get_image_height (buffer));

				arv_stream_push_buffer(callback_data->stream, buffer);
				callback_data->counter++;
			} else {
				callback_data->done = TRUE;
			}

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
		ArvStreamCallbackData callback_data;

		printf ("Found camera '%s'\n", arv_camera_get_model_name (camera, NULL));

		/* Create the stream object without callback */
		callback_data.counter = 0;
		callback_data.done = FALSE;
		callback_data.stream = arv_camera_create_stream (camera, stream_callback, &callback_data, &error);
		if (ARV_IS_STREAM (callback_data.stream)) {
			int i;
			size_t payload;

			/* Retrieve the payload size for buffer creation */
			payload = arv_camera_get_payload (camera, &error);
			if (error == NULL) {
				/* Insert some buffers in the stream buffer pool */
				for (i = 0; i < 2; i++)
					arv_stream_push_buffer (callback_data.stream, arv_buffer_new (payload, NULL));
			}

			if (error == NULL)
				/* Start the acquisition */
				arv_camera_start_acquisition (camera, &error);

			if (error == NULL) {
				while (!callback_data.done) {
					usleep (1000);
				}
			}

			if (error == NULL)
				/* Stop the acquisition */
				arv_camera_stop_acquisition (camera, &error);

			/* Destroy the stream object */
			g_clear_object (&callback_data.stream);
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
