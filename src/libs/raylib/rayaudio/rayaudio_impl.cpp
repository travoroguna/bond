#include "rayaudio.h"
#include <raylib.h>
#include "../raymodule.h"


namespace bond::rayaudio {
    NativeErrorOr init_audio_device(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        InitAudioDevice();
        return Globs::BondNil;
    }

    NativeErrorOr close_audio_device(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        CloseAudioDevice();
        return Globs::BondNil;
    }

    NativeErrorOr is_audio_device_ready(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(0, arguments);
        return BOOL_(IsAudioDeviceReady());
    }

    NativeErrorOr set_master_volume(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(volume, Float, 0, arguments);
        SetMasterVolume(volume->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr load_wave(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fileName, String, 0, arguments);
        return GarbageCollector::instance().make<raylib::Wave>(LoadWave(fileName->get_value().c_str()));
    }

    NativeErrorOr load_wave_from_memory(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(fileType, String, 0, arguments);
        DEFINE(fileData, Integer, 1, arguments);
        DEFINE(dataSize, Integer, 2, arguments);
        TraceLog(LOG_WARNING, "load_wave_from_memory not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr is_wave_ready(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(wave, raylib::Wave, 0, arguments);
        return BOOL_(IsWaveReady(wave->get_value()));
    }

    NativeErrorOr load_sound(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fileName, String, 0, arguments);
        return GarbageCollector::instance().make<raylib::Sound>(LoadSound(fileName->get_value().c_str()));
    }

    NativeErrorOr load_sound_from_wave(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(wave, raylib::Wave, 0, arguments);
        return GarbageCollector::instance().make<raylib::Sound>(LoadSoundFromWave(wave->get_value()));
    }

    NativeErrorOr is_sound_ready(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(sound, raylib::Sound, 0, arguments);

        return BOOL_(IsSoundReady(sound->get_value()));
    }

    NativeErrorOr update_sound(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(sound, raylib::Sound, 0, arguments);
        DEFINE(data, Integer, 1, arguments);
        DEFINE(sampleCount, Integer, 2, arguments);
        TraceLog(LOG_WARNING, "update_sound not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr unload_wave(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(wave, raylib::Wave, 0, arguments);
        TraceLog(LOG_WARNING, "unload_wave is done automatically");
        return Globs::BondNil;
    }

    NativeErrorOr unload_sound(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(sound, raylib::Sound, 0, arguments);
        TraceLog(LOG_WARNING, "unload_sound is done automatically");
        return Globs::BondNil;
    }

    NativeErrorOr export_wave(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(wave, raylib::Wave, 0, arguments);
        DEFINE(fileName, String, 1, arguments);

        return BOOL_(ExportWave(wave->get_value(), fileName->get_value().c_str()));
    }

    NativeErrorOr export_wave_as_code(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(wave, raylib::Wave, 0, arguments);
        DEFINE(fileName, String, 1, arguments);

        return BOOL_(ExportWaveAsCode(wave->get_value(), fileName->get_value().c_str()));
    }

    NativeErrorOr play_sound(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(sound, raylib::Sound, 0, arguments);
        PlaySound(sound->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr stop_sound(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(sound, raylib::Sound, 0, arguments);
        StopSound(sound->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr pause_sound(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(sound, raylib::Sound, 0, arguments);
        PauseSound(sound->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr resume_sound(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(sound, raylib::Sound, 0, arguments);
        ResumeSound(sound->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr is_sound_playing(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(sound, raylib::Sound, 0, arguments);
        return BOOL_(IsSoundPlaying(sound->get_value()));
    }

    NativeErrorOr set_sound_volume(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(sound, raylib::Sound, 0, arguments);
        DEFINE(volume, Float, 1, arguments);
        SetSoundVolume(sound->get_value(), volume->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_sound_pitch(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(sound, raylib::Sound, 0, arguments);
        DEFINE(pitch, Float, 1, arguments);
        SetSoundPitch(sound->get_value(), pitch->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_sound_pan(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(sound, raylib::Sound, 0, arguments);
        DEFINE(pan, Float, 1, arguments);
        SetSoundPan(sound->get_value(), pan->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr wave_copy(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(wave, raylib::Wave, 0, arguments);

        return GarbageCollector::instance().make<raylib::Wave>(WaveCopy(wave->get_value()));
    }

    NativeErrorOr wave_crop(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(wave, raylib::Wave, 0, arguments);
        DEFINE(initSample, Integer, 1, arguments);
        DEFINE(finalSample, Integer, 2, arguments);

        WaveCrop(wave->get(), initSample->get_value(), finalSample->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr wave_format(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(4, arguments);
        DEFINE(wave, raylib::Wave, 0, arguments);
        DEFINE(sampleRate, Integer, 1, arguments);
        DEFINE(sampleSize, Integer, 2, arguments);
        DEFINE(channels, Integer, 3, arguments);
        WaveFormat(wave->get(), sampleRate->get_value(), sampleSize->get_value(), channels->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr load_wave_samples(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(wave, raylib::Wave, 0, arguments);
        TraceLog(LOG_WARNING, "load_wave_samples is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr unload_wave_samples(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(samples, Float, 0, arguments);
        TraceLog(LOG_WARNING, "unload_wave_samples is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr load_music_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(fileName, String, 0, arguments);

        return GarbageCollector::instance().make<raylib::Music>(LoadMusicStream(fileName->get_value().c_str()));
    }

    NativeErrorOr load_music_stream_from_memory(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(fileType, String, 0, arguments);
        DEFINE(data, Integer, 1, arguments);
        DEFINE(dataSize, Integer, 2, arguments);
        TraceLog(LOG_WARNING, "load_music_stream_from_memory is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr is_music_ready(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(music, raylib::Music, 0, arguments);

        return BOOL_(IsMusicReady(music->get_value()));
    }

    NativeErrorOr unload_music_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(music, raylib::Music, 0, arguments);
        TraceLog(LOG_WARNING, "unload_music_stream is done automatically");
        return Globs::BondNil;
    }

    NativeErrorOr play_music_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(music, raylib::Music, 0, arguments);
        PlayMusicStream(music->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr is_music_stream_playing(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(music, raylib::Music, 0, arguments);

        return BOOL_(IsMusicStreamPlaying(music->get_value()));
    }

    NativeErrorOr update_music_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(music, raylib::Music, 0, arguments);
        UpdateMusicStream(music->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr stop_music_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(music, raylib::Music, 0, arguments);
        StopMusicStream(music->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr pause_music_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(music, raylib::Music, 0, arguments);
        PauseMusicStream(music->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr resume_music_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(music, raylib::Music, 0, arguments);
        ResumeMusicStream(music->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr seek_music_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(music, raylib::Music, 0, arguments);
        DEFINE(position, Float, 1, arguments);
        SeekMusicStream(music->get_value(), position->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_music_volume(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(music, raylib::Music, 0, arguments);
        DEFINE(volume, Float, 1, arguments);
        SetMusicVolume(music->get_value(), volume->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_music_pitch(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(music, raylib::Music, 0, arguments);
        DEFINE(pitch, Float, 1, arguments);
        SetMusicPitch(music->get_value(), pitch->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_music_pan(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(music, raylib::Music, 0, arguments);
        DEFINE(pan, Float, 1, arguments);
        SetMusicPan(music->get_value(), pan->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr get_music_time_length(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(music, raylib::Music, 0, arguments);

        return GarbageCollector::instance().make<Float>(GetMusicTimeLength(music->get_value()));
    }

    NativeErrorOr get_music_time_played(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(music, raylib::Music, 0, arguments);

        return GarbageCollector::instance().make<Float>(GetMusicTimePlayed(music->get_value()));
    }

    NativeErrorOr load_audio_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(sampleRate, Integer, 0, arguments);
        DEFINE(sampleSize, Integer, 1, arguments);
        DEFINE(channels, Integer, 2, arguments);

        return GarbageCollector::instance().make<raylib::AudioStream>(
                LoadAudioStream(sampleRate->get_value(), sampleSize->get_value(), channels->get_value()));
    }

    NativeErrorOr is_audio_stream_ready(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);

        return BOOL_(IsAudioStreamReady(stream->get_value()));
    }

    NativeErrorOr unload_audio_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);
        TraceLog(LOG_WARNING, "unload_audio_stream is done automatically");
        return Globs::BondNil;
    }

    NativeErrorOr update_audio_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(3, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);
        DEFINE(data, Integer, 1, arguments);
        DEFINE(frameCount, Integer, 2, arguments);
        TraceLog(LOG_WARNING, "update_audio_stream is not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr is_audio_stream_processed(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);

        return BOOL_(IsAudioStreamProcessed(stream->get_value()));
    }

    NativeErrorOr play_audio_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);
        PlayAudioStream(stream->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr pause_audio_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);
        PauseAudioStream(stream->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr resume_audio_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);
        ResumeAudioStream(stream->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr is_audio_stream_playing(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);

        return BOOL_(IsAudioStreamPlaying(stream->get_value()));
    }

    NativeErrorOr stop_audio_stream(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);
        StopAudioStream(stream->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_audio_stream_volume(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);
        DEFINE(volume, Float, 1, arguments);
        SetAudioStreamVolume(stream->get_value(), volume->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_audio_stream_pitch(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);
        DEFINE(pitch, Float, 1, arguments);
        SetAudioStreamPitch(stream->get_value(), pitch->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_audio_stream_pan(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);
        DEFINE(pan, Float, 1, arguments);
        SetAudioStreamPan(stream->get_value(), pan->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_audio_stream_buffer_size_default(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(size, Integer, 0, arguments);
        SetAudioStreamBufferSizeDefault(size->get_value());
        return Globs::BondNil;
    }

    NativeErrorOr set_audio_stream_callback(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);
        DEFINE(callback, Integer, 1, arguments);
        TraceLog(LOG_WARNING, "set_audio_stream_callback Not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr attach_audio_stream_processor(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);
        DEFINE(processor, Integer, 1, arguments);
        TraceLog(LOG_WARNING, "attach_audio_stream_processor Not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr detach_audio_stream_processor(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(2, arguments);
        DEFINE(stream, raylib::AudioStream, 0, arguments);
        DEFINE(processor, Integer, 1, arguments);
        TraceLog(LOG_WARNING, "detach_audio_stream_processor Not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr attach_audio_mixed_processor(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(processor, Integer, 0, arguments);
        TraceLog(LOG_WARNING, "attach_audio_mixed_processor Not implemented");
        return Globs::BondNil;
    }

    NativeErrorOr detach_audio_mixed_processor(const std::vector <GcPtr<Object>> &arguments) {
        ASSERT_ARG_COUNT(1, arguments);
        DEFINE(processor, Integer, 0, arguments);
        TraceLog(LOG_WARNING, "detach_audio_mixed_processor Not implemented");
        return Globs::BondNil;
    }
}