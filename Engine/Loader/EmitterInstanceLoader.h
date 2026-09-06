#pragma once

#include <optional>

#include <Library/Utility/Tools/ConstructorMacro.h>

#include "Engine/Runtime/Particle/EmitterSettings.h"

#include <json.hpp>

namespace szg {

/// <summary>
/// エミッタ設定の読み書き
/// </summary>
class EmitterInstanceLoader {
	SZG_CLASS_DELETED(EmitterInstanceLoader)

public:
	/// <summary>
	/// 設定の読み込み
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <returns>不正なJSONの場合はnullopt</returns>
	static std::optional<EmitterInstanceSettings> Load(const nlohmann::json& json);

	/// <summary>
	/// 設定の書き出し
	/// </summary>
	/// <param name="settings_">エミッタ設定</param>
	/// <param name="json">定義JSON</param>
	static void Save(const EmitterInstanceSettings& settings_, nlohmann::json& json);

private:
	/// <summary>
	/// 描画セクションの読込
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void apply_draw_json(const nlohmann::json& json, EmitterInstanceSettings& settings_);

	/// <summary>
	/// 描画セクションの書出
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void write_draw_json(nlohmann::json& json, const EmitterInstanceSettings& settings_);

	/// <summary>
	/// スケジュールセクションの読込
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void apply_schedule_json(const nlohmann::json& json, EmitterInstanceSettings& settings_);

	/// <summary>
	/// スケジュールセクションの書出
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void write_schedule_json(nlohmann::json& json, const EmitterInstanceSettings& settings_);

	/// <summary>
	/// 形状セクションの読込
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void apply_shape_json(const nlohmann::json& json, EmitterInstanceSettings& settings_);

	/// <summary>
	/// 形状セクションの書出
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void write_shape_json(nlohmann::json& json, const EmitterInstanceSettings& settings_);

	/// <summary>
	/// 方向セクションの読込
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void apply_direction_json(const nlohmann::json& json, EmitterInstanceSettings& settings_);

	/// <summary>
	/// 方向セクションの書出
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void write_direction_json(nlohmann::json& json, const EmitterInstanceSettings& settings_);

	/// <summary>
	/// 初期値セクションの読込
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void apply_initial_json(const nlohmann::json& json, EmitterInstanceSettings& settings_);

	/// <summary>
	/// 初期値セクションの書出
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void write_initial_json(nlohmann::json& json, const EmitterInstanceSettings& settings_);

	/// <summary>
	/// 運動セクションの読込
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void apply_motion_json(const nlohmann::json& json, EmitterInstanceSettings& settings_);

	/// <summary>
	/// 運動セクションの書出
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void write_motion_json(nlohmann::json& json, const EmitterInstanceSettings& settings_);

	/// <summary>
	/// 回転セクションの読込
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void apply_rotation_json(const nlohmann::json& json, EmitterInstanceSettings& settings_);

	/// <summary>
	/// 回転セクションの書出
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void write_rotation_json(nlohmann::json& json, const EmitterInstanceSettings& settings_);

	/// <summary>
	/// UVセクションの読込
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void apply_uv_json(const nlohmann::json& json, EmitterInstanceSettings& settings_);

	/// <summary>
	/// UVセクションの書出
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void write_uv_json(nlohmann::json& json, const EmitterInstanceSettings& settings_);

	/// <summary>
	/// フリップブックセクションの読込
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void apply_flipbook_json(const nlohmann::json& json, EmitterInstanceSettings& settings_);

	/// <summary>
	/// フリップブックセクションの書出
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void write_flipbook_json(nlohmann::json& json, const EmitterInstanceSettings& settings_);

	/// <summary>
	/// フェードセクションの読込
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void apply_fade_json(const nlohmann::json& json, EmitterInstanceSettings& settings_);

	/// <summary>
	/// フェードセクションの書出
	/// </summary>
	/// <param name="json">定義JSON</param>
	/// <param name="settings_">エミッタ設定</param>
	static void write_fade_json(nlohmann::json& json, const EmitterInstanceSettings& settings_);
};

}; // szg
