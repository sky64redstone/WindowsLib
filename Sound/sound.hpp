#ifndef WINDOWS_SOUND_SOUND_HPP
	#define WINDOWS_SOUND_SOUND_HPP
	
	#include "util.hpp"
	
	namespace winLib {
		template <class T = float>
		class SoundFile {
		public:
			size_t channels, sample_size, samples, sample_rate;
			double duration, duration_in_samples;
			std::unique_ptr<T[]> raw_data;

			SoundFile() = default;
			SoundFile(const size_t channels, const size_t sample_size, const size_t sample_rate, const size_t samples) noexcept {
				this->channels = channels;
				this->sample_size = sample_size;
				this->samples = samples;
				this->sample_rate = sample_rate;
				this->duration = double(samples) / double(sample_rate);
				this->duration_in_samples = double(samples);

				this->raw_data = std::make_unique<T[]>(samples * channels);
			}

			T* data() const noexcept {
				return raw_data.get();
			}

			bool load_from_file(const std::string& file_name) const noexcept {
				std::ifstream ifs(file_name, std::ios::binary);

				if (!ifs.is_open()) {
					return false;
				}

				inline struct WaveFormatHeader {
					uint16_t format_tag;
					uint16_t channels;
					uint32_t samples_per_sec;
					uint32_t avg_bytes_per_sec;
					uint16_t block_align;
					uint16_t bits_per_sample;
				} header{ 0 };

				raw_data.reset();

				char dump[4];

				ifs.read(dump, 32);
				if (strcmp(dump, "RIFF"))
					return false;

				ifs.read(dump, 32); // unused

				ifs.read(dump, 32);
				if (strcmp(dump, "WAVE"))
					return false;

				ifs.read(dump, 32); // unused ("fmt ")
				ifs.read(dump, 32); // unused
				ifs.read(static_cast<char*>(&header), sizeof(WaveFormatHeader));

				int32_t chunk_size = 0;
				ifs.read(dump, 32);
				ifs.read(&chunk_size, sizeof(uint32_t));

				while (strncmp(dump, "data", 4)) {
					ifs.seekg(chunk_size, std::ios::cur);
					ifs.read(dump, 32);
					ifs.read((char*)&chunk_size, sizeof(uint32_t));
				}

				this->sample_size = header.bits_per_sample >> 3;
				this->samples = chunk_size / (header.channels * this->sample_size);
				this->channels = header.channels;
				this->sample_rate = header.samples_per_sec;
				this->raw_data = std::make_unique<T[]>(this->samples * this->channels);
				this->duration = double(samples) / double(sample_rate);
				this->duration_in_samples = double(samples);

				T* sample = raw_data.get();

				for (size_t i = 0; i < this->samples; i++) {
					for (size_t c = 0; c < this->channels; c++) {
						switch (this->sample_size) {
						case 1: {
							int8_t s = 0;
							ifs.read(static_cast<char*>(&s), sizeof(int8_t));
							*sample = T(s) / T(std::numeric_limits<int8_t>::max());
							break;
						}
						case 2: {
							int16_t s = 0;
							ifs.read(static_cast<char*>(&s), sizeof(int16_t));
							*sample = T(s) / T(std::numeric_limits<int16_t>::max());
							break;
						}
						case 3: {
							// 24-bit
							int32_t s = 0;
							ifs.read(static_cast<char*>(&s), 3);
							if (s & (1 << 23))
								s |= 0xFF000000;
							*sample = T(s) / T(std::pow(2, 23) - 1);
							break;
						}
						case 4: {
							int32_t s = 0;
							ifs.read(static_cast<char*>(&s), sizeof(int32_t));
							*sample = T(s) / T(std::numeric_limits<int32_t>::max());
							break;
						}
						}

						++sample;
					}
				}

				return true;
			}
		};

		template <class T = float>
		class SoundView {
		public:
			const T* data = nullptr;
			size_t samples = 0, stride = 1, offset = 0;

			SoundView() = default;
			SoundView(const T* data, const size_t samples) {
				setData(data, samples);
			}

			void setData(const T* data, const size_t samples, const size_t stride = 1, const size_t offset = 0) {
				this->data = data;
				this->samples = samples;
				this->stride = stride;
				this->offset = offset;
			}

			double getSample(const double sample) {
				double d = std::floor(sample);
				size_t p = static_cast<size_t>(d);

				return std::lerp(getValue(p), getValue(p + 1), sample - d);
			}

			T getValue(const size_t sample) {
				if (sample >= this->samples)
					return 0;
				return data[this->offset + sample * this->stride];
			}

			std::pair<double, double> getRange(const double sample1, const double sample2) {
				if (sample1 < 0 || sample2 < 0 || (sample1 > this->samples && sample2 > this->samples))
					return { 0, 0 };

				double d = getSample(sample1);
				double min = d, max = d;
				
				size_t n = static_cast<size_t>(std::ceil(sample1));
				size_t m = static_cast<size_t>(std::floor(sample2));

				for (size_t i = n; i < m; i++) {
					d = getValue(i);
					min = std::min(min, d);
					max = std::max(max, d);
				}

				d = getSample(sample2);
				min = std::min(min, d);
				max = std::max(max, d);

				return { min, max };
			}
		};

		template<class T = float>
		class SoundGeneric {
		public:
			std::vector<SoundView<T>> channel_view;
			SoundFile<T> sound_file;

			SoundGeneric() = default;
			SoundGeneric(std::string wav_file);
			SoundGeneric(std::istream& stream);
			SoundGeneric(const char* data, const size_t bytes);
			SoundGeneric(const size_t channels, const size_t sample_size, const size_t sample_rate, const size_t samples);
			
			bool loadAudioWaveform(std::string wav_file);
		};

		using Sound = SoundGeneric<float>;

		struct SoundInstance {
			Sound* sound = nullptr;
			double instance_time = 0.0;
			double duration = 0.0;
			double speed = 1.0;
			bool finished = false;
			bool loop = false;
			bool stop_flag = false;
		};

		using SoundOutput = std::list<SoundInstance>::iterator;

		namespace drivers {
			class SoundBase;
		}

		class SoundEngine {
		public:
			friend class drivers::SoundBase;

			std::unique_ptr<drivers::SoundBase> driver;

			std::list<SoundInstance> sounds;

			std::string inputDevice, outputDevice;

			uint32_t sample_rate = 44100, channels = 1, blocks = 8, block_samples = 512;
			double sample_per_time = 44100.0, time_per_sample = 1.0 / 44100.0, global_time = 0.0;
			float volume = 1.0f;

			SoundEngine();
			virtual ~SoundEngine();

			bool initAudio(uint32_t sample_rate = 44100, uint32_t channels = 1, uint32_t blocks = 8, uint32_t block_samples = 512);
			bool destroyAudio();

			std::vector<std::string> getOutputDevices();
			void useOutputDevice(const std::string& device);

			std::vector<std::string> getInputDevices();
			void useInputDevice(const std::string& device);

			void setVolume(const float volume);

			SoundOutput playSoundFile(SoundFile<float>* sound, bool loop = false, double speed = 1.0);
			void stopSoundFile(const SoundOutput& sound);
			void stopAll();

			uint32_t fillOutputBuffer(std::vector<float>& buffer, const uint32_t buffer_offset, const uint32_t required_samples);
		};
	}
#endif