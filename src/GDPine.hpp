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

	void _init_ipc(const unsigned int slot, const godot::String emulator_name, const bool default_slot);
	void _init_default_ipc(const DefaultConfigOption selection, unsigned int slot = 0);

	// To start, we will support only memory reads.
	uint8_t Read8(uint32_t addr);
	uint16_t Read16(uint32_t addr);
	uint32_t Read32(uint32_t addr);
	uint64_t Read64(uint32_t addr);
	PackedByteArray ReadMany(uint32_t addr, uint8_t length_bytes);

	GDPine();
	~GDPine();
};

VARIANT_ENUM_CAST(GDPine::DefaultConfigOption)

#endif
