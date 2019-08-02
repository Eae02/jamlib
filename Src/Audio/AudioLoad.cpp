#include "Audio.hpp"
#include "../Utils.hpp"
#include "../Asset.hpp"

#include <gsl/gsl>
#include <string>
#include <SDL_audio.h>

#define STB_VORBIS_NO_STDIO
#include <stb_vorbis.c>

namespace jm
{
	AudioClip LoadWAVAsset(gsl::span<const char> fileData, const std::string& name)
	{
		SDL_AudioSpec audioSpec;
		uint8_t* audioBuffer;
		uint32_t audioBufferLen;
		
		bool ok = SDL_LoadWAV_RW(SDL_RWFromConstMem(fileData.data(), fileData.size()), 1,
			&audioSpec, &audioBuffer, &audioBufferLen) != nullptr;
		
		if (!ok)
		{
			Panic(Concat({ "Error loading WAV from '", name, "': ", SDL_GetError(), "."}));
		}
		
		AudioFormat format;
		if (audioSpec.format == AUDIO_S16SYS)
		{
			format = audioSpec.channels == 1 ? AudioFormat::Mono16 : AudioFormat::Stereo16;
		}
		else if (audioSpec.format == AUDIO_U8)
		{
			format = audioSpec.channels == 1 ? AudioFormat::Mono8 : AudioFormat::Stereo8;
		}
		else
		{
			Panic(Concat({ "Error loading WAV from '", name, "': the file uses an incompatible format."}));
		}
		
		DisableAssetReload(name);
		
		AudioClip clip;
		clip.SetData(format, audioSpec.size, audioBuffer, audioSpec.freq);
		return clip;
	}
	
	AudioClip LoadVorbisAsset(gsl::span<const char> fileData, const std::string& name)
	{
		int numChannels, sampleRate;
		short* audioBuffer;
		
		int numSamples = stb_vorbis_decode_memory(
			reinterpret_cast<const uint8_t*>(fileData.data()),
			fileData.size_bytes(), &numChannels, &sampleRate, &audioBuffer);
		
		if (numSamples == -1)
		{
			Panic(Concat({ "Error loading OGG from '", name, "'." }));
		}
		
		AudioFormat format = numChannels == 1 ? AudioFormat::Mono16 : AudioFormat::Stereo16;
		
		DisableAssetReload(name);
		
		AudioClip clip;
		clip.SetData(format, numSamples * 2, audioBuffer, sampleRate);
		return clip;
	}
	
	void RegisterAudioAssetLoaders()
	{
		RegisterAssetLoader<AudioClip>("wav", &LoadWAVAsset);
		RegisterAssetLoader<AudioClip>("ogg", &LoadVorbisAsset);
	}
}
