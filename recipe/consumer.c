#include <aws/event-stream/event_stream.h>
#include <aws/common/byte_buf.h>
#include <string.h>
#include <stdio.h>
int main(void) {
    struct aws_allocator *a = aws_default_allocator();
    aws_event_stream_library_init(a);
    struct aws_byte_buf payload = aws_byte_buf_from_c_str("native-arm64-event");
    struct aws_event_stream_message message = {0}, decoded = {0};
    if (aws_event_stream_message_init(&message, a, NULL, &payload)) return 1;
    struct aws_byte_buf wire = aws_byte_buf_from_array(
        aws_event_stream_message_buffer(&message), aws_event_stream_message_total_length(&message));
    if (aws_event_stream_message_from_buffer(&decoded, a, &wire)) return 2;
    if (aws_event_stream_message_payload_len(&decoded) != payload.len ||
        memcmp(aws_event_stream_message_payload(&decoded), payload.buffer, payload.len)) return 3;
    aws_event_stream_message_clean_up(&decoded);
    wire.buffer[wire.len - 1] ^= 1;
    if (aws_event_stream_message_from_buffer(&decoded, a, &wire) == AWS_OP_SUCCESS) return 4;
    aws_event_stream_message_clean_up(&message);
    aws_event_stream_library_clean_up();
    puts("Event Stream roundtrip and corrupted checksum rejection passed");
    return 0;
}
