#include "Audio.hpp"
#include "Utils.hpp"

#include <AL/al.h>
#include <AL/alc.h>

namespace jm
{
	ALCdevice* alDevice;
	ALCcontext* alContext;
	
	void detail::InitializeOpenAL()
	{
		alDevice = alcOpenDevice(nullptr);
		if (alDevice == nullptr)
			Panic("Error opening OpenAL device.");
		
		alContext = alcCreateContext(alDevice, nullptr);
		if (alContext == nullptr)
			Panic("Error creating OpenAL context.");
		alcMakeContextCurrent(alContext);
	}
	
	void detail::CloseOpenAL()
	{
		alcDestroyContext(alContext);
		alcCloseDevice(alDevice);
	}
	
	void UpdateListener(const glm::vec3& position, const glm::vec3& velocity)
	{
		alListener3f(AL_POSITION, position.x, position.y, position.z);
		alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	}
	
	void SetMasterVolume(float volume)
	{
		alListenerf(AL_GAIN, volume);
	}
	
	AudioClip::AudioClip()
	{
		m_handle.deleter = &alDeleteBuffers;
		alGenBuffers(1, &m_handle.handle);
	}
	
	static const ALenum FORMAT_TRANSLATION[] = 
	{
		AL_FORMAT_MONO8, AL_FORMAT_MONO16, AL_FORMAT_STEREO8, AL_FORMAT_STEREO16
	};
	
	void AudioClip::SetData(AudioFormat format, size_t dataBytes, const void* data, int frequency)
	{
		alBufferData(m_handle.handle, FORMAT_TRANSLATION[(int)format], data, dataBytes, frequency);
	}
	
	AudioSource::AudioSource()
	{
		m_handle.deleter = &alDeleteSources;
		alGenSources(1, &m_handle.handle);
	}
	
	void AudioSource::SetPosition(glm::vec3 position)
	{
		alSource3f(m_handle.handle, AL_POSITION, position.x, position.y, position.z);
	}
	
	void AudioSource::SetVelocity(glm::vec3 velocity)
	{
		alSource3f(m_handle.handle, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	}
	
	void AudioSource::SetDirection(glm::vec3 direction)
	{
		alSource3f(m_handle.handle, AL_DIRECTION, direction.x, direction.y, direction.z);
	}
	
	void AudioSource::SetAttenuation(float rolloffFactor, float refDistance)
	{
		alSourcef(m_handle.handle, AL_ROLLOFF_FACTOR, rolloffFactor);
		alSourcef(m_handle.handle, AL_REFERENCE_DISTANCE, refDistance);
	}
	
	void AudioSource::SetVolume(float volume)
	{
		alSourcef(m_handle.handle, AL_GAIN, volume);
	}
	
	void AudioSource::SetPitch(float pitch)
	{
		alSourcef(m_handle.handle, AL_PITCH, pitch);
	}
	
	void AudioSource::SetClip(const AudioClip& clip)
	{
		alSourcei(m_handle.handle, AL_BUFFER, clip.m_handle.handle);
	}
	
	void AudioSource::SetIsLooping(bool isLooping)
	{
		alSourcei(m_handle.handle, AL_LOOPING, isLooping ? AL_TRUE : AL_FALSE);
	}
	
	void AudioSource::SetTransformRelative(bool relativeTransform)
	{
		alSourcei(m_handle.handle, AL_SOURCE_RELATIVE, relativeTransform ? AL_TRUE : AL_FALSE);
	}
	
	void AudioSource::Play()
	{
		alSourcePlay(m_handle.handle);
	}
	
	void AudioSource::Stop()
	{
		alSourceStop(m_handle.handle);
	}
	
	bool AudioSource::IsPlaying() const
	{
		ALint state;
		alGetSourcei(m_handle.handle, AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}
}
