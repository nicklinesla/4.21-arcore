// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

/* Public dependencies
*****************************************************************************/

#include "CoreMinimal.h"

#include "AudioMixerBuffer.h"
#include "AudioMixerSourceManager.h"

namespace Audio
{
	class FMixerDevice;
	class FMixerSourceVoice;
	class FMixerSource;
	class FMixerBuffer;
	class ISourceBufferQueueListener;

	static const int32 MAX_BUFFERS_QUEUED = 3;
	static const int32 LOOP_FOREVER = -1;


	/** Struct defining a source voice buffer. */
	struct FMixerSourceVoiceBuffer
	{
		/** Raw audio PCM data. */
		TArray<uint8> AudioData;

		/** The amount of real audio data in the byte array (may not be the size of the array in the case of procedural audio. */
		uint32 AudioBytes;

		/** How many times this buffer will loop. */
		int32 LoopCount;

		/** If this buffer is from real-time decoding and needs to make callbacks for more data. */
		uint32 bRealTimeBuffer : 1;

		FMixerSourceVoiceBuffer()
		{
			FMemory::Memzero(this, sizeof(*this));
		}
	};

	/** 
	 * FMixerSource
	 * Class which implements a sound source object for the audio mixer module.
	 */
	class FMixerSource :	public FSoundSource, 
							public ISourceBufferQueueListener
	{
	public:
		/** Constructor. */
		FMixerSource(FAudioDevice* InAudioDevice);

		/** Destructor. */
		~FMixerSource();

		//~ Begin FSoundSource Interface
		bool Init(FWaveInstance* InWaveInstance) override;
		void Update() override;
		bool PrepareForInitialization(FWaveInstance* InWaveInstance) override;
		bool IsPreparedToInit() override;
		void Play() override;
		void Stop() override;
		void Pause() override;
		bool IsFinished() override;
		FString Describe(bool bUseLongName) override;
		float GetPlaybackPercent() const override;
		//~ End FSoundSource Interface

		//~Begin ISourceBufferQueueListener
		void OnSourceBufferEnd() override;
		void OnRelease() override;
		void OnUpdatePendingDecodes() override;
		//~End ISourceBufferQueueListener

	private:

		/** Enum describing the data-read mode of an audio buffer. */
		enum class EBufferReadMode : uint8
		{
			/** Read the next buffer synchronously. */
			Synchronous,

			/** Read the next buffer asynchronously. */
			Asynchronous,

			/** Read the next buffer asynchronously but skip the first chunk of audio. */
			AsynchronousSkipFirstFrame
		};

		/** Submit the current decoded PCM buffer to the contained source voice. */
		void SubmitPCMBuffers();

		/** Submit the current decoded PCMRT (PCM RealTime) buffer to the contained source voice. */
		void SubmitPCMRTBuffers();

		/** Reads more PCMRT data for the real-time decoded audio buffers. */
		bool ReadMorePCMRTData(const int32 BufferIndex, EBufferReadMode BufferReadMode, bool* OutLooped = nullptr);

		/** Called when a buffer finishes for a real-time source and more buffers need to be read and submitted. */
		void ProcessRealTimeSource(const bool bBlockForData, const bool bOnRenderThread);

		/** Submits new real-time decoded buffers to a source voice. */
		void SubmitRealTimeSourceData(const bool bLooped, const bool bOnRenderThread);

		/** Frees any resources for this sound source. */
		void FreeResources();

		/** Updates the pitch parameter set from the game thread. */
		void UpdatePitch();
		
		/** Updates the volume parameter set from the game thread. */
		void UpdateVolume();

		/** Gets updated spatialization information for the voice. */
		void UpdateSpatialization();

		/** Updates and source effect on this voice. */
		void UpdateEffects();

		/** Updates the channel map of the sound if its a 3d sound.*/
		void UpdateChannelMaps();

		/** Computes the mono-channel map. */
		bool ComputeMonoChannelMap();

		/** Computes the stereo-channel map. */
		bool ComputeStereoChannelMap();

		/** Compute the channel map based on the number of channels. */
		bool ComputeChannelMap(const int32 NumChannels);

		/** Whether or not we should create the source voice with the HRTF spatializer. */
		bool UseObjectBasedSpatialization() const;

		/** Whether or not to use the spatialization plugin. */
		bool UseSpatializationPlugin() const;

		/** Whether or not to use the occlusion plugin. */
		bool UseOcclusionPlugin() const;

		/** Whether or not to use the reverb plugin. */
		bool UseReverbPlugin() const;

	private:

		FMixerDevice* MixerDevice;
		FMixerBuffer* MixerBuffer;
		FMixerSourceVoice* MixerSourceVoice;
		IAudioTask* AsyncRealtimeAudioTask;
		
		// Task used to store pending release/decode data
		struct FPendingReleaseData
		{
			FSoundBuffer* Buffer;
			IAudioTask* Task;

			FPendingReleaseData()
				: Buffer(nullptr)
				, Task(nullptr)
			{}
		};

		// Queue of pending release data. Pushed from audio thread, updated on audio render thread.
		TQueue<FPendingReleaseData*> PendingReleases;
		// Array of task data waiting to finished. Processed on audio render thread.
		TArray<FPendingReleaseData*> TasksWaitingToFinish;

		FCriticalSection RenderThreadCritSect;

		TArray<float> ChannelMap;
		TArray<float> StereoChannelMap;

		int32 CurrentBuffer;
		float PreviousAzimuth;

		// The decoded source buffers are using a shared pointer because the audio mixer thread 
		// will need to have a ref while playing back the sound. There is a small window where a 
		// flushed/destroyed sound can destroy the buffer before the sound finishes using the buffer.
		TArray<FMixerSourceBufferPtr> SourceVoiceBuffers;

		FSpatializationParams SpatializationParams;

		FThreadSafeBool bPlayedCachedBuffer;
		FThreadSafeBool bPlaying;
		FThreadSafeBool bLoopCallback;
		FThreadSafeBool bIsFinished;
		FThreadSafeBool bIsPlayingEffectTails;
		FThreadSafeBool bBuffersToFlush;
		FThreadSafeBool bFreeAsyncTask;

		// Whether or not we're currently releasing our resources. Prevents recycling the source until release is finished.
		FThreadSafeBool bIsReleasing;

		uint32 bResourcesNeedFreeing : 1;
		uint32 bEditorWarnedChangedSpatialization : 1;
		uint32 bUsingHRTFSpatialization : 1;
		uint32 bIs3D : 1;
		uint32 bDebugMode : 1;
	};
}
