#include "PostEffectPSOLoader.h"

using namespace szg;

#include "Engine/Application/Logger.h"
#include "Engine/GraphicsAPI/RenderingSystemValues.h"
#include "Engine/Loader/RenderPath/RenderNodeType.h"
#include "Engine/Module/Manager/RuntimeStorage/RuntimeStorage.h"
#include "Engine/Module/Render/RenderPipeline/PostEffect/Bloom/BloomPipeline.h"
#include "Engine/Module/Render/RenderPipeline/PostEffect/ChromaticAberration/ChromaticAberrationPipeline.h"
#include "Engine/Module/Render/RenderPipeline/PostEffect/GaussianBlur/GaussianBlurPipeline.h"
#include "Engine/Module/Render/RenderPipeline/PostEffect/Grayscale/GrayscalePipeline.h"
#include "Engine/Module/Render/RenderPipeline/PostEffect/LuminanceExtraction/LuminanceExtractionPipeline.h"
#include "Engine/Module/Render/RenderPipeline/PostEffect/MargeTexture4/MargeTexture4Pipeline.h"
#include "Engine/Module/Render/RenderPipeline/PostEffect/Outline/OutlinePipeline.h"
#include "Engine/Module/Render/RenderPipeline/PostEffect/RadialBlur/RadialBlurPipeline.h"

#define VECTOR2_SERIALIZER
#include "Engine/Assets/Json/JsonSerializer.h"

void PostEffectPSOLoader::setup(Reference<std::vector<RenderNodeLoader::ImmidiateData>> immediateData_) {
	immediateData = immediateData_;
}

std::unique_ptr<IPostEffectPipeline> PostEffectPSOLoader::entry_point(const nlohmann::json& json) {
	RuntimeStorage::ValueGroup& postEffectValueGroup = RuntimeStorage::GetValueList("PostEffect");
	std::any value;
	std::unique_ptr<IPostEffectPipeline> node = nullptr;

	const nlohmann::json& dataJson = json["Data"];
	const nlohmann::json& linkJson = json["Links"];
	const PostEffectType type = dataJson["Type"].get<PostEffectType>();
	switch (type) {
	case PostEffectType::Bloom:
	{
		auto temp = std::make_unique<BloomPipeline>();
		temp->initialize();
		temp->set_base_texture(immediateData->at(linkJson["Base"]).renderTexture);
		temp->set_blur_texture(immediateData->at(linkJson["CombinedTexture"]).renderTexture);
		if (!dataJson["EffectTag"].is_null()) {
			value = temp->data_mut();
		}
		node = std::move(temp);
	}
	break;
	case PostEffectType::ChromaticAberration:
	{
		auto temp = std::make_unique<ChromaticAberrationPipeline>();
		temp->initialize();
		temp->set_shader_texture(immediateData->at(linkJson["Base"]).renderTexture);
		if (!dataJson["EffectTag"].is_null()) {
			value = temp->data_mut();
		}
		node = std::move(temp);
	}
	break;
	case PostEffectType::GaussianBlur:
	{
		auto temp = std::make_unique<GaussianBlurPipeline>();
		temp->initialize();
		temp->set_base_texture(immediateData->at(linkJson["Base"]).renderTexture);
		if (!dataJson["EffectTag"].is_null()) {
			value = temp->data_mut();
		}
		node = std::move(temp);
	}
	break;
	case PostEffectType::Grayscale:
	{
		auto temp = std::make_unique<GrayscalePipeline>();
		temp->initialize();
		temp->set_shader_texture(immediateData->at(linkJson["Base"]).renderTexture);
		if (!dataJson["EffectTag"].is_null()) {
			value = temp->data_mut();
		}
		node = std::move(temp);
	}
	break;
	case PostEffectType::LuminanceExtraction:
	{
		auto temp = std::make_unique<LuminanceExtractionPipeline>();
		temp->initialize();
		temp->set_texture_resource(immediateData->at(linkJson["Base"]).renderTexture);
		if (!dataJson["EffectTag"].is_null()) {
			value = temp->data_mut();
		}
		node = std::move(temp);
	}
	break;
	case PostEffectType::TextureBlend4:
	{
		auto temp = std::make_unique<MargeTexture4Pipeline>();
		temp->initialize();
		temp->set_texture_resources({
			immediateData->at(linkJson["Base"]).renderTexture,
			immediateData->at(linkJson["MixTexture1"]).renderTexture,
			immediateData->at(linkJson["MixTexture2"]).renderTexture,
			immediateData->at(linkJson["MixTexture3"]).renderTexture
		});
		node = std::move(temp);
	}
	break;
	case PostEffectType::Outline:
	{
		auto temp = std::make_unique<OutlinePipeline>();
		temp->initialize();
		temp->set_shader_texture(
			immediateData->at(linkJson["Base"]).renderTexture,
			RenderingSystemValues::GetDepthStencilTexture()
		);
		node = std::move(temp);
		if (dataJson.value("UseRuntime", false)) {
			// do nothing
		}
	}
	break;
	case PostEffectType::RadialBlur:
	{
		auto temp = std::make_unique<RadialBlurPipeline>();
		temp->initialize();
		temp->set_shader_texture(immediateData->at(linkJson["Base"]).renderTexture);
		if (!dataJson["EffectTag"].is_null()) {
			value = temp->data_mut();
		}
		node = std::move(temp);
	}
	break;
	default:
		szgWarning("PostEffectPSOLoader::entry_point: Unknown PostEffectType \'{}\'", static_cast<i32>(type));
		return nullptr;
	}

	if (value.has_value()) {
		postEffectValueGroup[dataJson["EffectTag"].get<std::string>()] = std::move(value);
	}

	return node;
}
