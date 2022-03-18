#pragma once

#include <engine/sprite/sprite_atlas.h>

#include <engineincl.h>

namespace Techless
{

	struct SpriteAnimationFrame
	{
		Ptr<Sprite> FrameSprite = nullptr;
		uint Length = 1;

	public:

		inline friend void to_json(JSON& json, const SpriteAnimationFrame& Frame)
		{
			json = JSON{
				{"Sprite", Frame.FrameSprite->GetName()},
				{"Length", Frame.Length}
			};
		}

		inline friend void from_json(const JSON& json, SpriteAnimationFrame& Sequence)
		{
			std::string str = json.at("Sprite").get<std::string>();
			Sequence.FrameSprite = SpriteAtlas::Get(str);

			json.at("Length").get_to(Sequence.Length);
		}
	};

	/*
		a SpriteAnimationSequence is a type of AnimationSequence specifically for Sprites instead of
		generic child-based animation.
	*/

	class SpriteAnimationSequence
	{
	public:
		SpriteAnimationSequence() = default;
		SpriteAnimationSequence(SpriteAnimationSequence&& seq) = default;

		SpriteAnimationSequence(SpriteAnimationSequence& seq) = delete;
		SpriteAnimationSequence(const SpriteAnimationSequence& seq) = delete;

		void Add(const SpriteAnimationFrame& Frame)
		{ 
			TotalLength += Frame.Length;
			Sequence.push_back(Frame);
		};

		Ptr<Sprite> GetFrameSprite(uint FrameNumber)
		{
			Ptr<Sprite> CurrentSprite = nullptr;
			uint FramePos = 0;

			for (SpriteAnimationFrame& Frame : Sequence)
			{
				if (FramePos + Frame.Length > FrameNumber)
					return Frame.FrameSprite;
				else
					FramePos += Frame.Length;
			}

			return SpriteAtlas::GetMissingSprite();
		}
		
		inline uint GetLength() const { return TotalLength; };

		void SetFrameRate(uint newFrameRate) 
		{ 
			FrameRate = newFrameRate;
			FrameTime = 1.f / (float)newFrameRate; 
		};

		inline float GetFrameTime() const { return FrameTime; };
		inline uint GetFrameRate() const { return FrameRate; };

		bool Looped = false;
		std::string Name = "";

	private:

		std::vector<SpriteAnimationFrame> Sequence = {};
		
		uint TotalLength = 0;

		float FrameTime = 1.f / 60.f;
		uint FrameRate = 60;

	public:

		inline friend void to_json(JSON& json, const SpriteAnimationSequence& Sequence)
		{
			json = JSON{
				{"FrameRate", Sequence.FrameRate},
				{"Looped", Sequence.Looped},
				{"Frames", Sequence.Sequence}
			};
		}

		inline friend void from_json(const JSON& json, SpriteAnimationSequence& Sequence)
		{
			Sequence.SetFrameRate(json.at("FrameRate").get<uint>());

			json.at("Looped").get_to(Sequence.Looped);
			
			for (auto& el : json.at("Frames"))
			{
				Sequence.Add(el.get<SpriteAnimationFrame>());
			}
		}

	};

	struct SpriteAnimationSet
	{
		SpriteAnimationSet() = default;
		SpriteAnimationSet(const std::string& name)
			: Name(name) {};

		std::unordered_map<std::string, Ptr<SpriteAnimationSequence>> Sequences = {};

		std::string Default = "";
		std::string Name = "";

	public:

		inline friend void to_json(JSON& json, const SpriteAnimationSet& Set)
		{
			json = JSON{
				{"Default", Set.Default},
				{"Sequences", JSON::object()}
			};

			for (auto& [Name, Sequence] : Set.Sequences)
			{
				json.at("Sequences").emplace(Name, *Sequence);
			}
		};

		inline friend void from_json(const JSON& json, SpriteAnimationSet& Set)
		{
			json.at("Default").get_to(Set.Default);
			auto& Sequences = json["Sequences"];

			for (auto& Element : Sequences.items())
			{
				auto Seq = CreatePtr<SpriteAnimationSequence>(Element.value().get<SpriteAnimationSequence>());

				Seq->Name = Element.key();
				Set.Sequences[Element.key()] = Seq;
			}
		};

	};

	class AnimationAtlas
	{
	public:

		static void Load(const std::string& Directory);

		static Ptr<SpriteAnimationSet> Get(const std::string& Name);
		static bool Has(const std::string& Name);

	private:
		static std::unordered_map<std::string, Ptr<SpriteAnimationSet>> Animations;

	};


}