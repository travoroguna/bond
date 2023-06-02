#include "../../../bond.h"


namespace bond::rayaudio {
    NativeErrorOr init_audio_device(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr close_audio_device(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_audio_device_ready(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_master_volume(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_wave(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_wave_from_memory(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_wave_ready(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_sound(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_sound_from_wave(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_sound_ready(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr update_sound(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_wave(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_sound(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr export_wave(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr export_wave_as_code(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr play_sound(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr stop_sound(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr pause_sound(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr resume_sound(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_sound_playing(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_sound_volume(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_sound_pitch(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_sound_pan(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr wave_copy(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr wave_crop(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr wave_format(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_wave_samples(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_wave_samples(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_music_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_music_stream_from_memory(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_music_ready(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_music_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr play_music_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_music_stream_playing(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr update_music_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr stop_music_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr pause_music_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr resume_music_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr seek_music_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_music_volume(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_music_pitch(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_music_pan(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_music_time_length(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr get_music_time_played(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr load_audio_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_audio_stream_ready(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr unload_audio_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr update_audio_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_audio_stream_processed(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr play_audio_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr pause_audio_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr resume_audio_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr is_audio_stream_playing(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr stop_audio_stream(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_audio_stream_volume(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_audio_stream_pitch(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_audio_stream_pan(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_audio_stream_buffer_size_default(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr set_audio_stream_callback(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr attach_audio_stream_processor(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr detach_audio_stream_processor(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr attach_audio_mixed_processor(const std::vector<GcPtr<Object>> &arguments);

    NativeErrorOr detach_audio_mixed_processor(const std::vector<GcPtr<Object>> &arguments);
}