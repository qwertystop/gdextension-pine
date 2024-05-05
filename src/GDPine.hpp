#ifndef GDPINE_H
#define GDPINE_H

#include "../godot-cpp/gen/include/godot_cpp/classes/node.hpp"
#include "../godot-cpp/gen/include/godot_cpp/variant/packed_byte_array.hpp"
#include "../pine/src/pine.h"

using namespace godot;

class GDPine : public Node
{
	GDCLASS(GDPine, Node)

private:
	std::unique_ptr<PINE::Shared> pine_conn;
	static godot::String readable_ipc_status(PINE::Shared::IPCStatus status);

protected:
	static void _bind_methods();

public:
	/**
	 * Emulators for which a default slot exists
	 */
	enum DefaultConfigOption : unsigned char
	{
		PCSX2,
		RPCS3
	};

	void init_ipc(const unsigned int slot, const godot::String emulator_name, const bool default_slot) noexcept;
	void init_default_ipc(const DefaultConfigOption selection, unsigned int slot = 0) noexcept;

	// To start, we will support only memory reads.
	uint8_t Read8(const uint32_t addr) noexcept;
	uint16_t Read16(const uint32_t addr) noexcept;
	uint32_t Read32(const uint32_t addr) noexcept;
	uint64_t Read64(const uint32_t addr) noexcept;
	PackedByteArray ReadMany(const uint32_t addr, const uint32_t length_bytes, const bool trim_p = false) noexcept;

	GDPine();
	~GDPine();
};

VARIANT_ENUM_CAST(GDPine::DefaultConfigOption)

#endif
