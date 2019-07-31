#include "AudioUtils.hpp"

#include <chrono>
#include <random>

namespace jm
{
	struct SoundEffectSource
	{
		AudioSource source;
		const AudioClip* clip;
	};
	
	static std::vector<SoundEffectSource> soundEffectSources;
	
	void PlaySoundEffect(const AudioClip& clip, glm::vec3 position, SoundEffectParams params)
	{
		SoundEffectSource* source = nullptr;
		
		for (SoundEffectSource& sourceCan : soundEffectSources)
		{
			if (!sourceCan.source.IsPlaying())
			{
				source = &sourceCan;
				break;
			}
		}
		
		if (source == nullptr)
		{
			source = &soundEffectSources.emplace_back();
		}
		
		source->source.SetClip(clip);
		source->source.SetPosition(position);
		source->source.SetVolume(params.volume);
		source->source.SetPitch(params.pitch);
		source->source.SetAttenuation(params.rolloffFactor, params.refDistance);
		source->clip = &clip;
		
		source->source.Play();
	}
	
	MusicPlayer::MusicPlayer()
		: m_rng(std::chrono::high_resolution_clock::now().time_since_epoch().count())
	{
		m_source.SetTransformRelative(true);
	}
	
	void MusicPlayer::SetTracks(std::vector<const AudioClip*> tracks)
	{
		if (!m_tracks.empty())
		{
			m_volumeFade = 1.0f;
			m_isFadingOut = true;
		}
		
		m_tracks = std::move(tracks);
		m_nextTrack = m_tracks.size();
	}
	
	void MusicPlayer::Update(float dt)
	{
		bool isPlaying = m_source.IsPlaying();
		
		if (!active)
		{
			if (isPlaying)
				m_source.Stop();
			return;
		}
		
		bool selectNextTrack = !isPlaying;
		
		if (m_isFadingOut)
		{
			m_volumeFade -= dt * m_volumeFadeTimeScale;
			if (m_volumeFade < 0)
			{
				m_volumeFade = 1.0f;
				m_isFadingOut = false;
				selectNextTrack = true;
				m_source.Stop();
			}
		}
		
		m_source.SetVolume(m_volumeFade * volume);
		
		if (selectNextTrack && !m_tracks.empty())
		{
			if (m_nextTrack == m_tracks.size())
			{
				m_nextTrack = 0;
				std::shuffle(m_tracks.begin(), m_tracks.end(), m_rng);
			}
			
			m_source.SetClip(*m_tracks[m_nextTrack]);
			m_source.Play();
			
			m_nextTrack++;
		}
	}
}
